#include "JpegServer.h"

#include <QNetworkInterface>

JpegServer::JpegServer(QObject *parent)
	: QTcpServer(parent)
{
}
	
void JpegServer::setProvider(QObject *provider, const char * signalName, bool deleteImage)
{
	m_imageProvider = provider;
	m_signalName    = signalName;
	m_deleteImage   = deleteImage;
}


QString JpegServer::myAddress()
{
	QString ipAddress;
	
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// use the first non-localhost IPv4 address
	for (int i = 0; i < ipAddressesList.size(); ++i) 
	{
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
		    ipAddressesList.at(i).toIPv4Address())
			ipAddress = ipAddressesList.at(i).toString();
	}
	
	// if we did not find one, use IPv4 localhost
	if (ipAddress.isEmpty())
		ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	
	return QString("http://%1:%2/").arg(ipAddress).arg(serverPort());
}

void JpegServer::incomingConnection(int socketDescriptor)
{
	JpegServerThread *thread = new JpegServerThread(socketDescriptor, m_deleteImage);
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(m_imageProvider, m_signalName, thread, SLOT(imageReady(QImage*)), Qt::QueuedConnection);
	thread->start();
	qDebug() << "JpegServer: Client Connected, Socket Descriptor:"<<socketDescriptor;
}

/** Thread **/
#define BOUNDARY "JpegServerThread-uuid-0eda9b03a8314df4840c97113e3fe160"
#include <QImageWriter>
#include <QImage>

JpegServerThread::JpegServerThread(int socketDescriptor, bool deleteImage, QObject *parent)
    : QThread(parent), m_socketDescriptor(socketDescriptor), m_deleteImage(deleteImage)
{
	
}

JpegServerThread::~JpegServerThread()
{
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

void JpegServerThread::imageReady(QImage *image)
{
	if(!image)
		return;
	
	QImage tmp = *image;
	
	if(tmp.format() != QImage::Format_RGB32)
		tmp = tmp.convertToFormat(QImage::Format_RGB32);
		
// 	static int frameCounter = 0;
// 	frameCounter++;
// 	qDebug() << "JpegServerThread: Writing Frame#:"<<frameCounter;
	
	
	m_socket->write("Content-type: image/jpeg\r\n\r\n");
	
	QImageWriter writer(m_socket, "jpg");
	if(!writer.canWrite())
	{
		qDebug() << "ImageWriter can't write!";
	}
	else
	if(!writer.write(tmp))
	{
		qDebug() << "ImageWriter reported error:"<<writer.errorString();
		quit();
	}
	
	m_socket->write("--" BOUNDARY "\r\n");
	
	if(m_deleteImage)
	{
		delete image;
		image = 0;
	}
}



//    m_socket->write(block);
/*    m_socket->disconnectFromHost();
    m_socket->waitForDisconnected();*/
//! [4]
