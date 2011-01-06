#include "GLImageDrawable.h"

int GLImageDrawable::m_allocatedMemory = 0;
int GLImageDrawable::m_activeMemory    = 0;

#define IMAGE_ALLOCATION_CAP_MB 1
#define MAX_IMAGE_WIDTH  1600
#define MAX_IMAGE_HEIGHT 1600

#include "../imgtool/exiv2-0.18.2-qtbuild/src/image.hpp"

//#define DEBUG_MEMORY_USAGE

GLImageDrawable::GLImageDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
	, m_releasedImage(false)
	, m_allowAutoRotate(true)
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
	if(m_allocatedMemory > IMAGE_ALLOCATION_CAP_MB*1024*1024 && 
		!glWidget() && 
		!scene() && 
		canReleaseImage())
	{
		m_releasedImage = true;
		
		#ifdef DEBUG_MEMORY_USAGE
		qDebug() << "GLImagedDrawable::setImage(): Allocated memory ("<<(m_allocatedMemory/1024/1024)<<"MB ) exceedes" << IMAGE_ALLOCATION_CAP_MB << "MB cap - delaying load until go-live";
		#endif
		return;
	}
	
	m_releasedImage = false;
	
	//qDebug() << "GLImageDrawable::setImage: Size:"<<image.size();
	//QImage image = makeHistogram(tmp);
	//if(m_frame && m_frame->isValid() && ())
	QImage localImage;
	if(image.width()  > MAX_IMAGE_WIDTH || 
	   image.height() > MAX_IMAGE_HEIGHT)
	{
		localImage = image.scaled(MAX_IMAGE_WIDTH,MAX_IMAGE_HEIGHT,Qt::KeepAspectRatio);
		#ifdef DEBUG_MEMORY_USAGE
		qDebug() << "GLImageDrawable::setImage: Scaled image to"<<localImage.size()<<"with"<<(localImage.byteCount()/1024/1024)<<"MB memory usage";
		#endif
	}
	else
	{
		localImage = image.copy();
	}
	
	//if(!m_image.isNull() && xfadeEnabled())
	if(m_frame && 
	   m_frame->isValid() &&
	   xfadeEnabled())
	{
 		m_frame2 = m_frame;
		//m_frame2 = VideoFramePtr(new VideoFrame(m_image,1000/30));
		updateTexture(true); // true = read from m_frame2
		xfadeStart();
	}
	
	// Take the memory off the list because when crossfade is done, the frame should get freed
	if(m_frame)
	{
		m_allocatedMemory -= m_frame->pointerLength();
		#ifdef DEBUG_MEMORY_USAGE
		qDebug() << "GLImagedDrawable::setImage(): Allocated memory down to:"<<(m_allocatedMemory/1024/1024)<<"MB";
		#endif
	}
		
	if(0)
	{
		m_frame = VideoFramePtr(new VideoFrame(localImage, 1000/30));
	}
	else
	{
		
		m_frame = VideoFramePtr(new VideoFrame());
		//m_frame->setPixelFormat(QVideoFrame::Format_RGB32);
		//m_frame->setCaptureTime(QTime::currentTime());
		m_frame->setIsRaw(true);
		m_frame->setBufferType(VideoFrame::BUFFER_POINTER);
		m_frame->setHoldTime(1000/30);
		m_frame->setSize(localImage.size());
		//m_frame->setDebugPtr(true);
		
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
			localImage = localImage.convertToFormat(QImage::Format_ARGB32);
			m_frame->setPixelFormat(QVideoFrame::Format_ARGB32);
		}
		
		memcpy(m_frame->allocPointer(localImage.byteCount()), (const uchar*)localImage.bits(), localImage.byteCount());
	}
		
	m_allocatedMemory += localImage.byteCount();
	//m_image = localImage;
	
	// explicitly release the original image to see if that helps with memory...
	localImage = QImage();
	
 	#ifdef DEBUG_MEMORY_USAGE
 	qDebug() << "GLImagedDrawable::setImage(): Allocated memory up to:"<<(m_allocatedMemory/1024/1024)<<"MB";
 	#endif
	
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
 		#ifdef DEBUG_MEMORY_USAGE
 		qDebug() << "GLImagedDrawable::setImageFile(): Allocated memory ("<<(m_allocatedMemory/1024/1024)<<"MB ) exceedes" << IMAGE_ALLOCATION_CAP_MB << "MB cap - delaying load until go-live";
 		#endif
		return true;
	}
	
	
	QImage image(file);
	if(image.isNull())
	{
		qDebug() << "GLImageDrawable::setImageFile: "<<file<<" - Image loaded is Null!";
		return false;
	}
	//setImage(image);
	setObjectName(fileInfo.fileName());
	
	bool imageSet = false;
	if(m_allowAutoRotate)
	{
		try
		{
			Exiv2::Image::AutoPtr exiv = Exiv2::ImageFactory::open(file.toStdString()); 
			if(exiv.get() != 0)
			{
				exiv->readMetadata();
				Exiv2::ExifData& exifData = exiv->exifData();
				if (exifData.empty()) 
				{
					qDebug() << file << ": No Exif data found in the file";
				}

				QString rotateSensor = exifData["Exif.Image.Orientation"].toString().c_str();
				int rotationFlag = rotateSensor.toInt(); 
				int rotateDegrees = rotationFlag == 1 ||
						    rotationFlag == 2 ? 0 :
						    rotationFlag == 7 ||
						    rotationFlag == 8 ? -90 :
						    rotationFlag == 3 ||
						    rotationFlag == 4 ? -180 :
						    rotationFlag == 5 ||
						    rotationFlag == 6 ? -270 :
						    0;
				
				if(rotateDegrees != 0)
				{
					qDebug() << "GLImageDrawable::setImageFile: "<<file<<" - Rotating "<<rotateDegrees<<" degrees";
					 
					QTransform t = QTransform().rotate(rotateDegrees);
					image = image.transformed(t);
					
					setImage(image);
					
					imageSet = true;
				}
						
			}
		}
		catch (Exiv2::AnyError& e) 
		{
			std::cout << "Caught Exiv2 exception '" << e << "'\n";
			//return -1;
		}	
	}
	
	if(!imageSet)
		setImage(image);
	
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
		qDebug() << "GLImageDrawable::reloadImage(): Reloading image from disk:"<<m_imageFile;
		
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
	{
		m_allocatedMemory -= m_frame->pointerLength();
		//m_image = QImage();
		m_frame = VideoFramePtr(new VideoFrame());
		
		#ifdef DEBUG_MEMORY_USAGE
		qDebug() << "GLImagedDrawable::releaseImage(): Released memory, allocated down to:"<<(m_allocatedMemory/1024/1024)<<"MB";
		#endif
	}
}

