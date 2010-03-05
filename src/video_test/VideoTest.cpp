#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include <QPainter>

#include <QGraphicsView>
#include <QGraphicsScene>

#include "VideoTest.h"
#include "VideoThread.h"
#include "CameraClient.h"


VideoTest::VideoTest()
	: QGLWidget()
{
	setWindowTitle("Video Test");
	
	m_aspectRatioMode = Qt::KeepAspectRatio;
	m_updatePaintDevice = true;	
	m_thread = new VideoThread(this);
	
	//connect(m_thread, SIGNAL(newImage(QImage)), this, SLOT(newFrame(QImage)));
	
	m_surface = new QPainterVideoSurface;
	
	connect(m_surface, SIGNAL(frameChanged()), this, SLOT(repaint()));
	connect(m_surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)), this, SLOT(formatChanged(QVideoSurfaceFormat)));
	
	m_thread->setSurface(m_surface);
	
// 	connect(this, SIGNAL(readyForNextFrame()), m_thread, SLOT(readFrame()));
	
// 	m_client = new CameraClient(this);
// 	connect(m_client, SIGNAL(newImage(QImage)), this, SLOT(newFrame(QImage)));

 	//m_thread->setVideo("085_JumpBack.mov");
 	m_thread->setVideo("/home/josiah/Download/qt-master/qt-qt/examples/dviz/src/samples/jumpbacks/105_JumpBack.mov");
 	m_thread->start();
	//m_client->connectTo("10.1.5.68",8088);

	resize(320,240);

	emit readyForNextFrame();



}

void VideoTest::formatChanged(const QVideoSurfaceFormat &format)
{
	m_videoNativeSize = format.sizeHint();

	updateRects();
}

void VideoTest::updateRects()
{
//     q_ptr->prepareGeometryChange();

	if (m_videoNativeSize.isEmpty()) 
	{
		m_boundingRect = QRectF();
	} 
	else 
	if (m_aspectRatioMode == Qt::IgnoreAspectRatio) 
	{
		m_boundingRect = rect();
		m_sourceRect = QRectF(0, 0, 1, 1);
	} 
	else 
	if (m_aspectRatioMode == Qt::KeepAspectRatio) 
	{
		QSizeF size = m_videoNativeSize;
		size.scale(rect().size(), Qt::KeepAspectRatio);
	
		m_boundingRect = QRectF(0, 0, size.width(), size.height());
		m_boundingRect.moveCenter(rect().center());
	
		m_sourceRect = QRectF(0, 0, 1, 1);
	} 
	else 
	if (m_aspectRatioMode == Qt::KeepAspectRatioByExpanding) 
	{
		m_boundingRect = rect();
	
		QSizeF size = rect().size();
		size.scale(m_videoNativeSize, Qt::KeepAspectRatio);
	
		m_sourceRect = QRectF(
			0, 0, size.width() / m_videoNativeSize.width(), size.height() / m_videoNativeSize.height());
		m_sourceRect.moveCenter(QPointF(0.5, 0.5));
	}
}


VideoTest::~VideoTest()
{
	m_thread->quit();
	m_thread->wait();
	delete m_thread;
	m_thread = 0;
// 	delete m_client;
}

void VideoTest::newFrame(QImage frame)
{
// 	m_frame = frame;
// 	if(frame.size().width() > size().width() ||
// 	    frame.size().height() > size().height() )
// 		resize(frame.size());
// 	qDebug() << "Got new frame";
//  	repaint();

   	QPainter p(this);
//   	p.drawImage(rect(),m_frame);
  	
 	m_view->m_bg = frame;
  	m_view->render(&p);
  	
//  	m_view->repaint();
	
	
	QTimer::singleShot(0,this,SIGNAL(readyForNextFrame()));
}

void VideoTest::paintEvent(QPaintEvent*)
{
// 	qDebug() << "paint event";
 	QPainter painter(this);
// 	p.drawImage(rect(),m_frame);

	if (m_updatePaintDevice && 
		(painter.paintEngine()->type() == QPaintEngine::OpenGL ||
		 painter.paintEngine()->type() == QPaintEngine::OpenGL2)
	   ) 
	{
		m_updatePaintDevice = false;
	
		m_surface->setGLContext(const_cast<QGLContext *>(QGLContext::currentContext()));
		
		if(m_surface->supportedShaderTypes() & QPainterVideoSurface::GlslShader)
		{
			m_surface->setShaderType(QPainterVideoSurface::GlslShader);
		} 
		else 
		{
			m_surface->setShaderType(QPainterVideoSurface::FragmentProgramShader);
		}
	}
	
	if (m_surface && m_surface->isActive()) 
	{
		m_surface->paint(&painter, m_boundingRect, m_sourceRect);
		m_surface->setReady(true);
	}
}



