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
#include <QPixmapCache>
#include "ImageFilters.h"

#if QT_VERSION >= 0x040600
	#define QT46_SHADOW_ENAB 0
#endif

BoxContent::BoxContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
    , m_shadowClipDirty(true)
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

void BoxContent::setCornerRounding(QPointF x)
{
	m_cornerRounding = x;
	update();
}

QPointF BoxContent::cornerRounding()
{
	return m_cornerRounding;
}

void BoxContent::syncFromModelItem(AbstractVisualItem *model)
{
	if(!modelItem())
	{
		setModelItem(model);
		
		// Start out the last remembered model rev at the rev of the model
		// so we dont force a redraw of the cache just because we're a fresh
		// object.
		if(QPixmapCache::find(cacheKey()))
			m_lastModelRev = modelItem()->revision();
	}
	
	AbstractContent::syncFromModelItem(model);
	
	// Enable when model has had corner rounding added
	//setCornerRounding(dynamic_cast<BoxItem*>(model)->cornerRounding());
	
	if(modelItem()->revision() != m_lastModelRev)
	{
		//qDebug()<<"modelItem():"<<modelItem()->itemName()<<": last revision:"<<m_lastModelRev<<", this revision:"<<m_lastModelRev<<", cache dirty!";
		
		m_lastModelRev = modelItem()->revision();
		dirtyCache();
	}
	
	m_shadowClipDirty = true;
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
	return AbstractContent::contentHeightForWidth(width);
}


void BoxContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	// paint parent
	AbstractContent::paint(painter, option, widget);
	
	#if QT46_SHADOW_ENAB == 0
	if(modelItem()->shadowEnabled())
	{
		painter->save();
		
		QRect cRect = contentsRect();
		
		qreal penWidth = 0;
		if(modelItem()->outlineEnabled())
			penWidth = modelItem()->outlinePen().widthF();
			
		double x = modelItem()->shadowOffsetX();
		double y = modelItem()->shadowOffsetY();
// 		x += x == 0 ? 0 : x>0 ? penWidth : -penWidth;
// 		y += y == 0 ? 0 : y>0 ? penWidth : -penWidth;
			
		if(modelItem()->shadowBlurRadius() == 0)
		{
			// render a "cheap" shadow
			painter->setPen(Qt::NoPen);
			painter->setBrush(modelItem() ? modelItem()->shadowBrush() : Qt::black);
			
			if(m_shadowClipDirty)
				updateShadowClipPath();
				
			painter->setClipPath(m_shadowClipPath);
			
			painter->translate(x,y);
			painter->drawRect(cRect);
		}
		else
		{
			
			double radius = modelItem()->shadowBlurRadius();
			double radiusSquared = radius*radius;
			
			QPixmap cache;
			if(!QPixmapCache::find(cacheKey(),cache))
			{
				// create temporary pixmap to hold the foreground
				double blurSize = radiusSquared*2;
				QSize shadowSize((int)blurSize,(int)blurSize);
				QPixmap tmpPx(contentsRect().size()+shadowSize);
				tmpPx.fill(Qt::transparent);
				
				QPainter tmpPainter(&tmpPx);
				tmpPainter.save();
				
				// render the foreground
				QPoint tl = cRect.topLeft();
				tmpPainter.translate(tl.x() * -1 + radiusSquared, tl.y() * -1  + radiusSquared);
				drawForeground(&tmpPainter);
				tmpPainter.restore();
				
				// blacken the text by applying a color to the copy using a QPainter::CompositionMode_DestinationIn operation. 
				// This produces a homogeneously-colored pixmap.
				QRect rect = QRect(0, 0, tmpPx.width(), tmpPx.height());
				tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
				tmpPainter.fillRect(rect, modelItem()->shadowBrush().color());
				tmpPainter.end();
	
				// blur the colored text
				QImage  orignalImage   = tmpPx.toImage();
				QImage  blurredImage   = ImageFilters::blurred(orignalImage, rect, (int)radius);
				cache = QPixmap::fromImage(blurredImage);
				
				QPixmapCache::insert(cacheKey(), cache);
			}
			
			//qDebug() << "Drawing box shadow at offset:"<<QPoint(x,y)<<", topLeft:"<<cRect.topLeft()<<", size:"<<cache.size();
			painter->translate(x - radiusSquared,y - radiusSquared);
			painter->drawPixmap(cRect.topLeft(), cache);
		}
		
		painter->restore();
	}
	#endif
	
	drawForeground(painter);
}


