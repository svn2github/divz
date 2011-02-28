#include "GLImageDrawable.h"

int GLImageDrawable::m_allocatedMemory = 0;
int GLImageDrawable::m_activeMemory    = 0;

#define IMAGE_ALLOCATION_CAP_MB 1
#define MAX_IMAGE_WIDTH  1600
#define MAX_IMAGE_HEIGHT 1600

#include "../imgtool/exiv2-0.18.2-qtbuild/src/image.hpp"
#include "../3rdparty/md5/qtmd5.h"
#include "../ImageFilters.h"

//#define DEBUG_MEMORY_USAGE

GLImageDrawable::GLImageDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
	, m_releasedImage(false)
	, m_allowAutoRotate(true)
	// Border attributes
	, m_borderColor(Qt::black)
	, m_borderWidth(0.0) // start off disabled
	// Shadow attributes
	, m_shadowEnabled(false)
	, m_shadowBlurRadius(11.0)
	, m_shadowOffset(3.0,3.0) 
	, m_shadowColor(Qt::black)
	, m_shadowOpacity(1.0)
{
	setImage(QImage("dot.gif"));
	setCrossFadeMode(GLVideoDrawable::FrontAndBack);

	if(!file.isEmpty())
		setImageFile(file);

	connect(&m_dirtyBatchTimer, SIGNAL(timeout()), this, SLOT(reapplyBorderAndShadow()));
	m_dirtyBatchTimer.setSingleShot(true);
	m_dirtyBatchTimer.setInterval(50);
}

GLImageDrawable::~GLImageDrawable()
{}

void GLImageDrawable::setImage(const QImage& image, bool insidePaint)
{
	//qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" mark1: insidePaint:"<<insidePaint;
	if(m_allocatedMemory > IMAGE_ALLOCATION_CAP_MB*1024*1024 &&
		!liveStatus() &&
		canReleaseImage())
	{
		m_releasedImage = true;
		//qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" NOT LOADING";

		#ifdef DEBUG_MEMORY_USAGE
		qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" Allocated memory ("<<(m_allocatedMemory/1024/1024)<<"MB ) exceedes" << IMAGE_ALLOCATION_CAP_MB << "MB cap - delaying load until go-live";
		#endif
		return;
	}

	m_releasedImage = false;
	//qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" mark2";
	//image.save("whitedebug.png");


	if(m_frame &&
	   m_frame->isValid() &&
	   xfadeEnabled() &&
	   !insidePaint)
	{
 		m_frame2 = m_frame;
 		//qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" Starting crossfade with m_frame2";
		//m_frame2 = VideoFramePtr(new VideoFrame(m_image,1000/30));
		updateTexture(true); // true = read from m_frame2
		xfadeStart();
	}


	// Take the memory off the list because when crossfade is done, the frame should get freed
	if(m_frame)
	{
		m_allocatedMemory -= m_frame->pointerLength();
		#ifdef DEBUG_MEMORY_USAGE
		qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" Allocated memory down to:"<<(m_allocatedMemory/1024/1024)<<"MB";
		#endif
		//qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" mark4";
	}

	QImage localImage;
	if(m_shadowEnabled || m_borderWidth > 0.001)
		localImage = applyBorderAndShadow(image);
	else
		localImage = image;
		
	//qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" mark5";

	if(1)
	{
		//qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" Setting new m_frame";
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
			qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<": image was not in an acceptable format, converting to ARGB32 automatically.";
			localImage = localImage.convertToFormat(QImage::Format_ARGB32);
			m_frame->setPixelFormat(QVideoFrame::Format_ARGB32);
		}

		memcpy(m_frame->allocPointer(localImage.byteCount()), (const uchar*)localImage.bits(), localImage.byteCount());
	}

	m_allocatedMemory += localImage.byteCount();
	m_image = image;

	// explicitly release the original image to see if that helps with memory...
	//image = QImage();

 	#ifdef DEBUG_MEMORY_USAGE
 	qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" Allocated memory up to:"<<(m_allocatedMemory/1024/1024)<<"MB";
 	#endif
	//qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" mark7";
	updateTexture();

