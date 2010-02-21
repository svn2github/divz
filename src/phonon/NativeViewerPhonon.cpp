#include "NativeViewerPhonon.h"
#include <QApplication>
#include <QDesktopWidget>

#include "VideoSlideGroup.h"

#include "qvideo/QVideoProvider.h"

NativeViewerPhonon::NativeViewerPhonon()
	: NativeViewer()
	, m_videoGroup(0)
{
#ifdef PHONON_ENABLED
	m_player = new Phonon::VideoPlayer(Phonon::VideoCategory, 0);
	m_player->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
#endif 
}

NativeViewerPhonon::~NativeViewerPhonon()
{
#ifdef PHONON_ENABLED
	delete m_player;
#endif
}

void NativeViewerPhonon::setSlideGroup(SlideGroup *group)
{
	m_slideGroup = group;
	m_videoGroup = dynamic_cast<VideoSlideGroup*>(group);
}

void NativeViewerPhonon::show()
{
	QRect rect = containerWidget()->geometry();
	QPoint abs = WidgetUtil::absoluteWidgetPosition(containerWidget());
#ifdef PHONON_ENABLED
	m_player->play();
	m_player->resize(rect.size());
	m_player->move(abs);
	m_player->show();
#endif
}

void NativeViewerPhonon::close()
{
	hide();
}

void NativeViewerPhonon::hide()
{
#ifdef PHONON_ENABLED
	m_player->hide();
	m_player->pause();
#endif
}

QPixmap NativeViewerPhonon::snapshot()
{
// 	QRect rect = containerWidget()->geometry();
// 	QPoint abs = WidgetUtil::absoluteWidgetPosition(containerWidget());
// 	return QPixmap::grabWindow(qApp->desktop()->winId(), abs.x(), abs.y(), rect.width(), rect.height());
	if(m_videoGroup)
		return QVideoProvider::iconForFile(m_videoGroup->file());
	else
		return QPixmap();
}

void NativeViewerPhonon::setSlide(int x)
{
	// irrelevant 
}


void NativeViewerPhonon::setState(NativeViewer::NativeShowState state)
{
	m_state = state;
#ifdef PHONON_ENABLED
	switch(state)
	{
		case NativeViewer::Running:
			show();
			break;
		case NativeViewer::Paused:
			m_player->pause();
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
}

