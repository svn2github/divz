#include "StaticVideoSource.h"

StaticVideoSource::StaticVideoSource(QObject *parent)
	: VideoSource(parent)
{
	setImage(QImage());
}

void StaticVideoSource::setImage(const QImage& img)
{
	m_image = img.convertToFormat(QImage::Format_ARGB32);
	m_frame = VideoFrame(m_image,1000/30);
	enqueue(m_frame);
	emit frameReady();
}

void StaticVideoSource::run()
{
	while(!m_killed)
	{
//		qDebug() << "StaticVideoSource::run()";
// 		enqueue(m_frame);
// 		emit frameReady();
// 		msleep(m_frame.holdTime);
		msleep(100);
	}
}


