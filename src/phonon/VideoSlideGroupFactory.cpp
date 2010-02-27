#include "VideoSlideGroupFactory.h"

#include "VideoSlideGroup.h"

//#include "OutputInstance.h"
//#include "model/Output.h"
//#include "model/TextBoxItem.h"

#include "NativeViewerPhonon.h"
#include "model/Output.h"
/*
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>*/

#include "OutputInstance.h"
#include <QMessageBox>

#include <assert.h>

/** VideoSlideGroupFactory:: **/

VideoSlideGroupFactory::VideoSlideGroupFactory() : SlideGroupFactory() {}
/*VideoSlideGroupFactory::~VideoSlideGroupFactory()
{
}*/
	
SlideGroup * VideoSlideGroupFactory::newSlideGroup()
{
	return dynamic_cast<SlideGroup*>(new VideoSlideGroup());
}

AbstractSlideGroupEditor * VideoSlideGroupFactory::newEditor()
{
	// In futre: Call out to the system to launch native video editor with the file as argument?
	return 0;
}

NativeViewer * VideoSlideGroupFactory::newNativeViewer()
{
	return new NativeViewerPhonon();
}

SlideGroupViewControl * VideoSlideGroupFactory::newViewControl()
{
	return new VideoSlideGroupViewControl();
}


/// VideoSlideGroupViewControl Implementation
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QToolBar>
#include <QLabel>

VideoSlideGroupViewControl::VideoSlideGroupViewControl(OutputInstance *inst, QWidget *parent)
    : SlideGroupViewControl(inst,parent,false)
    , m_mediaObject(0)
{
	QVBoxLayout * layout = new QVBoxLayout(this);

	QWidget * baseWidget = new QWidget;
	
	m_playingLabel = new QLabel(this);
	
	layout->addWidget(m_playingLabel);
	layout->addWidget(baseWidget);
	layout->addStretch(1);

	m_playAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), baseWidget);
	m_playAction->setShortcut(tr("Crl+P"));
	m_playAction->setDisabled(true);
	m_pauseAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), baseWidget);
	m_pauseAction->setShortcut(tr("Ctrl+A"));
	m_pauseAction->setDisabled(true);
	m_stopAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), baseWidget);
	m_stopAction->setShortcut(tr("Ctrl+S"));
	m_stopAction->setDisabled(true);
	
	m_loopAction = new QAction(QPixmap(":/data/stock-redo.png"), tr("Loop"), baseWidget);
	m_loopAction->setShortcut(tr("Ctrl+L"));
	m_loopAction->setCheckable(true);
	
	connect(m_loopAction, SIGNAL(toggled(bool)), this, SLOT(loopActionToggled(bool)));

	QToolBar *bar = new QToolBar(baseWidget);

	bar->addAction(m_playAction);
	bar->addAction(m_pauseAction);
	bar->addAction(m_stopAction);
	bar->addAction(m_loopAction);

	m_seekSlider   = new Phonon::SeekSlider(baseWidget);
	m_volumeSlider = new Phonon::VolumeSlider(baseWidget);

	//m_volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

// 	QLabel *volumeLabel = new QLabel(baseWidget);
// 	volumeLabel->setPixmap(QPixmap(":/data/stock-volume.png"));

	QPalette palette;
	palette.setBrush(QPalette::Light, Qt::darkGray);

	m_timeLcd = new QLCDNumber(baseWidget);
	m_timeLcd->setPalette(palette);

	QHBoxLayout *playbackLayout = new QHBoxLayout(baseWidget);
	playbackLayout->setMargin(0);

	playbackLayout->addWidget(bar);
// 	playbackLayout->addStretch();
	playbackLayout->addWidget(m_seekSlider);
	playbackLayout->addWidget(m_timeLcd);

// 	playbackLayout->addWidget(volumeLabel);
	playbackLayout->addWidget(m_volumeSlider);

	m_timeLcd->display("00:00");

	m_controlBase = baseWidget;
}

void VideoSlideGroupViewControl::setIsPreviewControl(bool flag)
{
	SlideGroupViewControl::setIsPreviewControl(flag);
	m_controlBase->setVisible(!flag);
	if(flag)
	    setMaximumHeight(0);
}

