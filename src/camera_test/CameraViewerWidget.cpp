#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QDebug>

#include "CameraViewerWidget.h"
#include "CameraThread.h"
#include "CameraServer.h"
#include <QPainter>
#include <QApplication>
#include <QMessageBox>

CameraViewerWidget::CameraViewerWidget()
	: QGLWidget()
	, m_frameCount(0)
	, m_opacity(1)
	, m_readFrameCount(0)
	, m_lockRepaint(false)
	, m_isPrimaryConsumer(false)

{
	//setAttribute(Qt::WA_PaintOnScreen, true);
	//setAttribute(Qt::WA_OpaquePaintEvent, true);
	//setAttribute(Qt::WA_PaintOutsidePaintEvent);
	
	//setWindowTitle("Camera Test");
	srand ( time(NULL) );
	

}

void CameraViewerWidget::closeEvent(QCloseEvent*)
{
	//deleteLater();
	m_paintTimer.stop();
	m_thread->release(this);
	disconnect(m_thread,0,this,0);
}


void CameraViewerWidget::showEvent(QShowEvent*)
{
	//deleteLater();
	if(!m_paintTimer.isActive())
	{
		m_thread = CameraThread::threadForCamera(m_camera);
		connect(m_thread, SIGNAL(frameReady()), this, SLOT(frameReady()), Qt::QueuedConnection);
		m_thread->registerConsumer(this);
		m_paintTimer.start();
		
		m_elapsedTime.start();
	}
}

void CameraViewerWidget::setCamera(const QString& camera, int fps)
{
	m_camera = camera;
	qDebug() << "CameraViewerWidget::setCamera: In Thread ID "<<QThread::currentThreadId();
	m_thread = CameraThread::threadForCamera(camera);
	if(!m_thread)
	{
		repaint();
		return;
	}

	//connect(m_thread, SIGNAL(newImage(QImage)), this, SLOT(newFrame(QImage)));
	connect(m_thread, SIGNAL(frameReady()), this, SLOT(frameReady()), Qt::QueuedConnection);

	m_thread->registerConsumer(this);

	m_elapsedTime.start();


	updateOverlay();

	//emit readyForNextFrame();

	connect(&m_paintTimer, SIGNAL(timeout()), this, SLOT(callUpdate()));
	m_paintTimer.setInterval(1000/fps);
 	m_paintTimer.start();
}

void CameraViewerWidget::callUpdate()
{
	update();
}

void CameraViewerWidget::setFps(int fps)
{
	m_fps = fps;
	m_paintTimer.stop();
	m_paintTimer.setInterval(1000/fps);
	m_paintTimer.start();
}


void CameraViewerWidget::setPrimaryConsumer(bool flag)
{
	m_isPrimaryConsumer = flag;
	qDebug() << "CameraViewerWidget::setPrimaryConsumer: "<<objectName()<<": flag:"<<flag;
	return;
	if(flag)
	{
		connect(this, SIGNAL(readyForNextFrame()), m_thread, SLOT(readFrame()));
	}
	else
	{
		disconnect(this, SIGNAL(readyForNextFrame()), m_thread, SLOT(readFrame()));
	}
}

void CameraViewerWidget::setOverlayText(const QString& text)
{
	m_overlayText = text;
}

void CameraViewerWidget::showOverlayText(bool flag)
{
	m_showOverlayText = flag;
}

void CameraViewerWidget::updateOverlay()
{
	if(m_overlay.rect() != rect())
	    m_overlay = QPixmap(rect().width(), rect().height());

	m_overlay.fill(QColor(0,0,0,0));
	QPainter painter(&m_overlay);

	painter.setRenderHint(QPainter::Antialiasing, true);
	//painter.setPen(QPen(Qt::black, 12, Qt.ashDotLine, Qt::RoundCap));
	//painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
	//painter.drawEllipse(80, 80, 400, 240);

}


CameraViewerWidget::~CameraViewerWidget()
{
	if(m_thread)
		m_thread->release(this);
// 	m_thread->quit();
// 	m_thread->wait();
// 	delete m_thread;
// 	m_thread = 0;

// 	delete m_server;
// 	m_server = 0;
}

void CameraViewerWidget::setOpacity(qreal opac)
{
	m_opacity = opac;
	update();
}

