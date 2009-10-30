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
#include <QUrl>
#include <QDebug>
#include <QPixmapCache>
#include <QSvgRenderer>
#include <QImageReader>

#include "qvideo/QVideoProvider.h"
#include "MediaBrowser.h"

#define DEBUG_BACKGROUNDCONTENT 0

#define MAX_SCALED_WIDTH 4500
#define MAX_SCALED_HEIGHT 2500

BackgroundContent::BackgroundContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
    , m_still(false)
    , m_videoProvider(0)
    , m_sceneSignalConnected(false)
    , m_svgRenderer(0)
    , m_fileLoaded(false)
    , m_fileName("")
    , m_fileLastModified("")
{
	m_dontSyncToModel = true;
	
	setFrame(0);
	setFrameTextEnabled(false);
	setToolTip(tr("Background - right click for options."));
	
// 	setText("Welcome");
	
	setFlag(QGraphicsItem::ItemIsMovable,false);
	setFlag(QGraphicsItem::ItemIsFocusable,false);
	setFlag(QGraphicsItem::ItemIsSelectable,true);
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
	if(m_videoProvider)
	{
		m_videoProvider->disconnectReceiver(this);
		QVideoProvider::releaseProvider(m_videoProvider);
	}
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
	m_dontSyncToModel = true;
	
	
	if(model->fillVideoFile()!="" &&
		modelItem()->fillType() == AbstractVisualItem::Video)
		setVideoFile(model->fillVideoFile());
		
	if(modelItem()->fillImageFile() != "" &&
		modelItem()->fillType() == AbstractVisualItem::Image)
		setImageFile(modelItem()->fillImageFile());
	
	setPos(0,0);
	if(scene())
	{
		QRect r = scene()->sceneRect().toRect();
		//qDebug() << "BackgroundContent::syncFromModelItem(): Setting rect:"<<r;
		resizeContents(r);
		
		if(!m_sceneSignalConnected)
		{
			connect(dynamic_cast<MyGraphicsScene*>(scene()), SIGNAL(sceneRectChanged(const QRectF&)), this, SLOT(sceneRectChanged(const QRectF&)));
			m_sceneSignalConnected = true;
		}
			
	}
	setZValue(-9999);
	setVisible(true);
	update();
	
        m_dontSyncToModel = false;
}


void BackgroundContent::setImageFile(const QString &file)
{
	if(sceneContextHint() == MyGraphicsScene::Preview)
	{
		setPixmap(MediaBrowser::iconForImage(file,MEDIABROWSER_LIST_ICON_SIZE));
		m_fileLoaded = true;
		return;
	}
	
	if(m_videoProvider)
	{
		// TODO We ASSUME were playing the video before we got the image
		//m_videoProvider->pause();
		m_videoProvider->disconnectReceiver(this);
		QVideoProvider::releaseProvider(m_videoProvider);
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
	
	//qDebug() << "ImageContent::loadFile: "<<file<<": (current file:"<<m_fileName<<"), fileMod:"<<fileMod<<", m_fileLastModified:"<<m_fileLastModified;
	
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
				qDebug() << "BackgroundContent::loadFile: Unable to read"<<file<<": "<<reader.errorString();
			}
			else
			{
				QPixmap px = QPixmap::fromImage(image);
				setPixmap(px);
				m_fileLoaded = true;
				
				//qDebug() << "ImageContent::loadFile: "<<file<<": pixmap cache MISS on "<<cacheKey;
				if(!QPixmapCache::insert(cacheKey, px))
					qDebug() << "BackgroundContent::loadFile: "<<file<<": ::insert returned FALSE - pixmap not cached";
			}
		}
	}
}

void BackgroundContent::disposeSvgRenderer()
{
	if(m_svgRenderer)
	{
		disconnect(m_svgRenderer,0,this,0);
		delete m_svgRenderer;
		m_svgRenderer = 0;
	}
}

void BackgroundContent::loadSvg(const QString &file)
{
	disposeSvgRenderer();
	
	m_svgRenderer = new QSvgRenderer(file);
	m_fileLoaded = true;
	
	m_pixmap = QPixmap(m_svgRenderer->viewBox().size());
	
	if(m_imageSize != m_pixmap.size())
		m_imageSize = m_pixmap.size();
	
	connect(m_svgRenderer, SIGNAL(repaintNeeded()), this, SLOT(renderSvg()));
	renderSvg();	
}

void BackgroundContent::renderSvg()
{
	// not needed since we dont cache render or shadow in the background
	//dirtyCache();
	update();
}


void BackgroundContent::sceneRectChanged(const QRectF& rect)
{
	resizeContents(rect.toRect());
}

AbstractVisualItem * BackgroundContent::syncToModelItem(AbstractVisualItem *model)
{
	BackgroundItem * boxModel = dynamic_cast<BackgroundItem*>(AbstractContent::syncToModelItem(model));
	
	if(!boxModel)
	{
		return 0;
	}
	
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
	//qreal penWidth = m_modelItem ? m_modelItem->outlinePen().widthF() : 1.0;
	return AbstractContent::boundingRect();//.adjusted(-penWidth/2,-penWidth/2,penWidth,penWidth);
}


void BackgroundContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	// paint parent
	AbstractContent::paint(painter, option, widget);
	
	QRect cRect = contentsRect();
	
	painter->save();
		
	painter->setPen(Qt::NoPen);
	AbstractVisualItem::FillType fill = modelItem()->fillType();
	if(fill == AbstractVisualItem::Solid)
	{
		painter->setBrush(modelItem()->fillBrush());
		painter->drawRect(cRect); //QRect(QPoint(0,0),cRect.size()));
	}
	else
	if(fill == AbstractVisualItem::Gradient)
	{
		// Noop yet
		painter->setBrush(Qt::white);
		painter->drawRect(cRect); //QRect(QPoint(0,0),cRect.size()));
	}
	else
	if(fill == AbstractVisualItem::Image)
	{
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
				tmpRect = painter->combinedTransform().mapRect(tmpRect);
				
				QRect destRect(0,0,tmpRect.width(),tmpRect.height());
				
				// Limit the size of the final image inorder to prevent the user from
				// inadvertantly killing the speed of the painting by scaling a really huge image
				// in the media browser preview
				if(destRect.width()  > MAX_SCALED_WIDTH || 
				   destRect.height() > MAX_SCALED_HEIGHT)
				{
					float sx = ((float)MAX_SCALED_WIDTH)  / ((float)destRect.width());
					float sy = ((float)MAX_SCALED_HEIGHT) / ((float)destRect.height());
				
					float scale = qMin(sx,sy);
					destRect.setWidth( destRect.width()  * sx);
					destRect.setHeight(destRect.height() * sy);
				}
				
				// cache the scaled pixmap according to the transformed size of the view
				QString foregroundKey = QString("%1:%2:%3:%4")
							.arg(m_fileName).arg(m_fileLastModified)
							.arg(destRect.width()).arg(destRect.height());
							
				//qDebug() << "foregroundKey: "<<foregroundKey;
				
				QPixmap cache;
				if(!QPixmapCache::find(foregroundKey,cache))
				{
// 					QTime t;
// 					t.start();
// 					qDebug() << "BackgroundContent::drawForeground: Foreground pixmap dirty, redrawing size:"<<destRect;
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
							
						qDebug() << "BackgroundContent::paint:"<<modelItem()->itemName()<<": tl:"<<tl<<", br:"<<br<<", source:"<<source;
						tmpPainter.drawPixmap(destRect, m_pixmap, source);
					}
					else
						tmpPainter.drawPixmap(destRect, m_pixmap);
					
					tmpPainter.end();
					if(!QPixmapCache::insert(foregroundKey, cache))
						qDebug() << "BackgroundContent::paint:"<<modelItem()->itemName()<<": Can't cache the image. This will slow performance of cross fades and slide editor. Make the cache larger using the Program Settings menu.";
					
// 					qDebug() << "BackgroundContent::drawForeground: Foreground pixmap dirty, end drawing size:"<<destRect<<", elapsed: "<<t.elapsed();
				}
				
				painter->drawPixmap(cRect,cache);
			}
		}
	}
	else
	{
		if(m_imageSize.width() <= 0)
		{
			painter->fillRect(cRect,QBrush(Qt::gray));
		}
		else
		{
			painter->setBrush(Qt::NoBrush);
			painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
			painter->drawPixmap(cRect, m_pixmap);
		}
	}
	
	
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


void BackgroundContent::setVideoFile(const QString &name)
{
// 	if(!m_video->load(name))
// 	{
// 		qDebug() << "VideoFileContent::setFilename(): ERROR: Unable to load video"<<name;
// 		return;
// 	}
	QVideoProvider * p = QVideoProvider::providerForFile(name);
	
	if(m_videoProvider && m_videoProvider == p)
	{
		return;
	}
	else
	if(m_videoProvider)
	{
		m_videoProvider->disconnectReceiver(this);
		QVideoProvider::releaseProvider(m_videoProvider);
	}
	
	if(DEBUG_BACKGROUNDCONTENT)
		qDebug() << "BackgroundContent::setVideoFile: Loading"<<name;
	
	m_videoProvider = p;
	
	if(modelItem()->fillType() == AbstractVisualItem::Video)
	{
		m_videoProvider->play();
		
		m_still = false;
		
		// prime the pump, so to speak
		if(sceneContextHint() == MyGraphicsScene::Preview)
			setPixmap(QVideoProvider::iconForFile(name));
		else
			setPixmap(m_videoProvider->pixmap());
			
	}
	else
	{
		m_still = true;
	}
	
	m_videoProvider->connectReceiver(this, SLOT(setPixmap(const QPixmap &)));
}


void BackgroundContent::setPixmap(const QPixmap & pixmap)
{
	if(m_still && m_imageSize.width() > 0)
		return;
		
	m_pixmap = pixmap;

	if(m_imageSize != m_pixmap.size())
		m_imageSize = m_pixmap.size();

	update();
	
	if(sceneContextHint() != MyGraphicsScene::Live && 
		modelItem()->fillType() == AbstractVisualItem::Video &&
		m_imageSize.width() > 0)
	{
		if(DEBUG_BACKGROUNDCONTENT)
			qDebug() << "BackgroundContent::setPixmap(): sceneContextHint() != Live, setting m_still true"; 
		m_still = true;
		m_videoProvider->pause();
	}
        //GFX_CHANGED();
}



