#include "VideoSender.h"

#include <QNetworkInterface>
#include <QTime>

VideoSender::VideoSender(QObject *parent)
	: QTcpServer(parent)
	, m_adaptiveWriteEnabled(true)
	, m_source(0)
	, m_transmitFps(10)
	, m_transmitSize(160,120)
	//, m_transmitSize(0,0)
	//, m_scaledFrame(0)
	//, m_frame(0)
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
	
	VideoFramePtr frame = m_source->frame();
	if(!frame)
		return;
		
	sendLock();
	
	if(frame && frame->isValid())
	{
		m_origSize = frame->size();
		#ifdef DEBUG_VIDEOFRAME_POINTERS
		qDebug() << "VideoSender::frameReady(): Mark1: frame:"<<frame;
		#endif
		
// 		frame->incRef();
		if(m_transmitSize.isEmpty())
			m_transmitSize = m_origSize;
		
		//qDebug() << "VideoSender::frameReady: Downscaling video for transmission to "<<m_transmitSize;
		// To scale the video frame, first we must convert it to a QImage if its not already an image.
		// If we're lucky, it already is. Otherwise, we have to jump thru hoops to convert the byte 
		// array to a QImage then scale it.
		QImage scaledImage;
		if(!frame->image().isNull())
		{
			scaledImage = m_transmitSize == m_origSize ? 
				frame->image() : 
				frame->image().scaled(m_transmitSize);
		}
		else
		{
			#ifdef DEBUG_VIDEOFRAME_POINTERS
			qDebug() << "VideoSender::frameReady(): Scaling data from frame:"<<frame<<", pointer:"<<frame->pointer();
			#endif
			const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(frame->pixelFormat());
			if(imageFormat != QImage::Format_Invalid)
			{
				QImage image(frame->pointer(),
					frame->size().width(),
					frame->size().height(),
					frame->size().width() *
						(imageFormat == QImage::Format_RGB16  ||
						imageFormat == QImage::Format_RGB555 ||
						imageFormat == QImage::Format_RGB444 ||
						imageFormat == QImage::Format_ARGB4444_Premultiplied ? 2 :
						imageFormat == QImage::Format_RGB888 ||
						imageFormat == QImage::Format_RGB666 ||
						imageFormat == QImage::Format_ARGB6666_Premultiplied ? 3 :
						4),
					imageFormat);
					
				scaledImage = m_transmitSize == m_origSize ? 
					image.copy() : 
					image.scaled(m_transmitSize);
				//qDebug() << "Downscaled image from "<<image.byteCount()<<"bytes to "<<scaledImage.byteCount()<<"bytes, orig ptr len:"<<frame->pointerLength()<<", orig ptr:"<<frame->pointer();
			}
			else
			{
				qDebug() << "VideoSender::frameReady: Unable to convert pixel format to image format, cannot scale frame. Pixel Format:"<<frame->pixelFormat();
			}
		}
		
		#ifdef DEBUG_VIDEOFRAME_POINTERS
		qDebug() << "VideoSender::frameReady(): Mark2: frame:"<<frame;
		#endif
		
		// Now that we've got the image out of the original frame and scaled it, we have to construct a new
		// video frame to transmit on the wire from the scaledImage (assuming the sccaledImage is valid.)
		// We attempt to transmit in its native format without converting it if we can to save local CPU power.
		if(!scaledImage.isNull())
		{
			m_captureTime = frame->captureTime();

			QImage::Format format = scaledImage.format();
			m_pixelFormat = 
				format == QImage::Format_ARGB32 ? QVideoFrame::Format_ARGB32 :
				format == QImage::Format_RGB32  ? QVideoFrame::Format_RGB32  :
				format == QImage::Format_RGB888 ? QVideoFrame::Format_RGB24  :
				format == QImage::Format_RGB16  ? QVideoFrame::Format_RGB565 :
				format == QImage::Format_RGB555 ? QVideoFrame::Format_RGB555 :
				//format == QImage::Format_ARGB32_Premultiplied ? QVideoFrame::Format_ARGB32_Premultiplied :
				// GLVideoDrawable doesn't support premultiplied - so the format conversion below will convert it to ARGB32 automatically
				QVideoFrame::Format_Invalid;
				
			if(m_pixelFormat == QVideoFrame::Format_Invalid)
			{
				qDebug() << "VideoFrame: image was not in an acceptable format, converting to ARGB32 automatically.";
				scaledImage = scaledImage.convertToFormat(QImage::Format_ARGB32);
				m_pixelFormat = QVideoFrame::Format_ARGB32;
			}
			
			uchar *ptr = (uchar*)malloc(sizeof(uchar) * scaledImage.byteCount());
			const uchar *src = (const uchar*)scaledImage.bits();
			memcpy(ptr, src, scaledImage.byteCount());
			
			m_dataPtr = QSharedPointer<uchar>(ptr);
			m_byteCount = scaledImage.byteCount();
			m_imageFormat = scaledImage.format();
			m_imageSize = scaledImage.size();
			
			m_holdTime = m_transmitFps <= 0 ? frame->holdTime() : 1000/m_transmitFps;
			
			#ifdef DEBUG_VIDEOFRAME_POINTERS
			qDebug() << "VideoSender::frameReady(): Mark5: frame:"<<frame;
			#endif
		}
	}
	
	sendUnlock();
	
	if(m_transmitFps <= 0)
		emit receivedFrame();
	
	#ifdef DEBUG_VIDEOFRAME_POINTERS
	qDebug() << "VideoSender::frameReady(): Mark6: m_frame:"<<m_frame;
	#endif
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
		m_sender->sendLock();
		
		QSize originalSize = m_sender->origSize(); //scaledSize.isEmpty() ? (xmitFrame ? xmitFrame->size() : QSize()) : (origFrame ? origFrame->size() : QSize());
		QSharedPointer<uchar> dataPtr = m_sender->dataPtr();
		if(dataPtr)
		{
					
			QTime time = m_sender->captureTime();
			int timestamp = time.hour()   * 60 * 60 * 1000 +
					time.minute() * 60 * 1000      + 
					time.second() * 1000           +
					time.msec();
			
			int byteCount = m_sender->byteCount();
			
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
			
			if(byteCount > 0)
			{
				QSize imageSize = m_sender->imageSize();
				
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
							imageSize.width(), 
							imageSize.height(),
							(int)m_sender->pixelFormat(),
							(int)m_sender->imageFormat(),
							(int)VideoFrame::BUFFER_IMAGE,
							timestamp, 
							m_sender->holdTime(),
							originalSize.width(), 
							originalSize.height());
				//qDebug() << "VideoSenderThread::frameReady: header data:"<<headerData;
				
				m_socket->write((const char*)&headerData,HEADER_SIZE);
				m_socket->write((const char*)dataPtr.data(),byteCount);
			}
	
			m_socket->flush();
			
		}
		
		m_sender->sendUnlock();
	}

}

