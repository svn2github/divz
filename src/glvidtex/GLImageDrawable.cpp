#include "GLImageDrawable.h"

int GLImageDrawable::m_allocatedMemory = 0;
int GLImageDrawable::m_activeMemory    = 0;

#define IMAGE_ALLOCATION_CAP_MB 128
#define MAX_IMAGE_WIDTH 2000
#define MAX_IMAGE_HEIGHT 2000

GLImageDrawable::GLImageDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
	, m_releasedImage(false)
{
	//setImage(QImage("dot.gif"));
	setCrossFadeMode(GLVideoDrawable::FrontAndBack);
	
	if(!file.isEmpty())
		setImageFile(file);
}

GLImageDrawable::~GLImageDrawable()
{}

void GLImageDrawable::setImage(const QImage& image)
{
// 	if(m_allocatedMemory > IMAGE_ALLOCATION_CAP_MB*1024*1024 && 
// 		!glWidget() && 
// 		!scene() && 
// 		canReleaseImage())
// 	{
// 		m_releasedImage = true;
// 		qDebug() << "GLImagedDrawable::setImage(): Allocated memory ("<<(m_allocatedMemory/1024/1024)<<"MB ) exceedes" << IMAGE_ALLOCATION_CAP_MB << "MB cap - delaying load until go-live";
// 		return;
// 	}
	
	//qDebug() << "GLImageDrawable::setImage: Size:"<<image.size();
	//QImage image = makeHistogram(tmp);
	//if(m_frame && m_frame->isValid() && ())
	QImage localImage = image;
	if(image.width() > MAX_IMAGE_WIDTH || image.height() > MAX_IMAGE_HEIGHT)
	{
		localImage = image.scaled(MAX_IMAGE_WIDTH,MAX_IMAGE_HEIGHT,Qt::KeepAspectRatio);
		qDebug() << "GLImageDrawable::setImage: Scaled image to"<<localImage.size()<<"with"<<(localImage.byteCount()/1024/1024)<<"MB memory usage";
	}
	
	if(!m_image.isNull() && xfadeEnabled())
	{
 		m_frame2 = m_frame;
		//m_frame2 = VideoFramePtr(new VideoFrame(m_image,1000/30));
		updateTexture(true); // true = read from m_frame2
		xfadeStart();
		
		// Take the memory off the list because when crossfade is done, the frame should get freed
		m_allocatedMemory -= m_frame->pointerLength();
	}
		
				
	//m_frame = VideoFramePtr(new VideoFrame(localImage, 1000/30));
	m_frame = VideoFramePtr(new VideoFrame());
	//m_frame->setPixelFormat(QVideoFrame::Format_RGB32);
	//m_frame->setCaptureTime(QTime::currentTime());
	m_frame->setIsRaw(true);
	m_frame->setBufferType(VideoFrame::BUFFER_POINTER);
	m_frame->setHoldTime(1000/30);
	m_frame->setSize(localImage.size());
	
	/*	
	// Setup frame
	m_frame->setBufferType(VideoFrame::BUFFER_IMAGE);
	*/
	QImage::Format format = localImage.format();
	m_frame->setPixelFormat(
		format == QImage::Format_ARGB32 ? QVideoFrame::Format_ARGB32 :
		format == QImage::Format_RGB32  ? QVideoFrame::Format_RGB32  :
		format == QImage::Format_RGB888 ? QVideoFrame::Format_RGB24  :
		format == QImage::Format_RGB16  ? QVideoFrame::Format_RGB565 :
		format == QImage::Format_RGB555 ? QVideoFrame::Format_RGB555 :
		//format == QImage::Format_ARGB32_Premultiplied ? QVideoFrame::Format_ARGB32_Premultiplied :
		// GLVideoDrawable doesn't support premultiplied - so the format conversion below will convert it to ARGB32 automatically
		QVideoFrame::Format_Invalid);
		
	if(m_frame->pixelFormat() == QVideoFrame::Format_Invalid)
	{
		qDebug() << "VideoFrame: image was not in an acceptable format, converting to ARGB32 automatically.";
		m_image = localImage.convertToFormat(QImage::Format_ARGB32);
		m_frame->setPixelFormat(QVideoFrame::Format_ARGB32);
	}
	else
	{
		m_image = image;
	}
	
	/*
	m_frame->setImage(m_image);
	m_frame->setSize(localImage.size());*/
	memcpy(m_frame->allocPointer(localImage.byteCount()), (const uchar*)localImage.bits(), localImage.byteCount());
	m_allocatedMemory += localImage.byteCount();
// 	qDebug() << "GLImagedDrawable::setImage(): Allocated memory up to:"<<(m_allocatedMemory/1024/1024)<<"MB";
	
	updateTexture();
	
// 	QString file = QString("debug-%1-%2.png").arg(metaObject()->className()).arg(QString().sprintf("%p",((void*)this)));
// 	m_image.save(file);
// 	qDebug() << "QImageDrawable::setImage: "<<(QObject*)this<<": Wrote: "<<file;
	
	if(fpsLimit() <= 0.0)
		updateGL();
		
	//qDebug() << "GLImageDrawable::setImage(): Set image size:"<<m_frame->image().size();
	
	// TODO reimp so this code works
// 	if(m_visiblePendingFrame)
// 	{
// 		//qDebug() << "GLVideoDrawable::frameReady: "<<this<<", pending visible set, calling setVisible("<<m_tempVisibleValue<<")";
// 		m_visiblePendingFrame = false;
// 		GLDrawable::setVisible(m_tempVisibleValue);
// 	}
}

