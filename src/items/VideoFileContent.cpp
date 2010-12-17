
#ifndef UINT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include "VideoFileContent.h"
#include "frames/Frame.h"
// #include "items/BezierCubicItem.h"
#include "items/TextProperties.h"
#include "model/VideoFileItem.h"
#include "items/CornerItem.h"
#include "AppSettings.h"
// #include "CPixmap.h"
#include "RenderOpts.h"
#include <QDebug>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QMimeData>
#include <QPainter>
#include <QTextDocument>
#include <QTextFrame>
#include <QUrl>
#include <QAbstractTextDocumentLayout>
#include <QDebug>

#define DEBUG_VIDEOFILECONTENT 0
  
VideoFileContent::VideoFileContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
    , m_videoProvider(0)
    , m_still(false)
#ifdef PHONON_ENABLED
    , m_proxy(0)
    , m_player(0)
    , m_tuplet(0)
#endif
{
	m_dontSyncToModel = true;
	
	setFrame(0);
	setFrameTextEnabled(false);
        setToolTip(tr("Video - right click for options."));

	// add play/pause button
	m_bSwap = new ButtonItem(ButtonItem::Control, Qt::blue, QIcon(":/data/action-pause.png"), this);
	m_bSwap->setToolTip(tr("Pause Video"));
	connect(m_bSwap, SIGNAL(clicked()), this, SLOT(slotTogglePlay()));
	addButtonItem(m_bSwap);
	
	m_dontSyncToModel = false;
}

VideoFileContent::~VideoFileContent()
{
	if(m_videoProvider)
		m_videoProvider->disconnectReceiver(this);
	QVideoProvider::releaseProvider(m_videoProvider);
}

void VideoFileContent::slotTogglePlay()
{
	if(m_videoProvider)
	{
		if(m_videoProvider->isPlaying())
		{
			m_bSwap->setToolTip(tr("Pause Video"));
			m_bSwap->setIcon(QIcon(":/data/action-pause.png"));
			m_videoProvider->play();
		}
		else
		{
			m_bSwap->setToolTip(tr("Play Video"));
			m_bSwap->setIcon(QIcon(":/data/action-play.png"));
			m_videoProvider->pause();
		}
	}
}

void VideoFileContent::applySceneContextHint(MyGraphicsScene::ContextHint hint)
{
	AbstractContent::applySceneContextHint(hint);
}

QWidget * VideoFileContent::createPropertyWidget()
{	
	return 0;
}


void VideoFileContent::syncFromModelItem(AbstractVisualItem *model)
{
        if(!modelItem())
		setModelItem(model);
	
	QFont font;
        VideoFileItem * boxmodel = dynamic_cast<VideoFileItem*>(model);
	
	AbstractContent::syncFromModelItem(model);
	
	m_dontSyncToModel = true;
	
	if(DEBUG_VIDEOFILECONTENT)
		qDebug() << "VideoFileContent::syncFromModel(): Got file: "<<model->fillVideoFile();
        setFilename(AppSettings::applyResourcePathTranslations(model->fillVideoFile()));

        m_dontSyncToModel = false;
}

AbstractVisualItem * VideoFileContent::syncToModelItem(AbstractVisualItem *model)
{
	
        VideoFileItem * boxModel = dynamic_cast<VideoFileItem*>(AbstractContent::syncToModelItem(model));
	
	if(!boxModel)
		return 0;
	
	setModelItemIsChanging(true);
	
	if(!filename().isEmpty())
	{
		if(DEBUG_VIDEOFILECONTENT)
			qDebug() << "VideoFileContent::syncToModelItem(): Saving filename: "<<filename();
        	boxModel->setFillVideoFile(filename());
        }
	
	setModelItemIsChanging(false);
	
	return model;
}


