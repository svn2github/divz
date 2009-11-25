#include "MainGraphicsView.h"

#include <math.h>
#include <QtOpenGL/QGLWidget>
#include <QTimer>
#include <QDebug>

MainGraphicsView::MainGraphicsView(QWidget * parent)
	: QGraphicsView(parent)
	, m_canZoom(true)
	, m_zoomChanging(false)
{
	setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	setCacheMode(QGraphicsView::CacheBackground);
	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	setTransformationAnchor(AnchorUnderMouse);
	setResizeAnchor(AnchorViewCenter);
	setBackgroundBrush(Qt::lightGray);
	setDragMode(QGraphicsView::ScrollHandDrag);
// 	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
// 	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void MainGraphicsView::keyPressEvent(QKeyEvent *event)
{
	if(event->modifiers() & Qt::ControlModifier)
	{
		switch (event->key())
		{
			case Qt::Key_Plus:
				scaleView(qreal(1.2));
				break;
			case Qt::Key_Minus:
			case Qt::Key_Equal:
				scaleView(1 / qreal(1.2));
				break;
			default:
				QGraphicsView::keyPressEvent(event);
		}
	}
	else
	{
		switch (event->key())
		{
			case Qt::Key_Left:
				emit prevImage();
				break;
			case Qt::Key_Right:
				emit nextImage();
				break;
			default:
				QGraphicsView::keyPressEvent(event);
		}
	}
}



void MainGraphicsView::wheelEvent(QWheelEvent *event)
{
	qreal scale = pow((double)2, event->delta() / 240.0);
	if(event->modifiers() & Qt::ShiftModifier)
	{
// 		m_wheelCounter += scale;
// 		qDebug() << "Scroll Delta:"<<scale<<", m_wheelCounter:"<<m_wheelCounter;
// 		if(m_wheelCounter > 3)
// 			m_wheelCounter = 0;
		if(scale > 0)
			emit nextImage();
		else
			emit prevImage();
	}
	else
	{
		scaleView(scale);
	}
}

void MainGraphicsView::scaleView(qreal scaleFactor)
{
	if(!m_canZoom)
		return;

	m_zoomChanging = true;
	
	qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;

	scale(scaleFactor, scaleFactor);
	
	QTimer::singleShot(1, this, SLOT(unlockZoomChanging()));
}


void MainGraphicsView::resizeEvent(QResizeEvent *)
{
	adjustViewScaling();
}

void MainGraphicsView::adjustViewScaling()
{
	if(!scene() || m_zoomChanging)
		return;
		
	float sx = ((float)width())  / scene()->width();
	float sy = ((float)height()) / scene()->height();

	float scale = qMin(sx,sy);
	setTransform(QTransform().scale(scale,scale));
	update();
}

void MainGraphicsView::unlockZoomChanging()
{
	m_zoomChanging = false;
}