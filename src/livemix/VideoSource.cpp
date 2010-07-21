#include "VideoSource.h"

#include "VideoWidget.h"

//////////////

VideoSource::VideoSource(QObject *parent)
	: QThread(parent)
	, m_killed(false)
	, m_isBuffered(true)
{}

void VideoSource::registerConsumer(VideoWidget *consumer)
{
	m_consumerList.append(consumer);
}

VideoSource::~VideoSource() {}

VideoFrame VideoSource::frame()
{
	if(m_frameQueue.isEmpty())
		return VideoFrame();
	//qDebug() << "VideoSource::frame(): Queue size: "<<m_frameQueue.size();
	return m_frameQueue.dequeue();
}

void VideoSource::enqueue(VideoFrame frame)
{
	if(!m_isBuffered)
		m_frameQueue.clear();
	m_frameQueue.enqueue(frame);
	
	emit frameReady();
}

void VideoSource::setIsBuffered(bool flag)
{
	m_isBuffered = flag;
	if(!flag)
		m_frameQueue.clear();
}

void VideoSource::release(VideoWidget *consumer)
{
	if(!consumer)
		return;
		
	if(!m_consumerList.contains(consumer))
		return;
		
	m_consumerList.removeAll(consumer);
	
	m_refCount --;
	//qDebug() << "CameraThread::release: m_refCount:"<<m_refCount;
	if(m_refCount <= 0)
	{
		m_killed = true;
		quit();
		wait();
		deleteLater();
	}
}