#ifdef PHONON_ENABLED
void VideoFileContent::phononStateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
	if(timeLcd)
	{
		switch (newState) {
			case Phonon::ErrorState:
			if (mediaObject->errorType() == Phonon::FatalError) {
				QMessageBox::warning(0, tr("Fatal Error"),
				mediaObject->errorString());
			} else {
				QMessageBox::warning(0, tr("Error"),
				mediaObject->errorString());
			}
			break;
		//![9]
		//![10]
			case Phonon::PlayingState:
				playAction->setEnabled(false);
				pauseAction->setEnabled(true);
				stopAction->setEnabled(true);
				break;
			case Phonon::StoppedState:
				stopAction->setEnabled(false);
				playAction->setEnabled(true);
				pauseAction->setEnabled(false);
				timeLcd->display("00:00");
				break;
			case Phonon::PausedState:
				pauseAction->setEnabled(false);
				stopAction->setEnabled(true);
				playAction->setEnabled(true);
				break;
		//![10]
			case Phonon::BufferingState:
				break;
			default:
			;
		}
	}
}

void VideoFileContent::phononTick(qint64 time)
{
	if(timeLcd)
	{
		QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
		
		timeLcd->display(displayTime.toString("mm:ss"));
	}
}

#endif

void VideoFileContent::setFilename(const QString &name)
{
// 	if(!m_video->load(name))
// 	{
// 		qDebug() << "VideoFileContent::setFilename(): ERROR: Unable to load video"<<name;
// 		return;
// 	}
	if(sceneContextHint() == MyGraphicsScene::StaticPreview)
	{
		setPixmap(QVideoProvider::iconForFile(name));
	}
	else
#ifdef PHONON_ENABLED
	if(modelItem()->videoEndAction() == AbstractVisualItem::VideoStop)
	{
		qDebug() << "VideoFileContent::setVideoFile(): Using Phonon";
// 			if(m_proxy)
// 				delete m_proxy;
// 			if(m_player)
// 				delete m_player;
		if(m_tuplet)
			delete m_tuplet;
		
		if(!m_proxy)
			m_proxy = new QGraphicsProxyWidget(this);
		
		/*
		m_tuplet = QVideoProvider::phononForFile(name);

		m_proxy->setWidget(m_tuplet->video());
		m_proxy->setGeometry(contentsRect());
	
		m_tuplet->media->play();
		*/

		
		m_player = new Phonon::VideoPlayer(Phonon::VideoCategory, 0);
// 		connect(m_player, SIGNAL(finished()), this, SLOT(phononPlayerFinished()));
		
		m_proxy->setWidget(m_player);
		m_proxy->setGeometry(contentsRect());

		m_player->play(name);
		
	}
	else
#endif
	{
		
		QVideoProvider * p = QVideoProvider::providerForFile(name);
		
		if(p)
		{
			if(DEBUG_VIDEOFILECONTENT)
				qDebug() << "VideoFileContent::setFilename(): Loading file:"<<name;
					
			if(m_videoProvider && m_videoProvider == p)
			{
				if(DEBUG_VIDEOFILECONTENT)
					qDebug() << "VideoFileContent::setFilename(): Provider pointer matches existing provider, nothing changed.";
				return;
			}
			else
			if(m_videoProvider)
			{
				if(DEBUG_VIDEOFILECONTENT)
					qDebug() << "VideoFileContent::setFilename(): Disconnecting existing provider";
				m_videoProvider->disconnectReceiver(this);
				QVideoProvider::releaseProvider(m_videoProvider);
			}
			
			if(DEBUG_VIDEOFILECONTENT)
				qDebug() << "VideoFileContent::setFilename(): Loading"<<name;
			
			
			m_still = false;
			
			m_videoProvider = p;
			m_videoProvider->play();
			
			// prime the pump, so to speak
			setPixmap(m_videoProvider->pixmap());
			
			m_videoProvider->connectReceiver(this, SLOT(setPixmap(const QPixmap &)));
		}
		else
		{
			//if(DEBUG_VIDEOFILECONTENT)
				qDebug() << "VideoFileContent::setFilename(): Unable to get provider for "<<name;
		}
	}
}

