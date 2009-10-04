#include "BackgroundContent.h"
#include "frames/Frame.h"
// #include "items/BezierCubicItem.h"
#include "items/TextProperties.h"
#include "model/BackgroundItem.h"
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

BackgroundContent::BackgroundContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
{
	m_dontSyncToModel = true;
	
	setFrame(0);
	setFrameTextEnabled(false);
	setToolTip(tr("Background - right click for options."));
	
// 	setText("Welcome");
	
	setFlag(QGraphicsItem::ItemIsMovable,false);
	setFlag(QGraphicsItem::ItemIsFocusable,false);
	setFlag(QGraphicsItem::ItemIsSelectable,false);
	#if QT_VERSION >= 0x040600
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	#endif
	// allow some items (eg. the shape controls for text) to be shown
	// TODO reconsider setting this to true for background, maybe conditional on scene context hint
	setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
	setAcceptHoverEvents(false);
	
	foreach(CornerItem *c, m_cornerItems)
	{
		//c->dispose(false);
		m_cornerItems.removeAll(c);
		delete c;
		c =0;
	}
	
	foreach(ButtonItem *c, m_controlItems)
	{
		//c->dispose(false);
		m_controlItems.removeAll(c);
		delete c;
		c =0;
	}
	
	m_dontSyncToModel = false;
}

BackgroundContent::~BackgroundContent()
{
// 	delete m_shapeEditor;
// 	delete m_text;
}

QWidget * BackgroundContent::createPropertyWidget()
{
	return 0;
}

void BackgroundContent::syncFromModelItem(AbstractVisualItem *model)
{
        m_dontSyncToModel = true;
	setModelItem(model);
	
	//QFont font;
	//Item * boxmodel = dynamic_cast<BoxItem*>(model);
	
// 	setText(textModel->text());
// 	
// 	font.setFamily(textModel->fontFamily());
// 	font.setPointSize((int)textModel->fontSize());
// 	setFont(font);
	
	AbstractContent::syncFromModelItem(model);
	
	
	setPos(0,0);
	if(scene())
		resizeContents(scene()->sceneRect().toRect());
	setZValue(-99999);
	
        m_dontSyncToModel = false;
}

AbstractVisualItem * BackgroundContent::syncToModelItem(AbstractVisualItem *model)
{
	BackgroundItem * boxModel = dynamic_cast<BackgroundItem*>(AbstractContent::syncToModelItem(model));
	
	if(!boxModel)
	{
		//setModelItemIsChanging(false);
                //qDebug("BackgroundContent::syncToModelItem: textModel is null, cannot sync\n");
		return 0;
	}
	setModelItemIsChanging(true);

        //qDebug("TextContent:syncToModelItem: Syncing to model! Yay!");
// 	textModel->setText(text());
// 	textModel->setFontFamily(font().family());
// 	textModel->setFontSize(font().pointSize());
	
	setModelItemIsChanging(false);
	
	return model;
}

QPixmap BackgroundContent::renderContent(const QSize & size, Qt::AspectRatioMode /*ratio*/) const
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

int BackgroundContent::contentHeightForWidth(int width) const
{
// 	// if no text size is available, use default
// 	if (m_textRect.width() < 1 || m_textRect.height() < 1)
		return AbstractContent::contentHeightForWidth(width);
//	return (m_textRect.height() * width) / m_textRect.width();
}


QRectF BackgroundContent::boundingRect() const 
{
	qreal penWidth = m_modelItem ? m_modelItem->outlinePen().widthF() : 1.0;
	return AbstractContent::boundingRect().adjusted(-penWidth/2,-penWidth/2,penWidth,penWidth);
}


void BackgroundContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	// paint parent
	AbstractContent::paint(painter, option, widget);
	
	QRect cRect = contentsRect();
// 	QRect sRect = m_textRect;
	painter->save();
	painter->translate(cRect.topLeft());
// 	if (sRect.width() > 0 && sRect.height() > 0)
// 	{
// 		qreal xScale = (qreal)cRect.width() / (qreal)sRect.width();
// 		qreal yScale = (qreal)cRect.height() / (qreal)sRect.height();
// 		if (!qFuzzyCompare(xScale, 1.0) || !qFuzzyCompare(yScale, 1.0))
// 		painter->scale(xScale, yScale);
// 	}
	
// 	QPen pen;
//  	pen.setWidthF(3);
//  	pen.setColor(QColor(0,0,0,255));
//  
//  	QBrush brush(QColor(255,0,0,255));
// 	QPen p = modelItem()->outlinePen();
// 	p.setJoinStyle(Qt::MiterJoin);
// 	if(sceneContextHint() == MyGraphicsScene::Preview)
// 	{
// 		QTransform tx = painter->transform();
// 		qreal scale = qMax(tx.m11(),tx.m22());
// 		p.setWidthF(1/scale * p.widthF());
// 	}
// 		
	painter->setPen(Qt::NoPen);
 	painter->setBrush(modelItem()->fillBrush());
	painter->drawRect(QRect(QPoint(0,0),cRect.size()));
	
// 	//const bool opaqueContent = contentOpaque();
// 	const bool drawSelection = RenderOpts::HQRendering ? false : isSelected();
// 	
// 	// draw the selection only as done in EmptyFrame.cpp
// 	if (drawSelection) 
// 	{
// 		painter->setRenderHint(QPainter::Antialiasing, true);
// 		//painter->setCompositionMode(QPainter::CompositionMode_Xor);
// 		painter->setPen(QPen(RenderOpts::hiColor, 1.0));
// 		// FIXME: this draws OUTSIDE (but inside the safe 2px area)
// 		painter->drawRect(cRect); //QRectF(cRect).adjusted(-1.0, -1.0, +1.0, +1.0));
// 	}
// 			
	painter->restore();
}

