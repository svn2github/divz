#include "VideoFileContent.h"
#include "frames/Frame.h"
// #include "items/BezierCubicItem.h"
#include "items/TextProperties.h"
#include "model/VideoFileItem.h"
#include "items/CornerItem.h"
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
//     , m_text(0)
//     , m_textRect(0, 0, 0, 0)
//     , m_textMargin(4)
{
	m_dontSyncToModel = true;
	
	setFrame(0);
	setFrameTextEnabled(false);
        setToolTip(tr("Video - right click for options."));
	
// 	setText("Welcome");
        //for(int i=0;i<m_cornerItems.size();i++)
        //	m_cornerItems.at(i)->setDefaultLeftOp(CornerItem::Scale);

//         m_video = new QVideo(this);
//         //connect(m_video, SIGNAL(movieStateChanged(QMovie::MovieState)),
//         //           this, SLOT(movieStateChanged(QMovie::MovieState)));
//         connect(m_video, SIGNAL(currentFrame(QFFMpegVideoFrame)),
//                      this, SLOT(setVideoFrame(QFFMpegVideoFrame)));

	// add play/pause button
	m_bSwap = new ButtonItem(ButtonItem::Control, Qt::blue, QIcon(":/data/action-pause.png"), this);
	m_bSwap->setToolTip(tr("Pause Video"));
	connect(m_bSwap, SIGNAL(clicked()), this, SLOT(slotTogglePlay()));
	addButtonItem(m_bSwap);
	
	m_dontSyncToModel = false;
}

VideoFileContent::~VideoFileContent()
{
// 	delete m_shapeEditor;
// 	delete m_text;
	m_videoProvider->disconnectReceiver(this);
	QVideoProvider::releaseProvider(m_videoProvider);
}

void VideoFileContent::slotTogglePlay()
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

void VideoFileContent::applySceneContextHint(MyGraphicsScene::ContextHint hint)
{
	AbstractContent::applySceneContextHint(hint);
}

QWidget * VideoFileContent::createPropertyWidget()
{
/*	TextProperties * p = new TextProperties();
	
	// common properties
	connect(p->bFront, SIGNAL(clicked()), this, SLOT(slotStackFront()));
	connect(p->bRaise, SIGNAL(clicked()), this, SLOT(slotStackRaise()));
	connect(p->bLower, SIGNAL(clicked()), this, SLOT(slotStackLower()));
	connect(p->bBack, SIGNAL(clicked()), this, SLOT(slotStackBack()));
	connect(p->bDel, SIGNAL(clicked()), this, SIGNAL(deleteItem()), Qt::QueuedConnection);
	
	// shape properties
	p->bEditShape->setChecked(isShapeEditing());
	connect(this, SIGNAL(notifyShapeEditing(bool)), p->bEditShape, SLOT(setChecked(bool)));
	connect(p->bEditShape, SIGNAL(toggled(bool)), this, SLOT(setShapeEditing(bool)));
	p->bClearShape->setVisible(hasShape());
	connect(this, SIGNAL(notifyHasShape(bool)), p->bClearShape, SLOT(setVisible(bool)));
	connect(p->bClearShape, SIGNAL(clicked()), this, SLOT(clearShape()));
	
	return p;*/
	
	return 0;
}


void VideoFileContent::syncFromModelItem(AbstractVisualItem *model)
{
        if(!modelItem())
		setModelItem(model);
	
	QFont font;
        VideoFileItem * boxmodel = dynamic_cast<VideoFileItem*>(model);
	
// 	font.setFamily(textModel->fontFamily());
// 	font.setPointSize((int)textModel->fontSize());
// 	setFont(font);
	
	AbstractContent::syncFromModelItem(model);
	
	m_dontSyncToModel = true;
	
	if(DEBUG_VIDEOFILECONTENT)
		qDebug() << "VideoFileContent::syncFromModel(): Got file: "<<model->fillVideoFile();
        setFilename(model->fillVideoFile());
	
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


void VideoFileContent::setFilename(const QString &name)
{
// 	if(!m_video->load(name))
// 	{
// 		qDebug() << "VideoFileContent::setFilename(): ERROR: Unable to load video"<<name;
// 		return;
// 	}
	QVideoProvider * p = QVideoProvider::providerForFile(name);
	
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
			if(sceneContextHint() == MyGraphicsScene::Preview)
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
		if(sceneContextHint() == MyGraphicsScene::Preview)
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
		m_imageSize = m_pixmap.size();

	        // Adjust scaling while maintaining aspect ratio
		resizeContents(contentsRect(),true);
		
		//if(DEBUG_VIDEOFILECONTENT)
		//	qDebug() << "VideFileContent::setPixmap(): new frame size:"<<m_imageSize; 
	}

	update();
	
	if(sceneContextHint() != MyGraphicsScene::Live && m_imageSize.width() > 0)
	{
		if(DEBUG_VIDEOFILECONTENT)
			qDebug() << "VideFileContent::setPixmap(): sceneContextHint() != Live, setting m_still true"; 
		m_still = true;
		m_videoProvider->pause();
	}
        //GFX_CHANGED();
}