bool GLImageDrawable::setImageFile(const QString& file)
{
	//qDebug() << "GLImageDrawable::setImageFile(): file:"<<file;
	if(file.isEmpty())
		return false;
	
	QFileInfo fileInfo(file);
	if(!fileInfo.exists())
	{
		qDebug() << "GLImageDrawable::setImageFile: "<<file<<" does not exist!";
		return false;
	}
	internalSetFilename(file);
	
	if(m_allocatedMemory > IMAGE_ALLOCATION_CAP_MB*1024*1024 && 
		!glWidget() && 
		!scene() && 
		canReleaseImage())
	{
		m_releasedImage = true;
 		qDebug() << "GLImagedDrawable::setImageFile(): Allocated memory ("<<(m_allocatedMemory/1024/1024)<<"MB ) exceedes" << IMAGE_ALLOCATION_CAP_MB << "MB cap - delaying load until go-live";
		return true;
	}
	
	
	QImage image(file);
	if(image.isNull())
	{
		qDebug() << "GLImageDrawable::setImageFile: "<<file<<" - Image loaded is Null!";
		return false;
	}
	setImage(image);
	setObjectName(fileInfo.fileName());
	
	return true;
	
}

void GLImageDrawable::internalSetFilename(QString file)
{
	m_imageFile = file;
	emit imageFileChanged(file);
}

void GLImageDrawable::setVideoSource(VideoSource*)
{
	// Hide access to this method by making it private and reimpl to do nothing
}

void GLImageDrawable::reloadImage()
{
	if(!m_imageFile.isEmpty())
	{
		//qDebug() << "GLImageDrawable::reloadImage(): Reloading image from disk:"<<m_imageFile;
		setImageFile(m_imageFile);
	}
// 	else
// 		qDebug() << "GLImageDrawable::reloadImage(): No image file, unable to reload image.";
}

void GLImageDrawable::releaseImage()
{
	if(!canReleaseImage())
	{
// 		qDebug() << "GLImageDrawable::releaseImage(): No image file, cannot release image.";
		return;
	}
	m_releasedImage = true;
	if(m_frame)
		m_allocatedMemory -= m_frame->pointerLength();
	m_image = QImage();
	m_frame = VideoFramePtr(new VideoFrame());
	//qDebug() << "GLImagedDrawable::releaseImage(): Released memory, allocated down to:"<<(m_allocatedMemory/1024/1024)<<"MB";
}

void GLImageDrawable::setGLWidget(GLWidget* widget)
{
	if(widget)
	{
		if(m_releasedImage)
			reloadImage();
		
		if(m_frame)
			m_activeMemory += m_frame->pointerLength();;
		//qDebug() << "GLImagedDrawable::setGLWidget(): Active memory usage up to:"<<(m_activeMemory/1024/1024)<<"MB";
		
		GLDrawable::setGLWidget(widget);
	}
	else
	{
		GLDrawable::setGLWidget(widget);
	
		if(m_frame)
			m_activeMemory -= m_frame->pointerLength();;
		//qDebug() << "GLImagedDrawable::setGLWidget(): Active memory usage down to:"<<(m_activeMemory/1024/1024)<<"MB";
		if(canReleaseImage() &&
		   m_allocatedMemory > IMAGE_ALLOCATION_CAP_MB*1024*1024) // 10 MB
			releaseImage();
	}
}

bool GLImageDrawable::canReleaseImage()
{
	return !m_imageFile.isEmpty();
}
