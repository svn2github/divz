#include "VideoSender.h"

#include <QNetworkInterface>
#include <QTime>

VideoSender::VideoSender(QObject *parent)
	: QTcpServer(parent)
	, m_adaptiveWriteEnabled(true)
	, m_source(0)
	, m_transmitFps(10)
	, m_transmitSize(160,120)
{
	connect(&m_fpsTimer, SIGNAL(timeout()), this, SLOT(fpsTimer()));
	setTransmitFps(m_transmitFps);
}

VideoSender::~VideoSender()
{
	setVideoSource(0);
}

void VideoSender::setTransmitSize(const QSize& size)
{
	m_transmitSize = size;
}

void VideoSender::setTransmitFps(int fps)
{
	m_transmitFps = fps;
	if(fps <= 0)
		m_fpsTimer.stop();
	else
	{
		m_fpsTimer.setInterval(1000 / fps);
		m_fpsTimer.start();
	}
}

void VideoSender::fpsTimer()
{
	emit receivedFrame();
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
	
	if(!m_transmitSize.isEmpty())
	{
		//qDebug() << "VideoSender::frameReady: Downscaling video for transmission to "<<m_transmitSize;
		// To scale the video frame, first we must convert it to a QImage if its not already an image.
		// If we're lucky, it already is. Otherwise, we have to jump thru hoops to convert the byte 
		// array to a QImage then scale it.
		QImage scaledImage;
		if(!m_frame.image.isNull())
		{
			scaledImage = m_frame.image.scaled(m_transmitSize);
		}
		else
		{
			const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(m_frame.pixelFormat);
			if(imageFormat != QImage::Format_Invalid)
			{
				QImage image((const uchar*)m_frame.byteArray.constData(),
					m_frame.size.width(),
					m_frame.size.height(),
					m_frame.size.width() *
						(imageFormat == QImage::Format_RGB16  ||
						imageFormat == QImage::Format_RGB555 ||
						imageFormat == QImage::Format_RGB444 ||
						imageFormat == QImage::Format_ARGB4444_Premultiplied ? 2 :
						imageFormat == QImage::Format_RGB888 ||
						imageFormat == QImage::Format_RGB666 ||
						imageFormat == QImage::Format_ARGB6666_Premultiplied ? 3 :
						4),
					imageFormat);
					
				scaledImage = image.scaled(m_transmitSize);
			}
			else
			{
				qDebug() << "VideoSender::frameReady: Unable to convert pixel format to image format, cannot scale frame. Pixel Format:"<<m_frame.pixelFormat;
			}
		}
		
		// Now that we've got the image out of the original frame and scaled it, we have to construct a new
		// video frame to transmit on the wire from the scaledImage (assuming the sccaledImage is valid.)
		// We attempt to transmit in its native format without converting it if we can to save local CPU power.
		if(scaledImage.isNull())
		{
			m_scaledFrame = m_frame;
		}
		else
		{
			m_scaledFrame.bufferType = VideoFrame::BUFFER_IMAGE;
			m_scaledFrame.captureTime = m_frame.captureTime;

			QImage::Format format = scaledImage.format();
			m_scaledFrame.pixelFormat =
				format == QImage::Format_ARGB32 ? QVideoFrame::Format_ARGB32 :
				format == QImage::Format_RGB32  ? QVideoFrame::Format_RGB32  :
				format == QImage::Format_RGB888 ? QVideoFrame::Format_RGB24  :
				format == QImage::Format_RGB16  ? QVideoFrame::Format_RGB565 :
				format == QImage::Format_RGB555 ? QVideoFrame::Format_RGB555 :
				//format == QImage::Format_ARGB32_Premultiplied ? QVideoFrame::Format_ARGB32_Premultiplied :
				// GLVideoDrawable doesn't support premultiplied - so the format conversion below will convert it to ARGB32 automatically
				QVideoFrame::Format_Invalid;
				
			if(m_scaledFrame.pixelFormat == QVideoFrame::Format_Invalid)
			{
				qDebug() << "VideoFrame: image was not in an acceptable format, converting to ARGB32 automatically.";
				scaledImage = scaledImage.convertToFormat(QImage::Format_ARGB32);
				m_scaledFrame.pixelFormat = QVideoFrame::Format_ARGB32;
			}
			
			m_scaledFrame.image = scaledImage;
			m_scaledFrame.setSize(scaledImage.size());
			m_scaledFrame.holdTime = m_transmitFps <= 0 ? m_frame.holdTime : 1000/m_transmitFps;
		}
	}
	
	if(m_transmitFps <= 0)
		emit receivedFrame();
	else
		m_frame.holdTime = 1000/m_transmitFps;
}

void VideoSender::incomingConnection(int socketDescriptor)
{
	VideoSenderThread *thread = new VideoSenderThread(socketDescriptor, m_adaptiveWriteEnabled);
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(this, SIGNAL(receivedFrame()), thread, SLOT(frameReady()), Qt::QueuedConnection);
	thread->moveToThread(thread);
	thread->setSender(this);
	thread->start();
	//qDebug() << "VideoSender: Client Connected, Socket Descriptor:"<<socketDescriptor;
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
	
	qDebug() << "VideoSenderThread: Connection from "<<m_socket->peerAddress().toString(); //, Socket Descriptor:"<<socketDescriptor;
	
	
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
		//qDebug() << "VideoSenderThread::frameReady():"<<m_socket->bytesToWrite()<<"bytes pending write on socket, not sending image"<<frameCounter;
	}
	else
	{
		QSize scaledSize   = m_sender->transmitSize();
		VideoFrame frame   = scaledSize.isEmpty() ? m_sender->frame() : m_sender->scaledFrame();
		QSize originalSize = scaledSize.isEmpty() ? frame.size        : m_sender->frame().size;
		
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
					"%d " // holdTime
					"%d " // original size X
					"%d", // original size Y
					byteCount, 
					frame.size.width(), frame.size.height(),
					(int)frame.pixelFormat,(int)frame.image.format(),
					(int)frame.bufferType,
					timestamp,frame.holdTime,
					originalSize.width(), originalSize.height());
		//qDebug() << "VideoSenderThread::frameReady: header data:"<<headerData;
		
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

