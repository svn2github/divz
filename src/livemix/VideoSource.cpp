#include "VideoSource.h"

#include "VideoWidget.h"

//////////////

extern "C" {
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

bool VideoSource::isLibAVInit = false;

void VideoSource::initAV()
{
	if(isLibAVInit)
		return;
	isLibAVInit = true;
	
	//qDebug() << "VideoSource::initAV()";
	
	avcodec_init();
	avcodec_register_all();
	avdevice_register_all();
	av_register_all();
}

//////////////

VideoSource::VideoSource(QObject *parent)
	: QThread(parent)
	, m_killed(false)
	, m_isBuffered(true)
{
	initAV();
}

void VideoSource::registerConsumer(VideoWidget *consumer)
{
	m_consumerList.append(consumer);
	//qDebug() << "VideoSource::registerConsumer(): "<<this<<": consumer list:"<<m_consumerList.size(); //m_refCount:"<<m_refCount;
}

VideoSource::~VideoSource() {}

void VideoSource::run()
{
	exec();
}

VideoFrame VideoSource::frame()
{
	if(!m_isBuffered ||
	    m_frameQueue.isEmpty())
		return m_singleFrame;
	//qDebug() << "VideoSource::frame(): Queue size: "<<m_frameQueue.size();
	return m_frameQueue.dequeue();
}

void VideoSource::enqueue(VideoFrame frame)
{
	m_singleFrame = frame;
	
	if(m_isBuffered)
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
	
	//m_refCount --;
	//qDebug() << "VideoSource::release(): "<<this<<": consumer list:"<<m_consumerList.size(); //m_refCount:"<<m_refCount;
	//if(m_refCount <= 0)
	if(m_consumerList.isEmpty())
	{
		destroySource();
	}
}

void VideoSource::destroySource()
{
	//qDebug() << "VideoSource::destroySource(): "<<this;
	m_killed = true;
	quit();
	wait();
	deleteLater();
}