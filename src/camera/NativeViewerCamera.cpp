#include "NativeViewerCamera.h"
#include <QApplication>
#include <QDesktopWidget>

#include "CameraSlideGroup.h"

#include "qvideo/QVideoProvider.h"

NativeViewerCamera::NativeViewerCamera()
	: NativeViewer()
	, m_cameraGroup(0)
	, m_state(NativeViewer::Running)
// 	, m_autoPlay(true)
{

	m_widget = new CameraViewerWidget();
	m_widget->setCursor(Qt::BlankCursor);
	m_widget->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::ToolTip);

}

NativeViewerCamera::~NativeViewerCamera()
{
	if(m_cameraGroup)
		m_cameraGroup->removeNativeViewer(outputId());
	delete m_widget;
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
#ifdef PHONON_ENABLED
	m_widget->hide();
// 	m_media->pause();
#endif
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


void NativeViewerCamera::setState(NativeViewer::NativeShowState state)
{
	//qDebug() << "NativeViewerCamera::setState: state:"<<state<<", start";
	m_state = state;
#ifdef PHONON_ENABLED
	switch(state)
	{
		case NativeViewer::Running:
			show();
			break;
		case NativeViewer::Paused:
// 			m_media->pause();
			break;
		case NativeViewer::Black:
		case NativeViewer::White:
		case NativeViewer::Done:
			hide();
			break;
		default:
			break;
	}
#endif
	//qDebug() << "NativeViewerCamera::setState: state:"<<state<<", done";
}

