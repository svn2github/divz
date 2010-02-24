#include "NativeViewerWebKit.h"
#include <QApplication>
#include <QDesktopWidget>

#include "WebSlideGroup.h"

#include "qvideo/QVideoProvider.h"

NativeViewerWebKit::NativeViewerWebKit()
	: NativeViewer()
	, m_webGroup(0)
{
	m_widget = new GLFrameRenderWidget(QGLFormat(QGL::SampleBuffers));
	m_widget->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	m_widget->setParentViewer(this);
}

NativeViewerWebKit::~NativeViewerWebKit()
{
	if(m_webGroup)
		m_webGroup->setNativeViewer(0);
	delete m_widget;
}

void NativeViewerWebKit::setSlideGroup(SlideGroup *group)
{
	m_slideGroup = group;
	m_webGroup = dynamic_cast<WebSlideGroup*>(group);
	m_webGroup->setNativeViewer(this);
}

void NativeViewerWebKit::show()
{
	QRect rect = containerWidget()->geometry();
	QPoint abs = WidgetUtil::absoluteWidgetPosition(containerWidget());
	//qDebug() << "NativeViewerWebKit::show: Showing at "<<abs<<", size:"<<rect.size();
	
	m_widget->resize(rect.size());
	m_widget->move(abs);
	m_widget->show();
		
// 	// the second 'move' is required to move it to an 'odd' 
// 	// place, at least on X11 systems that I've tested.
// 	// For example, over a task bar or halfway off screen, etc.
// 	// And yes, 2 moves are required - before AND after the show()
// 	// - I have no idea why both are required, but it doesn't 
// 	// work without both.
 	m_widget->move(abs);

	//qDebug() << "NativeViewerWebKit::show: done.";
}

void NativeViewerWebKit::close()
{
	hide();
}

void NativeViewerWebKit::hide()
{
	//qDebug() << "NativeViewerWebKit::close: mark start";
#ifdef WebKit_ENABLED
	m_widget->hide();
#endif
	//qDebug() << "NativeViewerWebKit::close: mark end";
}

QPixmap NativeViewerWebKit::snapshot()
{
// 	QRect rect = containerWidget()->geometry();
// 	QPoint abs = WidgetUtil::absoluteWidgetPosition(containerWidget());
// 	return QPixmap::grabWindow(qApp->desktop()->winId(), abs.x(), abs.y(), rect.width(), rect.height());

// 	if(m_webGroup)
// 		return QVideoProvider::iconForFile(m_webGroup->file());
// 	else

	return m_widget->snapshot();
}

void NativeViewerWebKit::setSlide(int /*x*/)
{
	// irrelevant 
}


void NativeViewerWebKit::setState(NativeViewer::NativeShowState state)
{
// 	qDebug() << "NativeViewerWebKit::setState: state:"<<state<<", start";
	m_state = state;
	
	switch(state)
	{
		case NativeViewer::Running:
			m_widget->fadeBlack(false);
			break;
		case NativeViewer::Paused:
			// NA for webkit since content is not linear temporal
			break;
		case NativeViewer::Black:
			m_widget->fadeBlack(true);
		case NativeViewer::White:
			// This is the 'Clear' state - NA for webkit
			break;
		case NativeViewer::Done:
			// Not used - included in the enum for compat with PPT
			break;
		default:
			break;
	}
// 	qDebug() << "NativeViewerWebKit::setState: state:"<<state<<", done";
}


/// GLFrameRenderWidget
GLFrameRenderWidget::GLFrameRenderWidget(const QGLFormat &format, QWidget *parent)
	: QGLWidget(format,parent)
	, m_blackOpacity(0)
	, m_fadeInc(0)
	, m_fadeDir(1)
	, m_lockPainter(false)
	, m_view(0)
	, m_pixmapAllDirty(true)
	, m_viewer(0)
	, m_blackFlag(false)
{
	connect(&m_fadeTimer, SIGNAL(timeout()), this, SLOT(fadeStep()));
	
	connect(&m_copyTimer, SIGNAL(timeout()), this, SLOT(update()));
	m_copyTimer.setSingleShot(true);
	m_copyTimer.setInterval(0);
}

GLFrameRenderWidget::~GLFrameRenderWidget()
{
// 	if(m_view)
// 		m_view->removeEventFilter(this);
	m_view = 0;
}

void GLFrameRenderWidget::setParentViewer(NativeViewerWebKit *viewer)
{
	m_viewer = viewer;
}

