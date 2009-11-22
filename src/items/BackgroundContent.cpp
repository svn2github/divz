#include "BackgroundContent.h"
#include "frames/Frame.h"
// #include "items/BezierCubicItem.h"
#include "items/TextProperties.h"
#include "model/BackgroundItem.h"
#include "items/CornerItem.h"
#include "AppSettings.h"
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
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QApplication>

#include "qvideo/QVideoProvider.h"
#include "3rdparty/md5/md5.h"
#include "MediaBrowser.h"
#include "AppSettings.h"

#define DEBUG_BACKGROUNDCONTENT 0

#define MAX_SCALED_WIDTH 4500
#define MAX_SCALED_HEIGHT 2500

#define ZOOM_FACTOR 1.5

#define PTR(p) (QString().sprintf("%p",p))

BackgroundContent::BackgroundContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
    , m_still(false)
    , m_videoProvider(0)
    , m_sceneSignalConnected(false)
    , m_svgRenderer(0)
    , m_fileLoaded(false)
    , m_fileName("")
    , m_fileLastModified("")
    , m_lastForegroundKey("")
    , m_zoomInit(false)
    , m_zoomEnabled(false)
#ifdef PHONON_ENABLED
    , m_proxy(0)
    , m_player(0)
    , m_tuplet(0)
#endif
{
	m_dontSyncToModel = true;
	
	setFrame(0);
	setFrameTextEnabled(false);
	setToolTip(tr("Background - right click for options."));
	
	setFlag(QGraphicsItem::ItemIsMovable,false);
	setFlag(QGraphicsItem::ItemIsFocusable,false);
	setFlag(QGraphicsItem::ItemIsSelectable,true);
	#ifdef PHONON_ENABLED
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
	
	m_zoomAnimationTimer = new QTimer(this);
	connect(m_zoomAnimationTimer, SIGNAL(timeout()), this, SLOT(animateZoom()));
	
	m_dontSyncToModel = false;
}

BackgroundContent::~BackgroundContent()
{
	if(m_videoProvider)
	{
		m_videoProvider->disconnectReceiver(this);
		QVideoProvider::releaseProvider(m_videoProvider);
	}
	
	if(!m_lastForegroundKey.isEmpty())
		QPixmapCache::remove(m_lastForegroundKey);
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
	
	
	if(model->fillVideoFile()!="" &&
		model->fillType() == AbstractVisualItem::Video)
		setVideoFile(AppSettings::applyResourcePathTranslations(model->fillVideoFile()));
		
	if(model->fillImageFile() != "" &&
		model->fillType() == AbstractVisualItem::Image)
		setImageFile(AppSettings::applyResourcePathTranslations(model->fillImageFile()));
		
	if(   model->zoomEffectEnabled() 
 	   && model->zoomSpeed() > 0 
 	   && sceneContextHint() == MyGraphicsScene::Live)
	{
		m_zoomEnabled = true;
		
		m_zoomAnimationTimer->start(1000 / 20); // / model->zoomSpeed());
		
		QSize size = contentsRect().size();
			
		double width  = size.width();
		double height = size.height();
		
		double aspectRatio = m_pixmap.isNull() || m_pixmap.height() <=0 ? 0 : m_pixmap.width() / m_pixmap.height();
		if(aspectRatio == 0 || aspectRatio == 1) 
			aspectRatio = AppSettings::liveAspectRatio();
			
		if(!m_zoomInit)
		{
			//qDebug() << "aspectRatio: "<<aspectRatio;
			
			double startWidth = height * aspectRatio;
			m_zoomDir = 1;
			QPointF delta;
			
			m_zoomStartSize.setX(startWidth);
			m_zoomStartSize.setY(height);
			 
			m_zoomEndSize.setX(startWidth * model->zoomFactor());
			m_zoomEndSize.setY(height     * model->zoomFactor());
			
			bool zoomIn = true;
			if(model->zoomDirection() == AbstractVisualItem::ZoomIn)
				zoomIn = true;
			else
			if(model->zoomDirection() == AbstractVisualItem::ZoomOut)
				zoomIn = false;
			else
			if(model->zoomDirection() == AbstractVisualItem::ZoomRandom)
				zoomIn = qrand() < RAND_MAX/2;
			
			m_zoomCurSize = zoomIn ? m_zoomStartSize : m_zoomEndSize;
			m_zoomDir     = zoomIn ? 1 : -1;
			
			delta.setX(m_zoomEndSize.x() - m_zoomCurSize.x());
			delta.setY(m_zoomEndSize.y() - m_zoomCurSize.y());
			//step.setX(delta.x()/ZOOM_STEPS);
			//step.setY(delta.y()/ZOOM_STEPS);
//			m_zoomInit = true;
		}
		
		// allow it to go below 1.0 for step size by using 75.0 when the max of the zoomSpeed slider in config is 100
		m_zoomStep.setX(75.0 / (100.0 - ((double)model->zoomSpeed())) * aspectRatio);
		m_zoomStep.setY(75.0 / (100.0 - ((double)model->zoomSpeed())));
		
		if(model->zoomAnchorPoint() == AbstractVisualItem::ZoomAnchorRandom)
		{
			// pick a third intersection
			double x = qrand() < RAND_MAX/2 ? .33 : .66;
			double y = qrand() < RAND_MAX/2 ? .33 : .66;
			
			// apply a fudge factor
// 			x += 0.15 - ((double)qrand()) / ((double)RAND_MAX) * 0.075;
// 			y += 0.15 - ((double)qrand()) / ((double)RAND_MAX) * 0.075;
			
			m_zoomDestPoint = QPointF(x,y);
			//qDebug() << "ZoomRandom:	"<<x<<","<<y;
			
			//qDebug() << model->itemName() << "Random zoom anchor: "<<m_zoomDestPoint;
		}
		else
		{
			double x = .0, y = .0;
			switch(model->zoomAnchorPoint())
			{
				case AbstractVisualItem::ZoomTopLeft:		x = .33; y = .33; break;
				case AbstractVisualItem::ZoomTopMid:		x = .50; y = .25; break;
				case AbstractVisualItem::ZoomTopRight:		x = .66; y = .33; break;
				case AbstractVisualItem::ZoomRightMid:		x = .75; y = .50; break;
				case AbstractVisualItem::ZoomBottomRight:	x = .66; y = .66; break;
				case AbstractVisualItem::ZoomBottomMid:		x = .50; y = .75; break;
				case AbstractVisualItem::ZoomBottomLeft:	x = .33; y = .66; break;
				case AbstractVisualItem::ZoomLeftMid:		x = .25; y = .50; break;
				case AbstractVisualItem::ZoomCenter:
				default:					x = .50; y = .50; break;
			};
			
			m_zoomDestPoint = QPointF(x,y);
		}

	}
	else
	{
		m_zoomEnabled = false;
		if(m_zoomAnimationTimer->isActive())
			m_zoomAnimationTimer->stop();
	}
	
	setZValue(-9999);
	setVisible(true);
	update();
	
        m_dontSyncToModel = false;
}