QPixmap VideoFileContent::renderContent(const QSize & size, Qt::AspectRatioMode /*ratio*/) const
{
	// get the base empty pixmap
	QSize textSize = boundingRect().size().toSize();
	const float w = size.width(),
			h = size.height(),
			tw = textSize.width(),
			th = textSize.height();
	if (w < 2 || h < 2 || tw < 2 || th < 2)
		return QPixmap();
	
	// draw text (centered, maximized keeping aspect ratio)
	float scale = qMin(w / (tw + 16), h / (th + 16));
	QPixmap pix(size);
	pix.fill(Qt::transparent);
	QPainter pixPainter(&pix);
	pixPainter.translate((w - (int)((float)tw * scale)) / 2, (h - (int)((float)th * scale)) / 2);
	pixPainter.scale(scale, scale);
	//m_text->drawContents(&pixPainter);
// 	pixPainter.drawText(0,0,m_text);
	pixPainter.end();
	return pix;
}

int VideoFileContent::contentHeightForWidth(int width) const
{
	// if no image size is available, use default
 	if (m_imageSize.width() < 1 || m_imageSize.height() < 1)
		return AbstractContent::contentHeightForWidth(width);
		
        return (m_imageSize.height() * width) / m_imageSize.width();
}

void VideoFileContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	// paint parent
	AbstractContent::paint(painter, option, widget);
	
	// Scale the video frame to fit the contents rect
	QRect cRect = contentsRect();
        QSize sRect = m_imageSize;
	painter->save();
	painter->translate(cRect.topLeft());
	if (sRect.width() > 0 && sRect.height() > 0)
	{
		qreal xScale = (qreal)cRect.width() / (qreal)sRect.width();
		qreal yScale = (qreal)cRect.height() / (qreal)sRect.height();
			if (!qFuzzyCompare(xScale, 1.0) || !qFuzzyCompare(yScale, 1.0))
			painter->scale(xScale, yScale);
	}
	
	painter->drawPixmap(0,0, m_pixmap);

        painter->restore();
	painter->save();
	
	// Draw a blank gray frame if no video frame present
        if(m_imageSize.width() <= 0)
	{
		QRect fillRect = cRect;
		
        	if(modelItem()->outlineEnabled())
		{
			QPen p = modelItem()->outlinePen();
			if(sceneContextHint() == MyGraphicsScene::StaticPreview)
			{
				QTransform tx = painter->transform();
				qreal scale = qMax(tx.m11(),tx.m22());
				p.setWidthF(1/scale * p.widthF());
			}
				
			painter->setPen(p);
			painter->setBrush(QBrush(Qt::NoBrush));
			
			int w = p.width();
			fillRect.adjust(w/2,w/2,-w/2,-w/2);
		}
		
		painter->fillRect(fillRect,QBrush(Qt::gray));
        }
	
	// Draw an outline around the frame if requested
	if(modelItem()->outlineEnabled())
	{
		QPen p = modelItem()->outlinePen();
		p.setJoinStyle(Qt::MiterJoin);
		if(sceneContextHint() == MyGraphicsScene::StaticPreview)
		{
			QTransform tx = painter->transform();
			qreal scale = qMax(tx.m11(),tx.m22());
			p.setWidthF(1/scale * p.widthF());
		}
			
		painter->setPen(p);
		painter->setBrush(QBrush(Qt::NoBrush));
		painter->drawRect(cRect);
 	}
 	painter->restore();
 	
}

// QRectF VideoFileContent::boundingRect() const 
// {
// 	qreal penWidth = m_modelItem ? m_modelItem->outlinePen().widthF() : 1.0;
// 	return AbstractContent::boundingRect().adjusted(-penWidth/2,-penWidth/2,penWidth,penWidth);
// }

