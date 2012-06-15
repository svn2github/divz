#include "NativeViewerPhonon.h"
#include <QApplication>
#include <QDesktopWidget>

#include "VideoSlideGroup.h"

#ifdef DVIZ_HAS_QVIDEO
#include "qvideo/QVideoProvider.h"
#endif

NativeViewerPhonon::NativeViewerPhonon(bool makeReusable)
	: NativeViewer()
	, m_videoGroup(0)
	, m_state(NativeViewer::Running)
	, m_autoPlay(true)
	, m_isReusable(makeReusable)
{
#ifdef PHONON_ENABLED
	m_media  = new Phonon::MediaObject(this);
	m_audio  = new Phonon::AudioOutput(Phonon::MusicCategory, this);
	m_widget = new Phonon::VideoWidget(0);
	Phonon::createPath(m_media, m_audio);
	Phonon::createPath(m_media, m_widget);

	m_widget->setCursor(Qt::BlankCursor);
	m_widget->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::ToolTip);
#endif 
}

NativeViewerPhonon::~NativeViewerPhonon()
{
#ifdef PHONON_ENABLED
	if(m_videoGroup)
		m_videoGroup->removeNativeViewer(outputId());
	delete m_widget;
#endif
}

void NativeViewerPhonon::dispose()
{
	if(!m_isReusable)
		NativeViewer::dispose();
	else
		close();
}

void NativeViewerPhonon::setSlideGroup(SlideGroup *group)
{
	m_slideGroup = group;
	m_videoGroup = dynamic_cast<VideoSlideGroup*>(group);
	m_videoGroup->addNativeViewer(outputId(),this);
#ifdef PHONON_ENABLED
	//qDebug() << "NativeViewerPhonon::setSlideGroup: Loading file:"<<m_videoGroup->file();
	m_media->enqueue(m_videoGroup->file());
	//qDebug() << "NativeViewerPhonon::setSlideGroup: File loaded";
#endif
}

void NativeViewerPhonon::show()
{
	if(state() != NativeViewer::Running)
		return;

	QRect rect = containerWidget()->geometry();
	QPoint abs = WidgetUtil::absoluteWidgetPosition(containerWidget());
	//qDebug() << "NativeViewerPhonon::show: Showing at "<<abs<<", size:"<<rect.size();
#ifdef PHONON_ENABLED
	if(m_autoPlay)
		m_media->play();
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

	//qDebug() << "NativeViewerPhonon::show: done.";
}

void NativeViewerPhonon::close()
{
	hide();
}

void NativeViewerPhonon::hide()
{
	//qDebug() << "NativeViewerPhonon::close: mark start";
#ifdef PHONON_ENABLED
	m_widget->hide();
	m_media->pause();
#endif
	//qDebug() << "NativeViewerPhonon::close: mark end";
}

QPixmap NativeViewerPhonon::snapshot()
{
// 	QRect rect = containerWidget()->geometry();
// 	QPoint abs = WidgetUtil::absoluteWidgetPosition(containerWidget());
// 	return QPixmap::grabWindow(qApp->desktop()->winId(), abs.x(), abs.y(), rect.width(), rect.height());
	#ifdef DVIZ_HAS_QVIDEO
	if(m_videoGroup)
		return QVideoProvider::iconForFile(m_videoGroup->file());
	else
	#endif
		return QPixmap();
}

void NativeViewerPhonon::setSlide(int x)
{
	// irrelevant 
}


void NativeViewerPhonon::setState(NativeViewer::NativeShowState state)
{
	//qDebug() << "NativeViewerPhonon::setState: state:"<<state<<", start";
	m_state = state;
#ifdef PHONON_ENABLED
	switch(state)
	{
		case NativeViewer::Running:
			show();
			break;
		case NativeViewer::Paused:
			m_media->pause();
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
	//qDebug() << "NativeViewerPhonon::setState: state:"<<state<<", done";
}