// 	QString file = QString("debug-%1-%2.png").arg(metaObject()->className()).arg(QString().sprintf("%p",((void*)this)));
// 	m_image.save(file);
// 	qDebug() << "QImageDrawable::setImage: "<<(QObject*)this<<": Wrote: "<<file;

	if(fpsLimit() <= 0.0 &&
	   !insidePaint)
	{
		//qDebug() << "GLImageDrawable::setImage(): "<<(QObject*)this<<" mark8";
		updateGL();
		if(!liveStatus())
			m_needUpdate = true;
	}

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
	{
		internalSetFilename(file);
		return false;
	}
	
	
	QFileInfo fileInfo(file);
	if(!fileInfo.exists())
	{
		if(!m_cachedImageFilename.isEmpty() && 
		    m_cachedImageFilename == file)
		{
			qDebug() << "GLImageDrawable::setImageFile: "<<file<<" does not exist, used cached image packed in drawable";
			internalSetFilename(file);
			setImage(m_cachedImage);
			return true;
		}
		else
		{
			qDebug() << "GLImageDrawable::setImageFile: "<<file<<" does not exist!";
			return false;
		}
	}
	
	internalSetFilename(file);

	if(m_allocatedMemory > IMAGE_ALLOCATION_CAP_MB*1024*1024 &&
		!liveStatus() &&
		canReleaseImage())
	{
		m_releasedImage = true;
 		#ifdef DEBUG_MEMORY_USAGE
 		qDebug() << "GLImageDrawable::setImageFile(): "<<(QObject*)this<<" Allocated memory ("<<(m_allocatedMemory/1024/1024)<<"MB ) exceedes" << IMAGE_ALLOCATION_CAP_MB << "MB cap - delaying load until go-live";
 		#endif
		return true;
	}

