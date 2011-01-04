#include "EditorGraphicsView.h"

EditorGraphicsView::EditorGraphicsView(QWidget * parent)
			: QGraphicsView(parent)
			, m_canZoom(true)
			, m_scaleFactor(1.)
			, m_autoResize(false)
{
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	setCacheMode(QGraphicsView::CacheBackground);
	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	setTransformationAnchor(AnchorUnderMouse);
	setResizeAnchor(AnchorViewCenter);
	
	//setFrameStyle(QFrame::NoFrame);
	//setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
	setBackgroundBrush(Qt::gray);
	
}

void EditorGraphicsView::keyPressEvent(QKeyEvent *event)
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
		QGraphicsView::keyPressEvent(event);
}


void EditorGraphicsView::wheelEvent(QWheelEvent *event)
{
	scaleView(pow((double)2, event->delta() / 240.0));
}


void EditorGraphicsView::scaleView(qreal scaleFactor)
{
	if(!m_canZoom)
		return;

	m_scaleFactor *= scaleFactor;

	qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;

	scale(scaleFactor, scaleFactor);
}

void EditorGraphicsView::setScaleFactor(qreal scaleFactor)
{
	m_scaleFactor = scaleFactor;
	
	qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;

	scale(scaleFactor, scaleFactor);
}

void EditorGraphicsView::resizeEvent(QResizeEvent *)
{
	adjustViewScaling();
}

void EditorGraphicsView::adjustViewScaling()
{
	if(!scene())
		return;
		
	if(!m_autoResize)
		return;
	
	float sx = ((float)width()) / scene()->width();
	float sy = ((float)height()) / scene()->height();

	float scale = qMin(sx,sy);
	setTransform(QTransform().scale(scale,scale));
	//qDebug("Scaling: %.02f x %.02f = %.02f",sx,sy,scale);
	update();
	//m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
	//m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

void EditorGraphicsView::setAutoResize(bool flag)
{
	m_autoResize = flag;
	if(flag)
	{
		//setFrameStyle(QFrame::NoFrame);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		adjustViewScaling();
	}
	else
	{
		//setFrameStyle(QFrame::NoFrame);
		setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
}
