#include "CameraSlideGroupFactory.h"

#include "CameraSlideGroup.h"

//#include "OutputInstance.h"
//#include "model/Output.h"
//#include "model/TextBoxItem.h"

#include "NativeViewerCamera.h"
#include "model/Output.h"
#include "OutputControl.h"
#include "MainWindow.h" // to access MainWindow::outputControl(...)

#include "livemix/VideoWidget.h"

#include "OutputInstance.h"
#include <QMessageBox>

#include <assert.h>

/** CameraSlideGroupFactory:: **/

CameraSlideGroupFactory::CameraSlideGroupFactory() : SlideGroupFactory() {}
/*CameraSlideGroupFactory::~CameraSlideGroupFactory()
{
}*/
	
SlideGroup * CameraSlideGroupFactory::newSlideGroup()
{
	return dynamic_cast<SlideGroup*>(new CameraSlideGroup());
}

AbstractSlideGroupEditor * CameraSlideGroupFactory::newEditor()
{
	// In futre: Call out to the system to launch native video editor with the file as argument?
	return 0;
}

NativeViewer * CameraSlideGroupFactory::newNativeViewer(OutputInstance */*instance*/)
{
	return new NativeViewerCamera();
}

SlideGroupViewControl * CameraSlideGroupFactory::newViewControl()
{
	return new CameraSlideGroupViewControl();
}


/// CameraSlideGroupViewControl Implementation
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QToolBar>
#include <QLabel>

CameraSlideGroupViewControl::CameraSlideGroupViewControl(OutputInstance *inst, QWidget *parent)
    : SlideGroupViewControl(inst,parent,false)
//     , m_mediaObject(0)
//     , m_syncedMediaObject(0)
    , m_native(0)
{
	QVBoxLayout * layout = new QVBoxLayout(this);

	QWidget * baseWidget = new QWidget;
	
	QHBoxLayout * hbox1 = new QHBoxLayout(baseWidget);
	hbox1->setMargin(0);

	QLabel *label = new QLabel("Text Overlay:");
	hbox1->addWidget(label);

	m_overlayText = new QLineEdit();
	hbox1->addWidget(m_overlayText);
	connect(m_overlayText, SIGNAL(returnPressed()), this, SLOT(showOverlayText()));

	QPushButton * btn = new QPushButton("Show");
	btn->setCheckable(true);
	hbox1->addWidget(btn);
	connect(btn, SIGNAL(toggled(bool)), this, SLOT(showOverlayText(bool)));
	//m_showQuickSlideBtn = btn;
	
 	m_playingLabel = new QLabel(this);
// 	
 	layout->addWidget(m_playingLabel);
 	layout->addWidget(baseWidget);
 	layout->addStretch(1);
// 
// 	m_playAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), baseWidget);
// 	m_playAction->setShortcut(tr("Crl+P"));
// 	m_playAction->setDisabled(true);
// 	m_pauseAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), baseWidget);
// 	m_pauseAction->setShortcut(tr("Ctrl+A"));
// 	m_pauseAction->setDisabled(true);
// 	m_stopAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), baseWidget);
// 	m_stopAction->setShortcut(tr("Ctrl+S"));
// 	m_stopAction->setDisabled(true);
// 	
// 	m_loopAction = new QAction(QPixmap(":/data/stock-redo.png"), tr("Loop"), baseWidget);
// 	m_loopAction->setShortcut(tr("Ctrl+L"));
// 	m_loopAction->setCheckable(true);
// 	
// 	connect(m_loopAction, SIGNAL(toggled(bool)), this, SLOT(loopActionToggled(bool)));
// 
// 	QToolBar *bar = new QToolBar(baseWidget);
// 
// 	bar->addAction(m_playAction);
// 	bar->addAction(m_pauseAction);
// 	bar->addAction(m_stopAction);
// 	bar->addAction(m_loopAction);
// 
// 	m_seekSlider   = new Phonon::SeekSlider(baseWidget);
// 	m_volumeSlider = new Phonon::VolumeSlider(baseWidget);
// 
// 	//m_volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
// 
// // 	QLabel *volumeLabel = new QLabel(baseWidget);
// // 	volumeLabel->setPixmap(QPixmap(":/data/stock-volume.png"));
// 
// 	QPalette palette;
// 	palette.setBrush(QPalette::Light, Qt::darkGray);
// 
// 	m_timeLcd = new QLCDNumber(baseWidget);
// 	m_timeLcd->setPalette(palette);
// 
// 	QHBoxLayout *playbackLayout = new QHBoxLayout(baseWidget);
// 	playbackLayout->setMargin(0);
// 
// 	playbackLayout->addWidget(bar);
// // 	playbackLayout->addStretch();
// 	playbackLayout->addWidget(m_seekSlider);
// 	playbackLayout->addWidget(m_timeLcd);
// 
// // 	playbackLayout->addWidget(volumeLabel);
// 	playbackLayout->addWidget(m_volumeSlider);
// 
// 	m_timeLcd->display("00:00");

	//m_controlBase = baseWidget;
}

