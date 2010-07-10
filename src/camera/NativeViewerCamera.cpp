#include "NativeViewerCamera.h"
#include <QApplication>
#include <QDesktopWidget>

#include "CameraSlideGroup.h"

#include "qvideo/QVideoProvider.h"

NativeViewerCamera::NativeViewerCamera()
	: NativeViewer()
	, m_state(NativeViewer::Running)
	, m_cameraGroup(0)
	, m_fadeSpeed(1000)
	, m_fadeSteps(15)
// 	, m_autoPlay(true)
{

	m_widget = new CameraViewerWidget();
	m_widget->setCursor(Qt::BlankCursor);
	m_widget->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::ToolTip);

	connect(&m_fadeTimer, SIGNAL(timeout()), this, SLOT(fadeStep()));
	m_fadeTimer.setInterval(m_fadeSpeed/m_fadeSteps);
}

NativeViewerCamera::~NativeViewerCamera()
{
	if(m_cameraGroup)
		m_cameraGroup->removeNativeViewer(outputId());
	delete m_widget;
}

void NativeViewerCamera::setFadeSpeed(int val)
{
	m_fadeSpeed = val;
	m_fadeTimer.setInterval(m_fadeSpeed/m_fadeSteps);
}

void NativeViewerCamera::setFadeQuality(int val)
{
	m_fadeSteps = val;
	m_fadeTimer.setInterval(m_fadeSpeed/m_fadeSteps);
}

void NativeViewerCamera::setSlideGroup(SlideGroup *group)
{
	m_slideGroup = group;
	m_cameraGroup = dynamic_cast<CameraSlideGroup*>(group);
	m_cameraGroup->addNativeViewer(outputId(),this);
	
	m_widget->setCamera(m_cameraGroup->device());
}

void NativeViewerCamera::show()
{
	if(state() != NativeViewer::Running)
		return;

	QRect rect = containerWidget()->geometry();
	QPoint abs = WidgetUtil::absoluteWidgetPosition(containerWidget());
	//qDebug() << "NativeViewerCamera::show: Showing at "<<abs<<", size:"<<rect.size();
#ifdef PHONON_ENABLED
// 	if(m_autoPlay)
// 		m_media->play();
	if(!m_widget->parentWidget())
	{
		m_widget->resize(rect.size());
		m_widget->move(abs);
	}
	m_widget->show();
	// the second 'move' is required to move it to an 'odd' 
	// place, at least on X11 systems that I've tested.
	// For example, over a task bar or halfway off screen, etc.
	// And yes, 2 moves are required - before AND after the show()
	// - I have no idea why both are required, but it doesn't 
	// work without both.
	if(!m_widget->parentWidget())
		m_widget->move(abs);
#endif

	//qDebug() << "NativeViewerCamera::show: done.";
}

void NativeViewerCamera::close()
{
	hide();
}

void NativeViewerCamera::hide()
{
	//qDebug() << "NativeViewerCamera::close: mark start";
	m_widget->hide();
// 	m_media->pause();
	//qDebug() << "NativeViewerCamera::close: mark end";
}

QPixmap NativeViewerCamera::snapshot()
{
// 	QRect rect = containerWidget()->geometry();
// 	QPoint abs = WidgetUtil::absoluteWidgetPosition(containerWidget());
// 	return QPixmap::grabWindow(qApp->desktop()->winId(), abs.x(), abs.y(), rect.width(), rect.height());
// 	if(m_cameraGroup)
// 		return QVideoProvider::iconForFile(m_cameraGroup->file());
// 	else
		return QPixmap();
}

void NativeViewerCamera::setSlide(int x)
{
	// irrelevant 
}

void NativeViewerCamera::fadeBlack(bool flag)
{
	m_fadeDirection = flag ? -1 : 1;
	m_fadeOpacity = flag ? .999 : 0.001;
	
	m_fadeTimer.start();
	// 760-733-9969
}

void NativeViewerCamera::fadeStep()
{
	qreal inc = (qreal)1/m_fadeSteps * (qreal)m_fadeDirection;
	m_fadeOpacity += inc;
// 	qDebug() << "fadeStep: "<<m_fadeOpacity<<","<<m_fadeDirection<<","<<inc;
	
	if(m_fadeOpacity <= 0 || m_fadeOpacity >=1)
		m_fadeTimer.stop();
	else
		m_widget->setOpacity(m_fadeOpacity);
}

void NativeViewerCamera::setState(NativeViewer::NativeShowState state)
{
// 	qDebug() << "NativeViewerCamera::setState: state:"<<state;
	m_state = state;
	switch(state)
	{
		case NativeViewer::Running:
			show();
			fadeBlack(false);
			break;
		case NativeViewer::Paused:
// 			m_media->pause();
			break;
		case NativeViewer::Black:
			fadeBlack(true);
			break;
		case NativeViewer::White:
		case NativeViewer::Done:
			hide();
			break;
		default:
			break;
	}
	//qDebug() << "NativeViewerCamera::setState: state:"<<state<<", done";
}

