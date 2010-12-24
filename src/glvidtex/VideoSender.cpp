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
	, m_scaledFrame(0)
	, m_frame(0)
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
	
	VideoFramePtr f = m_source->frame();
	if(!f)
		return;
		
	m_frameMutex.lock();
	
	if(f->isValid())
	{
// 		if(m_frame)
// 		{
// 			#ifdef DEBUG_VIDEOFRAME_POINTERS
// 			qDebug() << "VideoSender::frameReady(): Releasing old m_frame:"<<m_frame;
// 			#endif 
// 			if(m_frame->release())
// 			{
// 				#ifdef DEBUG_VIDEOFRAME_POINTERS
// 				qDebug() << "VideoSender::frameReady(): Deleting old m_frame:"<<m_frame;
// 				#endif
// 				delete m_frame;
// 			}
// 		}

		m_frame = f;
		#ifdef DEBUG_VIDEOFRAME_POINTERS
		qDebug() << "VideoSender::frameReady(): Received new m_frame:"<<m_frame;
		#endif
	}
// 	else
// 	{
// 		#ifdef DEBUG_VIDEOFRAME_POINTERS
// 		qDebug() << "VideoSender::frameReady(): Releasing invalid frame:"<<f;
// 		#endif
// 		if(f->release())
// 		{
// 			#ifdef DEBUG_VIDEOFRAME_POINTERS
// 			qDebug() << "VideoSender::frameReady(): Deleting invalid frame:"<<f;
// 			#endif
// 			delete f;
// 		}
// 	}
// 	
	//QImage image((const uchar*)m_frame->byteArray().constData(),m_frame->size().width(),m_frame->size().height(),QImage::Format_RGB32);
	//emit testImage(image.scaled(160,120), m_frame->captureTime());
	//emit testImage(image, QTime::currentTime()); //m_frame->captureTime());
	
	if(m_frame && m_frame->isValid() && !m_transmitSize.isEmpty())
	{
		#ifdef DEBUG_VIDEOFRAME_POINTERS
		qDebug() << "VideoSender::frameReady(): Mark1: m_frame:"<<m_frame;
		#endif
		
// 		m_frame->incRef();
		
		//qDebug() << "VideoSender::frameReady: Downscaling video for transmission to "<<m_transmitSize;
		// To scale the video frame, first we must convert it to a QImage if its not already an image.
		// If we're lucky, it already is. Otherwise, we have to jump thru hoops to convert the byte 
		// array to a QImage then scale it.
		QImage scaledImage;
		if(!m_frame->image().isNull())
		{
			scaledImage = m_frame->image().scaled(m_transmitSize);
		}
		else
		{
			#ifdef DEBUG_VIDEOFRAME_POINTERS
			qDebug() << "VideoSender::frameReady(): Scaling data from frame:"<<m_frame<<", pointer:"<<m_frame->pointer();
			#endif
			const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(m_frame->pixelFormat());
			if(imageFormat != QImage::Format_Invalid)
			{
				QImage image(m_frame->pointer(),
					m_frame->size().width(),
					m_frame->size().height(),
					m_frame->size().width() *
						(imageFormat == QImage::Format_RGB16  ||
						imageFormat == QImage::Format_RGB555 ||
						imageFormat == QImage::Format_RGB444 ||
						imageFormat == QImage::Format_ARGB4444_Premultiplied ? 2 :
						imageFormat == QImage::Format_RGB888 ||
						imageFormat == QImage::Format_RGB666 ||
						imageFormat == QImage::Format_ARGB6666_Premultiplied ? 3 :
						4),
					imageFormat);
					
				//scaledImage = image.isNull() ? image : image.scaled(m_transmitSize);
				scaledImage = image.scaled(m_transmitSize);
				//qDebug() << "Downscaled image from "<<image.byteCount()<<"bytes to "<<scaledImage.byteCount()<<"bytes, orig ptr len:"<<m_frame->pointerLength()<<", orig ptr:"<<m_frame->pointer();
			}
			else
			{
				qDebug() << "VideoSender::frameReady: Unable to convert pixel format to image format, cannot scale frame. Pixel Format:"<<m_frame->pixelFormat();
			}
		}
		
		#ifdef DEBUG_VIDEOFRAME_POINTERS
		qDebug() << "VideoSender::frameReady(): Mark2: m_frame:"<<m_frame;
		#endif
		
		// Now that we've got the image out of the original frame and scaled it, we have to construct a new
		// video frame to transmit on the wire from the scaledImage (assuming the sccaledImage is valid.)
		// We attempt to transmit in its native format without converting it if we can to save local CPU power.
		if(!scaledImage.isNull())
// 		{
// 			m_scaledFrame = m_frame;
// 		}
// 		else
		{
// 			if(m_scaledFrame)
// 			{
// 				#ifdef DEBUG_VIDEOFRAME_POINTERS
// 				qDebug() << "VideoSender::frameReady(): Releasing old m_scaledFrame:"<<m_scaledFrame;
// 				#endif
// 				if(m_scaledFrame->release())
// 				{
// 					#ifdef DEBUG_VIDEOFRAME_POINTERS
// 					qDebug() << "VideoSender::frameReady(): Deleting old m_scaledFrame:"<<m_scaledFrame;
// 					#endif 
// 					delete m_scaledFrame;
// 				}
// 			}
				
			#ifdef DEBUG_VIDEOFRAME_POINTERS
			qDebug() << "VideoSender::frameReady(): Mark3: m_frame:"<<m_frame;
			#endif
			
			m_scaledFrame = VideoFramePtr(new VideoFrame());
			
			#ifdef DEBUG_VIDEOFRAME_POINTERS
			qDebug() << "VideoSender::frameReady(): Creating new scaled frame:"<<m_scaledFrame<<", calling incRef()";
			#endif
			
			m_scaledFrame->incRef();
			
			#ifdef DEBUG_VIDEOFRAME_POINTERS
			qDebug() << "VideoSender::frameReady(): Mark4: m_frame:"<<m_frame;
			#endif
			
			//qDebug() << "VideoSender::frameReady: Allocated new scaledFrame:"<<m_scaledFrame;
			m_scaledFrame->setBufferType(VideoFrame::BUFFER_IMAGE);
			//m_scaledFrame->setBufferType(VideoFrame::BUFFER_POINTER);
			
			#ifdef DEBUG_VIDEOFRAME_POINTERS
			qDebug() << "VideoSender::frameReady(): Creating new scaled frame:"<<m_scaledFrame<<" from old m_frame:"<<m_frame;
			#endif
			
			m_scaledFrame->setCaptureTime(m_frame->captureTime());

			QImage::Format format = scaledImage.format();
			m_scaledFrame->setPixelFormat(
				format == QImage::Format_ARGB32 ? QVideoFrame::Format_ARGB32 :
				format == QImage::Format_RGB32  ? QVideoFrame::Format_RGB32  :
				format == QImage::Format_RGB888 ? QVideoFrame::Format_RGB24  :
				format == QImage::Format_RGB16  ? QVideoFrame::Format_RGB565 :
				format == QImage::Format_RGB555 ? QVideoFrame::Format_RGB555 :
				//format == QImage::Format_ARGB32_Premultiplied ? QVideoFrame::Format_ARGB32_Premultiplied :
				// GLVideoDrawable doesn't support premultiplied - so the format conversion below will convert it to ARGB32 automatically
				QVideoFrame::Format_Invalid);
				
			if(m_scaledFrame->pixelFormat() == QVideoFrame::Format_Invalid)
			{
				qDebug() << "VideoFrame: image was not in an acceptable format, converting to ARGB32 automatically.";
				scaledImage = scaledImage.convertToFormat(QImage::Format_ARGB32);
				m_scaledFrame->setPixelFormat(QVideoFrame::Format_ARGB32);
			}
			
			//scaledImage.scanLine(0);
			m_scaledFrame->setImage(scaledImage);
			/*uchar *ptr = m_scaledFrame->allocPointer(scaledImage.byteCount());
			const uchar *src = (const uchar*)scaledImage.bits();
			memcpy(ptr, src, scaledImage.byteCount());
			qDebug() << "VideoSender::frameReady(): m_scaledFrame:"<<m_scaledFrame<<", Copied"<<scaledImage.byteCount()<<"bytes from img ptr:"<<src<<" into frame pointer:"<<ptr;*/  
			
			m_scaledFrame->setSize(scaledImage.size());
			m_scaledFrame->setHoldTime(m_transmitFps <= 0 ? m_frame->holdTime() : 1000/m_transmitFps);
			
			#ifdef DEBUG_VIDEOFRAME_POINTERS
			qDebug() << "VideoSender::frameReady(): Mark5: m_frame:"<<m_frame;
			#endif
		}
		
// 		if(m_frame->release())
// 			delete m_frame;
	}
	
	m_frameMutex.unlock();
	
	if(m_transmitFps <= 0)
		emit receivedFrame();
	else
	if(m_frame)
		m_frame->setHoldTime(1000/m_transmitFps);
	
	#ifdef DEBUG_VIDEOFRAME_POINTERS
	qDebug() << "VideoSender::frameReady(): Mark6: m_frame:"<<m_frame;
	#endif
}

