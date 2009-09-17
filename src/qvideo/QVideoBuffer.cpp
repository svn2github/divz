#include <QtCore>

#include "QVideoBuffer.h"
#include "QVideo.h"

QVideoBuffer::QVideoBuffer(QObject * parent) : QObject(parent), 
	m_buffering(false)
{
}

QVideoBuffer::~QVideoBuffer(){}

void QVideoBuffer::flush()
{
	QImage * image;
	int buffer_count = m_buffer_queue.count();
	for(int i = 0; i < buffer_count; i++)
	{
		image = m_buffer_queue.dequeue().frame;
		delete image;
		image = 0;
	}
	emit nowEmpty();
	m_needs_frame = true;
}

QFFMpegVideoFrame QVideoBuffer::getNextFrame()
{
	QFFMpegVideoFrame next_frame;
	
	m_buffer_mutex.lock();
	next_frame = m_buffer_queue.dequeue();
	m_buffer_mutex.unlock();

	if(m_buffer_queue.isEmpty())
	{
                //qDebug("Buffer Empty!");
                emit nowEmpty();
                m_needs_frame = true;
	}

	return next_frame;
}

void QVideoBuffer::addFrame(QFFMpegVideoFrame frame)
{
	m_buffer_mutex.lock();
	m_buffer_queue.enqueue(frame);
	m_buffer_mutex.unlock();
	m_needs_frame = false;
}
