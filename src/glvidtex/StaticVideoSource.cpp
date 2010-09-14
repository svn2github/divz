#include "StaticVideoSource.h"

StaticVideoSource::StaticVideoSource(QObject *parent)
	: VideoSource(parent)
	, m_frameUpdated(false)
{
// 	setIsBuffered(false);
	setImage(QImage());
}

void StaticVideoSource::setImage(const QImage& img)
{
	m_image = img.convertToFormat(QImage::Format_ARGB32);
	m_frame = VideoFrame(m_image,1000/30);
	
// 	VideoFrame frame;
// 	//frame.captureTime = QTime::currentTime();
// 	
// 	frame.isRaw = true;
// 	frame.bufferType = VideoFrame::BUFFER_BYTEARRAY;
// 	const uchar *bits = m_image.bits();
// // 	frame.byteArray.clear();
// // 	frame.byteArray.resize(m_image.byteCount());
// 	//frame.byteArray.fill(0);
// 	frame.byteArray.append((const char*)bits, m_image.byteCount());
// 	frame.holdTime = 1000/30;
// 	frame.setSize(m_image.size());
//	
//	m_frame = frame;
	
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
			m_frameUpdated = false;
			emit frameReady();
		}
		msleep(100);
	}
}