void VideoSlideGroupViewControl::setSlideGroup(SlideGroup *g, Slide *curSlide, bool allowProgressDialog)
{
	SlideGroupViewControl::setSlideGroup(g);

	if(m_releasingSlideGroup)
		return;

	m_slideViewer->setSlideGroup(m_group);
	
	m_videoGroup = dynamic_cast<VideoSlideGroup*>(g);
	m_mediaObject = 0;
	if(!m_videoGroup)
	{
		qDebug() << "VideoSlideGroupViewControl::setSlideGroup(): Unable to cast group to VideoSlideGroup";
		return;
	}

	NativeViewerPhonon * native = m_videoGroup->nativeViewer(view()->output()->id());
	if(!native)
	{
		qDebug() << "VideoSlideGroupViewControl::setSlideGroup(): No native viewer on video group yet, cannot init slots";
		return;
	}
	
	m_playingLabel->setText(QString("Video: <b>%1</b>").arg(QFileInfo(m_videoGroup->file()).fileName()));
	
	m_mediaObject = native->mediaObject();
	m_mediaObject->setTickInterval(1000);

	connect(m_mediaObject, SIGNAL(tick(qint64)), this, SLOT(phononTick(qint64)));
	connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(phononStateChanged(Phonon::State, Phonon::State)));
	connect(m_mediaObject, SIGNAL(finished()), this, SLOT(phononPlayerFinished()));

	connect(m_playAction, SIGNAL(triggered()), m_mediaObject, SLOT(play()));
	connect(m_pauseAction, SIGNAL(triggered()), m_mediaObject, SLOT(pause()) );
	connect(m_stopAction, SIGNAL(triggered()), m_mediaObject, SLOT(stop()));
	
	m_seekSlider->setMediaObject(m_mediaObject);
	m_volumeSlider->setAudioOutput(native->audioOutput());
	
	m_loopAction->setChecked(m_videoGroup->endOfGroupAction() == SlideGroup::LoopToStart);
	
	connect(m_videoGroup, SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)), this, SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));
}

void VideoSlideGroupViewControl::slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value)
{
	if(fieldName == "endOfGroupAction")
	{
		m_loopAction->setChecked(m_videoGroup->endOfGroupAction() == SlideGroup::LoopToStart);
	}
}

void VideoSlideGroupViewControl::phononPlayerFinished()
{
	if(!m_videoGroup)
		return;
		
	SlideGroup::EndOfGroupAction action = m_videoGroup->endOfGroupAction();
	if(action == SlideGroup::LoopToStart)
	{
		if(m_videoGroup && m_videoGroup->nativeViewer(view()->output()->id()))
		{
			Phonon::MediaObject * media = m_videoGroup->nativeViewer(view()->output()->id())->mediaObject();
			qDebug() << "VideoSlideGroupViewControl::phononPlayerFinished(): playing file again";
			media->seek(0);
			media->play();
		}
	}
	else
	{
		// This will allow the signals that trigger going to the next group to be emitted 
		// if 'action' requires it.
		m_slideViewer->nextSlide();
	}
	
}

void VideoSlideGroupViewControl::loopActionToggled(bool flag)
{
	if(!m_videoGroup)
		return;
	
	m_videoGroup->setEndOfGroupAction(flag ? SlideGroup::LoopToStart : SlideGroup::Stop);
}



void VideoSlideGroupViewControl::phononStateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
	if(!m_mediaObject)
		return;
	if(m_timeLcd)
	{
		switch (newState) {
			case Phonon::ErrorState:
			if (m_mediaObject->errorType() == Phonon::FatalError)
			{
				QMessageBox::warning(0, tr("Fatal Error"), m_mediaObject->errorString());
			}
			else
			{
				QMessageBox::warning(0, tr("Error"), m_mediaObject->errorString());
			}
			break;
		//![9]
		//![10]
			case Phonon::PlayingState:
				m_playAction->setEnabled(false);
				m_pauseAction->setEnabled(true);
				m_stopAction->setEnabled(true);
				break;
			case Phonon::StoppedState:
				m_stopAction->setEnabled(false);
				m_playAction->setEnabled(true);
				m_pauseAction->setEnabled(false);
				m_timeLcd->display("00:00");
				break;
			case Phonon::PausedState:
				m_pauseAction->setEnabled(false);
				m_stopAction->setEnabled(true);
				m_playAction->setEnabled(true);
				break;
		//![10]
			case Phonon::BufferingState:
				break;
			default:
			;
		}
	}
}

void VideoSlideGroupViewControl::phononTick(qint64 time)
{
	if(!m_mediaObject)
		return;
	if(m_timeLcd)
	{
		QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);

		m_timeLcd->display(displayTime.toString("mm:ss"));
	}
}
