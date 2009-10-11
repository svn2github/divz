#include "BoxContent.h"
#include "frames/Frame.h"
// #include "items/BezierCubicItem.h"
#include "items/TextProperties.h"
#include "model/BoxItem.h"
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

BoxContent::BoxContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
//     , m_text(0)
//     , m_textRect(0, 0, 0, 0)
//     , m_textMargin(4)
{
	m_dontSyncToModel = true;
	
	setFrame(0);
	setFrameTextEnabled(false);
	setToolTip(tr("Rectangle - right click for options."));
	
// 	setText("Welcome");
	for(int i=0;i<m_cornerItems.size();i++)
		m_cornerItems.at(i)->setDefaultLeftOp(CornerItem::Scale);
	
	m_dontSyncToModel = false;
}

BoxContent::~BoxContent()
{
// 	delete m_shapeEditor;
// 	delete m_text;
}

QWidget * BoxContent::createPropertyWidget()
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

void BoxContent::syncFromModelItem(AbstractVisualItem *model)
{
	AbstractContent::syncFromModelItem(model);
}

AbstractVisualItem * BoxContent::syncToModelItem(AbstractVisualItem *model)
{	
	return AbstractContent::syncToModelItem(model);
}

QPixmap BoxContent::renderContent(const QSize & size, Qt::AspectRatioMode /*ratio*/) const
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

int BoxContent::contentHeightForWidth(int width) const
{
// 	// if no text size is available, use default
// 	if (m_textRect.width() < 1 || m_textRect.height() < 1)
		return AbstractContent::contentHeightForWidth(width);
//	return (m_textRect.height() * width) / m_textRect.width();
}


QRectF BoxContent::boundingRect() const 
{
	qreal penWidth = m_modelItem ? m_modelItem->outlinePen().widthF() : 1.0;
	return AbstractContent::boundingRect().adjusted(-penWidth/2,-penWidth/2,penWidth,penWidth);
}


void BoxContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	// paint parent
	AbstractContent::paint(painter, option, widget);
	
	QRect cRect = contentsRect();
	
	QPen p(Qt::NoPen);
	
	
	if(modelItem()->shadowEnabled())
	{
		painter->save();
		
		qreal penWidth = 0;
		if(modelItem()->outlineEnabled())
		{
			penWidth = modelItem()->outlinePen().widthF();
		}
			
			
		painter->setPen(Qt::NoPen);
		painter->setBrush(modelItem() ? modelItem()->shadowBrush() : Qt::black);
		
		int x = modelItem()->shadowOffsetX();
		int y = modelItem()->shadowOffsetY();
		x += x == 0 ? 0 : x>0 ? penWidth : -penWidth;
		y += y == 0 ? 0 : y>0 ? penWidth : -penWidth;
		//qDebug() << "Boxshadow: "<<x<<","<<y;
		painter->translate(x,y);
		
		painter->drawRect(cRect);
		
		painter->restore();
	}
	
	painter->save();
	
	
	if(modelItem()->outlineEnabled())
	{
		p = modelItem()->outlinePen();
		p.setJoinStyle(Qt::MiterJoin);
		if(sceneContextHint() == MyGraphicsScene::Preview)
		{
			QTransform tx = painter->transform();
			qreal scale = qMax(tx.m11(),tx.m22());
			p.setWidthF(1/scale * p.widthF());
		}
		
	}
	
	// If no fill, no outline, and in the editor, draw a 1px outline inorder to be better able to edit/remove the box
	if(modelItem()->fillType() == AbstractVisualItem::None 
		&& !modelItem()->outlineEnabled()
		&& sceneContextHint() == MyGraphicsScene::Editor)
		p = QPen(Qt::blue, 0);
	
	painter->setPen(p);
	
	if(modelItem()->fillType() != AbstractVisualItem::None)
		painter->setBrush(modelItem()->fillBrush());
	
	painter->drawRect(cRect);
	
	painter->restore();
}