void CameraViewerWidget::frameReady()
{
	//qDebug() << "CameraViewerWidget::frameReady(): thread:"<<QThread::currentThread()<<", main:"<<QApplication::instance()->thread()<<", in main?" << ( QApplication::instance()->thread() == QThread::currentThread() ? true:false);
	if(!m_thread)
		return;
		
		
	m_frame = m_thread->getImage();
	if(m_frame.size() != m_sourceRect.size() ||
	   rect().size() != m_cachedFrameRect.size())
	{
		m_cachedFrameRect = rect();
		//qDebug() << "Frame Size:"<<m_frame.size();
		//QMessageBox::information(this, "Debug", QString("Frame: %1 x %2").arg(m_frame.size().width()).arg(m_frame.size().height()));
		//resize(m_frame.size());

		//resize(QSize(1024,768));
		m_sourceRect = m_frame.rect();
		m_targetRect = QRect(0,0,0,0);
		if(m_sourceRect.height() > m_sourceRect.width())
		{
			qreal ar = (qreal)m_sourceRect.width() / (qreal)m_sourceRect.height();
			int width = (int)((qreal)rect().height() * ar);
			if(width > rect().width())
			{
				int height = (int)((qreal)rect().width() / .75);
				m_targetRect = QRect(0, rect().height()/2 - height/2, rect().width(), height);
			}
			else
				m_targetRect = QRect(rect().width()/2 - width/2, 0, width, rect().height());
		}
		else
		{
			qreal ar = (qreal)m_sourceRect.height() / (qreal)m_sourceRect.width();
			int height = (int)((qreal)rect().width() * ar);
			if(height > rect().height())
			{
				int width = (int)((qreal)rect().height() / .75);
				m_targetRect = QRect(rect().width()/2 - width/2, 0, width, rect().height());
			}
			else
				m_targetRect = QRect(0, rect().height()/2 - height/2, rect().width(), height);


			//qDebug() << "WIDE: m_sourceRect:"<<m_sourceRect<<", m_targetRect:"<<m_targetRect<<", ar:"<<ar<<", rect:"<<rect()<<", height: "<<height;
		}

	}
	//qDebug() << "CameraViewerWidget::newFrame(): My Frame Count #: "<<m_readFrameCount ++;

	//if(m_lockRepaint)
	//    return;
	//m_lockRepaint = true;
	//qDebug() << "CameraViewerWidget::newFrame: In ID "<<QThread::currentThreadId();
		

	//update();
	//repaint();
// 	if(m_isPrimaryConsumer)
// 	{
// 		//QTimer::singleShot(0,this,SIGNAL(readyForNextFrame()));
// 	}
// 	
	//QTimer::singleShot(0,this,SIGNAL(repaint()));
	//repaint();
	//update();
	//QPainter p(this);
	//p.drawImage(m_targetRect,m_frame,m_sourceRect);
// 	}
// 	else
// 		update();
	//QTimer::singleShot(1000/30, this, SLOT(callUpdate()));
	
// 	m_paintTimer.stop();
// 	m_paintTimer.setInterval(1000/30);
// 	m_paintTimer.start();
	QTimer::singleShot(0, this, SLOT(updateTimer()));
}

void CameraViewerWidget::updateTimer()
{
	//m_paintTimer.stop();
// 	int fps =( rand() % 30 + 25 );
// 	m_paintTimer.setInterval(1000/30);
	//m_paintTimer.start();
	//qDebug() << "CameraViewerWidget::updateTimer: fps:"<<fps; //In ID "<<QThread::currentThreadId();
}

void CameraViewerWidget::paintEvent(QPaintEvent*)
{

	m_lockRepaint = true;

	//if(m_thread)
	//	newFrame(m_thread->getImage());

	QPainter p(this);
	//p.setRenderHint(QPainter::SmoothPixmapTransform);
	//p.setRenderHint(QPainter::Antialiasing);

	//qDebug() << "CameraViewerWidget::paintEvent(): "<<objectName()<<", thread:"<<thread()<<", main:"<<QApplication::instance()->thread();
	//qDebug() << "CameraViewerWidget::paintEvent(): My Frame Count #: "<<m_frameCount ++;

	if(m_opacity <= 0)
		return;

	if(m_opacity < 1 || m_targetRect != rect())
		p.fillRect(rect(),Qt::black);

	if(m_opacity>0 && m_opacity<1)
		p.setOpacity(m_opacity);

	if(!m_thread)
	{
		p.fillRect(rect(),Qt::black);
		p.setPen(Qt::white);
		p.drawText(5,15,QString("Error: Cannot find camera \"%1\"").arg(m_camera));
	}
	else
	{

		p.drawImage(m_targetRect,m_frame,m_sourceRect);

		int sec = (m_elapsedTime.elapsed() / 1000);

		m_frameCount ++;
		int fps = (m_frameCount <= 0 ? 1 : m_frameCount) / (sec <= 0 ? 1 : sec);
		p.drawText(5,15,QString("fps: %1, frames: %3, time: %2").arg(fps).arg(sec).arg(m_frameCount));
		//qDebug() << QString("fps: %1, frames: %3, time: %2").arg(fps).arg(sec).arg(m_frameCount);

		if(m_showOverlayText)
		{
			// render m_overlayText neatly and nicely
		}

		//p.drawPixmap(rect(),m_overlay);
	}

	m_lockRepaint = false;
}
