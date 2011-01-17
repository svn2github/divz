
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QDebug>
#include <QApplication>

#include <assert.h>

#include "DVizSharedMemoryThread.h"

// For defenition of FRAME_*
#include "../glvidtex/SharedMemorySender.h"

QMap<QString,DVizSharedMemoryThread *> DVizSharedMemoryThread::m_threadMap;
QMutex DVizSharedMemoryThread::threadCacheMutex;

DVizSharedMemoryThread::DVizSharedMemoryThread(const QString& key, QObject *parent)
	: VideoSource(parent)
	, m_fps(30)
	, m_key(key)
	, m_timeAccum(0)
	, m_frameCount(0)
{
	connect(&m_readTimer, SIGNAL(timeout()), this, SLOT(readFrame()));
	m_readTimer.setInterval(1000/m_fps);
}

void DVizSharedMemoryThread::destroySource()
{
	qDebug() << "DVizSharedMemoryThread::destroySource(): "<<this;
	QMutexLocker lock(&threadCacheMutex);
	m_threadMap.remove(m_key);
	m_sharedMemory.detach();
	
	VideoSource::destroySource();
}

DVizSharedMemoryThread * DVizSharedMemoryThread::threadForKey(const QString& key)
{
	if(key.isEmpty())
		return 0;
		
	QMutexLocker lock(&threadCacheMutex);
	
	if(m_threadMap.contains(key))
	{
		DVizSharedMemoryThread *v = m_threadMap[key];
		qDebug() << "DVizSharedMemoryThread::threadForKey(): "<<v<<": "<<key<<": [CACHE HIT] +";
		return v;
	}
	else
	{
		DVizSharedMemoryThread *v = new DVizSharedMemoryThread(key);
		m_threadMap[key] = v;
		qDebug() << "DVizSharedMemoryThread::threadForKey(): "<<v<<": "<<key<<": [CACHE MISS] -";
// 		v->start();
// 		v->moveToThread(v);
		v->m_readTimer.start();

		return v;
	}
}


void DVizSharedMemoryThread::run()
{
	while(!m_killed)
	{
		readFrame();
		msleep(1000 / m_fps);
	}
	
	m_sharedMemory.detach();
}

void DVizSharedMemoryThread::readFrame()
{
	if(!m_sharedMemory.isAttached())
	{
		m_sharedMemory.setKey(m_key);
		
		if(m_sharedMemory.isAttached())
			m_sharedMemory.detach();
			
		m_sharedMemory.attach(QSharedMemory::ReadWrite);
	}
	
	
	m_readTime.restart();
		
	if(!m_sharedMemory.isAttached())
		m_sharedMemory.attach(QSharedMemory::ReadWrite);
	else
	{
		QImage image(FRAME_WIDTH,
				FRAME_HEIGHT,
				FRAME_FORMAT);
			
		uchar *to = image.scanLine(0);
		
		m_sharedMemory.lock();
		const uchar *from = (uchar*)m_sharedMemory.data();
		memcpy(to, from, qMin(m_sharedMemory.size(), image.byteCount()));
		qDebug() << "DVizSharedMemoryThread::run: ShMem size: "<<m_sharedMemory.size()<<", image size:"<<image.byteCount();
	
		m_sharedMemory.unlock();
		
		//image.save("/mnt/phc/Video/tests/frame.jpg");
		
		//enqueue(new VideoFrame(image.convertToFormat(QImage::Format_RGB555),1000/m_fps));
		enqueue(new VideoFrame(image,1000/m_fps));
		
		emit frameReady();
	}
	
	
	m_frameCount ++;
	m_timeAccum  += m_readTime.elapsed();

	if(m_frameCount % m_fps == 0)
	{
		QString msPerFrame;
		msPerFrame.setNum(((double)m_timeAccum) / ((double)m_frameCount), 'f', 2);
	
		qDebug() << "DVizSharedMemoryThread::readFrame(): Avg MS per Frame:"<<msPerFrame;
	}
			
	if(m_frameCount % (m_fps * 10) == 0)
	{
		m_timeAccum  = 0;
		m_frameCount = 0;
	}
}


void DVizSharedMemoryThread::setFps(int fps)
{
	m_fps = fps;
	m_readTimer.setInterval(1000/m_fps);
}

DVizSharedMemoryThread::~DVizSharedMemoryThread()
{
	m_killed = true;
	quit();
	wait();
}