void BoxContent::drawForeground(QPainter * painter)
{
	QRect cRect = contentsRect();
	
	QPen p(Qt::NoPen);
	
	painter->save();
	
	if(modelItem()->outlineEnabled())
	{
		p = modelItem()->outlinePen();
		p.setJoinStyle(Qt::MiterJoin);
		if(sceneContextHint() == MyGraphicsScene::Preview)
		{
// 			QTransform tx = painter->transform();
// 			qreal scale = qMax(tx.m11(),tx.m22());
// 			p.setWidthF(1/scale * p.widthF());
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
	
	QPointF rounding = cornerRounding();
		
	// If shadow is enabled but 0 blur, that means the shadow uses a QPainterPath for clipping.
	// Since QPainterPath doesn't support rounded rects as of Qt4.5, we dont draw the foreground rounded.
	// However, we can round the rect if we have a non-0 blur OR if the shadow is turned off completly.
	if(modelItem()->shadowEnabled() && modelItem()->shadowBlurRadius() == 0)
		rounding = QPoint(0,0);
		
	if(rounding.isNull())
	{
		painter->drawRect(cRect);
		
		// Qt 4.6RC anomaly: at less than 100% opacity, the pen is drawn as the foreground color
		// (e.g. black pen outline, yellow bg - black rect drawn ON TOP OF yello rect)
		#if QT_VERSION >= 0x040600
			painter->setPen(Qt::NoPen);
			painter->drawRect(cRect);
		#endif
	}	
	else
		painter->drawRoundedRect(cRect,rounding.x(),rounding.y(),Qt::RelativeSize);
	
	painter->restore();
}

void BoxContent::updateShadowClipPath()
{
	QPainterPath boundingPath;
		boundingPath.addRect(boundingRect());
		
		QPainterPath contentPath;
		
		// This outline-only clipping path is very buggy! 
		// It only "looks" like it works for bottom-right shadows - it cuts off anything
		// that goes top-left. And, the distance from the top-left sides of the outline
		// is too big of a gap - should be touching. I'll leave this code in for now,
		// since its better than nothing, but it needs to be revisited later.
		if(modelItem()->outlineEnabled() &&
			modelItem()->fillType() == AbstractVisualItem::None)
		{
			// need to "cut hole" in clip path where bg of slide shows thru
			// e.g. with just an outline, show shadow for all sides of outline,
			// not just, for example, the bottom right side
			QPainterPath hole;
			// assume outline is drawn "around" contents rect
			QRect holeRect = contentsRect();
			double x = modelItem()->shadowOffsetX();
			double y = modelItem()->shadowOffsetY();
			
			holeRect = holeRect.adjusted(
//  				x > 0 ? x : 0,
//  				y > 0 ? y : 0,
//  				x * -1,
//  				y * -1
				(int)(x * 4), 
				(int)(y * 4),
				0,
				0
				//-x*2,
				//-y*2
			);
				
			//qDebug() << "shadow hole: "<<holeRect<<", contents:"<<contentsRect();
			//hole.addRect(holeRect);
			//contentPath = contentPath.subtracted(hole);
			contentPath.addRect(holeRect);
		}
		else
		{
			contentPath.addRect(contentsRect());
		}
		
	m_shadowClipPath = boundingPath.subtracted(contentPath);

}
