#include "VideoSource.h"

#include "VideoWidget.h"

VideoSource::VideoSource(QObject *parent)
	: QThread(parent)
	, m_killed(false)
{}

void VideoSource::registerConsumer(VideoWidget *consumer)
{
	m_consumerList.append(consumer);
}

VideoSource::~VideoSource() {}


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
