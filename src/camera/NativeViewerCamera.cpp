#include "NativeViewerCamera.h"
#include <QApplication>
#include <QDesktopWidget>

#include "CameraSlideGroup.h"

#include "livemix/VideoWidget.h"
#include "livemix/CameraThread.h"

#include "qvideo/QVideoProvider.h"

NativeViewerCamera::NativeViewerCamera()
	: NativeViewer()
	, m_state(NativeViewer::Black)
	, m_cameraGroup(0)
	, m_fadeSpeed(1000)
	, m_fadeSteps(15)
// 	, m_autoPlay(true)
{

	m_widget = new VideoWidget();
	m_widget->setCursor(Qt::BlankCursor);
	m_widget->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::ToolTip);
	m_widget->setOpacity(0);
	m_widget->setFps(24);
	
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
	
	QString device = m_cameraGroup->device();
	
	m_camera = CameraThread::threadForCamera(device);
	#ifndef Q_OS_WIN
	m_camera->enableRawFrames(true);
	#endif
	//m_camera->enableRawFrames(true);
	m_widget->setVideoSource(m_camera);
	//m_widget->setCamera(m_cameraGroup->device());
	
	qDebug() << "NativeViewerCamera::setSlideGroup(): Using device: "<<device<<", m_camera:"<<m_camera;
}

void NativeViewerCamera::show()
{
	if(state() != NativeViewer::Running)
		return;

	QRect rect = containerWidget()->geometry();
	QPoint abs = WidgetUtil::absoluteWidgetPosition(containerWidget());
	//qDebug() << "NativeViewerCamera::show: Showing at "<<abs<<", size:"<<rect.size();
	
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

	//m_widget->setOpacity(0);
	//fadeBlack(false);
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
}

void NativeViewerCamera::fadeStep()
{
	qreal inc = (qreal)1/m_fadeSteps * (qreal)m_fadeDirection;
	m_fadeOpacity += inc;
 	//qDebug() << "NativeViewerCamera::fadeStep(): "<<m_fadeOpacity<<","<<m_fadeDirection<<","<<inc;
	
	if(m_fadeOpacity <= 0 || m_fadeOpacity >=1)
	{
		m_fadeTimer.stop();
		m_widget->setOpacity(m_fadeDirection < 0 ? 0 : 1.0);
	}
	else
		m_widget->setOpacity(m_fadeOpacity);
}

void NativeViewerCamera::setState(NativeViewer::NativeShowState state)
{
 	//qDebug() << "NativeViewerCamera::setState: state:"<<state;
 	NativeViewer::NativeShowState oldState = m_state;
	m_state = state;
	switch(state)
	{
		case NativeViewer::Running:
			show();
			if(oldState != NativeViewer::Running)
				fadeBlack(false);
			break;
		case NativeViewer::Paused:
// 			m_media->pause();
			break;
		case NativeViewer::Black:
			if(oldState != NativeViewer::Black) 
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

