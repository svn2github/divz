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
	, m_thread(0)
	, m_frameCount(0)
	, m_opacity(1)
	, m_readFrameCount(0)
	, m_lockRepaint(false)
	, m_aspectRatioMode(Qt::KeepAspectRatio)
	, m_adjustDx1(0)
	, m_adjustDy1(0)
	, m_adjustDx2(0)
	, m_adjustDy2(0)
	, m_showOverlayText(true)
	, m_overlayText("")
{
	//setAttribute(Qt::WA_PaintOnScreen, true);
	//setAttribute(Qt::WA_OpaquePaintEvent, true);
	//setAttribute(Qt::WA_PaintOutsidePaintEvent);
	srand ( time(NULL) );
	connect(&m_paintTimer, SIGNAL(timeout()), this, SLOT(callUpdate()));
}

void CameraViewerWidget::closeEvent(QCloseEvent*)
{
	disconnectCamera();
}

void CameraViewerWidget::disconnectCamera()
{
	m_paintTimer.stop();
	m_thread->release(this);
	disconnect(m_thread,0,this,0);
}


void CameraViewerWidget::showEvent(QShowEvent*)
{
	if(!m_paintTimer.isActive())
	{
		setCamera(m_camera,fps());
		
// 		m_thread = CameraThread::threadForCamera(m_camera);
// 		connect(m_thread, SIGNAL(frameReady()), this, SLOT(frameReady()), Qt::QueuedConnection);
// 		m_thread->registerConsumer(this);
// 		m_paintTimer.start();
// 		
// 		m_elapsedTime.start();
	}
}

void CameraViewerWidget::setCamera(const QString& camera, int fps)
{
	if(m_thread)
		disconnectCamera();
		
	m_camera = camera;
	//qDebug() << "CameraViewerWidget::setCamera: In Thread ID "<<QThread::currentThreadId();
	m_thread = CameraThread::threadForCamera(camera);
	if(!m_thread)
	{
		repaint();
		return;
	}

	connect(m_thread, SIGNAL(frameReady()), this, SLOT(frameReady()), Qt::QueuedConnection);
	m_thread->registerConsumer(this);
	
	m_elapsedTime.start();

	setFps(fps);
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
	if(m_sourceRect.isEmpty())
		return;

	if(m_overlay.size() != m_targetRect.size())
	    m_overlay = QPixmap(m_targetRect.width(), m_targetRect.height());

	m_overlay.fill(QColor(0,0,0,0));
	QPainter painter(&m_overlay);
	
	QRect window = QRect(0,0,m_sourceRect.size().width(),m_sourceRect.size().height());
	painter.setWindow(window);
	
	painter.setRenderHint(QPainter::Antialiasing, true);
// 	painter.setPen(QPen(Qt::black, 12, Qt::DashDotLine, Qt::RoundCap));
// 	painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
// 	painter.drawEllipse(80, 80, 400, 240);
	
	if(!m_overlayText.isEmpty() &&
	   m_showOverlayText)
	{
		int rectHeight = window.height() / 4;
		QRect target(0,rectHeight * 3, window.width(), rectHeight);
		int size = 12;
		QSize textSize(0,0);
		QFont font;
		while(  textSize.width()  < target.width() * .9 &&
			textSize.height() < target.height() * .9)
		{
			size += 4;
			font = QFont("Sans-Serif",size,QFont::Bold);
			textSize = QFontMetrics(font).size(Qt::TextSingleLine, m_overlayText);
		}
		
		QRect textRect(target.x() + target.width() / 2  - textSize.width() / 2,
			       target.y() + target.height() / 2 - textSize.height () / 2 + size, 
			       textSize.width(), textSize.height());
		
		QPainterPath path;
		path.addText(textRect.topLeft(),font,m_overlayText);
		
		painter.setPen(QPen(Qt::black,3));
		painter.setBrush(Qt::white);
		painter.drawPath(path);
		
		/*painter.setFont(font);
		
		painter.setPen(Qt::black);
		painter.drawText( target.x() + target.width() / 2  - textSize.width() / 2 - 2,
				  target.y() + target.height() / 2 - textSize.height () / 2 + size - 2,
				  m_overlayText);
		
		painter.setPen(Qt::black);
		painter.drawText( target.x() + target.width() / 2  - textSize.width() / 2 + 2,
				  target.y() + target.height() / 2 - textSize.height () / 2 + size + 2,
				  m_overlayText);
		
		painter.setPen(Qt::white);
		painter.drawText( target.x() + target.width() / 2  - textSize.width() / 2,
				  target.y() + target.height() / 2 - textSize.height () / 2 + size,
				  m_overlayText);*/ 
		
	}

}


CameraViewerWidget::~CameraViewerWidget()
{
	if(m_thread)
		m_thread->release(this);
}

void CameraViewerWidget::setOpacity(qreal opac)
{
	m_opacity = opac;
	update();
}

void CameraViewerWidget::resizeEvent(QResizeEvent*) 
{ 
	updateRects();
}

void CameraViewerWidget::setSourceRectAdjust( int dx1, int dy1, int dx2, int dy2 )
{
	m_adjustDx1 = dx1;
	m_adjustDy1 = dy1;
	m_adjustDx2 = dx2;
	m_adjustDy2 = dy2;
}	

void CameraViewerWidget::updateRects()
{
	m_sourceRect = m_frame.rect();
	m_origSourceRect = m_sourceRect; 
	
	m_sourceRect.adjust(m_adjustDx1,m_adjustDy1,m_adjustDx2,m_adjustDy2);
	
	QSize nativeSize = m_frame.size();
	
	if (nativeSize.isEmpty()) 
	{
		m_targetRect = QRect();
	} 
	else 
	if (m_aspectRatioMode == Qt::IgnoreAspectRatio) 
	{
		m_targetRect = rect();
	} 
	else 
	if (m_aspectRatioMode == Qt::KeepAspectRatio) 
	{
		QSizeF size = nativeSize;
		size.scale(rect().size(), Qt::KeepAspectRatio);
	
		m_targetRect = QRect(0, 0, size.width(), size.height());
		m_targetRect.moveCenter(rect().center());
	} 
	else 
	if (m_aspectRatioMode == Qt::KeepAspectRatioByExpanding) 
	{
		m_targetRect = rect();
	
		QSize size = rect().size();
		size.scale(nativeSize, Qt::KeepAspectRatio);
	
		m_sourceRect = QRect(QPoint(0,0),size); 
		m_sourceRect.moveCenter(QPoint(size.width() / 2, size.height() / 2));
	}
	
	//qDebug() << "updateRects(): source: "<<m_sourceRect<<", target:" <<m_targetRect;
	updateOverlay();

	
}


void CameraViewerWidget::frameReady()
{
	if(!m_thread)
		return;
		
	m_frame = m_thread->getImage();
	
	if(m_frame.size() != m_origSourceRect.size())
		updateRects();
		
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
		p.drawText(m_targetRect.x() + 5,m_targetRect.y() + 15,QString("fps: %1, frames: %3, time: %2").arg(fps).arg(sec).arg(m_frameCount));
		//qDebug() << QString("fps: %1, frames: %3, time: %2").arg(fps).arg(sec).arg(m_frameCount);

		if(m_showOverlayText)
		{
			// render m_overlayText neatly and nicely
		}

		//p.drawPixmap(0,0,m_overlay);
		p.drawPixmap(m_targetRect.topLeft(),m_overlay);
		//p.drawPixmap(m_targetRect, m_overlay);
	}

	m_lockRepaint = false;
}