void BackgroundContent::animateZoom()
{
	//elapsed = (elapsed + qobject_cast<QTimer*>(sender())->interval()) % 2000;
	//printf("elapsed=%d\n",elapsed);
	if(m_zoomDir > 0)
	{
		m_zoomCurSize.setX(m_zoomCurSize.x() + m_zoomStep.x());
		m_zoomCurSize.setY(m_zoomCurSize.y() + m_zoomStep.y());
		if(m_zoomCurSize.x() >= m_zoomEndSize.x() &&
		   m_zoomCurSize.y() >= m_zoomEndSize.y())
		   	if(modelItem()->zoomLoop())
				m_zoomDir = -1;
			else
				if(m_zoomAnimationTimer->isActive())
					m_zoomAnimationTimer->stop();
			
	}
	else
	{
		m_zoomCurSize.setX(m_zoomCurSize.x() - m_zoomStep.x());
		m_zoomCurSize.setY(m_zoomCurSize.y() - m_zoomStep.y());
		if(m_zoomCurSize.x() <= m_zoomStartSize.x() &&
		   m_zoomCurSize.y() <= m_zoomStartSize.y())
			if(modelItem()->zoomLoop())
				m_zoomDir = +1;
			else
				if(m_zoomAnimationTimer->isActive())
					m_zoomAnimationTimer->stop();
	}
	//qDebug() << "AnimateZoom: "<<PTR(this)<<modelItem()->itemName()<<": size:"<<m_zoomCurSize<<", step:"<<m_zoomStep;
	
	update();
}

