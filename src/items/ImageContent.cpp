#include "ImageContent.h"
#include "model/ImageItem.h"
#include "AppSettings.h"
#include <QDebug>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QMimeData>
#include <QPainter>
#include <QDebug>
#include <QPixmapCache>
#include <QSvgRenderer>
#include <QTime>
#include <QImageReader>

#include "ImageFilters.h"
#include "MediaBrowser.h"

#if QT_VERSION >= 0x040600
	#define QT46_SHADOW_ENAB 0
#endif

#define DEBUG_MARK() qDebug() << "mark: "<<__FILE__<<":"<<__LINE__

#define DEBUG_IMAGECONTENT 0


ImageContent::ImageContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
    , m_shadowClipDirty(true)
    , m_svgRenderer(0)
    , m_fileLoaded(false)
    , m_fileName("")
    , m_lastModelRev(0)
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
	
	if(modelItem()->revision() != m_lastModelRev)
	{
		//if(DEBUG_IMAGECONTENT)
			//qDebug()<<"ImageContent::syncFromModelItem: modelItem():"<<modelItem()->itemName()<<": last revision:"<<m_lastModelRev<<", this revision:"<<modelItem()->revision()<<", cache dirty!";
		
		m_lastModelRev = modelItem()->revision();
		dirtyCache();
	}
	
	
	loadFile(AppSettings::applyResourcePathTranslations(modelItem()->fillImageFile()));
	
	m_shadowClipDirty = true;
}

void ImageContent::loadFile(const QString &file)
{
	if(sceneContextHint() == MyGraphicsScene::Preview)
	{
		setPixmap(MediaBrowser::iconForImage(file,MEDIABROWSER_LIST_ICON_SIZE));
		m_fileLoaded = true;
		return;
	}
	
	// JPEGs, especially large ones (e.g. file on disk is > 2MB, etc) take a long time to load, decode, and convert to pixmap.
	// (Long by UI standards anyway, e.g. > .2sec). So, we optimize away extreneous loadings by not reloading if the file & mtime
	// has not changed. If we're a new item, we also check the global pixmap cache for an already-loaded copy of this image, 
	// again keyed by file name + mtime. For SVG files, though, we only perform the first check (dont reload if not changed),
	// but we dont cache a pixmap copy of them for scaling reasons (so we can have clean scaling.)
	QString fileMod = QFileInfo(file).lastModified().toString();
	if(file == m_fileName && fileMod == m_fileLastModified)
	{
		//qDebug() << "ImageContent::loadFile: "<<file<<": no change, not reloading";
		return;
	}
	
// 	qDebug() << "ImageContent::loadFile: "<<file<<": (current file:"<<m_fileName<<"), fileMod:"<<fileMod<<", m_fileLastModified:"<<m_fileLastModified;
	
	m_fileName = file;
	m_fileLastModified = fileMod;
	
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
		QPixmap cache;
		QString cacheKey = QString("%1:%2").arg(file).arg(fileMod);
		if(QPixmapCache::find(cacheKey,cache))
		{
			setPixmap(cache);
			m_fileLoaded = true;
			//qDebug() << "ImageContent::loadFile: "<<file<<": pixmap cache hit on "<<cacheKey;
		}
		else
		{
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
				
				//qDebug() << "ImageContent::loadFile: "<<file<<": pixmap cache MISS on "<<cacheKey;
				if(!QPixmapCache::insert(cacheKey, px))
					qDebug() << "ImageContent::loadFile: "<<file<<": ::insert returned FALSE - pixmap not cached";
			}
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
	dirtyCache();
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
	if(DEBUG_IMAGECONTENT)
		qDebug()<<"ImageContent::setPixmap: modelItem():"<<modelItem()->itemName()<<": got pixmap, size:"<<pixmap.size();
	
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

void ImageContent::dirtyCache()
{
        QPixmapCache::remove(cacheKey());
        QPixmapCache::remove(cacheKey() + "foreground");
}


#define DEBUG_TMARK() DEBUG_MARK() << ":" << t.restart() << "ms"

void ImageContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	QTime total;
	total.start();
	QTime t;
	t.start();
	// paint parent
	AbstractContent::paint(painter, option, widget);
	
	//DEBUG_TMARK();
	
	#if QT46_SHADOW_ENAB == 0
	if(modelItem()->shadowEnabled())
	{
		//DEBUG_TMARK();
		
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
				double blurSize = radiusSquared*2;
				QSize shadowSize((int)blurSize,(int)blurSize);
				QSize tmpPxSize = cRect.size()+shadowSize;
				
				//qDebug()<<"ImageContent::paint(): modelItem:"<<modelItem()->itemName()<<": shadow cache redraw: cRect:"<<cRect<<", cRect.size():"<<cRect.size()<<", tmpPxSize:"<<tmpPxSize<<", shadowsize:"<<shadowSize<<", blurSize:"<<blurSize;
				
				// create temporary pixmap to hold the foreground
				QPixmap tmpPx(tmpPxSize);
				tmpPx.fill(Qt::transparent);
				
				QPainter tmpPainter(&tmpPx);
				tmpPainter.save();
				
				// render the foreground
				QPoint tl = cRect.topLeft();
				tmpPainter.translate(tl.x() * -1 + radiusSquared, tl.y() * -1  + radiusSquared);
				drawForeground(&tmpPainter,false);
				tmpPainter.restore();
				
				// blacken the text by applying a color to the copy using a QPainter::CompositionMode_DestinationIn operation. 
				// This produces a homogeneously-colored pixmap.
				QRect rect = QRect(0, 0, tmpPx.width(), tmpPx.height());
				tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
				tmpPainter.fillRect(rect, modelItem()->shadowBrush().color());
				tmpPainter.end();
	
				// blur the colored image
				QImage  orignalImage   = tmpPx.toImage();
				QImage  blurredImage   = ImageFilters::blurred(orignalImage, rect, (int)radius);
				cache = QPixmap::fromImage(blurredImage);
				
				if(sceneContextHint() != MyGraphicsScene::Preview)
					QPixmapCache::insert(cacheKey(), cache);
			}
			
			//qDebug()<<"ImageContent::paint(): Drawing shadow at offset:"<<QPoint(x,y)<<", topLeft:"<<cRect.topLeft()<<", size:"<<cache.size()<<", radiusSquared:"<<radiusSquared;
			painter->translate(x - radiusSquared,y - radiusSquared);
			painter->drawPixmap(cRect.topLeft(), cache);
		}
		//DEBUG_TMARK();
		
		painter->restore();
		//DEBUG_TMARK();
	}
	#endif
	//DEBUG_TMARK();
	drawForeground(painter);
	//DEBUG_TMARK();
	
	
	if(modelItem()->outlineEnabled())
	{
				
		QRect cRect = contentsRect();
		
		QPen p = modelItem()->outlinePen();
		p.setJoinStyle(Qt::MiterJoin);
// 		if(sceneContextHint() == MyGraphicsScene::Preview)
// 		{
// 			QTransform tx = painter->transform();
// 			qreal scale = qMax(tx.m11(),tx.m22());
// 			p.setWidthF(1/scale * p.widthF());
// 		}
// 		
		painter->setPen(p);
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(cRect);
		//DEBUG_TMARK();
		
		#if QT_VERSION >= 0x040600
			// Qt 4.6 RC 1 FILLS the bloody rect above instead of just drawing an outline - IF the opacity of our object is < 1
			//qDebug() << "ImageContent::paint():  opacity="<<opacity()<<", painter opacity:"<<painter->opacity();
			if(painter->opacity() < 1.0)
				drawForeground(painter);
		#endif
	}
	
	//qDebug() << "ImageContent::paint(): \t \t Elapsed:"<<(((double)total.elapsed())/1000.0)<<" sec";
}


