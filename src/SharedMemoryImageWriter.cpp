#ifndef SharedMemoryImageWriter_CPP
#define SharedMemoryImageWriter_CPP

#include "SharedMemoryImageWriter.h"

#include "MainWindow.h"
#include <QPainter>
#include <QImage>
#include <QTime>

#define WRITER_FPS 10
#define BYTES_PER_PIXEL 4

SharedMemoryImageWriter::SharedMemoryImageWriter(QObject *parent)
	: QObject(parent)
	, m_scene(0)
{
	connect(&m_frameTimer, SIGNAL(timeout()), this, SLOT(generateFrame()));
	m_frameTimer.setInterval(1000/WRITER_FPS);
	
	QImage testImage(FRAME_WIDTH,
			 FRAME_HEIGHT,
			 FRAME_FORMAT);
	m_memorySize = testImage.byteCount();	
	
	m_timeAccum = 0;
	m_frameCount = 0;
}

SharedMemoryImageWriter::~SharedMemoryImageWriter()
{
	if(m_sharedMemory.isAttached())
		m_sharedMemory.detach();
}
	
void SharedMemoryImageWriter::enable(const QString& key, QGraphicsScene *scene)
{
	m_sharedMemory.setKey(key);
	if(m_sharedMemory.isAttached())
		m_sharedMemory.detach();
		
	if (!m_sharedMemory.attach(QSharedMemory::ReadWrite)) 
	{
		// Shared memory does not exist, create it
		if (!m_sharedMemory.create(m_memorySize, QSharedMemory::ReadWrite)) 
		{
			qDebug() << "SharedMemoryImageWriter::enable("<<key<<"): Error:"<<m_sharedMemory.errorString();
			return;
		}
		else 
		{
			m_sharedMemory.lock();
			memset(m_sharedMemory.data(), 0, m_sharedMemory.size());
			m_sharedMemory.unlock();
		}
	}
	
	m_scene = scene;
	m_frameTimer.start();
}

void SharedMemoryImageWriter::disable()
{
	m_frameTimer.stop();
	if(m_sharedMemory.isAttached())
		m_sharedMemory.detach();
}

void SharedMemoryImageWriter::updateRects()
{
	QRect targetFrame(0,0, FRAME_WIDTH, FRAME_HEIGHT);
	QSize nativeSize = m_sourceRect.size();
	nativeSize.scale(targetFrame.size(), Qt::KeepAspectRatio);
	
	m_targetRect = QRect(0, 0, nativeSize.width(), nativeSize.height());
	m_targetRect.moveCenter(targetFrame.center());
}

void SharedMemoryImageWriter::generateFrame()
{
	if(!m_scene || !MainWindow::mw())
		return;
	QTime time;
	time.start();
	
	QImage image(FRAME_WIDTH,
	             FRAME_HEIGHT,
		     FRAME_FORMAT);
	memset(image.scanLine(0), 0, image.byteCount());
	
	QPainter painter(&image);
	painter.fillRect(image.rect(),Qt::transparent);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setRenderHint(QPainter::TextAntialiasing, false);
	
	if(!m_sourceRect.isValid())
		m_sourceRect = MainWindow::mw()->standardSceneRect();
		
	if(m_sourceRect != m_targetRect)
		updateRects();
	
	m_scene->render(&painter,
		m_targetRect,
		m_sourceRect);
	
	painter.end();
	
 	int size = image.byteCount();
 	const uchar *from = image.scanLine(0);
 	
 	m_sharedMemory.lock();
 	uchar *to = (uchar*)m_sharedMemory.data();
 	memset(to, 0, m_sharedMemory.size());
 	memcpy(to, from, qMin(m_sharedMemory.size(), size));
 	m_sharedMemory.unlock();
	
	m_frameCount ++;
	m_timeAccum  += time.elapsed();
	
	if(m_frameCount % WRITER_FPS == 0)
	{
		QString msPerFrame;
		msPerFrame.setNum(((double)m_timeAccum) / ((double)m_frameCount), 'f', 2);
	
		//qDebug() << "SharedMemoryImageWriter::generateFrame(): Avg MS per Frame:"<<msPerFrame;
	}
			
	if(m_frameCount % (WRITER_FPS * 10) == 0)
	{
		m_timeAccum  = 0;
		m_frameCount = 0;
	}
	
	//qDebug() << "SharedMemoryImageWriter::generateFrame(): frame time:"<<time.elapsed()<<", avg:"<<msPerFrame;
}
	
	
#endif
