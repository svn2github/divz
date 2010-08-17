#include "StaticVideoSource.h"

StaticVideoSource::StaticVideoSource(QObject *parent)
	: VideoSource(parent)
	, m_frameUpdated(false)
{
	setImage(QImage());
}

void StaticVideoSource::setImage(const QImage& img)
{
	m_image = img.convertToFormat(QImage::Format_ARGB32);
	m_frame = VideoFrame(m_image,1000/30);
	enqueue(m_frame);
	emit frameReady();
	m_frameUpdated = true;
}

void StaticVideoSource::run()
{
	while(!m_killed)
	{
 		if(m_frameUpdated)
 		{
			enqueue(m_frame);
			emit frameReady();
		}
		msleep(100);
	}
}


