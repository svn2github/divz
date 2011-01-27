#include "GLImageDrawable.h"

int GLImageDrawable::m_allocatedMemory = 0;
int GLImageDrawable::m_activeMemory    = 0;

#define IMAGE_ALLOCATION_CAP_MB 1
#define MAX_IMAGE_WIDTH  1600
#define MAX_IMAGE_HEIGHT 1600

#include "../imgtool/exiv2-0.18.2-qtbuild/src/image.hpp"
#include "../3rdparty/md5/qtmd5.h"

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
		qDebug() << "GLImagedDrawable::setImage(): "<<(QObject*)this<<" Allocated memory ("<<(m_allocatedMemory/1024/1024)<<"MB ) exceedes" << IMAGE_ALLOCATION_CAP_MB << "MB cap - delaying load until go-live";
		#endif
		return;
	}
	
	m_releasedImage = false;
	
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
		qDebug() << "GLImagedDrawable::setImage(): "<<(QObject*)this<<" Allocated memory down to:"<<(m_allocatedMemory/1024/1024)<<"MB";
		#endif
	}
		
	QImage localImage = image;
	
	if(1)
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
	//m_image = image;
	
	// explicitly release the original image to see if that helps with memory...
	//image = QImage();
	
 	#ifdef DEBUG_MEMORY_USAGE
 	qDebug() << "GLImagedDrawable::setImage(): "<<(QObject*)this<<" Allocated memory up to:"<<(m_allocatedMemory/1024/1024)<<"MB";
 	#endif
	
	updateTexture();
	
// 	QString file = QString("debug-%1-%2.png").arg(metaObject()->className()).arg(QString().sprintf("%p",((void*)this)));
// 	m_image.save(file);
// 	qDebug() << "QImageDrawable::setImage: "<<(QObject*)this<<": Wrote: "<<file;
	
	if(fpsLimit() <= 0.0)
		updateGL();
		
	//qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" Set image size:"<<m_frame->image().size();
	
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
	//qDebug() << "GLImageDrawable::setImageFile(): "<<(QObject*)this<<" file:"<<file;
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
 		qDebug() << "GLImagedDrawable::setImageFile(): "<<(QObject*)this<<" Allocated memory ("<<(m_allocatedMemory/1024/1024)<<"MB ) exceedes" << IMAGE_ALLOCATION_CAP_MB << "MB cap - delaying load until go-live";
 		#endif
		return true;
	}
	
// 	QString fileMod = fileInfo.lastModified().toString();
// 	if(file == m_imageFile && fileMod == m_fileLastModified)
// 	{
// 		qDebug() << "GLImagedDrawable::setImageFile(): "<<(QObject*)this<<" "<<file<<": no change, not reloading";
// 		return;
// 	}

	//setImage(image);
	setObjectName(fileInfo.fileName());
	
	QSize size = rect().size().toSize();
	
	
	
	QString tempDir = QDir::temp().absolutePath();
	QString glTempDir = QString("%1/glvidtex").arg(tempDir);
	QString imgTempDir = QString("%1/glimagedrawable").arg(glTempDir);
	
	if(!QDir(glTempDir).exists())
		QDir(tempDir).mkdir("glvidtex");
	if(!QDir(imgTempDir).exists())
		QDir(glTempDir).mkdir("glimagedrawable");
	
	QString md5sum = MD5::md5sum(fileInfo.absoluteFilePath());
	QString cachedImageKey = QString("%1/%2.jpg")
		.arg(imgTempDir)
		.arg(md5sum);

	QImage image;
	
	if(QFile(cachedImageKey).exists() && 
	   QFileInfo(file).lastModified() <= QFileInfo(cachedImageKey).lastModified())
	{
		qDebug() << "GLImageDrawable::setImageFile: "<<file<<" - Loaded image from cache: "<<cachedImageKey;
		image = QImage(cachedImageKey);	
	}
	else
	{
		// We only need to cache it if we do something *more* than just load the bits - like rotate or scale it.
		bool cacheNeeded = false;
		
		image = QImage(file);
		if(image.isNull())
		{
			qDebug() << "GLImageDrawable::setImageFile: "<<file<<" - Image loaded is Null!";
			return false;
		}
		
		if(image.width()  > MAX_IMAGE_WIDTH ||
		   image.height() > MAX_IMAGE_HEIGHT)
		{
			image = image.scaled(MAX_IMAGE_WIDTH,MAX_IMAGE_HEIGHT,Qt::KeepAspectRatio);
			
			#ifdef DEBUG_MEMORY_USAGE
			qDebug() << "GLImageDrawable::setImageFile: Scaled image to"<<image.size()<<"with"<<(image.byteCount()/1024/1024)<<"MB memory usage";
			#endif
			
			cacheNeeded = true;
		}
		
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
						//qDebug() << file << ": No Exif data found in the file";
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
						
						cacheNeeded = true;
					}
							
				}
			}
			catch (Exiv2::AnyError& e) 
			{
				std::cout << "Caught Exiv2 exception '" << e << "'\n";
				//return -1;
			}	
		}
		
		// Write out cached image
		if(cacheNeeded)
		{
			qDebug() << "GLImageDrawable::setImageFile: "<<file<<" - Cache needed, loaded original image, writing cache:"<<cachedImageKey;
		
			image.save(cachedImageKey,"JPEG");
		}
	}
	
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
		qDebug() << "GLImageDrawable::reloadImage(): "<<(QObject*)this<<" Reloading image from disk:"<<m_imageFile;
		
		setImageFile(m_imageFile);
	}
// 	else
// 		qDebug() << "GLImageDrawable::reloadImage(): "<<(QObject*)this<<" No image file, unable to reload image.";
}

void GLImageDrawable::releaseImage()
{
	if(!canReleaseImage())
	{
// 		qDebug() << "GLImageDrawable::releaseImage(): "<<(QObject*)this<<" No image file, cannot release image.";
		return;
	}
	m_releasedImage = true;
	if(m_frame)
	{
		m_allocatedMemory -= m_frame->pointerLength();
		//m_image = QImage();
		m_frame = VideoFramePtr(new VideoFrame());
		
		#ifdef DEBUG_MEMORY_USAGE
		qDebug() << "GLImagedDrawable::releaseImage(): "<<(QObject*)this<<" Released memory, allocated down to:"<<(m_allocatedMemory/1024/1024)<<"MB";
		#endif
	}
}

void GLImageDrawable::setLiveStatus(bool flag)
{
	GLVideoDrawable::setLiveStatus(flag);
	if(flag)
	{
		if(m_releasedImage)
			reloadImage();
		
		if(m_frame)
			m_activeMemory += m_frame->pointerLength();;
		
		#ifdef DEBUG_MEMORY_USAGE
		qDebug() << "GLImagedDrawable::setLiveStatus("<<flag<<"): "<<(QObject*)this<<" Active memory usage up to:"<<(m_activeMemory/1024/1024)<<"MB";
		#endif
	}
	else
	{
		if(m_frame)
			m_activeMemory -= m_frame->pointerLength();;
		#ifdef DEBUG_MEMORY_USAGE
		qDebug() << "GLImagedDrawable::setLiveStatus("<<flag<<"): "<<(QObject*)this<<" Active memory usage down to:"<<(m_activeMemory/1024/1024)<<"MB";
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

void GLImageDrawable::setAllowAutoRotate(bool flag)
{
	m_allowAutoRotate = flag;
}
