#include "GLImageDrawable.h"

GLImageDrawable::GLImageDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
{
	//setImage(QImage("dot.gif"));
	setCrossFadeMode(GLVideoDrawable::FrontAndBack);
	
	if(!file.isEmpty())
		setImageFile(file);
	
	//QTimer::singleShot(1500, this, SLOT(testXfade()));
}

GLImageDrawable::~GLImageDrawable()
{
// 	if(m_frame)
// 		delete m_frame;
}
	
void GLImageDrawable::testXfade()
{
	qDebug() << "GLImageDrawable::testXfade(): loading file #2";
	setImageFile("dsc_6645.jpg");
}
	
QImage makeHistogram(const QImage& image)
{
	if(image.isNull())
		return image;
		
	QSize smallSize = image.size();
	smallSize.scale(160,120,Qt::KeepAspectRatio);
	
	QImage origScaled = image.scaled(smallSize);
	if(origScaled.format() != QImage::Format_RGB32)
		origScaled = origScaled.convertToFormat(QImage::Format_RGB32);
	
	// Setup our list of grayscale values and set to 0
	int rgbHisto[256];
	memset(&rgbHisto, 0, 256);
	for(int i=0;i<256;i++)
		rgbHisto[i] = 0;
	
	// Convert each pixel to grayscale and count the number of times each
	// grayscale value appears in the image
	for(int y=0; y<smallSize.height(); y++)
	{
		const uchar *line = (const uchar*)origScaled.scanLine(y);
		for(int x=0; x<smallSize.width(); x+=3)
		{
			const uchar r = line[x];
			const uchar g = x+1 >= smallSize.width() ? 0 : line[x+1];
			const uchar b = x+2 >= smallSize.width() ? 0 : line[x+2];
			if(r || g || b)
			{
				//const QRgb pixel = (QRgb)val;
				//int gray = qRed(pixel) * .30 + qGreen(pixel) * .59 + qBlue(pixel) * .11;
				int gray = r * .30 + g * .59 + b * .11;
				//qDebug() << "val:"<<(int)val<<", r:"<<qRed(pixel)<<", g:"<<qGreen(pixel)<<", b:"<<qBlue(pixel)<<", gray:"<<gray;
				//qDebug() << "r:"<<r<<", g:"<<g<<", b:"<<b<<", gray:"<<gray;
				rgbHisto[gray] ++;
			} 
		}
	}
	
	// Calc the max and avg pixel counts
 	int max=0;
 	int avg=0;
 	for(int i=0;i<256;i++)
 	{
 		int count = rgbHisto[i];
 		if(count > max)
 			max = count;
 		avg += count;
 	}
 		
 	avg /= 255;
 	//qDebug() << "Avg:"<<avg<<", max:"<<max;

	// Draw a very simple bar graph of the pixel counts
	QImage histogram(QSize(255,128), QImage::Format_RGB32);
	int maxHeight = 128;
	QPainter p(&histogram);
	p.setPen(Qt::blue);
	p.fillRect(histogram.rect(), Qt::gray);
	for(int i=0;i<256;i++)
	{
		int count = rgbHisto[i];
// 		if(count > avg*4)
// 			count = avg*4;
		double perc = ((double)count)/((double)(max*.5));
		double val = perc * maxHeight;
		int scaled = (int)val;
		//qDebug() << "i:"<<i<<", count:"<<count<<", perc:"<<perc<<", val:"<<val<<", scaled:"<<scaled;
		
		p.drawLine(i,maxHeight-scaled, i,maxHeight);
		
	}
	p.end();
	
	// Render side by side with the original image
	QImage histogramOutput(smallSize.width() + 255, smallSize.height(), QImage::Format_RGB32);
	QPainter p2(&histogramOutput);
	p2.fillRect(histogramOutput.rect(),Qt::gray);
	p2.drawImage(QPointF(0,0),origScaled);
	p2.drawImage(QPointF(smallSize.width(),0),histogram);
	
	return histogramOutput;
}


void GLImageDrawable::setImage(const QImage& image)
{
	//qDebug() << "GLImageDrawable::setImage: Size:"<<image.size();
	//QImage image = makeHistogram(tmp);
	//if(m_frame && m_frame->isValid() && ())
	QImage localImage = image;
	if(image.width() > 2000 || image.height() > 2000)
	{
		localImage = image.scaled(2000,2000,Qt::KeepAspectRatio);
		qDebug() << "GLImageDrawable::setImage: Scaled image to"<<localImage.size()<<"with"<<(localImage.byteCount()/1024/1024)<<"MB memory usage";
	}
	
	if(!m_image.isNull() && xfadeEnabled())
	{
 		m_frame2 = m_frame;
		//m_frame2 = VideoFramePtr(new VideoFrame(m_image,1000/30));
		updateTexture(true); // true = read from m_frame2
		xfadeStart();
	}
		
				
	//m_frame = VideoFramePtr(new VideoFrame(localImage, 1000/30));
	m_frame = VideoFramePtr(new VideoFrame());
	//m_frame->setPixelFormat(QVideoFrame::Format_RGB32);
	m_frame->setCaptureTime(QTime::currentTime());
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
	
	QImage image(file);
	if(image.isNull())
	{
		qDebug() << "GLImageDrawable::setImageFile: "<<file<<" - Image loaded is Null!";
		return false;
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