void CameraSlideGroupViewControl::setIsPreviewControl(bool flag)
{
	SlideGroupViewControl::setIsPreviewControl(flag);
 	//m_controlBase->setVisible(!flag);
// 	if(flag)
// 	    setMaximumHeight(0);
}

void CameraSlideGroupViewControl::showOverlayText(bool flag)
{
	if(!m_native)
		return;
		
	if(flag)
	{
		m_native->cameraViewer()->setOverlayText(m_overlayText->text());
		m_overlayText->selectAll();
		m_native->cameraViewer()->showOverlayText(true);
	}
	else
		m_native->cameraViewer()->showOverlayText(false);
}

void CameraSlideGroupViewControl::setSlideGroup(SlideGroup *g, Slide *curSlide, bool allowProgressDialog)
{
	SlideGroupViewControl::setSlideGroup(g);

	if(m_releasingSlideGroup)
		return;

	m_slideViewer->setSlideGroup(m_group);
	
	m_cameraGroup = dynamic_cast<CameraSlideGroup*>(g);
// 	m_mediaObject = 0;
	if(!m_cameraGroup)
	{
		qDebug() << "CameraSlideGroupViewControl::setSlideGroup(): Unable to cast group to CameraSlideGroup";
		return;
	}

	m_native = m_cameraGroup->nativeViewer(view()->output()->id());
	if(!m_native)
	{
		qDebug() << "CameraSlideGroupViewControl::setSlideGroup(): No native viewer on camera group yet, cannot init slots";
		return;
	}
	
	//m_playingLabel->setText(QString("Camera: <b>%1</b>").arg(QFileInfo(m_cameraGroup->device()).fileName()));
	m_playingLabel->setText(QString("Camera: <b>%1</b>").arg(m_cameraGroup->device()));
	
// 	m_mediaObject = native->mediaObject();
// 	m_mediaObject->setTickInterval(1000);
	
	// HACK this connecting to the output control, setting sync connectiosn, etc, really should be done in a 
	// specific function - such as setOutputControl(), etc - for now, this can be here, because we "know"
	// that setSlideGroup will only be called once in the life of this view control due to program design
	
// 	OutputControl * ctrl = MainWindow::mw()->outputControl(view()->output()->id());
// 	if(ctrl)
// 	{
// 		// we have control, connect to sync changed signal
// 		connect(ctrl, SIGNAL(outputIsSyncedChanged(bool)), this, SLOT(outputIsSyncedChanged()));
// 		// trigger the connection
// 		outputIsSyncedChanged();
// 	}
	
	
// 	connect(m_mediaObject, SIGNAL(tick(qint64)), this, SLOT(phononTick(qint64)));
// 	connect(m_mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(phononStateChanged(Phonon::State, Phonon::State)));
// 	connect(m_mediaObject, SIGNAL(finished()), this, SLOT(phononPlayerFinished()));
// 
// 	connect(m_playAction, SIGNAL(triggered()), m_mediaObject, SLOT(play()));
// 	connect(m_pauseAction, SIGNAL(triggered()), m_mediaObject, SLOT(pause()) );
// 	connect(m_stopAction, SIGNAL(triggered()), m_mediaObject, SLOT(stop()));
// 	
// 	m_seekSlider->setMediaObject(m_mediaObject);
// 	m_volumeSlider->setAudioOutput(native->audioOutput());
// 	
// 	if(m_isPreviewControl)
// 	{
// 		native->setAutoPlay(false);
// 		native->audioOutput()->setMuted(true);
// 		m_mediaObject->pause();
// 	}
// 	
// 	
// 	
// 	
// 	m_loopAction->setChecked(m_cameraGroup->endOfGroupAction() == SlideGroup::LoopToStart);
// 	
// 	connect(m_cameraGroup, SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)), this, SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));
}

