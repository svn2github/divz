#ifndef SharedMemoryImageWriter_CPP
#define SharedMemoryImageWriter_CPP

#include "SharedMemoryImageWriter.h"

#include "MainWindow.h"
#include <QPainter>
#include <QImage>
#include <QTime>

#define WRITER_FPS 20
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define BYTES_PER_PIXEL 4

SharedMemoryImageWriter::SharedMemoryImageWriter(QObject *parent)
	: QObject(parent)
	, m_scene(0)
{
	connect(&m_frameTimer, SIGNAL(timeout()), this, SLOT(generateFrame()));
	m_frameTimer.setInterval(1000/WRITER_FPS);
	
	//m_memorySize = FRAME_WIDTH * FRAME_HEIGHT * BYTES_PER_PIXEL; 
	
	m_image = QImage(FRAME_WIDTH,FRAME_HEIGHT,
		QImage::Format_ARGB32_Premultiplied);
		//QImage::Format_ARGB4444_Premultiplied);
	m_memorySize = m_image.byteCount();	
	
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
	QSize nativeSize = m_sourceRect.size();
	nativeSize.scale(m_image.size(), Qt::KeepAspectRatio);
	
	m_targetRect = QRect(0, 0, nativeSize.width(), nativeSize.height());
	m_targetRect.moveCenter(m_image.rect().center());
}

void SharedMemoryImageWriter::generateFrame()
{
	if(!m_scene || !MainWindow::mw())
		return;
	QTime time;
	time.start();
	
	QPainter painter(&m_image);
	painter.fillRect(0,0,FRAME_WIDTH,FRAME_HEIGHT,QColor(0,0,0,0)); // fully transparent background
	painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setRenderHint(QPainter::TextAntialiasing, false);
	
	if(!m_sourceRect.isValid())
		m_sourceRect = MainWindow::mw()->standardSceneRect();
		
	if(m_sourceRect != m_targetRect)
		updateRects();
	
	m_scene->render(&painter,
		//QRectF(0,0,FRAME_WIDTH,FRAME_HEIGHT),
		m_targetRect,
		m_sourceRect);
	
	painter.end();
	
// 	QBuffer buffer;
// 	buffer.open(QBuffer::ReadWrite);
// 	QDataStream out(&buffer);
// 	out << m_image;
// 	int size = buffer.size();
// 	
 	int size = m_image.byteCount();
 	m_sharedMemory.lock();
 	uchar *to = (uchar*)m_sharedMemory.data();
 	const uchar *from = m_image.scanLine(0);
 	memcpy(to, from, qMin(m_sharedMemory.size(), size));
 	m_sharedMemory.unlock();
	
	m_frameCount ++;
	m_timeAccum += time.elapsed();
	
	QString msPerFrame;
	msPerFrame.setNum(((double)m_timeAccum) / ((double)m_frameCount), 'f', 2);
	
	if(m_frameCount % 30 == 0)
	{
		m_timeAccum = 0;
		m_timeAccum = 0;
	}
	
	if(m_frameCount % 4 == 0)
	{
		//qDebug() << "SharedMemoryImageWriter::generateFrame(): Avg MS per Frame:"<<msPerFrame;
	}
			
	//qDebug() << "SharedMemoryImageWriter::generateFrame(): frame time:"<<time.elapsed()<<", avg:"<<msPerFrame;
}
	
	
#endif