VideoFramePtr VideoSender::frame() 
{ 
// 	QMutexLocker lock(&m_frameMutex);
// 	if(m_frame)
// 	{
// 		#ifdef DEBUG_VIDEOFRAME_POINTERS
// 		qDebug() << "VideoSender::frame(): Calling incRef() on m_frame:"<<m_frame;
// 		#endif
// 		
// 		m_frame->incRef(); 
// 	}
	return m_frame; 
}
VideoFramePtr VideoSender::scaledFrame()
{
// 	QMutexLocker lock(&m_frameMutex);
// 	//qDebug() << "VideoSender::scaledFrame: m_scaledFrame:"<<m_scaledFrame; 
// 	if(m_scaledFrame)
// 	{
// 		#ifdef DEBUG_VIDEOFRAME_POINTERS
// 		qDebug() << "VideoSender::scaledFrame(): Calling incRef() on m_scaledFrame:"<<m_scaledFrame;
// 		#endif
// 		
// 		m_scaledFrame->incRef(); 
// 	}
	return m_scaledFrame; 
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
		QSize scaledSize   = m_sender->transmitSize();
		VideoFramePtr origFrame   = m_sender->frame();
		VideoFramePtr scaledFrame = m_sender->scaledFrame();
		VideoFramePtr xmitFrame   = scaledSize.isEmpty() ? origFrame : scaledFrame;
		QSize originalSize = scaledSize.isEmpty() ? (xmitFrame ? xmitFrame->size() : QSize()) : (origFrame ? origFrame->size() : QSize());
// 		if(!scaledSize.isEmpty() && origFrame->release())
// 			delete origFrame;
		if(xmitFrame)
		{
			
			QTime time = xmitFrame->captureTime();
			int timestamp = time.hour()   * 60 * 60 * 1000 +
					time.minute() * 60 * 1000      + 
					time.second() * 1000           +
					time.msec();
			
			int byteCount = 
				xmitFrame->bufferType() == VideoFrame::BUFFER_IMAGE ? 
					xmitFrame->image().byteCount() : 
					xmitFrame->pointerLength();
			
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
							xmitFrame->size().width(), xmitFrame->size().height(),
							(int)xmitFrame->pixelFormat(),(int)xmitFrame->image().format(),
							(int)xmitFrame->bufferType(),
							timestamp,xmitFrame->holdTime(),
							originalSize.width(), originalSize.height());
				//qDebug() << "VideoSenderThread::frameReady: header data:"<<headerData;
				
				m_socket->write((const char*)&headerData,HEADER_SIZE);
				
				if(xmitFrame->bufferType() == VideoFrame::BUFFER_IMAGE)
				{
					QImage copy = xmitFrame->image().copy();
					//copy.scanLine(0);
					const uchar *bits = (const uchar*)copy.bits();
					m_socket->write((const char*)bits,byteCount);
				}
				else
				if(xmitFrame->bufferType() == VideoFrame::BUFFER_POINTER)
				{
					//xmitFrame->incRef();
					m_socket->write((const char*)xmitFrame->pointer(),byteCount);
					//xmitFrame->release();
					//if(xmitFrame->release())
					//	delete xmitFrame;
				}
			}
	
			m_socket->flush();
			
			//if(xmitFrame->release())
			//	delete xmitFrame;
			
	// 		if(xmitFrame->release())
	// 			delete xmitFrame;
		}