QPixmap	CameraSlideGroupFactory::generatePreviewPixmap(SlideGroup */*group*/, QSize iconSize, QRect /*sceneRect*/)
{
	return QPixmap(":/data/stock-panel-screenshot.png").scaled(iconSize);
}



// void CameraSlideGroupViewControl::outputIsSyncedChanged()
// {
// 	// check sync status
// 	// if synced,
// 	// get sync source and connect to the state changed and tick signals to update/sync our media object
// 	OutputControl * ctrl = MainWindow::mw()->outputControl(view()->output()->id());
// 	if(!ctrl)
// 	{
// 		qDebug() << "CameraSlideGroupViewControl::outputIsSyncedChanged(): No OutputControl for output ID"<<view()->output()->id();
// 		return;
// 	}
// 	
// 	if(!ctrl->outputIsSynced())
// 	{
// 		if(m_syncedMediaObject)
// 		{
// 			if(m_mediaObject)
// 				disconnect(m_syncedMediaObject,0,m_mediaObject,0);
// 			disconnect(m_syncedMediaObject,0,this,0);
// 			qDebug() << "CameraSlideGroupViewControl::outputIsSyncedChanged(): Disconnected from synced output";
// 			m_syncedMediaObject = 0; 
// 		}
// 		
// 		return;
// 	}
// 	
// 	OutputInstance * syncSource = ctrl->syncSource();
// 	if(!syncSource)
// 	{
// 		qDebug() << "CameraSlideGroupViewControl::outputIsSyncedChanged(): Cant get sync source";
// 		return;
// 	}
// 	
// 	OutputControl * syncedCtrl = MainWindow::mw()->outputControl(syncSource->output()->id());
// 	if(!syncedCtrl)
// 	{
// 		qDebug() << "CameraSlideGroupViewControl::outputIsSyncedChanged(): Cant get output control for sync source";
// 		return;
// 	}
// 	
// 	SlideGroupViewControl * slideVideCtrl = syncedCtrl->viewControl();
// 	if(!slideVideCtrl)
// 	{
// 		qDebug() << "CameraSlideGroupViewControl::outputIsSyncedChanged(): OutputControl::viewControl() is null";
// 		return;
// 	}
// 	
// 	CameraSlideGroupViewControl * videoCtrl = dynamic_cast<CameraSlideGroupViewControl*>(slideVideCtrl);
// 	if(!videoCtrl)
// 	{
// 		qDebug() << "CameraSlideGroupViewControl::outputIsSyncedChanged(): Cannot cast slideVideCtrl to CameraSlideGroupViewControl";
// 		return;
// 	}
// 		
// 	if(!videoCtrl->mediaObject())
// 	{
// 		qDebug() << "CameraSlideGroupViewControl::outputIsSyncedChanged(): videoCtrl->mediaObject() is NULL";
// 		return;
// 	}
// 		
// 	m_syncedMediaObject = videoCtrl->mediaObject();
// 		
// 	if(m_mediaObject)
// 		connect(m_syncedMediaObject, SIGNAL(tick(qint64)), m_mediaObject, SLOT(seek(qint64)));
// 	else
// 		qDebug() << "CameraSlideGroupViewControl::outputIsSyncedChanged(): Our own m_mediaObject is null, cannot sync ticks";
// 		
// 	connect(m_syncedMediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(syncedPhononStateChanged(Phonon::State, Phonon::State)));
// 	
// 	qDebug() << "CameraSlideGroupViewControl::outputIsSyncedChanged(): Connected to main output";
// }

