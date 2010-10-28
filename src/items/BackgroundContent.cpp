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
#include <QPushButton>

#include "SlideGroupViewer.h"

#include "qvideo/QVideoProvider.h"
#include "qvideo/QVideo.h"
#include "3rdparty/md5/qtmd5.h"
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
    , m_videoPlaying(false)
    , m_inDestructor(false)
    , m_controlBase(0)
    , m_slider(0)
    , m_lockSeekValueChanging(false)
    , m_startVideoPausedInPreview(true)
    , m_playBtn(0)
    , m_pauseBtn(0)
    , m_videoPauseEventCompleted(false)
#ifdef PHONON_ENABLED
    , m_proxy(0)
    , m_player(0)
    , m_tuplet(0)
#endif
    , m_isUserPlaying(false)
    , m_isUserPaused(false)
    , m_viewerWidget(0)
    , m_speedSlider(0)
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

// 	qDebug() << "BackgroundContent::~BackgroundContent(): hit "<<this;

	m_inDestructor = true;

	if(m_videoProvider)
	{
 		if(m_videoPlaying)
 			m_videoProvider->pause();
		m_videoProvider->disconnectReceiver(this);
		QVideoProvider::releaseProvider(m_videoProvider);
	}

	if(m_controlBase)
	{
 		m_controlBase->deleteLater();
 		m_controlBase = 0;
//
	}
	if(m_slider)
	{
 		m_slider->deleteLater();
 		m_slider = 0;
	}

	if(m_videoPollTimer.isActive())
		m_videoPollTimer.stop();
#ifdef PHONON_ENABLED
	if(m_proxy)
		delete m_proxy;
#endif

	if(!m_lastForegroundKey.isEmpty())
		QPixmapCache::remove(m_lastForegroundKey);
}

// :: QVideoConsumer
bool BackgroundContent::allowMediaStop(QVideoProvider* p)
{
	if(!m_inDestructor &&
	    isVisible()    &&
	    p == m_videoProvider)
		return false;
	return true;
}

// ::AbstractDisposeable
void BackgroundContent::dispose(bool anim)
{
	m_inDestructor = true;

	if(m_videoPollTimer.isActive())
	{
		m_videoPollTimer.stop();
	}

	if(m_videoProvider && m_videoPlaying)
	{
		m_videoPlaying = false;
		m_videoProvider->pause();
	}

//  	qDebug() << "BackgroundContent::dispose(): hit "<<this;

	AbstractContent::dispose(anim);
}

