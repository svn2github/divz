#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QDebug>

#include "VideoWidget.h"
#include "CameraThread.h"
#include "CameraServer.h"
#include <QPainter>
#include <QApplication>
#include <QMessageBox>

VideoWidget::VideoWidget()
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
	, m_frameHoldTime(1000/30)
{
	//setAttribute(Qt::WA_PaintOnScreen, true);
	//setAttribute(Qt::WA_OpaquePaintEvent, true);
	//setAttribute(Qt::WA_PaintOutsidePaintEvent);
	srand ( time(NULL) );
	connect(&m_paintTimer, SIGNAL(timeout()), this, SLOT(callUpdate()));
	//qDebug() << "Default hold time: "<<m_frameHoldTime;
	m_paintTimer.setInterval(m_frameHoldTime);
}

void VideoWidget::closeEvent(QCloseEvent*)
{
// 	disconnectCamera();
	m_paintTimer.stop();
}

void VideoWidget::disconnectVideoSource()
{
	m_paintTimer.stop();
	m_thread->release(this);
	disconnect(m_thread,0,this,0);
	m_thread = 0;
}


void VideoWidget::showEvent(QShowEvent*)
{
	m_paintTimer.start();
	if(!m_paintTimer.isActive())
	{
// 		setVideoSource(m_thread);
		
// 		m_thread = CameraThread::threadForCamera(m_camera);
// 		connect(m_thread, SIGNAL(frameReady()), this, SLOT(frameReady()), Qt::QueuedConnection);
// 		m_thread->registerConsumer(this);
// 		m_paintTimer.start();
// 		
// 		m_elapsedTime.start();
	}
}

void VideoWidget::setVideoSource(VideoSource *source)
{
	if(m_thread)
		disconnectVideoSource();
		
	//qDebug() << "VideoWidget::setCamera: In Thread ID "<<QThread::currentThreadId();
	m_thread = source;
	if(!m_thread)
	{
		repaint();
		return;
	}

	connect(m_thread, SIGNAL(frameReady(int)), this, SLOT(frameReady(int)), Qt::QueuedConnection);
	m_thread->registerConsumer(this);
	
	m_elapsedTime.start();
	m_paintTimer.start();
}

void VideoWidget::callUpdate()
{
	update();
}


void VideoWidget::setOverlayText(const QString& text)
{
	m_overlayText = text;
}

void VideoWidget::showOverlayText(bool flag)
{
	m_showOverlayText = flag;
}

void VideoWidget::updateOverlay()
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


VideoWidget::~VideoWidget()
{
	if(m_thread)
		m_thread->release(this);
}

void VideoWidget::setOpacity(qreal opac)
{
	m_opacity = opac;
	update();
}

void VideoWidget::resizeEvent(QResizeEvent*) 
{ 
	updateRects();
}

void VideoWidget::setSourceRectAdjust( int dx1, int dy1, int dx2, int dy2 )
{
	m_adjustDx1 = dx1;
	m_adjustDy1 = dy1;
	m_adjustDx2 = dx2;
	m_adjustDy2 = dy2;
}	

void VideoWidget::updateRects()
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


void VideoWidget::frameReady(int holdTime)
{
	if(!m_thread)
		return;
		
	m_frameHoldTime = holdTime;
	m_frame = m_thread->frame();
	
	if(m_frame.size() != m_origSourceRect.size())
		updateRects();
		
	//QTimer::singleShot(0, this, SLOT(updateTimer()));
}

void VideoWidget::updateTimer()
{
	
	if(m_paintTimer.interval() != m_frameHoldTime)
	{
		//qDebug() << "VideoWidget::updateTimer: new hold time: "<<m_frameHoldTime;
		m_paintTimer.setInterval(m_frameHoldTime);
	}
}

void VideoWidget::paintEvent(QPaintEvent*)
{

	m_lockRepaint = true;

	QPainter p(this);
	//p.setRenderHint(QPainter::SmoothPixmapTransform);
	//p.setRenderHint(QPainter::Antialiasing);

	//qDebug() << "VideoWidget::paintEvent(): "<<objectName()<<", thread:"<<thread()<<", main:"<<QApplication::instance()->thread();
	//qDebug() << "VideoWidget::paintEvent(): My Frame Count #: "<<m_frameCount ++;

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
		p.drawText(5,15,QString("Error: Invalid Video Source"));
	}
	else
	{
		p.drawImage(m_targetRect,m_frame,m_sourceRect);

		int sec = (m_elapsedTime.elapsed() / 1000);

		m_frameCount ++;
		int fps = (m_frameCount <= 0 ? 1 : m_frameCount) / (sec <= 0 ? 1 : sec);
		p.setPen(Qt::black);
		p.drawText(m_targetRect.x() + 6,m_targetRect.y() + 16,QString("fps: %1, frames: %3, time: %2").arg(fps).arg(sec).arg(m_frameCount));
		p.setPen(Qt::white);
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
	
	updateTimer();
}