// void CameraSlideGroupViewControl::slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value)
// {
// 	if(fieldName == "endOfGroupAction")
// 	{
// 		m_loopAction->setChecked(m_cameraGroup->endOfGroupAction() == SlideGroup::LoopToStart);
// 	}
// }

// void CameraSlideGroupViewControl::phononPlayerFinished()
// {
// 	if(!m_cameraGroup)
// 		return;
// 		
// 	SlideGroup::EndOfGroupAction action = m_cameraGroup->endOfGroupAction();
// 	if(action == SlideGroup::LoopToStart)
// 	{
// 		if(m_cameraGroup && m_cameraGroup->nativeViewer(view()->output()->id()))
// 		{
// 			Phonon::MediaObject * media = m_cameraGroup->nativeViewer(view()->output()->id())->mediaObject();
// 			qDebug() << "CameraSlideGroupViewControl::phononPlayerFinished(): playing file again";
// 			media->seek(0);
// 			media->play();
// 		}
// 	}
// 	else
// 	{
// 		// This will allow the signals that trigger going to the next group to be emitted 
// 		// if 'action' requires it.
// 		m_slideViewer->nextSlide();
// 	}
// 	
// }
// 
// void CameraSlideGroupViewControl::loopActionToggled(bool flag)
// {
// 	if(!m_cameraGroup)
// 		return;
// 	
// 	m_cameraGroup->setEndOfGroupAction(flag ? SlideGroup::LoopToStart : SlideGroup::Stop);
// }
// 
// 
// void CameraSlideGroupViewControl::syncedPhononStateChanged(Phonon::State newState, Phonon::State /* oldState */)
// {
// 	qDebug() << "CameraSlideGroupViewControl::syncedPhononStateChanged: newState:"<<newState;
// 	if(!m_mediaObject)
// 		return;
// 	switch (newState) 
// 	{
// 		case Phonon::ErrorState:
// 		break;
// 		case Phonon::PlayingState:
// 			m_mediaObject->play();
// 			break;
// 		case Phonon::StoppedState:
// 			m_mediaObject->stop();
// 			break;
// 		case Phonon::PausedState:
// 			m_mediaObject->pause();
// 			break;
// 		case Phonon::BufferingState:
// 			break;
// 		default:
// 		;
// 	}
// }
// 
// void CameraSlideGroupViewControl::phononStateChanged(Phonon::State newState, Phonon::State /* oldState */)
// {
// 	//qDebug() << "CameraSlideGroupViewControl::phononStateChanged: newState:"<<newState;
// 	if(!m_mediaObject)
// 		return;
// 	if(m_timeLcd)
// 	{
// 		switch (newState) {
// 			case Phonon::ErrorState:
// 			if (m_mediaObject->errorType() == Phonon::FatalError)
// 			{
// 				QMessageBox::warning(0, tr("Fatal Error"), m_mediaObject->errorString());
// 			}
// 			else
// 			{
// 				QMessageBox::warning(0, tr("Error"), m_mediaObject->errorString());
// 			}
// 			break;
// 		//![9]
// 		//![10]
// 			case Phonon::PlayingState:
// 				m_playAction->setEnabled(false);
// 				m_pauseAction->setEnabled(true);
// 				m_stopAction->setEnabled(true);
// 				break;
// 			case Phonon::StoppedState:
// 				m_stopAction->setEnabled(false);
// 				m_playAction->setEnabled(true);
// 				m_pauseAction->setEnabled(false);
// 				m_timeLcd->display("00:00");
// 				break;
// 			case Phonon::PausedState:
// 				m_pauseAction->setEnabled(false);
// 				m_stopAction->setEnabled(true);
// 				m_playAction->setEnabled(true);
// 				break;
// 		//![10]
// 			case Phonon::BufferingState:
// 				break;
// 			default:
// 			;
// 		}
// 	}
// }
// 
// void CameraSlideGroupViewControl::phononTick(qint64 time)
// {
// 	if(!m_mediaObject)
// 		return;
// 	if(m_timeLcd)
// 	{
// 		QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
// 
// 		m_timeLcd->display(displayTime.toString("mm:ss"));
// 	}
// }