void VideoFileContent::setPixmap(const QPixmap & pixmap)
{
	//qDebug() << "VideFileContent::setPixmap(): hit: m_still:"<<m_still<<", m_imageSize:"<<m_imageSize;
	if(m_still && m_imageSize.width() > 0)
		return;
		
	m_pixmap = pixmap;
	
	if(m_imageSize != m_pixmap.size())
	{
		QSize oldSize = m_imageSize;
		m_imageSize = m_pixmap.size();

		// Adjust scaling while maintaining aspect ratio
// 		qDebug() << "VideoFileContent::setPixmap(): Got new size: "<<m_pixmap.size()<<", old:"<<oldSize<<", OLD contentsRect():"<<contentsRect()<<", pos:"<<pos();
		resizeContents(contentsRect(),true);
// 		qDebug() << "VideoFileContent::setPixmap(): Got new size: "<<m_pixmap.size()<<", old:"<<oldSize<<", NEW contentsRect():"<<contentsRect()<<", pos:"<<pos();
		
		//if(DEBUG_VIDEOFILECONTENT)
		//	qDebug() << "VideFileContent::setPixmap(): new frame size:"<<m_imageSize; 
	}

	update();
	
	if(sceneContextHint() != MyGraphicsScene::Live && 
	   sceneContextHint() != MyGraphicsScene::Preview && 
	   m_imageSize.width() > 0)
	{
		if(DEBUG_VIDEOFILECONTENT)
			qDebug() << "VideFileContent::setPixmap(): sceneContextHint() != Live/Preview, setting m_still true"; 
		m_still = true;
		if(m_videoProvider)
			m_videoProvider->pause();
	}
        //GFX_CHANGED();
}


QWidget * VideoFileContent::controlWidget()
{
#ifdef PHONON_ENABLED
	if(!m_player)
		return 0;
		
	mediaObject = m_player->mediaObject();

	mediaObject->setTickInterval(1000);

	connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(phononTick(qint64)));
	connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(phononStateChanged(Phonon::State, Phonon::State)));
	
	QWidget * baseWidget = new QWidget;

	playAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), baseWidget);
	playAction->setShortcut(tr("Crl+P"));
	playAction->setDisabled(true);
	pauseAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), baseWidget);
	pauseAction->setShortcut(tr("Ctrl+A"));
	pauseAction->setDisabled(true);
	stopAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), baseWidget);
	stopAction->setShortcut(tr("Ctrl+S"));
	stopAction->setDisabled(true);
    
    
	connect(playAction, SIGNAL(triggered()), mediaObject, SLOT(play()));
	connect(pauseAction, SIGNAL(triggered()), mediaObject, SLOT(pause()) );
	connect(stopAction, SIGNAL(triggered()), mediaObject, SLOT(stop()));
	
	QToolBar *bar = new QToolBar(baseWidget);

	bar->addAction(playAction);
	bar->addAction(pauseAction);
	bar->addAction(stopAction);
	
	seekSlider = new Phonon::SeekSlider(baseWidget);
	seekSlider->setMediaObject(mediaObject);
	
	volumeSlider = new Phonon::VolumeSlider(baseWidget);
	volumeSlider->setAudioOutput(m_player->audioOutput());
	
	volumeSlider->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	
	QLabel *volumeLabel = new QLabel(baseWidget);
	volumeLabel->setPixmap(QPixmap(":/data/stock-volume.png"));
	
	QPalette palette;
	palette.setBrush(QPalette::Light, Qt::darkGray);
	
	timeLcd = new QLCDNumber(baseWidget);
	timeLcd->setPalette(palette);
	
	QHBoxLayout *playbackLayout = new QHBoxLayout(baseWidget);
	playbackLayout->setMargin(0);
	
	playbackLayout->addWidget(bar);
// 	playbackLayout->addStretch();
	playbackLayout->addWidget(seekSlider);
 	playbackLayout->addWidget(timeLcd);

	playbackLayout->addWidget(volumeLabel);
	playbackLayout->addWidget(volumeSlider);
	
	timeLcd->display("00:00"); 
	
	qDebug() << "BackgroundContent::controlWidget(): baseWidget:"<<baseWidget;
	
	return baseWidget;
#else
	return 0;
#endif
}