void ImageContent::drawForeground(QPainter *painter, bool screenTranslation)
{
	QTime total;
	total.start();
	QTime t;
	t.start();
	
	//qDebug() << "ImageContent::drawForeground(): start";
	
	QRect cRect = contentsRect();
	
	painter->save();
	
	//DEBUG_TMARK();
	
	if(!m_fileLoaded)
	{
		painter->fillRect(cRect,Qt::gray);
		//DEBUG_TMARK();
	}
	else
	{
		if(m_svgRenderer)
		{
			m_svgRenderer->render(painter,cRect);
			//DEBUG_TMARK();
		}
		else
		{
			
			static int dbg_counter =0;
			dbg_counter++;
			
			// this rect describes our "model" height in terms of item coordinates
			QRect tmpRect(0,0,cRect.width(),cRect.height());
			
			// This is the key to getting good looking scaled & cached pixmaps -
			// it transforms our item coordinates into view coordinates. 
			// What this means is that if our item is 100x100, but the view is scaled
			// by 1.5, our final pixmap would be scaled by the painter to 150x150.
			// That means that even though after the cache generation we tell drawPixmap()
			// to use our 100x100 rect, it will do the same transform and figure out that
			// it needs to scale the pixmap to 150x150. Therefore, what we are doing here
			// is calculating what drawPixmap() will *really* need, even though we tell
			// it something different (100x100). Then, we dont scale the pixamp to 100x100 - no,
			// we scale it only to 150x150. And then the painter can render the pixels 1:1
			// rather than having to scale up and make it look pixelated.
			if(screenTranslation)
				tmpRect = painter->combinedTransform().mapRect(tmpRect);
			
			QRect destRect(0,0,tmpRect.width(),tmpRect.height());
			
			// cache the scaled pixmap according to the transformed size of the view
			QString foregroundKey = QString(cacheKey()+":%1:%2").arg(destRect.width()).arg(destRect.height());
			
			//qDebug() << "ImageContent::drawForeground: " << dbg_counter << "cRect:"<<cRect<<", tmpRect:"<<tmpRect;
			
			QPixmap cache;
			if(!QPixmapCache::find(foregroundKey,cache))
			{
				//qDebug() << "ImageContent::drawForeground: " << dbg_counter << "Foreground pixmap dirty, redrawing";
				cache = QPixmap(destRect.size());
				cache.fill(Qt::transparent);
				
				QPainter tmpPainter(&cache);
				tmpPainter.setRenderHint(QPainter::HighQualityAntialiasing, true);
				tmpPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);
				
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
						px.width()  - (int)(br.x() * px.width())  + (px.x() + x1),
						px.height() - (int)(br.y() * px.height()) + (px.y() + y1)
					);
						
					qDebug() << "ImageContent::drawForeground:"<<modelItem()->itemName()<<": tl:"<<tl<<", br:"<<br<<", source:"<<source;
					tmpPainter.drawPixmap(destRect, m_pixmap, source);
				}
				else
					tmpPainter.drawPixmap(destRect, m_pixmap);
				
				tmpPainter.end();
				if(sceneContextHint() != MyGraphicsScene::Preview)
					if(!QPixmapCache::insert(foregroundKey, cache))
						qDebug() << "ImageContent::drawForeground:"<<modelItem()->itemName()<<": Can't cache the image. This will slow performance of cross fades and slide editor. Make the cache larger using the Program Settings menu.";
			}
			
			painter->drawPixmap(cRect,cache);
		}
	}
	
	
	painter->restore();
	
	//DEBUG_TMARK();
	//qDebug() << "ImageContent::drawForeground(): \t \t Elapsed:"<<(((double)total.elapsed())/1000.0)<<" sec";
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
