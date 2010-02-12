#include "JpegServer.h"

#include <QNetworkInterface>

JpegServer::JpegServer(QObject *parent)
	: QTcpServer(parent)
	, m_imageProvider(0)
	, m_signalName(0)
	, m_adaptiveWriteEnabled(true)
{
}
	
void JpegServer::setProvider(QObject *provider, const char * signalName)
{
	m_imageProvider = provider;
	m_signalName    = signalName;
}

void JpegServer::incomingConnection(int socketDescriptor)
{
	JpegServerThread *thread = new JpegServerThread(socketDescriptor, m_adaptiveWriteEnabled);
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(m_imageProvider, m_signalName, thread, SLOT(imageReady(QImage*)), Qt::QueuedConnection);
	thread->start();
	qDebug() << "JpegServer: Client Connected, Socket Descriptor:"<<socketDescriptor;
}

/** Thread **/
#define BOUNDARY "JpegServerThread-uuid-0eda9b03a8314df4840c97113e3fe160"
#include <QImageWriter>
#include <QImage>

JpegServerThread::JpegServerThread(int socketDescriptor, bool adaptiveWriteEnabled, QObject *parent)
    : QThread(parent)
    , m_socketDescriptor(socketDescriptor)
    , m_adaptiveWriteEnabled(adaptiveWriteEnabled)
{
	
}

JpegServerThread::~JpegServerThread()
{
	m_socket->abort();
	delete m_socket;
}

void JpegServerThread::run()
{
	m_socket = new QTcpSocket();
	
	if (!m_socket->setSocketDescriptor(m_socketDescriptor)) 
	{
		emit error(m_socket->error());
		return;
	}
	
	writeHeaders();
	
	m_writer.setDevice(m_socket);
	m_writer.setFormat("jpg");
	//m_writer.setQuality(80);
	
	// enter event loop
	exec();
	
	// when imageReady() signal arrives, write data with header to socket
}

void JpegServerThread::writeHeaders()
{
	m_socket->write("HTTP/1.0 200 OK\r\n");
	m_socket->write("Server: DViz/JpegServer - Josiah Bryan <josiahbryan@gmail.com>\r\n");
	m_socket->write("Content-Type: multipart/x-mixed-replace;boundary=" BOUNDARY "\r\n");
	m_socket->write("\r\n");
	m_socket->write("--" BOUNDARY "\r\n");
}

void JpegServerThread::imageReady(QImage *tmp)
{
	QImage image = *tmp;
	static int frameCounter = 0;
 	frameCounter++;
//  	qDebug() << "JpegServerThread: [START] Writing Frame#:"<<frameCounter;
	
	if(m_adaptiveWriteEnabled && m_socket->bytesToWrite() > 0)
	{
		qDebug() << "JpegServerThread::imageReady():"<<m_socket->bytesToWrite()<<"bytes pending write on socket, not sending image"<<frameCounter;
	}
	else
	{
		if(image.format() != QImage::Format_RGB32)
			image = image.convertToFormat(QImage::Format_RGB32);
		
		if(m_socket->state() == QAbstractSocket::ConnectedState)
		{
			m_socket->write("Content-type: image/jpeg\r\n\r\n");
		}
		
		if(!m_writer.write(image))
		{
			qDebug() << "ImageWriter reported error:"<<m_writer.errorString();
			quit();
		}
		
		m_socket->write("--" BOUNDARY "\r\n");
	}

}

