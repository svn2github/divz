#include "VideoReceiver.h"

#include <QCoreApplication>
#include <QTime>
#include <QTimer>

VideoReceiver::VideoReceiver(QObject *parent) 
	: VideoSource(parent)
	, m_socket(0)
	, m_boundary("")
	, m_firstBlock(true)
	, m_dataBlock("")
	, m_autoResize(-1,-1)
	, m_autoReconnect(true)
	, m_byteCount(-1)
	
{
// #ifdef MJPEG_TEST
// 	m_label = new QLabel();
// 	m_label->setWindowTitle("VideoReceiver Test");
// 	m_label->setGeometry(QRect(0,0,320,240));
// 	m_label->show();
// #endif
}
VideoReceiver::~VideoReceiver()
{
}
  
bool VideoReceiver::connectTo(const QString& host, int port, QString url, const QString& user, const QString& pass)
{
	if(url.isEmpty())
		url = "/";
		
	m_host = host;
	m_port = port;
	m_url = url;
	m_user = user;
	m_pass = pass;
	
	if(m_socket)
	{
		m_socket->abort();
		delete m_socket;
		m_socket = 0;
	}
		
	m_socket = new QTcpSocket(this);
	connect(m_socket, SIGNAL(readyRead()),    this,   SLOT(dataReady()));
	connect(m_socket, SIGNAL(disconnected()), this,   SLOT(lostConnection()));
	connect(m_socket, SIGNAL(disconnected()), this, SIGNAL(socketDisconnected()));
	connect(m_socket, SIGNAL(connected()),    this, SIGNAL(socketConnected()));
	connect(m_socket, SIGNAL(connected()),    this,   SLOT(connectionReady()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(socketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(lostConnection(QAbstractSocket::SocketError)));
	
	m_socket->connectToHost(host,port);
	m_socket->setReadBufferSize(1024 * 1024);
	
	
	m_time.start();
	m_debugFps = true;
	m_frameCount = 0;
	m_latencyAccum = 0;
	
	return true;
}

void VideoReceiver::connectionReady()
{
	qDebug() << "Connected";
}

void VideoReceiver::log(const QString& str)
{
	qDebug() << "VideoReceiver::log(): "<<str;
}

void VideoReceiver::lostConnection()
{
	if(m_autoReconnect)
		QTimer::singleShot(1000,this,SLOT(reconnect()));
}

void VideoReceiver::lostConnection(QAbstractSocket::SocketError error)
{
	qDebug() << "VideoReceiver::lostConnection("<<error<<"):" << m_socket->errorString();
	
	if(error == QAbstractSocket::ConnectionRefusedError)
		lostConnection();
}


void VideoReceiver::reconnect()
{
	log(QString("Attempting to reconnect to %1:%2%3").arg(m_host).arg(m_port).arg(m_url));
	connectTo(m_host,m_port,m_url);
}

void VideoReceiver::dataReady()
{
	QByteArray bytes = m_socket->readAll();
	if(bytes.size() > 0)
	{
		m_dataBlock.append(bytes);
//		qDebug() << "dataReady(), read bytes:"<<bytes.count()<<", buffer size:"<<m_dataBlock.size();
		
		processBlock();
	}
}

void VideoReceiver::processBlock()
{
				
	#define HEADER_SIZE 256
	
	// First thing server sends is a single 256-byte header containing the initial frame byte count
	// Byte count and frame size CAN change in-stream as needed.
	if(m_byteCount < 0)
	{
		if(m_dataBlock.size() >= HEADER_SIZE)
		{
			QByteArray header = m_dataBlock.left(HEADER_SIZE);
			m_dataBlock.remove(0,HEADER_SIZE);
			
			const char *headerData = header.constData();
			sscanf(headerData,"%d",&m_byteCount);
		}
	}
	
	if(m_byteCount >= 0)
	{
		int frameSize = m_byteCount+HEADER_SIZE;
		
		while(m_dataBlock.size() >= frameSize)
		{
			QByteArray block = m_dataBlock.left(frameSize);
			m_dataBlock.remove(0,frameSize);
			
			QByteArray header = block.left(HEADER_SIZE);
			block.remove(0,HEADER_SIZE);
			
			const char *headerData = header.constData();
			
								
			int byteTmp,imgX,imgY,pixelFormatId,imageFormatId,bufferType,timestamp,holdTime;
			
			sscanf(headerData,
					"%d " // byteCount
					"%d " // w
					"%d " // h
					"%d " // pixelFormat
					"%d " // image.format
					"%d " // bufferType
					"%d " // timestamp
					"%d", // holdTime
					&byteTmp, 
					&imgX,
					&imgY,
					&pixelFormatId,
					&imageFormatId,
					&bufferType,
					&timestamp,
					&holdTime);
					
			//qDebug() << "header data:"<<headerData;
			if(byteTmp != m_byteCount)
			{
				m_byteCount = byteTmp;
				frameSize = m_byteCount + HEADER_SIZE;
				qDebug() << "Frame size changed: "<<frameSize;
			}
			//qDebug() << "processBlock(): header data:"<<headerData;
			//QImage frame = QImage::fromData(block);
		
			//QImage frame = QImage::fromData(block);
			//QImage frame((const uchar*)block.constData(), imgX, imgY, (QImage::Format)formatId); 
			if(pixelFormatId == 0)
				pixelFormatId = (int)QVideoFrame::Format_RGB32;
				
			//VideoFrame frame;
			m_frame.holdTime    = holdTime;
			m_frame.captureTime = timestampToQTime(timestamp);
			m_frame.pixelFormat = (QVideoFrame::PixelFormat)pixelFormatId;
			m_frame.bufferType  = (VideoFrame::BufferType)bufferType;
			
			//qDebug() << "final pixelformat:"<<pixelFormatId;
			
			
			if(m_frame.bufferType == VideoFrame::BUFFER_IMAGE)
			{
				m_frame.image = QImage((const uchar*)block.constData(), imgX, imgY, (QImage::Format)imageFormatId);
				m_frame.isRaw = false;
			}
			else
			{
				m_frame.byteArray.clear();
				m_frame.byteArray.append((const char*)block.constData(), m_byteCount);
				m_frame.isRaw = true;
			}
			
			m_frame.setSize(QSize(imgX,imgY));

			enqueue(m_frame);
			/*
			//480,480, QImage::Format_RGB32);
			
			if(!frame.isNull())
			{
				//qDebug() << "processBlock(): New image received, original size:"<<frame.size()<<", bytes:"<<block.length()<<", format:"<<formatId;
				
				if(m_autoResize.width()>0 && m_autoResize.height()>0 && 
				m_autoResize != frame.size())
					frame = frame.scaled(m_autoResize);
			
				
				//		qDebug() << "processBlock(): Emitting new image, size:"<<frame.size();
				emit newImage(frame);
			}	*/
			
			
			#ifdef MJPEG_TEST
// 			QPixmap pix = QPixmap::fromImage(frame);
// 			m_label->setPixmap(pix);
// 			m_label->resize(pix.width(),pix.height());
// 			//qDebug() << "processBlock(): latency: "<<;
			#endif
			
			int msecLatency = msecTo(timestamp);
			m_latencyAccum += msecLatency;
			
			if (!(m_frameCount % 100)) 
			{
				QString framesPerSecond;
				framesPerSecond.setNum(m_frameCount /(m_time.elapsed() / 1000.0), 'f', 2);
				
				QString latencyPerFrame;
				latencyPerFrame.setNum((((double)m_latencyAccum) / ((double)m_frameCount)), 'f', 3);
				
				if(m_debugFps && framesPerSecond!="0.00")
					qDebug() << "VideoReceiver: Receive FPS: " << qPrintable(framesPerSecond) << qPrintable(QString(", Receive Latency: %1 ms").arg(latencyPerFrame));
		
				m_time.start();
				m_frameCount = 0;
				m_latencyAccum = 0;
				
				//lastFrameTime = time.elapsed();
			}
			m_frameCount++;
		}
	}
}


QTime VideoReceiver::timestampToQTime(int ts)
{
	int hour = ts / 1000 / 60 / 60;
	int msTmp = ts - (hour * 60 * 60 * 1000);
	int min = msTmp / 1000 / 60;
	msTmp -= min * 60 * 1000;
	int sec = msTmp / 1000;
	msTmp -= sec * 1000;
	
	return QTime(hour, min, sec, msTmp);
}


int VideoReceiver::msecTo(int timestamp)
{
	QTime time = QTime::currentTime();
	//int currentTimestamp = time.hour() + time.minute() + time.second() + time.msec();
	int currentTimestamp = 
			time.hour()   * 60 * 60 * 1000 +
			time.minute() * 60 * 1000      + 
			time.second() * 1000           +
			time.msec();
	return currentTimestamp - timestamp;
}

void VideoReceiver::exit()
{
	if(m_socket)
	{
		m_socket->abort();
		m_socket->disconnectFromHost();
		//m_socket->waitForDisconnected();
		m_socket->deleteLater();
		//delete m_socket;
		m_socket = 0;
	}
}


