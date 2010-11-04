#include "VideoSender.h"

#include <QNetworkInterface>
#include <QTime>

VideoSender::VideoSender(QObject *parent)
	: QTcpServer(parent)
	, m_adaptiveWriteEnabled(true)
	, m_source(0)
{
}

VideoSender::~VideoSender()
{
	setVideoSource(0);
}
	
void VideoSender::setVideoSource(VideoSource *source)
{
	if(m_source == source)
		return;
		
	if(m_source)
		disconnectVideoSource();
	
	m_source = source;
	if(m_source)
	{	
		connect(m_source, SIGNAL(frameReady()), this, SLOT(frameReady()));
		connect(m_source, SIGNAL(destroyed()), this, SLOT(disconnectVideoSource()));
		
		//qDebug() << "GLVideoDrawable::setVideoSource(): "<<objectName()<<" m_source:"<<m_source;
		//setVideoFormat(m_source->videoFormat());
		
		frameReady();
	}
	else
	{
		qDebug() << "VideoSender::setVideoSource(): "<<this<<" Source is NULL";
	}

}

void VideoSender::disconnectVideoSource()
{
	if(!m_source)
		return;
	disconnect(m_source, 0, this, 0);
	m_source = 0;
}


void VideoSender::frameReady()
{
	if(!m_source)
		return;
	
	// This seems to prevent crashes during startup of an application when a thread is pumping out frames
	// before the app has finished initalizing.
	//QMutexLocker lock(&m_frameReadyLock);
	
	VideoFrame f = m_source->frame();
	if(f.isValid())
		m_frame = f;
	
	//QImage image((const uchar*)m_frame.byteArray.constData(),m_frame.size.width(),m_frame.size.height(),QImage::Format_RGB32);
	//emit testImage(image.scaled(160,120), m_frame.captureTime);
	//emit testImage(image, QTime::currentTime()); //m_frame.captureTime);
	
	emit receivedFrame();
	
}

void VideoSender::incomingConnection(int socketDescriptor)
{
	VideoSenderThread *thread = new VideoSenderThread(socketDescriptor, m_adaptiveWriteEnabled);
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(this, SIGNAL(receivedFrame()), thread, SLOT(frameReady()), Qt::QueuedConnection);
	thread->moveToThread(thread);
	thread->setSender(this);
	thread->start();
	qDebug() << "VideoSender: Client Connected, Socket Descriptor:"<<socketDescriptor;
}


/** Thread **/
#define BOUNDARY "VideoSenderThread-uuid-0eda9b03a8314df4840c97113e3fe160"
#include <QImageWriter>
#include <QImage>

VideoSenderThread::VideoSenderThread(int socketDescriptor, bool adaptiveWriteEnabled, QObject *parent)
    : QThread(parent)
    , m_socketDescriptor(socketDescriptor)
    , m_adaptiveWriteEnabled(adaptiveWriteEnabled)
    , m_sentFirstHeader(false)
{
	//connect(m_sender, SIGNAL(destroyed()),    this, SLOT(quit()));
}

VideoSenderThread::~VideoSenderThread()
{
	m_sender = 0;
	
	m_socket->abort();
	delete m_socket;
	m_socket = 0;
}

void VideoSenderThread::setSender(VideoSender *s)
{
	m_sender = s;
}

void VideoSenderThread::run()
{
	m_socket = new QTcpSocket();
	connect(m_socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
	
	if (!m_socket->setSocketDescriptor(m_socketDescriptor)) 
	{
		emit error(m_socket->error());
		return;
	}
	
	// enter event loop
	exec();
	
	// when frameReady() signal arrives, write data with header to socket
}


void VideoSenderThread::frameReady()
{
	if(!m_socket)
		return;
		
	if(!m_sender)
		return;
		
	
	//QTime time = QTime::currentTime();
	//QImage image = *tmp;
	static int frameCounter = 0;
 	frameCounter++;
//  	qDebug() << "VideoSenderThread: [START] Writing Frame#:"<<frameCounter;
	
	if(m_adaptiveWriteEnabled && m_socket->bytesToWrite() > 0)
	{
		qDebug() << "VideoSenderThread::frameReady():"<<m_socket->bytesToWrite()<<"bytes pending write on socket, not sending image"<<frameCounter;
	}
	else
	{
		VideoFrame frame = m_sender->frame();
		
		QTime time = frame.captureTime;
		int timestamp = time.hour()   * 60 * 60 * 1000 +
				time.minute() * 60 * 1000      + 
				time.second() * 1000           +
				time.msec();
		
		int byteCount = 
			frame.bufferType == VideoFrame::BUFFER_IMAGE ? 
			frame.image.byteCount() : 
			frame.byteArray.size();
		
		#define HEADER_SIZE 256
		
		if(!m_sentFirstHeader)
		{
			m_sentFirstHeader = true;
			char headerData[HEADER_SIZE];
			memset(&headerData, 0, HEADER_SIZE);
			sprintf((char*)&headerData,"%d",byteCount);
			//qDebug() << "header data:"<<headerData;
			
			m_socket->write((const char*)&headerData,HEADER_SIZE);
		}
		
		char headerData[HEADER_SIZE];
		memset(&headerData, 0, HEADER_SIZE);
		
		sprintf((char*)&headerData,
					"%d " // byteCount
					"%d " // w
					"%d " // h
					"%d " // pixelFormat
					"%d " // image.format
					"%d " // bufferType
					"%d " // timestamp
					"%d", // holdTime
					byteCount, 
					frame.size.width(), frame.size.height(),
					(int)frame.pixelFormat,(int)frame.image.format(),
					(int)frame.bufferType,
					timestamp,frame.holdTime);
		//qDebug() << "header data:"<<headerData;
		
		m_socket->write((const char*)&headerData,HEADER_SIZE);
		
		if(frame.bufferType == VideoFrame::BUFFER_IMAGE)
		{
			const uchar *bits = (const uchar*)frame.image.bits();
			m_socket->write((const char*)bits,byteCount);
		}
		else
		if(frame.bufferType == VideoFrame::BUFFER_BYTEARRAY)
		{
			const uchar *bits = (const uchar*)frame.byteArray.constData();
			m_socket->write((const char*)bits,byteCount);
		}

		m_socket->flush();

		//QTime time2 = QTime::currentTime();
		//int timestamp2 = time.hour() + time.minute() + time.second() + time.msec();
		//qDebug() << "write time: "<<(timestamp2-timestamp);
	}

}