// ::QGraphicsItem
void BackgroundContent::show()
{
	if(m_videoProvider && !m_videoPlaying) // && (sceneContextHint() != MyGraphicsScene::Preview || !m_startVideoPausedInPreview))
	{
		//qDebug() << "BackgroundContent::show: Playing video";
		m_videoPlaying = true;
		m_videoProvider->play();
 	}
 	else
 	{
 		//qDebug() << "BackgroundContent::show: No provider, not playing video";
 	}
 	AbstractContent::show();

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


	if(model->fillVideoFile() != "" &&
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
	if(m_viewerWidget)
		m_viewerWidget->update();
	else
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

	if(m_viewerWidget)
		m_viewerWidget->update();
	else
		update();
}

#define BG_IMG_CACHE_DIR "dviz-backgroundimagecache"
void BackgroundContent::setImageFile(const QString &file)
{


	if(m_videoProvider)
	{
		// TODO We ASSUME were playing the video before we got the image
		//m_videoProvider->pause();
 		if(m_videoPlaying)
 		{
 			m_videoPlaying = false;
 			m_videoProvider->pause();
 		}

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
		qDebug() << "BackgroundContent::setImageFile: "<<file<<": no change, not reloading";
		return;
	}

// 	qDebug() << "BackgroundContent::setImageFile: "<<file<<": (current file:"<<m_fileName<<"), fileMod:"<<fileMod<<", m_fileLastModified:"<<m_fileLastModified;

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

		// We adjust the size of the expected image if over 2.0 because over 2
		// the pixelation is more visible.
		if(modelItem()->zoomEffectEnabled() && modelItem()->zoomFactor() > 2.0)
		{
			size.setWidth(size.width()   * modelItem()->zoomFactor());
			size.setHeight(size.height() * modelItem()->zoomFactor());
		}

		QDir path(QString("%1/%2").arg(AppSettings::cachePath()).arg(BG_IMG_CACHE_DIR));
		if(!path.exists())
			QDir(AppSettings::cachePath()).mkdir(BG_IMG_CACHE_DIR);

		QString cacheKey = QString("%1/%2/%3-%4x%5%6-auto_ar.jpg")
					.arg(AppSettings::cachePath())
					.arg(BG_IMG_CACHE_DIR)
					.arg(MD5::md5sum(file))
					.arg(size.width())
					.arg(size.height())
					.arg(sceneContextHint() == MyGraphicsScene::StaticPreview ? "-icon192" : "");
					//.arg(modelItem()->zoomEffectEnabled() ? "-zoomed" : "");

		if(sceneContextHint() == MyGraphicsScene::StaticPreview)
		{
// 			qDebug() << "BackgroundContent::setImageFile: "<<file<<": static preview, warming";

			QString cacheKey = QString("%1/%2/%3-%4x%5-icon192-auto_ar.jpg")
						.arg(AppSettings::cachePath())
						.arg(BG_IMG_CACHE_DIR)
						.arg(MD5::md5sum(file))
						.arg(size.width())
						.arg(size.height());
						//.arg(modelItem()->zoomEffectEnabled() ? "-zoomed" : "");

			if(!QPixmapCache::find(cacheKey,cache))
			{
// 				qDebug() << "BackgroundContent::setImageFile: "<<file<<": static preview, not in QPixmapCache, starting thread";
				new BackgroundImageWarmingThreadManager(dynamic_cast<BackgroundItem*>(modelItem()),cacheKey,contentsRect());
			}
		}

		if(!m_lastImageKey.isEmpty() &&
		    m_lastImageKey != cacheKey)
			QPixmapCache::remove(m_lastImageKey);

		m_lastImageKey = cacheKey;
		//qDebug() << "BackgroundContent::setImageFile: file:"<<file<<", size:"<<size<<", cacheKey:"<<cacheKey;

		if(QPixmapCache::find(cacheKey,cache) && QFileInfo(file).lastModified() <= QFileInfo(cacheKey).lastModified())
		{
			setPixmap(cache);
			m_fileLoaded = true;
			//qDebug() << "ImageContent::loadFile: "<<file<<": pixmap cache hit on "<<cacheKey;
// 			qDebug() << "BackgroundContent::setImageFile: file:"<<file<<", size:"<<size<<": hit RAM (loaded scaled from memory)";
		}
		else
		{
// 			qDebug() << "BackgroundContent::setImageFile: "<<file<<": Not in RAM";
			QPixmap cache;
			if(sceneContextHint() == MyGraphicsScene::StaticPreview)
			{
// 				qDebug() << "BackgroundContent::setImageFile: "<<file<<": static preview, using MB";
				cache = MediaBrowser::iconForImage(file,QSize(192,120)); // MEDIABROWSER_LIST_ICON_SIZE);
			}
			else
			{
// 				qDebug() << "BackgroundContent::setImageFile: "<<file<<": using internal load file";
				QImage * cacheImg = internalLoadFile(file,cacheKey,contentsRect());
				if(cacheImg)
				{
					cache = QPixmap::fromImage(*cacheImg);
					delete cacheImg;
					cacheImg = 0;
				}
			}

			QPixmapCache::insert(cacheKey,cache);
			setPixmap(cache);
			m_fileLoaded = true;
		}
	}
}

QImage * BackgroundContent::internalLoadFile(QString file,QString cacheKey, QRect contentsRect)
{
	QImage * cache = 0;
	if(QFile(cacheKey).exists() && QFileInfo(file).lastModified() <= QFileInfo(cacheKey).lastModified())
	{
		cache = new QImage();
		cache->load(cacheKey);
		return cache;


		//qDebug() << "BackgroundContent::setImageFile: file:"<<file<<", size:"<<size<<": hit DISK (loaded scaled from disk cache)";
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
// 			qDebug() << "BackgroundContent::setImageFile: file:"<<file<<": loading from DISK";
			// Re-render the image if AR difference between Image and Item is greater than X%
			// TODO: Make the handling of a mistmatched AR user-selectable a la Windows Desktop Background dialog:
			// The options could be: If picture size different than slide, either (A) stretch - default,
			// (B) center, or (C) auto-center if difference greater than some %
			// The user would see 'size' but what they really mean (without knowing it) is the A/R difference.

			QSize imageSize = image.size();
			double imageAr  = (double)imageSize.width() / (double)imageSize.height();

			QRect rect = contentsRect;
			QSize size = rect.size();
			double itemAr = (double)rect.width() / (double)rect.height();

			double diff = fabs(imageAr - itemAr);
			double percentDiff = diff / imageAr;
			if(percentDiff > 0.05) //0.125) // arbitrary difference
			{
				// Repaint image with image centered at original ar, with field of black around it

				int width, height;
				if(imageSize.width() > imageSize.height())
				{
					// set width to rect.width and then scale height according to imageAr
					width = rect.width();
					height = width * (1/imageAr);
				}
				else
				{
					// set height to rect.height since its taller than it is wide, and scale width according to imageAr
					height = rect.height();
					width = height * imageAr;
				}
				int y = rect.height()/2 - height/2;
				int x = rect.width()/2  - width/2;

				// declar a QRect just for ease of debugging
				QRect targetRect(x,y,width,height);

				QImage newImage(rect.size(),QImage::Format_ARGB32_Premultiplied);

				QPainter painter(&newImage);
				painter.fillRect(newImage.rect(),Qt::black);

				painter.drawImage(targetRect,image);

				image = newImage;

// 				qDebug() << "Fixing AR difference, imageAr:"<<imageAr<<", itemAr:"<<itemAr<<",diff:"<<diff<<",precentDiff:"<<percentDiff<<", targetRect:"<<targetRect<<", imageSize:"<<imageSize<<",rect:"<<rect;

			}
			else
			{
// 				qDebug() << "[NOT] Fixing AR difference, imageAr:"<<imageAr<<", itemAr:"<<itemAr<<",diff:"<<diff<<",precentDiff:"<<percentDiff;
			}

			cache = new QImage(image.scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
			cache->save(cacheKey,"JPEG");

			return cache;
		}
		else
		{
			qDebug() << "BackgroundContent::setImageFile: file:"<<file<<" NOT LOADED";
		}
	}

	return 0;
}