//  		if(origFrame)
//  		{
//  			#ifdef DEBUG_VIDEOFRAME_POINTERS
//  			qDebug() << "VideoSenderThread::frameReady(): Releasing origFrame:"<<origFrame;
//  			#endif
//  			
//  			if(origFrame->release())
// 			{
// 				#ifdef DEBUG_VIDEOFRAME_POINTERS
// 				qDebug() << "VideoSenderThread::frameReady(): Deleting origFrame:"<<origFrame;
// 				#endif
// 				
// 				delete origFrame;
// 			}
// 		}
//  			
//  		if(scaledFrame)
//  		{
//  			#ifdef DEBUG_VIDEOFRAME_POINTERS
//  			qDebug() << "VideoSenderThread::frameReady(): Releasing scaledFrame:"<<scaledFrame;
//  			#endif
//  			
//  			if(scaledFrame->release())
// 			{
// 				#ifdef DEBUG_VIDEOFRAME_POINTERS
// 				qDebug() << "VideoSenderThread::frameReady(): Deleting scaledFrame:"<<scaledFrame;
// 				#endif
// 				 
// 				delete scaledFrame;
// 			}
// 		}

		//QTime time2 = QTime::currentTime();
		//int timestamp2 = time.hour() + time.minute() + time.second() + time.msec();
		//qDebug() << "write time: "<<(timestamp2-timestamp);
	}

}

