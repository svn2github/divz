#include "ImageContent.h"
#include "model/ImageItem.h"
#include <QDebug>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QMimeData>
#include <QPainter>
#include <QDebug>
#include <QPixmapCache>
#include <QSvgRenderer>

#include <QImageReader>

#include "ImageFilters.h"

#if QT_VERSION >= 0x040600
	#define QT46_SHADOW_ENAB 0
#endif

ImageContent::ImageContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
    , m_shadowClipDirty(true)
    , m_svgRenderer(0)
    , m_fileLoaded(false)
{
	m_dontSyncToModel = true;
	
	setToolTip(tr("Image - right click for options."));
	
	//for(int i=0;i<m_cornerItems.size();i++)
	//	m_cornerItems.at(i)->setDefaultLeftOp(CornerItem::Scale);
	
	m_dontSyncToModel = false;
}

ImageContent::~ImageContent()
{
}

QWidget * ImageContent::createPropertyWidget()
{
	return 0;
}

void ImageContent::syncFromModelItem(AbstractVisualItem *model)
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
	
	loadFile(modelItem()->fillImageFile());
	
	if(modelItem()->revision() != m_lastModelRev)
	{
		//qDebug()<<"modelItem():"<<modelItem()->itemName()<<": last revision:"<<m_lastModelRev<<", this revision:"<<m_lastModelRev<<", cache dirty!";
		
		m_lastModelRev = modelItem()->revision();
		dirtyCache();
	}
	
	m_shadowClipDirty = true;
}

void ImageContent::loadFile(const QString &file)
{
	//qDebug() << "ImageContent::loadFile: "<<file;
	if(file.isEmpty())
	{
		m_fileLoaded = false;
		disposeSvgRenderer();
		m_pixmap = QPixmap();
		return;
	}
	
	if(file.endsWith(".svg",Qt::CaseInsensitive))
	{
		loadSvg(file);
	}
	else
	{
		disposeSvgRenderer();
		QImageReader reader(file);
		QImage image = reader.read();
		if(image.isNull())
		{
			qDebug() << "ImageContent::loadFile: Unable to read"<<file<<": "<<reader.errorString();
		}
		else
		{
			QPixmap px = QPixmap::fromImage(image);
			setPixmap(px);
			m_fileLoaded = true;
		}
	}
}

void ImageContent::disposeSvgRenderer()
{
	if(m_svgRenderer)
	{
		disconnect(m_svgRenderer,0,this,0);
		delete m_svgRenderer;
		m_svgRenderer = 0;
	}
}

void ImageContent::loadSvg(const QString &file)
{
	disposeSvgRenderer();
	
	m_svgRenderer = new QSvgRenderer(file);
	m_fileLoaded = true;
	
	m_pixmap = QPixmap(m_svgRenderer->viewBox().size());
	checkSize();
	
	connect(m_svgRenderer, SIGNAL(repaintNeeded()), this, SLOT(renderSvg()));
	renderSvg();	
}

void ImageContent::renderSvg()
{
	/*
	m_pixmap.fill(Qt::transparent);
	QPainter p(&m_pixmap);
	m_svgRenderer->render(&p);
	p.end();
	*/
	update();
}

void ImageContent::checkSize()
{
	if(m_imageSize != m_pixmap.size())
	{
		m_imageSize = m_pixmap.size();
		
		// Adjust scaling while maintaining aspect ratio
		resizeContents(contentsRect(),true);
	}
}

void ImageContent::setPixmap(const QPixmap & pixmap)
{
	m_pixmap = pixmap;
	//qDebug() << "ImageContent::setPixmap: Got pixmap, size:"<<pixmap.size();
	
	checkSize();
	update();
}

AbstractVisualItem * ImageContent::syncToModelItem(AbstractVisualItem *model)
{	
	return AbstractContent::syncToModelItem(model);
}

QPixmap ImageContent::renderContent(const QSize & size, Qt::AspectRatioMode /*ratio*/) const
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

int ImageContent::contentHeightForWidth(int width) const
{
	// if no image size is available, use default
 	if (m_imageSize.width() < 1 || m_imageSize.height() < 1)
		return AbstractContent::contentHeightForWidth(width);
		
        return (m_imageSize.height() * width) / m_imageSize.width();
}


void ImageContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
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
				//qDebug()<<"ImageContent::paint(): modelItem:"<<modelItem()->itemName()<<": Cache redraw";
				
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


void ImageContent::drawForeground(QPainter *painter)
{
	QRect cRect = contentsRect();
	
	painter->save();
	
	if(!m_fileLoaded)
	{
		painter->fillRect(cRect,Qt::gray);
	}
	else
	{
		if(m_svgRenderer)
		{
			m_svgRenderer->render(painter,cRect);
		}
		else
		{
			if(!sourceOffsetTL().isNull() || !sourceOffsetBR().isNull())
			{
				QPointF tl = sourceOffsetTL();
				QPointF br = sourceOffsetBR();
				QRect px = m_pixmap.rect();
				int x1 = (int)(tl.x() * px.width());
				int y1 = (int)(tl.y() * px.height());
				QRect source( 
					px.x() + x1,
					px.y() + y1,
					px.width()  + (int)(br.x() * px.width())  - (px.x() + x1),
					px.height() + (int)(br.y() * px.height()) - (px.y() + y1)
				);
					
				//qDebug() << "ImageContent::drawForeground:"<<modelItem()->itemName()<<": tl:"<<tl<<", br:"<<br<<", source:"<<source;
				painter->drawPixmap(cRect, m_pixmap, source);
			}
			else
				painter->drawPixmap(cRect, m_pixmap);
		}
	}
	
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
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(cRect);
	}
	
	painter->restore();
}

void ImageContent::updateShadowClipPath()
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