BackgroundImageWarmingThreadManager::BackgroundImageWarmingThreadManager(BackgroundItem *model, QString key, QRect rect) : m_model(model), m_cacheKey(key), m_rect(rect)
{
	QPixmap cache;

	if(QPixmapCache::find(key,cache))
	{
		//qDebug()<<"TextBoxWarmingThreadManager(): modelItem:"<<model->itemName()<<": Cache HIT";
		deleteLater();
	}
	else
	if(QFile(key).exists() && QFileInfo(model->fillImageFile()).lastModified() <= QFileInfo(key).lastModified())
	{
		qDebug()<<"TextBoxWarmingThreadManager(): modelItem:"<<model->itemName()<<": Cache load from"<<key<<" done";
		cache.load(key);
		QPixmapCache::insert(key,cache);
		deleteLater();
		qDebug()<<"TextBoxWarmingThreadManager(): modelItem:"<<model->itemName()<<": Cache load from"<<key<<" finish";
	}
	else
	{
		//qDebug()<<"BackgroundImageWarmingThreadManager(): modelItem:"<<model->itemName()<<": Cache MISS";
		m_thread = new BackgroundImageWarmingThread(model,key,rect);
		connect(m_thread, SIGNAL(renderDone(QImage*)), this, SLOT(renderDone(QImage*)));
		connect(m_thread, SIGNAL(finished()), m_thread, SLOT(deleteLater()));
		m_thread->start();
	}
}

void BackgroundImageWarmingThreadManager::renderDone(QImage *image)
{
	if(!image)
	{
		deleteLater();
		return;
	}

	QPixmap cache = QPixmap::fromImage(*image);
	cache.save(m_cacheKey,"JPEG");
	QPixmapCache::insert(m_cacheKey, cache);
	delete image; // QPixmap::fromImage() made a copy, so we dont need to waste this memory here
	deleteLater();
}

BackgroundImageWarmingThread::BackgroundImageWarmingThread(BackgroundItem *model, QString key, QRect rect) : m_model(model), m_cacheKey(key), m_rect(rect) {}
void BackgroundImageWarmingThread::run()
{
	if(!m_model)
	{
		qDebug()<<"BackgroundImageWarmingThread::run(): m_model is null";
		return;
	}

	QString file = AppSettings::applyResourcePathTranslations(m_model->fillImageFile());
	QImage * image = BackgroundContent::internalLoadFile(file,m_cacheKey,m_rect);
	emit renderDone(image);
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
	if(m_viewerWidget)
		m_viewerWidget->update();
	else
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


void BackgroundContent::setViewerWidget(SlideGroupViewer *viewer)
{
	m_viewerWidget = viewer;
}

void BackgroundContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	if(m_viewerWidget && m_viewerWidget!=widget)
	{
		return;	
	}
	else
	{
		// paint parent
		AbstractContent::paint(painter, option, widget);
		paintBackground(painter,option->exposedRect.toRect());
	}
}