// 	QString fileMod = fileInfo.lastModified().toString();
// 	if(file == m_imageFile && fileMod == m_fileLastModified)
// 	{
// 		qDebug() << "GLImageDrawable::setImageFile(): "<<(QObject*)this<<" "<<file<<": no change, not reloading";
// 		return;
// 	}

	//setImage(image);
	
	if(!m_cachedImageFilename.isEmpty() &&
	    QFileInfo(file).lastModified() <= m_cachedImageMtime)
	{
		qDebug() << "GLImageDrawable::setImageFile: "<<file<<" - Loaded image from bytes packed in drawable";
		setImage(m_cachedImage);
		return true;
	}
	
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
	setObjectName(QFileInfo(file).fileName());
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
		//qDebug() << "GLImageDrawable::reloadImage(): "<<(QObject*)this<<" Reloading image from disk:"<<m_imageFile;

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
		qDebug() << "GLImageDrawable::releaseImage(): "<<(QObject*)this<<" Released memory, allocated down to:"<<(m_allocatedMemory/1024/1024)<<"MB";
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
		qDebug() << "GLImageDrawable::setLiveStatus("<<flag<<"): "<<(QObject*)this<<" Active memory usage up to:"<<(m_activeMemory/1024/1024)<<"MB";
		#endif

		if(m_needUpdate)
		{
			m_needUpdate = false;
 			//updateTexture();
 			//updateGL();
		}
	}
	else
	{
		if(m_frame)
			m_activeMemory -= m_frame->pointerLength();;
		#ifdef DEBUG_MEMORY_USAGE
		qDebug() << "GLImageDrawable::setLiveStatus("<<flag<<"): "<<(QObject*)this<<" Active memory usage down to:"<<(m_activeMemory/1024/1024)<<"MB";
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


void GLImageDrawable::loadPropsFromMap(const QVariantMap& map, bool /*onlyApplyIfChanged*/)
{

	m_cachedImageFilename = map["cached_image_filename"].toString();
	
	if(!m_cachedImageFilename.isEmpty())
	{
		QTime t;
		t.start();
			
		m_cachedImageBytes = map["cached_image_bytes"].toByteArray();
		if(!m_cachedImageBytes.isEmpty())
		{
			QImage image;
			image.loadFromData(m_cachedImageBytes);
			
			if(!image.isNull())
			{
				m_cachedImage = image;
				m_cachedImageMtime = map["cached_image_mtime"].toDateTime();
				
				qDebug() << "GLImageDrawable::loadPropsFromMap: Unpacked "<<m_cachedImageBytes.size()/1024<<" Kb from map for cached image "<<m_cachedImageFilename<<" in "<<t.elapsed()<<"ms";
			}
			else
			{
				qDebug() << "GLImageDrawable::loadPropsFromMap: Unpacked "<<m_cachedImageBytes.size()/1024<<" Kb from map for cached image "<<m_cachedImageFilename<<" in "<<t.elapsed()<<"ms - NULL IMAGE";
				m_cachedImageFilename = "";
			}
		}
		else
		{
			qDebug() << "GLImageDrawable::loadPropsFromMap: Unpacked "<<m_cachedImageBytes.size()/1024<<" Kb from map for cached image "<<m_cachedImageFilename<<" in "<<t.elapsed()<<"ms - NO DATA STORED";
			m_cachedImageFilename = "";
		}
	}

	GLDrawable::loadPropsFromMap(map);
}

QVariantMap GLImageDrawable::propsToMap()
{
	QVariantMap map = GLDrawable::propsToMap();

	// Pack the image into the map so that when unpacked,
	// if the file doesnt exist, we can use the cached image
	if(!m_imageFile.isEmpty())
	{
		QTime t;
		t.start();
		
		if(m_cachedImageBytes.isEmpty() || 
		   QFileInfo(m_imageFile).lastModified() > m_cachedImageMtime)
		{
			QFile file(m_imageFile);
			if (file.open(QIODevice::ReadOnly))
			{
				m_cachedImageMtime = QFileInfo(m_imageFile).lastModified();
				m_cachedImageBytes = file.readAll();
			}
		}
			
		map["cached_image_bytes"]    = m_cachedImageBytes;
		map["cached_image_filename"] = m_imageFile;
		map["cached_image_mtime"]    = m_cachedImageMtime;
		
		qDebug() << "GLImageDrawable::propsToMap: Packed "<<m_cachedImageBytes.size()/1024<<" Kb into map for cached image "<<m_imageFile<<" in "<<t.elapsed()<<"ms";
	}

	return map;
}

void GLImageDrawable::setBorderColor(const QColor& value)
{
	m_borderColor = value;
	shadowOrBorderDirty();
}

void GLImageDrawable::setBorderWidth(double value)
{
	m_borderWidth = value;
	shadowOrBorderDirty();
}

void GLImageDrawable::setShadowEnabled(bool value)
{
	m_shadowEnabled = value;
	shadowOrBorderDirty();
}

void GLImageDrawable::setShadowBlurRadius(double value)
{
	m_shadowBlurRadius = value;
	shadowOrBorderDirty();
}

void GLImageDrawable::setShadowOffset(const QPointF& value)
{
	m_shadowOffset = value;
	shadowOrBorderDirty();
}

void GLImageDrawable::setShadowColor(const QColor& value)
{
	m_shadowColor = value;
	shadowOrBorderDirty();
}

void GLImageDrawable::setShadowOpacity(double value)
{
	m_shadowOpacity = value;
	shadowOrBorderDirty();
}

void GLImageDrawable::shadowOrBorderDirty()
{
	if(m_dirtyBatchTimer.isActive())
		m_dirtyBatchTimer.stop();
		
	m_dirtyBatchTimer.start();
}

void GLImageDrawable::reapplyBorderAndShadow()
{
	setImage(m_image);
}

QImage GLImageDrawable::applyBorderAndShadow(const QImage& sourceImg)
{
	if(!m_shadowEnabled && m_borderWidth < 0.001)
		return sourceImg;
		
	QSizeF originalSizeWithBorder = sourceImg.size();
	if(m_borderWidth > 0.001)
	{
		double x = m_borderWidth * 2;
		originalSizeWithBorder.rwidth() += x;
		originalSizeWithBorder.rheight() += x;
		
		if(!m_shadowEnabled)
		{
			QImage cache(originalSizeWithBorder.toSize(),QImage::Format_ARGB32_Premultiplied);
			memset(cache.scanLine(0),0,cache.byteCount());
			QPainter p(&cache);
			drawImageWithBorder(&p,sourceImg);
			return cache;
		}
	}
	
	
	QSizeF shadowSize = m_shadowEnabled ? QSizeF(shadowOffset().x(),shadowOffset().y()) : QSizeF(0,0);
	QSizeF padSize(12.,12.); // ???
	QSize sumSize = (originalSizeWithBorder + shadowSize + padSize).toSize();
	//qDebug() << "RichTextRenderer::update(): textWidth: "<<textWidth<<", shadowSize:"<<shadowSize<<", docSize:"<<docSize<<", sumSize:"<<sumSize;
	QImage cache(sumSize,QImage::Format_ARGB32); //_Premultiplied);
	memset(cache.scanLine(0),0,cache.byteCount());
	
	QPainter cachePainter(&cache);
	
	double padSizeHalfX = 0;//adSize.width() / 2;
	double padSizeHalfY = 0;//padSize.height() / 2;
	
	double radius = shadowBlurRadius();
	
	// create temporary pixmap to hold a copy of the text
	QSizeF blurSize = ImageFilters::blurredSizeFor(originalSizeWithBorder, (int)radius);
	//qDebug() << "Blur size:"<<blurSize<<", doc:"<<doc.size()<<", radius:"<<radius;
	QImage tmpImage(blurSize.toSize(),QImage::Format_ARGB32_Premultiplied);
	memset(tmpImage.scanLine(0),0,tmpImage.byteCount());
	
	// render the source image into a temporary buffer for bluring
	QPainter tmpPainter(&tmpImage);
	
	tmpPainter.save();
	tmpPainter.translate(radius + padSizeHalfX, radius + padSizeHalfY);
	
	if(m_borderWidth > 0.001)
		drawImageWithBorder(&tmpPainter, sourceImg);
	else
		tmpPainter.drawImage(0,0,sourceImg);
	
	tmpPainter.restore();
	
	bool debug = false;
	
	if(debug)
		tmpImage.save("debug/shadow-stage1.png");
	
	// color the orignal image by applying a color to the copy using a QPainter::CompositionMode_DestinationIn operation. 
	// This produces a homogeneously-colored pixmap.
	QRect rect = tmpImage.rect();
	tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
	
	QColor color = shadowColor();
	color.setAlpha((int)(255.0 * m_shadowOpacity));
	tmpPainter.fillRect(rect, color);
	
	tmpPainter.end();
	
	if(debug)
		tmpImage.save("debug/shadow-stage2.png");

	// blur the colored text
	ImageFilters::blurImage(tmpImage, (int)radius);
	
	if(debug)
		tmpImage.save("debug/shadow-stage3.png");
	
	// render the blurred text at an offset into the cache
	cachePainter.save();
	cachePainter.translate(shadowOffset().x() - radius,
			       shadowOffset().y() - radius);
	cachePainter.drawImage(0, 0, tmpImage);
	cachePainter.restore();
	
	if(debug)
		cache.save("debug/shadow-stage4.png");
	
	// Render the original image (with or without the border) on top of the blurred copy
	if(m_borderWidth > 0.001)
		drawImageWithBorder(&cachePainter,sourceImg);
	else
		cachePainter.drawImage(0,0,sourceImg);
	
	if(debug)
		cache.save("debug/shadow-stage5.png");
	
	return cache;
}

void GLImageDrawable::drawImageWithBorder(QPainter *p, const QImage &sourceImg)
{
	Q_ASSERT(p);
	int bw = (int)(m_borderWidth / 2);
	p->drawImage(bw,bw,sourceImg); //drawImage(bw,bw,sourceImg);
	p->setPen(QPen(m_borderColor,m_borderWidth));
	p->drawRect(sourceImg.rect().adjusted(bw,bw,bw,bw)); //QRectF(sourceImg.rect()).adjusted(-bw,-bw,bw,bw));
}