#define BG_IMG_CACHE_DIR "dviz-backgroundimagecache"
void BackgroundContent::setImageFile(const QString &file)
{
	
	
	if(m_videoProvider)
	{
		// TODO We ASSUME were playing the video before we got the image
		//m_videoProvider->pause();
		m_videoProvider->disconnectReceiver(this);
		QVideoProvider::releaseProvider(m_videoProvider);
		m_videoProvider = 0;
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
	
/*	if()
	{
		setPixmap(MediaBrowser::iconForImage(file,MEDIABROWSER_LIST_ICON_SIZE));
		m_fileLoaded = true;
		return;
	}*/
	
	if(file.endsWith(".svg",Qt::CaseInsensitive))
	{
		loadSvg(file);
	}
	else
	{
		disposeSvgRenderer();
		//QString cacheKey = QString("%1:%2:%3x%4").arg(file).arg(fileMod);
		
		QPixmap cache;
		
		QSize size = contentsRect().size();
		
		if(modelItem()->zoomEffectEnabled() && modelItem()->zoomFactor() > 2.0)
		{
			size.setWidth(size.width()   * modelItem()->zoomFactor());
			size.setHeight(size.height() * modelItem()->zoomFactor());
		}
		
		QDir path(QString("%1/%2").arg(AppSettings::cachePath()).arg(BG_IMG_CACHE_DIR));
		if(!path.exists())
			QDir(AppSettings::cachePath()).mkdir(BG_IMG_CACHE_DIR);
			
		QString cacheKey = QString("%1/%2/%3-%4x%5%6")
					.arg(AppSettings::cachePath())
					.arg(BG_IMG_CACHE_DIR)
					.arg(MD5::md5sum(QFileInfo(file).fileName()))
					.arg(size.width())
					.arg(size.height())
					.arg(sceneContextHint() == MyGraphicsScene::Preview ? "-icon192" : "");
					//.arg(modelItem()->zoomEffectEnabled() ? "-zoomed" : "");
		
		if(!m_lastImageKey.isEmpty() &&
		    m_lastImageKey != cacheKey)
			QPixmapCache::remove(m_lastImageKey);
			
		m_lastImageKey = cacheKey;
		//qDebug() << "BackgroundContent::setImageFile: file:"<<file<<", size:"<<size<<", cacheKey:"<<cacheKey;

		if(QPixmapCache::find(cacheKey,cache))
		{
			setPixmap(cache);
			m_fileLoaded = true;
			//qDebug() << "ImageContent::loadFile: "<<file<<": pixmap cache hit on "<<cacheKey;
			//qDebug() << "BackgroundContent::setImageFile: file:"<<file<<", size:"<<size<<": hit RAM (loaded scaled from memory)";
		}
		else
		{
			if(QFile(cacheKey).exists())
			{
				cache.load(cacheKey);
				QPixmapCache::insert(cacheKey,cache);
				setPixmap(cache);
				m_fileLoaded = true;
				//qDebug() << "BackgroundContent::setImageFile: file:"<<file<<", size:"<<size<<": hit DISK (loaded scaled from disk cache)";
			}
			else
			{
				if(sceneContextHint() == MyGraphicsScene::Preview)
				{
					cache = MediaBrowser::iconForImage(file,QSize(192,120)); // MEDIABROWSER_LIST_ICON_SIZE);
					cache.save(cacheKey,"PNG");
					
					setPixmap(cache);
					m_fileLoaded = true;
					
					//qDebug() << "BackgroundContent::setImageFile: file:"<<file<<", size:"<<size<<": loaded MediaBrowser Icon, scaled and cached";
					
					//qDebug() << "ImageContent::loadFile: "<<file<<": pixmap cache MISS on "<<cacheKey;
					if(!QPixmapCache::insert(cacheKey, cache))
						qDebug() << "BackgroundContent::loadFile: "<<file<<": QPixmapCache::insert returned FALSE - pixmap not cached";
				}
				else
				{
					QImageReader reader(file);
					QImage image = reader.read();
					if(image.isNull())
					{
						if(reader.errorString().indexOf("Unable")>-1)
						{
							qDebug() << "BackgroundContent::setImageFile: Unable to read"<<file<<": "<<reader.errorString()<<", Trying to force-reset some cache space";
							
							QPixmapCache::setCacheLimit(10 * 1024);
							QPixmap testPm(1024,768);
							testPm.fill(Qt::lightGray);
							if(QPixmapCache::insert("test",testPm))
								qDebug() << "BackgroundContent::setImageFile: Unable to insert text pixmap into cache after shrinkage";
							
							//QPixmapCache::setCacheLimit(AppSettings::pixmapCacheSize() * 1024);
			
							image = reader.read();
							if(image.isNull())
							{
								qDebug() << "BackgroundContent::setImageFile: Still unable to read"<<file<<": "<<reader.errorString();
							}
						}
						else
						{
							qDebug() << "BackgroundContent::setImageFile: Unable to read"<<file<<": "<<reader.errorString();
						}
		
					}
		
					if(!image.isNull())
					{
						cache = QPixmap::fromImage(image.scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
						cache.save(cacheKey,"PNG");
						
						setPixmap(cache);
						m_fileLoaded = true;
						
						//qDebug() << "BackgroundContent::setImageFile: file:"<<file<<", size:"<<size<<": loaded original, scaled and cached";
						
						//qDebug() << "ImageContent::loadFile: "<<file<<": pixmap cache MISS on "<<cacheKey;
						if(!QPixmapCache::insert(cacheKey, cache))
							qDebug() << "BackgroundContent::loadFile: "<<file<<": QPixmapCache::insert returned FALSE - pixmap not cached";
					}
					else
					{
						qDebug() << "BackgroundContent::setImageFile: file:"<<file<<", size:"<<size<<": NOT LOADED";
					}
				}
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
				if(m_zoomEnabled)
				{
// 					if(m_zoomedPixmapSize != m_zoomCurSize)
// 					{
// 						m_zoomedPixmap = m_pixmap.scaled(m_zoomCurSize.x(), m_zoomCurSize.y(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
// 						m_zoomedPixmapSize = m_zoomCurSize;
// 					}
					
					painter->save();
					painter->setRenderHint(QPainter::Antialiasing);
					painter->setRenderHint(QPainter::SmoothPixmapTransform);
					painter->setClipRect(cRect);
					
					double xf = (1/m_zoomDestPoint.x());
					double yf = (1/m_zoomDestPoint.y());
					QRectF pr(cRect.width()/xf - m_zoomCurSize.x()/xf,cRect.height()/yf - m_zoomCurSize.y()/yf,m_zoomCurSize.x(),m_zoomCurSize.y());
					//qDebug() << PTR(this)<<": Paint at size"<<pr;
					painter->drawPixmap(pr,m_pixmap,m_pixmap.rect());
					//qDebug() << PTR(this)<<": End paint at size"<<pr;
					painter->restore();
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
					
					if(m_lastForegroundKey != foregroundKey &&
					!m_lastForegroundKey.isEmpty())
						QPixmapCache::remove(m_lastForegroundKey);
						
					m_lastForegroundKey = foregroundKey;
					
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
			
			if(m_zoomEnabled)
			{
				painter->save();
				painter->setRenderHint(QPainter::Antialiasing);
				painter->setRenderHint(QPainter::SmoothPixmapTransform);
				painter->setClipRect(cRect);
				
				double xf = (1/m_zoomDestPoint.x());
				double yf = (1/m_zoomDestPoint.y());
				QRectF pr(cRect.width()/xf - m_zoomCurSize.x()/xf,cRect.height()/yf - m_zoomCurSize.y()/yf,m_zoomCurSize.x(),m_zoomCurSize.y());
				//qDebug() << PTR(this)<<": Paint at size"<<pr;
				painter->drawPixmap(pr,m_pixmap,m_pixmap.rect());
				//qDebug() << PTR(this)<<": End paint at size"<<pr;
				painter->restore();
					
			}
			else
			{
				painter->drawPixmap(cRect, m_pixmap);
			}
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

#ifdef PHONON_ENABLED
void BackgroundContent::phononPlayerFinished()
{
	qDebug() << "VideoFileContent::phononPlayerFinished(): m_fileName="<<m_fileName;
//  	m_player->play(m_fileName);
	
}
#endif

void BackgroundContent::setVideoFile(const QString &name)
{
// 	if(!m_video->load(name))
// 	{
// 		qDebug() << "BackgroundContent::setFilename(): ERROR: Unable to load video"<<name;
// 		return;
// 	}
	m_fileName = name;
	if(DEBUG_BACKGROUNDCONTENT)
		qDebug() << "BackgroundContent::setVideoFile(): name="<<name;
	
	bool testBeta = true;
	
	if(modelItem()->fillType() == AbstractVisualItem::Video)
	{
		if(sceneContextHint() == MyGraphicsScene::Preview)
		{
			setPixmap(QVideoProvider::iconForFile(name));
		}
		#ifdef PHONON_ENABLED
		else
		if(modelItem()->videoEndAction() == AbstractVisualItem::VideoStop)
		{
			if(DEBUG_BACKGROUNDCONTENT)
				qDebug() << "BackgroundContent::setVideoFile(): Using Phonon";
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
			connect(m_player, SIGNAL(finished()), this, SLOT(phononPlayerFinished()));
			
			m_proxy->setWidget(m_player);
			m_proxy->setGeometry(contentsRect());

			m_player->play(name);
			
		}
		else
		#endif
		{
			if(DEBUG_BACKGROUNDCONTENT)	
				qDebug() << "BackgroundContent::setVideoFile(): Using FFMPEG";
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
			
			
			m_videoProvider->play();
			
			m_still = false;
			
			// prime the pump, so to speak
			setPixmap(m_videoProvider->pixmap());
				
			m_videoProvider->connectReceiver(this, SLOT(setPixmap(const QPixmap &)));
		}
	}
	else
	{
		m_still = true;
	}
	
	
	
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
		if(m_videoProvider)
			m_videoProvider->pause();
	}
        //GFX_CHANGED();
}

#ifdef PHONON_ENABLED
void BackgroundContent::phononStateChanged(Phonon::State newState, Phonon::State /* oldState */)
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

void BackgroundContent::phononTick(qint64 time)
{
	if(timeLcd)
	{
		QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
		
		timeLcd->display(displayTime.toString("mm:ss"));
	}
}
#endif

QWidget * BackgroundContent::controlWidget()
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