void GLFrameRenderWidget::fadeBlack(bool flag)
{
	m_blackFlag = flag;
	m_fadeDir = flag ? 1:-1;
// 	qDebug() << "GLFrameRenderWidget::fadeBlack(): flag:"<<flag<<",m_fadeDir:"<<m_fadeDir;
	
	if(m_viewer && !m_viewer->slideGroup())
	{
		qDebug() << "GLFrameRenderWidget::fadeBlack(): fade black, but no group, so starting black";
		m_blackOpacity = flag ? 1 : 0;
	}
	else
	{
// 		qDebug() << "GLFrameRenderWidget::fadeBlack(): fade black called, doing live fade";
		fadeStart();
	}
}

void GLFrameRenderWidget::fadeStart()
{
	// Reduce requested quality by half to put less of a load on the cpu
	int frames = m_viewer ? m_viewer->fadeQuality() / 2 : 10;
	if(frames <= 0)
		frames = 10;
	m_fadeTimer.setInterval((m_viewer ? m_viewer->fadeSpeed() : 1000) / frames);
	m_fadeInc = 1.0/(qreal)frames;

	m_fadeTimer.start();
}

void GLFrameRenderWidget::fadeDone()
{
	m_fadeTimer.stop();
}

void GLFrameRenderWidget::fadeStep()
{
	if((m_fadeDir > 0 && m_blackOpacity < 1) || (m_fadeDir<0 && m_blackOpacity>0))
	{
		m_blackOpacity += m_fadeInc * m_fadeDir;
// 		qDebug() << "GLFrameRenderWidget::fadeStep(): m_blackOpacity:"<<m_blackOpacity;
		update();
	}
	else
	{
		fadeDone();
	}
}
		
void GLFrameRenderWidget::setWidget(QWidget *widget)
{
	if(m_view)
		m_view->removeEventFilter(this);
		
	m_view = widget;
	m_view->installEventFilter(this);
	
	m_pixmap = QPixmap(m_view->size());
	m_pixmap.fill(Qt::black);
	m_pixmapAllDirty = true;
}

bool GLFrameRenderWidget::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == m_view) 
	{
		//qDebug() << "eventFilter: Got event type: "<<event->type();
		if (event->type() == QEvent::Resize)
		{
			m_pixmap = QPixmap(m_view->size());
			m_pixmap.fill(Qt::black);
			m_pixmapAllDirty = true;
		}
		else
		if (event->type() == QEvent::Paint) 
		{
			QPaintEvent *paintEvent = static_cast<QPaintEvent*>(event);
			//qDebug() << "GLFrameRenderWidget: Repaint requested for " << paintEvent->rect()<<", region: "<<paintEvent->region();
			if(m_lockPainter)
			{
				//qDebug() << "GLFrameRenderWidget: Received paint event inside copy painter lock, ignoring";
			}
			else
			{
				if(paintEvent->rect().topLeft() == QPoint(0,0))
				{
					// QRegions in the paint event don't paint what we need when scrolling the view
					m_region = m_region.united(QRect(QPoint(0,0),m_view->size()));
				}
				else
				{
					m_region = m_region.united(paintEvent->region());
				}
				
				if(!m_copyTimer.isActive())
				{
					//qDebug() << "GLFrameRenderWidget: Starting repaint timer";
					m_copyTimer.start();
				}
			}
			
		}
	} 
	// pass the event on to the parent class
	return QWidget::eventFilter(obj, event);
}

void GLFrameRenderWidget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	paintWidget(&painter);
}

void GLFrameRenderWidget::paintWidget(QPainter *painter)
{
	if(m_lockPainter)
	{
		//qDebug() << "GLFrameRenderWidget: copy painter locked, ignoring";
	}
	else
	{
		static int repaints = 0;
		repaints++;
		//qDebug() << "GLFrameRenderWidget: [ PAINT"<<repaints<<"] Repainting region:"<<m_region;
		
		m_lockPainter = true;
		
		painter->setRenderHint(QPainter::SmoothPixmapTransform);
		
		if(m_view)
		{
			if(m_pixmapAllDirty)
			{
				m_pixmapAllDirty = false;
				m_view->render(&m_pixmap);
			}
			else
				m_view->render(&m_pixmap,m_region.boundingRect().topLeft(),m_region);
		}
		
		painter->fillRect(rect(), Qt::black);
		//(int)(width() - m_pixmap.width())/2, (int)(height() - m_pixmap.height())/2
		QPixmap scaled = m_pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		int x = (int)(width() - scaled.width())/2;
		int y = (int)(height() - scaled.height())/2;
		painter->drawPixmap( x,y, scaled);
		
		QColor color = QColor(0,0,0,(int)(255*(!m_view ? 1 : m_blackOpacity)));
		if(m_blackOpacity > 0 || !m_view)
			painter->fillRect(rect(),color);
		
		//painter->end();
		
		m_lockPainter = false;
		
		m_region = QRegion();

	}
}

QPixmap GLFrameRenderWidget::snapshot()
{
	QPixmap pix(size());
	QPainter painter(&pix);
	paintWidget(&painter);
	painter.end();
	return pix;
}
