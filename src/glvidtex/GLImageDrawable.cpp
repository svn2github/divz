#include "GLImageDrawable.h"

GLImageDrawable::GLImageDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
{
	// Setup frame
	m_frame.bufferType = VideoFrame::BUFFER_IMAGE;

	if(!file.isEmpty())
		setImageFile(file);
}
	
void GLImageDrawable::setImage(const QImage& image)
{
	m_image = image;
	m_frame.image = image;
	m_frame.setSize(image.size());
	
	updateTexture();
	
	if(fpsLimit() <= 0.0)
		updateGL();
	
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
	QFileInfo fileInfo(file);
	if(!fileInfo.exists())
	{
		qDebug() << "GLImageDrawable::setImageFile: "<<file<<" does not exist!";
		return false;
	}
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
