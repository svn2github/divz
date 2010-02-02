#include "MjpegClient.h"

#include <QTimer>

MjpegClient::MjpegClient(QObject *parent) 
	: QObject(parent)
	, m_socket(0)
// 	, m_log(0)
// 	, m_inst(0)
	, m_boundary("")
	, m_firstBlock(true)
	, m_blockSize(0)
	, m_dataBlock("")
	
{
#ifdef MJPEG_TEST
	m_label = new QLabel();
	m_label->setWindowTitle("MJPEG Test");
	m_label->setGeometry(QRect(0,0,320,240));
	m_label->show();
#endif
}
MjpegClient::~MjpegClient()
{
// 	delete m_parser;
}
	
// void MjpegClient::setLogger(MainWindow *log)
// {
// 	m_log = log;
// }
/*
void MjpegClient::setInstance(OutputInstance *inst)
{
	m_inst = inst;
}*/

bool MjpegClient::connectTo(const QString& host, int port, QString url)
{
	if(m_socket)
		m_socket->abort();
		
	m_socket = new QTcpSocket(this);
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(dataReady()));
	connect(m_socket, SIGNAL(disconnected()), this, SIGNAL(socketDisconnected()));
	connect(m_socket, SIGNAL(connected()), this, SIGNAL(socketConnected()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(socketError(QAbstractSocket::SocketError)));
	
	m_blockSize = 0;
	m_socket->connectToHost(host,port);
	
	if(url.isEmpty())
		url = "/";
		
	char data[256];
	sprintf(data, "GET %s HTTP/1.0\r\n\r\n",qPrintable(url));
	m_socket->write((const char*)&data,strlen((const char*)data));
	
	return true;
}

void MjpegClient::log(const QString& str)
{
	qDebug() << "MjpegClient::log(): "<<str;
// 	if(m_log)
// 		m_log->log(str);
}

void MjpegClient::dataReady()
{
	m_blockSize = m_socket->bytesAvailable();
	
	char * data = (char*)malloc(m_blockSize * sizeof(char));
	if(data == NULL)
	{
		qDebug() << "Error allocating memory for incomming data - asked for "<<m_blockSize<<" bytes, got nothing, exiting.";
		exit();
		return;
	}
	
	int bytesRead = m_socket->read(data,m_blockSize);
	if(bytesRead > 0)
	{
		m_dataBlock.append(data,m_blockSize);
		processBlock();
	}
	
	free(data);
	data = 0;
	
	if(m_socket->bytesAvailable())
	{
		QTimer::singleShot(0, this, SLOT(dataReady()));
	}
}

void MjpegClient::processBlock()
{
	if(m_boundary.isEmpty())
	{
		// still waiting for boundary string defenition, check for content type in data block
		char * ctypeString = 0;
		if(m_dataBlock.contains("Content-Type:"))
		{
			ctypeString = "Content-Type:";
		}
		else
		if(m_dataBlock.contains("content-type:"))
		{
			// allow for buggy servers (some IP cameras - trendnet I'm looking at you!) sometimes dont use proper
			// case in their headers.
			ctypeString = "content-type:";
		}
		
		if(ctypeString)
		{
			int ctypeIdx = m_dataBlock.indexOf(ctypeString);
			if(ctypeIdx < 0)
			{
				qDebug() << "Error: Can't find content-type index in data block, exiting.";
				exit();
				return;
			}
			
			static QString boundaryMarker = "boundary=";
			int boundaryStartIdx = m_dataBlock.indexOf(boundaryMarker,ctypeIdx);
			if(boundaryStartIdx < 0)
			{
				qDebug() << "Error: Can't find boundary index after the first content-type index in data block, exiting.";
				exit();
				return;
			}
			
			int eolIdx = m_dataBlock.indexOf("\n",boundaryStartIdx);
			int pos = boundaryStartIdx + boundaryMarker.length();
			m_boundary = m_dataBlock.mid(pos, eolIdx - pos);
			m_boundary.replace("\r","");
// 			qDebug() << "processBlock(): m_boundary:"<<m_boundary<<", pos:"<<pos<<", eolIdx:"<<eolIdx;
		}
	}
	else
	{
		// we have the boundary string defenition, check for the boundary string in the data block.
		// If found, grab the data from the start of the block up to and including the boundary, leaving any data after the boundary in the block.
		// What we then have to process could look:
		// Content-Type.....\r\n(data)--(boundary)
		
		// If its the first block, we wait for the boundary, but discard it since the first block is the one that contains the server headers
		// like the boundary definition, Server:, Connection:, etc.
		int idx = m_dataBlock.indexOf(m_boundary);
		
		while(idx > 0)
		{
			QByteArray block = m_dataBlock.left(idx);
			
			int blockAndBoundaryLength = idx + m_boundary.length();
			m_dataBlock.remove(0,blockAndBoundaryLength);
			//qDebug() << "processBlock(): block length:"<<block.length()<<", blockAndBoundaryLength:"<<blockAndBoundaryLength;
			
			if(m_firstBlock)
			{
				QString string = block;
				//qDebug() << "processBlock(): Discarding block since first block flag is true. Dump of block:\n"<<string;
				m_firstBlock = false;
			}
			else
			{
				static const char * eol1 = "\n\n";
				static const char * eol2 = "\r\n\r\n";
				int headerLength = 0;
				if(block.contains(eol2))
					headerLength = block.indexOf(eol2,0) + 4;
				else
				if(block.contains(eol1))
					headerLength = block.indexOf(eol1,0) + 2;
				
				if(headerLength)
				{
					QString header = block.left(headerLength);
					
					block.remove(0,headerLength);
					
					// Block should now be just data
					//qDebug() << "processBlock(): block length:"<<block.length()<<", headerLength:"<<headerLength<<", header:"<<header;
					
					// TODO: do something with contents block - like write to a file, emit signal, whatever.
					if(block.length() > 0)
					{
					
						QImage frame = QImage::fromData(block);
						emit newImage(frame);
						
						#ifdef MJPEG_TEST
						QPixmap pix = QPixmap::fromImage(frame);
						m_label->setPixmap(pix);
						m_label->resize(pix.width(),pix.height());
						#endif
						
						
					
// 						static QString filename = "test.jpg";
// 						QFile file(filename);
// 						
// 						if(!file.open(QIODevice::WriteOnly))
// 						{
// 							qDebug() << "processBlock(): Unable to open "<<filename<<" for writing";
// 						}
// 						else
// 						{
// 							qint64 bytes = file.write(block);
// 							file.close();
// 							qDebug() << "processBlock(): Wrote"<<bytes<<" to "<<filename;
// 						}
					}
				}

			}
			
			// check for another boundary string in the data before exiting from processBlock()
			idx = m_dataBlock.indexOf(m_boundary);
		}
		
	}
	
// 	qDebug() << "processBlock(): End of processing, m_dataBlock.size() remaining:"<<m_dataBlock.size();
	
}

void MjpegClient::exit()
{
	if(m_socket)
	{
		m_socket->abort();
		m_socket->disconnectFromHost();
		//m_socket->waitForDisconnected();
		delete m_socket;
		m_socket = 0;
	}
}