void GLImageDrawable::setGLWidget(GLWidget* widget)
{
	if(widget)
	{
		if(m_releasedImage)
			reloadImage();
		
		if(m_frame)
			m_activeMemory += m_frame->pointerLength();;
		
		#ifdef DEBUG_MEMORY_USAGE
		qDebug() << "GLImagedDrawable::setGLWidget(): Active memory usage up to:"<<(m_activeMemory/1024/1024)<<"MB";
		#endif
		
		GLDrawable::setGLWidget(widget);
	}
	else
	{
		GLDrawable::setGLWidget(widget);
	
		if(m_frame)
			m_activeMemory -= m_frame->pointerLength();;
		#ifdef DEBUG_MEMORY_USAGE
		qDebug() << "GLImagedDrawable::setGLWidget(): Active memory usage down to:"<<(m_activeMemory/1024/1024)<<"MB";
		#endif
		if(canReleaseImage() &&
		   m_allocatedMemory > IMAGE_ALLOCATION_CAP_MB*1024*1024) 
			releaseImage();
	}
}

bool GLImageDrawable::canReleaseImage()
{
	return !m_imageFile.isEmpty();
}


QVariant GLImageDrawable::itemChange(GraphicsItemChange change, const QVariant & value)
{
	if(change == ItemSceneChange)
	{
		QGraphicsScene *scene = value.value<QGraphicsScene*>();
		//qDebug() << "GLImageDrawable::itemChange(): value:"<<value<<", scene:"<<scene;
		if(!scene)
		{
			if(m_frame)
			{
				m_activeMemory -= m_frame->pointerLength();;
				#ifdef DEBUG_MEMORY_USAGE
				qDebug() << "GLImagedDrawable::itemChange(): Active memory usage down to:"<<(m_activeMemory/1024/1024)<<"MB";
				#endif
				if(canReleaseImage() &&
				m_allocatedMemory > IMAGE_ALLOCATION_CAP_MB*1024*1024)
					releaseImage();
			}
		}
	}

	return GLDrawable::itemChange(change, value);
}

void GLImageDrawable::aboutToPaint()
{
	if(m_releasedImage)
	{
		reloadImage();
	
		if(m_frame)
			m_activeMemory += m_frame->pointerLength();
	}
	
	#ifdef DEBUG_MEMORY_USAGE
	qDebug() << "GLImagedDrawable::aboutToPaint(): Active memory usage up to:"<<(m_activeMemory/1024/1024)<<"MB";
	#endif
}

void GLImageDrawable::setAllowAutoRotate(bool flag)
{
	m_allowAutoRotate = flag;
}