void BackgroundContent::paintBackground(QPainter *painter, const QRect & exposedRect)
{
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
					QString foregroundTmp = QString("%1:%2:%3:%4")
								.arg(m_fileName).arg(m_fileLastModified)
								.arg(destRect.width()).arg(destRect.height());

					// ASSUME path exists due to using BG_IMG_CACHE_DIR in loading the image originally
// 					QDir path(QString("%1/%2").arg(AppSettings::cachePath()).arg(BG_IMG_CACHE_DIR));
// 					if(!path.exists())
// 						QDir(AppSettings::cachePath()).mkdir(BG_IMG_CACHE_DIR);

					QString foregroundKey = QString("%1/%2/%3-painter")
								.arg(AppSettings::cachePath())
								.arg(BG_IMG_CACHE_DIR)
								.arg(MD5::md5sum(foregroundTmp));

					//qDebug() << "foregroundKey: "<<foregroundKey;

					if(m_lastForegroundKey != foregroundKey &&
					  !m_lastForegroundKey.isEmpty())
					{
						QPixmapCache::remove(m_lastForegroundKey);
// 						QFile tmp(m_lastForegroundKey);
// 						if(tmp.exists())
// 							tmp.remove();
					}

					m_lastForegroundKey = foregroundKey;

					QPixmap cache;
					if(!QPixmapCache::find(foregroundKey,cache))
					{
// 						if(QFile(foregroundKey).exists())
// 						{
// 							cache.load(foregroundKey);
// 							QPixmapCache::insert(foregroundKey,cache);
// 							//qDebug() << "MediaBrowser::iconForImage: file:"<<file<<", size:"<<size<<": hit DISK (loaded scaled from disk cache)";
// 						}
// 						else
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
					}

					painter->setClipRect(exposedRect);
					painter->drawPixmap(cRect,cache);
				}
			}
		}
	}
	else // Video
	{
		if(m_imageSize.width() <= 0)
		{
// 			painter->fillRect(cRect,QBrush(Qt::gray));

// 			painter->fillRect(QRect(0,0,1024/2,768/2),QBrush(Qt::gray));
		}
		else
		{
			//painter->setBrush(Qt::NoBrush);
			//painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

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
				//qDebug() << "BackgroundContent::paint:"<<modelItem()->itemName()<<": Painting video frame";
				//painter->save();
				//painter->setCompositionMode(QPainter::CompositionMode_DestinationOver);
				//painter->setClipRect(exposedRect);

				/*
				QPixmap pm(m_pixmap.size());
				pm.fill(Qt::blue);
				painter->drawPixmap(cRect, pm);
				*/

				painter->drawPixmap(cRect, m_pixmap);
				//painter->restore();

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


void BackgroundContent::phononMediaAboutToFinish()
{
	qDebug() << "VideoFileContent::phononMediaAboutToFinish(): m_fileName="<<m_fileName;
//  	m_player->play(m_fileName);
	if(modelItem()->videoEndAction() == AbstractVisualItem::VideoLoop)
	{
		qDebug() << "VideoFileContent::phononMediaAboutToFinish(): enqueing file "<<m_fileName<<" inorder to loop video";
		m_player->mediaObject()->enqueue(m_fileName);
		m_player->mediaObject()->setTransitionTime(-100);
	}

}
void BackgroundContent::phononPlayerFinished()
{
	qDebug() << "VideoFileContent::phononPlayerFinished(): m_fileName="<<m_fileName;
//  	m_player->play(m_fileName);
	if(modelItem()->videoEndAction() == AbstractVisualItem::VideoLoop)
	{
		qDebug() << "VideoFileContent::phononPlayerFinished(): playing file again";
		m_player->seek(0);
		m_player->play();
	}

}



void BackgroundContent::phononPrefinishMarkReached(qint32 x)
{
	qDebug() << "VideoFileContent::phononPrefinishMarkReached(): m_fileName="<<m_fileName<<", x:"<<x;
//  	m_player->play(m_fileName);
	if(modelItem()->videoEndAction() == AbstractVisualItem::VideoLoop)
	{
		qDebug() << "VideoFileContent::phononPrefinishMarkReached(): playing file again";
		m_player->seek(0);
		m_player->play();
	}

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

// 	bool testBeta = true;

	if(modelItem()->fillType() == AbstractVisualItem::Video)
	{
		if(sceneContextHint() == MyGraphicsScene::StaticPreview ||
		   sceneContextHint() == MyGraphicsScene::Monitor)
		{
			setPixmap(QVideoProvider::iconForFile(name));
		}
		#ifdef PHONON_ENABLED
		else
// 		if(modelItem()->videoEndAction() == AbstractVisualItem::VideoStop)
		{
			if(DEBUG_BACKGROUNDCONTENT)
				qDebug() << "BackgroundContent::setVideoFile(): Using Phonon";
// 			if(m_proxy)
// 				delete m_proxy;
 			if(m_player)
 				delete m_player;
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
// 			connect(m_player->mediaObject(), SIGNAL(aboutToFinish()), this, SLOT(phononMediaAboutToFinish()));
			connect(m_player, SIGNAL(finished()), this, SLOT(phononPlayerFinished()));
// 			m_player->mediaObject()->setPrefinishMark(30);
// 			connect(m_player->mediaObject(), SIGNAL(prefinishMarkReached(qint32)), this, SLOT(phononPrefinishMarkReached(qint32)));

			m_proxy->setWidget(m_player);
			m_proxy->setGeometry(contentsRect());

			if(sceneContextHint() == MyGraphicsScene::Live)
				m_player->play(name);

		}
		#else
 		else
		{
			if(DEBUG_BACKGROUNDCONTENT)
				qDebug() << "BackgroundContent::setVideoFile(): Using FFMPEG";

			QVideoProvider * p = QVideoProvider::providerForFile(name);
			if(p)
			{

				if(m_videoProvider && m_videoProvider == p)
				{
					return;
				}
				else
				if(m_videoProvider)
				{
					// Copy the pointer and set m_vid.. to 0 for use in allowMediaStop(), etc.
					QVideoProvider * tmp = m_videoProvider;
					m_videoProvider = 0;

					if(m_videoPlaying)
	 					//if(!m_still || m_videoProvider->isPlaying())
	 				{
	 					//qDebug() << "BackgroundContent::setVideoFile: Visible, pausing video";
	 					m_videoPlaying = false;
 						tmp->pause();
 					}


					disconnect(tmp->videoObject(), 0, this, 0);

					tmp->disconnectReceiver(this);
					QVideoProvider::releaseProvider(tmp);
				}

				if(DEBUG_BACKGROUNDCONTENT)
					qDebug() << "BackgroundContent::setVideoFile: Loading"<<name;

				m_videoProvider = p;

				connect(m_videoProvider->videoObject(), SIGNAL(movieStateChanged(QMovie::MovieState)), this, SLOT(movieStateChanged(QMovie::MovieState)));

				//qDebug() << "BackgroundContent::setVideoFile: Playing video "<<name;

				//m_startVideoPausedInPreview = true;
				m_videoPauseEventCompleted = false;

// 				if(sceneContextHint() == MyGraphicsScene::Preview &&
// 				   m_startVideoPausedInPreview)
// 				{
// 					// Only really pause the video in preview if no other streams
// 					// are connected to it
// 					if(m_videoProvider->stopAllowed())
// 					{
// 						m_videoPlaying = false;
// 						m_videoProvider->pause();
// 					}
// 				}
// 				else
// 				{
					if(!m_videoPlaying)
					{
						//qDebug() << "BackgroundContent::setVideoFile: Visible, Playing video "<<name;
						m_videoProvider->play();
						m_videoPlaying = true;
					}
					else
					{
						//qDebug() << "BackgroundContent::setVideoFile: Not visible, not playing video";
					}
//				}

				m_still = false;

				// prime the pump, so to speak
				setPixmap(m_videoProvider->pixmap());

				m_videoProvider->connectReceiver(this, SLOT(setPixmap(const QPixmap &)));
			}
			else
			{
				//if(DEBUG_BACKGROUNDCONTENT)
					qDebug() << "BackgroundContent::setVideoFile: Unable to get provider for "<<name;
			}
		}
		#endif
	}
	else
	{
		qDebug() << "BackgroundContent::setVideoFile: Set m_still true";
		m_still = true;
	}



}


void BackgroundContent::setPixmap(const QPixmap & pixmap)
{
	if(m_still && m_imageSize.width() > 0)
		return;

	m_pixmap = pixmap;

	/*
	if(modelItem()->fillType() == AbstractVisualItem::Image)
	{
		// Re-render the image if AR difference between Image and Item is greater than X%
		// TODO: Make the handling of a mistmatched AR user-selectable a la Windows Desktop Background dialog:
		// The options could be: If picture size different than slide, either (A) stretch - default,
		// (B) center, or (C) auto-center if difference greater than some %
		// The user would see 'size' but what they really mean (without knowing it) is the A/R difference.

		QSize size = m_pixmap.size();
		double imageAr  = (double)size.width() / (double)size.height();

		QRect rect = contentsRect();
		double itemAr = (double)rect.width() / (double)rect.height();

		double diff = fabs(imageAr - itemAr);
		double percentDiff = diff / imageAr;
		if(percentDiff > 0.10) // arbitrary difference
		{
			// Repaint image with image centered at original ar, with field of black around it
			QPixmap pixmap(rect.size());
			pixmap.fill(Qt::black);
			QPainter painter(&pixmap);

			int width, height;
			if(size.width() > size.height())
			{
				// set width to rect.width and then scale height according to imageAr
				width = rect.width();
				height = width * (1/imageAr);
			}
			else
			{
				// set height to rect.height since its taller than it is wide, and scale width according to imageAr
				height = rect.height();
				width = height * imageAr;
			}
			int y = rect.height()/2 - height/2;
			int x = rect.width()/2  - width/2;

			// declar a QRect just for ease of debugging
			QRect targetRect(x,y,width,height);
			painter.drawPixmap(targetRect,m_pixmap);

			qDebug() << "Fixing AR difference, imageAr:"<<imageAr<<", itemAr:"<<itemAr<<",diff:"<<diff<<",precentDiff:"<<percentDiff<<", targetRect:"<<targetRect<<", size:"<<size<<",rect:"<<rect;

		}
		else
		{
			qDebug() << "[NOT] Fixing AR difference, imageAr:"<<imageAr<<", itemAr:"<<itemAr<<",diff:"<<diff<<",precentDiff:"<<percentDiff;
		}


	}
	*/

	if(m_imageSize != m_pixmap.size())
		m_imageSize = m_pixmap.size();

	if(m_viewerWidget)
		m_viewerWidget->update();
	else
		update();

	if(((sceneContextHint() != MyGraphicsScene::Live &&
	     sceneContextHint() != MyGraphicsScene::Preview)
	     || (sceneContextHint() == MyGraphicsScene::Preview && m_startVideoPausedInPreview && !m_isUserPlaying)) &&
		modelItem()->fillType() == AbstractVisualItem::Video &&
		m_imageSize.width() > 0)
	{
		if(DEBUG_BACKGROUNDCONTENT)
			qDebug() << "BackgroundContent::setPixmap(): sceneContextHint() != Live/Preview, setting m_still true";
// 		qDebug() << "BackgroundContent::setPixmap(): m_videoPlaying:"<<m_videoPlaying<<", provider:"<<m_videoProvider;
		m_still = true;
		if(m_videoProvider && m_videoPlaying)
		{
			m_videoPlaying = false;
			m_videoProvider->pause();
		}
		m_videoPauseEventCompleted = true;
	}
        //GFX_CHANGED();
}

#ifdef PHONON_ENABLED
void BackgroundContent::phononStateChanged(Phonon::State newState, Phonon::State /* oldState */)
{
	if(m_inDestructor)
		return;
	if(m_timeLcd)
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
				m_playAction->setEnabled(false);
				m_pauseAction->setEnabled(true);
				m_stopAction->setEnabled(true);
				break;
			case Phonon::StoppedState:
				m_stopAction->setEnabled(false);
				m_playAction->setEnabled(true);
				m_pauseAction->setEnabled(false);
				m_timeLcd->display("00:00");
				break;
			case Phonon::PausedState:
				m_pauseAction->setEnabled(false);
				m_stopAction->setEnabled(true);
				m_playAction->setEnabled(true);
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
	if(m_timeLcd)
	{
		QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);

		m_timeLcd->display(displayTime.toString("mm:ss"));
	}
}
#endif

void BackgroundContent::pollVideoClock()
{
// 	if(!m_slider)
// 		return;

	if(m_inDestructor)
		return;

	if(m_lockSeekValueChanging)
		return;

	m_lockSeekValueChanging = true;

  	qDebug() << "BackgroundContent::pollVideoClock(): hit controlBase: "<<m_controlBase;

	if(m_videoProvider &&
	   m_videoPlaying &&
	   modelItem()->fillVideoFile() != "" &&
	   modelItem()->fillType() == AbstractVisualItem::Video)
	{
		int clock = (int)m_videoProvider->videoClock();

	  	if(m_slider)
		{
			if(m_slider->maximum() != m_videoProvider->duration())
			{
				//qDebug() << "BackgroundContent::pollVideoClock(): Updating maximum to "<<m_videoProvider->duration();
				m_slider->setMaximum(m_videoProvider->duration());
			}
 			m_slider->setValue(clock);
	  	}

		if(m_timeLcd)
		{
// 			QTime displayTime(0, (clock / 60000) % 60, (clock / 1000) % 60);

			double time = (double)clock / 1000.0;
			double min = time/60;
			double sec = (min - (int)(min)) * 60;
			double ms  = (sec - (int)(sec)) * 60;
			QString str  =  (min<10? "0":"") + QString::number((int)min) + ":" +
					(sec<10? "0":"") + QString::number((int)sec) + "." +
					(ms <10? "0":"") + QString::number((int)ms );

			m_timeLcd->display(str);
		}

 		//qDebug() << "BackgroundContent::pollVideoClock(): Clock at "<<((int)clock)<<" / "<<m_videoProvider->duration(); //<<", time used:"<<t.elapsed();
	}

	m_lockSeekValueChanging = false;
}

void BackgroundContent::seek(int x)
{
	if(m_lockSeekValueChanging)
		return;

	int delta = x; // - m_videoProvider->videoClock();
	m_videoProvider->seekTo(abs(delta), delta < 0 ? AVSEEK_FLAG_BACKWARD : 0);
 	qDebug() << "BackgroundContent::seek(): x:"<<x<<", delta:"<<delta<<", AVSEEK_FLAG_BACKWARD:"<<AVSEEK_FLAG_BACKWARD;
}

void BackgroundContent::controlWidgetDestroyed()
{
	m_videoPollTimer.stop();
	m_slider = 0;
	m_controlBase = 0;
	m_pauseBtn = 0;
	m_playBtn = 0;
}

void BackgroundContent::movieStateChanged(QMovie::MovieState state)
{
// 	qDebug() << "BackgroundContent::moveStateChanged: m_startVideoPausedInPreview:"<<m_startVideoPausedInPreview<<",m_still:"<<m_still<<",state:"<<state<<"==QMovie::Running:"<<QMovie::Running<<",m_videoPauseEventCompleted:"<<m_videoPauseEventCompleted;
	if(m_startVideoPausedInPreview &&
	   m_still &&
	   state == QMovie::Running &&
	   m_isUserPlaying)
	{
// 		qDebug() << "BackgroundContent::moveStateChanged: Set m_still false";
		m_still = false;
		//m_startVideoPausedInPreview = false;
	}

	if(!m_isUserPlaying || !m_isUserPaused)
		return;

	if(!m_playBtn ||
	   !m_pauseBtn)
	{
// 		qDebug() << "BackgroundContent::moveStateChanged: Buttons destroyed/non existant, not updating";
		return;
	}

	if(state == QMovie::Running)
	{
		m_playBtn->setEnabled(false);
		m_pauseBtn->setEnabled(true);
	}
	else
	{
		m_playBtn->setEnabled(true);
		m_pauseBtn->setEnabled(false);
	}
}

void BackgroundContent::setVideoSpeed1(int speed)
{
// 	qDebug() << "BackgroundContent::setVideoSpeed1: speed:"<<speed;
	if(m_speedSlider->value() != speed)
		m_speedSlider->setValue(speed);
	double d = (double)speed / 10.0;
//  	qDebug() << "BackgroundContent::setVideoSpeed1:     d:"<<d;
	if(m_speedSpinBox->value() != d)
		m_speedSpinBox->setValue(d);
		
	if(m_videoProvider)
		m_videoProvider->videoObject()->setVideoSpeedMultiplier(d);
	modelItem()->setVideoSpeedMultiplier(d);
}

void BackgroundContent::setVideoSpeed2(double speed)
{
// 	qDebug() << "BackgroundContent::setVideoSpeed2: speed:"<<speed;
	if(m_speedSpinBox->value() != speed)
		m_speedSpinBox->setValue(speed);
		
	//m_speedSlider->setValue(speed * 10);
	double d = speed * 10.0;
//  	qDebug() << "BackgroundContent::setVideoSpeed2:     d:"<<d;
// 	setVideoSpeed1(d);
	if(m_speedSlider->value() != d)
		m_speedSlider->setValue(d);
	
	if(m_videoProvider)
		m_videoProvider->videoObject()->setVideoSpeedMultiplier(speed);
	modelItem()->setVideoSpeedMultiplier(speed);
}

QWidget * BackgroundContent::controlWidget()
{
//  	return 0;

	if(modelItem()->fillVideoFile() != "" &&
	   modelItem()->fillType() == AbstractVisualItem::Video)
	{
// 		if(m_controlBase)
// 			return m_controlBase;

		if(!m_videoProvider)
			return 0;

		if(!m_videoProvider->canPlayPause())
			return 0;

		m_controlBase = new QWidget();

// 		m_slider = new QSlider();
// 		m_slider->setOrientation(Qt::Horizontal);
// 		m_slider->setMaximum(m_videoProvider->duration());
// 		connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));

// 		m_videoPollTimer.setInterval(250);
// 		connect(&m_videoPollTimer, SIGNAL(timeout()), this, SLOT(pollVideoClock())); //, Qt::QueuedConnection);
// 		m_videoPollTimer.start();

		m_speedSlider = new QSlider(m_controlBase);
		m_speedSlider->setOrientation(Qt::Horizontal);
		m_speedSlider->setMinimum(1);
		m_speedSlider->setMaximum(20);
		connect(m_speedSlider, SIGNAL(valueChanged(int)), this, SLOT(setVideoSpeed1(int)));
		
		m_speedSpinBox = new QDoubleSpinBox(m_controlBase);
		m_speedSpinBox->setMinimum(0.1);
		m_speedSpinBox->setMaximum(2);
		m_speedSpinBox->setSingleStep(0.1);
// 		m_speedSpinBox->setPageStep(0.3);
		connect(m_speedSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setVideoSpeed2(double)));
		
		setVideoSpeed2(modelItem()->videoSpeedMultiplier());
		
		QHBoxLayout *layout = new QHBoxLayout(m_controlBase);
		layout->setMargin(0);


		m_playBtn = new QPushButton(qApp->style()->standardIcon(QStyle::SP_MediaPlay),"",m_controlBase);
		//m_playBtn->setEnabled(false);
// 		m_playAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), m_controlBase);
// 		m_playAction->setShortcut(tr("Crl+P"));

		m_pauseBtn = new QPushButton(qApp->style()->standardIcon(QStyle::SP_MediaPause),"",m_controlBase);
		//m_pauseBtn->setEnabled(true);
		//m_pauseAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), m_controlBase);
// 		m_pauseAction->setShortcut(tr("Ctrl+A"));


		//m_playBtn->setEnabled(!m_startVideoPausedInPreview && !m_videoPlaying);
		m_pauseBtn->setEnabled(m_startVideoPausedInPreview && m_videoPlaying);

		/// NOT showing Stop action right now because QVideo / QVideoDecoder have a "hard time" restarting playing after
		// stop() is called. After stop() is called, then play(), then there is a large delay before video starts flowing
		// again.

// 		m_stopAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), m_controlBase);
// 		m_stopAction->setShortcut(tr("Ctrl+S"));

// 		m_timeLcd = new QLCDNumber(m_controlBase);

		connect(m_playBtn,  SIGNAL(clicked()), this, SLOT(playBtnClicked()));
		connect(m_pauseBtn, SIGNAL(clicked()), this, SLOT(pauseBtnClicked()));
// 		connect(m_stopAction,  SIGNAL(triggered()), m_videoProvider->videoObject(), SLOT(stop()));

// 		QToolBar *bar = new QToolBar(m_controlBase);
//
// 		bar->addAction(m_playAction);
// 		bar->addAction(m_pauseAction);
// 		bar->addAction(m_stopAction);

// 		layout->addWidget(bar);

		layout->addWidget(new QLabel(QString("<b>%1</b>").arg(QFileInfo(m_videoProvider->canonicalFilePath()).fileName())));
		layout->addWidget(m_playBtn);
		layout->addWidget(m_pauseBtn);
		layout->addWidget(new QLabel("Speed:"));
		layout->addWidget(m_speedSlider);
		layout->addWidget(m_speedSpinBox);
		layout->addStretch(1);

// 		layout->addWidget(m_slider);
// 		layout->addWidget(m_timeLcd);




		connect(m_controlBase, SIGNAL(destroyed()), this, SLOT(controlWidgetDestroyed()));

		return m_controlBase;

	}
	else
	{
#ifdef PHONON_ENABLED
		if(!m_player)
			return 0;

		mediaObject = m_player->mediaObject();

		mediaObject->setTickInterval(1000);

		connect(mediaObject, SIGNAL(tick(qint64)), this, SLOT(phononTick(qint64)));
		connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(phononStateChanged(Phonon::State, Phonon::State)));

		QWidget * baseWidget = new QWidget;

		m_playAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), baseWidget);
		m_playAction->setShortcut(tr("Crl+P"));
		m_playAction->setDisabled(true);
		m_pauseAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), baseWidget);
		m_pauseAction->setShortcut(tr("Ctrl+A"));
		m_pauseAction->setDisabled(true);
		m_stopAction = new QAction(qApp->style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), baseWidget);
		m_stopAction->setShortcut(tr("Ctrl+S"));
		m_stopAction->setDisabled(true);


		connect(m_playAction, SIGNAL(triggered()), mediaObject, SLOT(play()));
		connect(m_pauseAction, SIGNAL(triggered()), mediaObject, SLOT(pause()) );
		connect(m_stopAction, SIGNAL(triggered()), mediaObject, SLOT(stop()));

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

		m_timeLcd = new QLCDNumber(baseWidget);
		m_timeLcd->setPalette(palette);

		QHBoxLayout *playbackLayout = new QHBoxLayout(baseWidget);
		playbackLayout->setMargin(0);

		playbackLayout->addWidget(bar);
	// 	playbackLayout->addStretch();
		playbackLayout->addWidget(seekSlider);
		playbackLayout->addWidget(m_timeLcd);

		playbackLayout->addWidget(volumeLabel);
		playbackLayout->addWidget(volumeSlider);

		m_timeLcd->display("00:00");

		qDebug() << "BackgroundContent::controlWidget(): baseWidget:"<<baseWidget;

		return baseWidget;
#else
		return 0;
#endif
	}
}

void BackgroundContent::playBtnClicked()
{
	m_isUserPlaying = true;
	m_isUserPaused = false;
	m_videoProvider->videoObject()->play();
}

void BackgroundContent::pauseBtnClicked()
{
	m_isUserPlaying = false;
	m_isUserPaused = true;
	m_videoProvider->videoObject()->pause();
}
