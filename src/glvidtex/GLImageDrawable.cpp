#include "GLImageDrawable.h"

GLImageDrawable::GLImageDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
{
	if(!file.isEmpty())
		setImageFile(file);
	
	//QTimer::singleShot(1500, this, SLOT(testXfade()));
}
	
void GLImageDrawable::testXfade()
{
	qDebug() << "GLImageDrawable::testXfade(): loading file #2";
	setImageFile("dsc_6645.jpg");
}
	
void GLImageDrawable::setImage(const QImage& image)
{
	if(m_frame.isValid() && xfadeEnabled())
	{
		m_frame2 = m_frame;
		updateTexture(true); // true = read from m_frame2
		xfadeStart();
	}
		
				
	// Setup frame
	m_frame.bufferType = VideoFrame::BUFFER_IMAGE;

	QImage::Format format = image.format();
	m_frame.pixelFormat =
		format == QImage::Format_ARGB32 ? QVideoFrame::Format_ARGB32 :
		format == QImage::Format_RGB32  ? QVideoFrame::Format_RGB32  :
		format == QImage::Format_RGB888 ? QVideoFrame::Format_RGB24  :
		format == QImage::Format_RGB16  ? QVideoFrame::Format_RGB565 :
		format == QImage::Format_RGB555 ? QVideoFrame::Format_RGB555 :
		//format == QImage::Format_ARGB32_Premultiplied ? QVideoFrame::Format_ARGB32_Premultiplied :
		// GLVideoDrawable doesn't support premultiplied - so the format conversion below will convert it to ARGB32 automatically
		QVideoFrame::Format_Invalid;
		
	if(m_frame.pixelFormat == QVideoFrame::Format_Invalid)
	{
		qDebug() << "VideoFrame: image was not in an acceptable format, converting to ARGB32 automatically.";
		m_image = image.convertToFormat(QImage::Format_ARGB32);
		m_frame.pixelFormat = QVideoFrame::Format_ARGB32;
	}
	else
	{
		m_image = image;
	}
	
	m_frame.image = m_image;
	m_frame.setSize(image.size());
	
	updateTexture();
	
	if(fpsLimit() <= 0.0)
		updateGL();
		
	//qDebug() << "GLImageDrawable::setImage(): Set image size:"<<m_frame.image.size();
	
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
	m_imageFile = file;
	QImage image(file);
	if(image.isNull())
	{
		qDebug() << "GLImageDrawable::setImageFile: "<<file<<" - Image loaded is Null!";
		return false;
	}
	setImage(image);
	return true;
	
}

void GLImageDrawable::setVideoSource(VideoSource*)
{
	// Hide access to this method by making it private and reimpl to do nothing
}
