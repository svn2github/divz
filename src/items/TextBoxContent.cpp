#include "TextBoxContent.h"
#include "frames/Frame.h"
#include "items/BezierCubicItem.h"
#include "items/TextProperties.h"
#include "model/TextItem.h"
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
#include <QTextOption>
#include <QTimer>
#include <QPainterPathStroker>
#include "items/CornerItem.h"
#include "model/TextBoxItem.h"

#include <QPixmapCache>
#include "ImageFilters.h"

#define DEBUG_LAYOUT 0

#if QT_VERSION >= 0x040600
	#define QT46_SHADOW_ENAB 0
#endif

#ifndef qFuzzyIsNull
	#define qFuzzyIsNull(a) (a<0.00001)
#endif

#define ZOOM_FACTOR 4

#define DEBUG_MARK() qDebug() << "mark: "<<__FILE__<<":"<<__LINE__
#define DEBUG_TMARK() DEBUG_MARK() << ":" << t.restart() << "ms"
#define DEBUG_TSTART() QTime total;total.start(); QTime t;t.start();

TextBoxContent::TextBoxContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
    , m_text(0)
    , m_shadowText(0)
    , m_textRect(0, 0, 0, 0)
    , m_textMargin(4)
    , m_lastModelRev(0)
{
	DEBUG_TSTART();
	m_dontSyncToModel = true;

	setFrame(0);
	setFrameTextEnabled(false);
	setToolTip(tr("Right click to Edit the text"));
	// create a text document
	m_text = new QTextDocument(this);
	// for drawing the shadow
	m_shadowText = new QTextDocument(this);

	#if QT_VERSION >= 0x040500
		m_textMargin = (int)m_text->documentMargin();
	#endif
	// template text
	QFont font;
	#ifdef Q_OS_WIN
		font.setFamily("Arial");
	#endif
	font.setPointSize(16);
	m_text->setDefaultFont(font);

	//connect(this, SIGNAL(resized()), this, SLOT(delayContentsResized()));
	for(int i=0;i<m_cornerItems.size();i++)
		m_cornerItems.at(i)->setDefaultLeftOp(CornerItem::Scale);

	m_zoomAnimationTimer = new QTimer(this);
	connect(m_zoomAnimationTimer, SIGNAL(timeout()), this, SLOT(animateZoom()));
	
	m_dontSyncToModel = false;
	
	//qDebug() << "TextBoxContent(): \t \t Elapsed:"<<(((double)total.elapsed())/1000.0)<<" sec";
}

TextBoxContent::~TextBoxContent()
{
	delete m_text;
	delete m_shadowText;
}

QString TextBoxContent::toHtml()
{
	TextItem * textModel = dynamic_cast<TextItem*>(modelItem());
	return textModel ? textModel->text() : "";
}



void TextBoxContent::setHtml(const QString & htmlCode)
{
        DEBUG_TSTART();
	
	m_text->setHtml(htmlCode);
	m_shadowText->setHtml(htmlCode);
	
	updateTextConstraints();
	syncToModelItem(0);

	// Apply outline pen to the html
	QTextCursor cursor(m_text);
	cursor.select(QTextCursor::Document);

	QTextCharFormat format;

	QPen p(Qt::NoPen);
	if(modelItem() && modelItem()->outlineEnabled())
	{
		p = modelItem()->outlinePen();
		p.setJoinStyle(Qt::MiterJoin);
	}

	format.setTextOutline(p);
	format.setForeground(modelItem() ? modelItem()->fillBrush() : Qt::white);

	
	cursor.mergeCharFormat(format);
	
	#if QT46_SHADOW_ENAB == 0
	// Setup the shadow text formatting if enabled
	if(modelItem() && modelItem()->shadowEnabled())
	{
		if(qFuzzyIsNull(modelItem()->shadowBlurRadius()))
		{
			QTextCursor cursor(m_shadowText);
			cursor.select(QTextCursor::Document);
	
			QTextCharFormat format;
			format.setTextOutline(Qt::NoPen);
			format.setForeground(modelItem() ? modelItem()->shadowBrush() : Qt::black);
	
			cursor.mergeCharFormat(format);
		}
	}
	#endif
	
	// Cache gets dirty in syncfrommodelitem conditionally based on model revision
	//dirtyCache();

	update();
	
	//qDebug() << "TextBoxContent::setHtml(): \t \t Elapsed:"<<(((double)total.elapsed())/1000.0)<<" sec";
}


QWidget * TextBoxContent::createPropertyWidget()
{
	TextProperties * p = new TextProperties();

	// common properties
	connect(p->bFront, SIGNAL(clicked()), this, SLOT(slotStackFront()));
	connect(p->bRaise, SIGNAL(clicked()), this, SLOT(slotStackRaise()));
	connect(p->bLower, SIGNAL(clicked()), this, SLOT(slotStackLower()));
	connect(p->bBack, SIGNAL(clicked()), this, SLOT(slotStackBack()));
	connect(p->bDel, SIGNAL(clicked()), this, SIGNAL(deleteItem()), Qt::QueuedConnection);


	return p;
}

void TextBoxContent::syncFromModelItem(AbstractVisualItem *model)
{
        DEBUG_TSTART();
	m_dontSyncToModel = true;
	if(!modelItem())
	{
		setModelItem(model);
		
		// Start out the last remembered model rev at the rev of the model
		// so we dont force a redraw of the cache just because we're a fresh
		// object.
		if(QPixmapCache::find(cacheKey()))
			m_lastModelRev = modelItem()->revision();
	}

	static int x = 0;
	x++;
	//qDebug() << x<<": TextBoxContent::syncFromModelItem() mark";
	QFont font;
	TextItem * textModel = dynamic_cast<TextItem*>(model);

	font.setFamily(textModel->fontFamily());
	font.setPointSize((int)textModel->fontSize());
	m_text->setDefaultFont(font);
	
	

	if(textModel->text().indexOf('<') < 0)
	{
		qDebug() << "TextBoxContent:: converting plain text from model item to html";
		m_text->setPlainText(textModel->text());
		textModel->setText(m_text->toHtml());
	}
	
	//qDebug() << "TextBoxContent:: Original HTML:"<<textModel->text();
	setHtml(textModel->text());
	//qDebug() << "TextBoxContent::      New HTML:"<<m_text->toHtml();
	

	AbstractContent::syncFromModelItem(model);
	
	if(modelItem()->revision() != m_lastModelRev)
	{
		//qDebug()<<"modelItem():"<<modelItem()->itemName()<<": last revision:"<<m_lastModelRev<<", this revision:"<<m_lastModelRev<<", cache dirty!";
		
		m_lastModelRev = modelItem()->revision();
		dirtyCache();
	}
	
	if(   model->zoomEffectEnabled() 
 	   && model->zoomSpeed() > 0 
 	   && sceneContextHint() == MyGraphicsScene::Live)
	{
		m_zoomEnabled = true;
		
		m_zoomAnimationTimer->start(1000 / 20); // / model->zoomSpeed());
		
		QSize size = contentsRect().size();
			
		double width  = size.width();
		double height = size.height();
		
		double aspectRatio = height == 0 ? 1 : width/height;
			
// 		if(!m_zoomInit)
// 		{
			//qDebug() << "aspectRatio: "<<aspectRatio;
			
			QPointF delta;
			
			m_zoomStartSize.setX(width);
			m_zoomStartSize.setY(height);
			 
			m_zoomEndSize.setX(width  * model->zoomFactor());
			m_zoomEndSize.setY(height * model->zoomFactor());
			
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
// 			m_zoomInit = true;
// 		}
		
		// allow it to go below 1.0 for step size by using 75.0 when the max of the zoomSpeed slider in config is 100
		m_zoomStep.setX(8.0 / (100.01 - ((double)model->zoomSpeed())) * aspectRatio);
		m_zoomStep.setY(8.0 / (100.01 - ((double)model->zoomSpeed())));
		
		if(model->zoomAnchorCenter())
			m_zoomDestPoint = QPointF(.5,.5);
		else
		{
			// pick a third intersection
			double x = qrand() < RAND_MAX/2 ? .33 : .66;
			double y = qrand() < RAND_MAX/2 ? .33 : .66;
			
			// apply a fudge factor
// 			x += 0.15 - ((double)qrand()) / ((double)RAND_MAX) * 0.075;
// 			y += 0.15 - ((double)qrand()) / ((double)RAND_MAX) * 0.075;
			
			m_zoomDestPoint = QPointF(x,y);
			
			//qDebug() << model->itemName() << "Random zoom anchor: "<<m_zoomDestPoint;
		}

	}
	else
	{
		m_zoomEnabled = false;
		if(m_zoomAnimationTimer->isActive())
			m_zoomAnimationTimer->stop();
	}

        m_dontSyncToModel = false;
}

AbstractVisualItem * TextBoxContent::syncToModelItem(AbstractVisualItem *model)
{
	DEBUG_TSTART();
	
	TextItem * textModel = dynamic_cast<TextItem*>(AbstractContent::syncToModelItem(model));

	setModelItemIsChanging(true);

	if(!textModel)
	{
		setModelItemIsChanging(false);
                //qDebug("TextBoxContent::syncToModelItem: textModel is null, cannot sync\n");
		return 0;
	}
        //qDebug("TextBoxContent:syncToModelItem: Syncing to model! Yay!");
	//textModel->setText(m_text->toHtml());
	textModel->setFontFamily(m_text->defaultFont().family());
	//textModel->setFontSize(m_text->defaultFont().pointSize());

	setModelItemIsChanging(false);

	//qDebug() << "TextBoxContent::syncToModelItem(): \t \t Elapsed:"<<(((double)total.elapsed())/1000.0)<<" sec";
	
	return model;
}


QPixmap TextBoxContent::renderContent(const QSize & size, Qt::AspectRatioMode /*ratio*/) const
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
	m_text->drawContents(&pixPainter);
	pixPainter.end();
	return pix;
}

int TextBoxContent::contentHeightForWidth(int width) const
{
	DEBUG_TSTART();
	// if no text size is available, use default
	if (m_textRect.width() < 1 || m_textRect.height() < 1)
		return AbstractContent::contentHeightForWidth(width);


	m_text->setTextWidth(width);
	QSizeF sz = m_text->documentLayout()->documentSize();
	
	// restore orig width
      	m_text->setTextWidth(contentsRect().width());
      	
      	//qDebug() << "TextBoxContent::contentHeightForWidth(): \t \t Elapsed:"<<(((double)total.elapsed())/1000.0)<<" sec";
      	return (int)sz.height();
}

void TextBoxContent::selectionChanged(bool /*selected*/)
{

}

void TextBoxContent::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
	emit backgroundMe();
	QGraphicsItem::mouseDoubleClickEvent(event);
}

void TextBoxContent::warmVisualCache(AbstractVisualItem *model)
{
	new TextBoxWarmingThreadManager(model);
}

TextBoxWarmingThreadManager::TextBoxWarmingThreadManager(AbstractVisualItem *model) : m_model(model)
{
	QPixmap cache;

	if(!QPixmapCache::find(AbstractContent::cacheKey(model),cache))
	{
		m_thread = new TextBoxWarmingThread(model);
		connect(m_thread, SIGNAL(renderDone(QImage*)), this, SLOT(renderDone(QImage*)));
		connect(m_thread, SIGNAL(finished()), m_thread, SLOT(deleteLater()));
		m_thread->start();
	}
	else
	{
		deleteLater();
	}
}

void TextBoxWarmingThreadManager::renderDone(QImage *image)
{
	QPixmapCache::insert(AbstractContent::cacheKey(m_model), QPixmap::fromImage(*image));
	deleteLater();
	delete image; // QPixmap::fromImage() made a copy, so we dont need to waste this memory here
}

TextBoxWarmingThread::TextBoxWarmingThread(AbstractVisualItem *model) : m_model(model) {}
void TextBoxWarmingThread::run()
{
	if(!m_model)
	{
		qDebug()<<"TextBoxWarmingThread::run(): m_model is null";
		return;
	}
	//qDebug()<<"TextBoxWarmingThread::run(): model ptr:"<<m_model<<", attempting to dynamic cast";
	
	TextBoxItem * model = dynamic_cast<TextBoxItem*>((AbstractVisualItem*)m_model);
	
	//int sleepTime = (int)(((float)qrand()) / ((float)RAND_MAX) * 10000.0 + 2000.0);
	
	qDebug()<<"TextBoxWarmingThread::run(): modelItem:"<<model->itemName();//<<": Cache redraw, sleep: "<<sleepTime;
	
	// Sleep doesnt work - if I sleep, then it seems the cache is never updated!
	//sleep((unsigned long)sleepTime);
	//sleep(1000);
	
	
			
	QString htmlCode = model->text();
	
	QTextDocument doc;
	QTextDocument shadowDoc;
	
	doc.setHtml(htmlCode);
	shadowDoc.setHtml(htmlCode);
	
	int textWidth = model->contentsRect().toRect().width();

	doc.setTextWidth(textWidth);
	shadowDoc.setTextWidth(textWidth);

	
	
	// Apply outline pen to the html
	QTextCursor cursor(&doc);
	cursor.select(QTextCursor::Document);

	QTextCharFormat format;

	QPen p(Qt::NoPen);
	if(model && model->outlineEnabled())
	{
		p = model->outlinePen();
		p.setJoinStyle(Qt::MiterJoin);
	}

	format.setTextOutline(p);
	format.setForeground(model ? model->fillBrush() : Qt::white);

	
	cursor.mergeCharFormat(format);
	
	#if QT46_SHADOW_ENAB == 0
	// Setup the shadow text formatting if enabled
	if(model && model->shadowEnabled())
	{
		if(qFuzzyIsNull(model->shadowBlurRadius()))
		{
			QTextCursor cursor(&shadowDoc);
			cursor.select(QTextCursor::Document);
	
			QTextCharFormat format;
			format.setTextOutline(Qt::NoPen);
			format.setForeground(model ? model->shadowBrush() : Qt::black);
	
			cursor.mergeCharFormat(format);
		}
	}
	#endif
	
			
	QSizeF shadowSize = model->shadowEnabled() ? QSizeF(model->shadowOffsetX(),model->shadowOffsetY()) : QSizeF(0,0);
	QImage *cache = new QImage((model->contentsRect().size()+shadowSize).toSize(),QImage::Format_ARGB32_Premultiplied);

	QPainter textPainter(cache);
	textPainter.fillRect(cache->rect(),Qt::transparent);
	
	QAbstractTextDocumentLayout::PaintContext pCtx;

	#if QT46_SHADOW_ENAB == 0
	if(model->shadowEnabled())
	{
		if(qFuzzyIsNull(model->shadowBlurRadius()))
		{
			// render a "cheap" version of the shadow using the shadow text document
			textPainter.save();

			textPainter.translate(model->shadowOffsetX(),model->shadowOffsetY());
			shadowDoc.documentLayout()->draw(&textPainter, pCtx);

			textPainter.restore();
		}
		else
		{
			double radius = model->shadowBlurRadius();
			double radiusSquared = radius*radius;
			
			// create temporary pixmap to hold a copy of the text
			double blurSize = (int)(radiusSquared*2);
			QSize shadowSize(blurSize,blurSize);
			QImage tmpImage((model->contentsRect().size()+shadowSize).toSize(),QImage::Format_ARGB32);
			
			// render the text
			QPainter tmpPainter(&tmpImage);
			tmpPainter.fillRect(tmpImage.rect(),Qt::transparent);
			
			tmpPainter.save();
			tmpPainter.translate(radiusSquared, radiusSquared);
			doc.documentLayout()->draw(&tmpPainter, pCtx);
			tmpPainter.restore();
			
			// blacken the text by applying a color to the copy using a QPainter::CompositionMode_DestinationIn operation. 
			// This produces a homogeneously-colored pixmap.
			QRect rect = tmpImage.rect();
			tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
			tmpPainter.fillRect(rect, model->shadowBrush().color());
			tmpPainter.end();

			// blur the colored text
			QImage  blurredImage   = ImageFilters::blurred(tmpImage, rect, (int)radius);
			
			// render the blurred text at an offset into the cache
			textPainter.save();
			textPainter.translate(model->shadowOffsetX() - radiusSquared,
					model->shadowOffsetY() - radiusSquared);
			textPainter.drawImage(0, 0, blurredImage);
			textPainter.restore();
		}
	}
	#endif
	
	doc.documentLayout()->draw(&textPainter, pCtx);
	
	textPainter.end();
	
	emit renderDone(cache);
}

void TextBoxContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	DEBUG_TSTART();
	// paint parent
	AbstractContent::paint(painter, option, widget);

	painter->save();

	//TODO should we clip to the rect or FORCE resize the rect? probably clip...
	//painter->setClipRect(contentsRect());
	painter->translate(contentsRect().topLeft()); // + QPoint(p.width(),p.width()));

	if(sceneContextHint() == MyGraphicsScene::Preview || !modelItem()->shadowEnabled())
	{
		// If we're drawing in a Preview scene, then we render directly with the painter
		// (rather than caching the results in a pixmap) because this allows the painter 
		// to scale the text glyphs directly (vector scaling), rather than scaling bits 
		// in a pixmap (bitmap scaling), producing more legible results at lower scalings
		
		//qDebug() << "TextBoxContent::paint: Rendering either preview or no shadow";
		
		QAbstractTextDocumentLayout::PaintContext pCtx;
		
		bool needRestore = false;
		
		if(m_zoomEnabled && sceneContextHint() == MyGraphicsScene::Live)
		{
			needRestore = true;
			painter->save();
			double xf = (1/m_zoomDestPoint.x());
			double yf = (1/m_zoomDestPoint.y());
			double sx = m_zoomCurSize.x() / m_zoomStartSize.x();
			double sy = m_zoomCurSize.y() / m_zoomStartSize.y();
			QRect cRect = contentsRect();
			painter->translate(cRect.width()/xf - m_zoomCurSize.x()/xf,cRect.height()/yf - m_zoomCurSize.y()/yf);
			painter->scale(sx,sy);
		}
			
			
	
		if(modelItem()->shadowEnabled())
		{
			painter->save();
			
			painter->translate(modelItem()->shadowOffsetX(),modelItem()->shadowOffsetY());
			m_shadowText->documentLayout()->draw(painter, pCtx);
	
			painter->restore();
		}
		
		m_text->documentLayout()->draw(painter, pCtx);
		
		if(needRestore)
			painter->restore();
		
	}
	else
	{
		QPixmap cache;
		//qDebug() << "TextBoxContent::paint: Rendering either live or with shadow";
		
		// The primary and only reason we cache the text rendering is inorder
		// to paint the text and shadow as a single unit (e.g. composite the
		// shadow+text BEFORE applying opacity rather than setting the opacity
		// before rendering the shaodw.) If we didnt cache the text as a pixmap
		// (e.g. render text directly) then when crossfading, the shadow
		// "apperas" to fade out last, after the text.
		
		// Update 20091015: Implemented very aggressive caching across TextBoxContent instances
		// that share the same modelItem() (see ::cacheKey()) inorder to avoid re-rendering 
		// potentially expensive drop shadows, below.
		
		
		
		
		if(!QPixmapCache::find(cacheKey(),cache) && m_text->toPlainText().trimmed() != "")
		{
			qDebug()<<"TextBoxContent::paint(): modelItem:"<<modelItem()->itemName()<<": Cache redraw";
			
			QSizeF shadowSize = modelItem()->shadowEnabled() ? QSizeF(modelItem()->shadowOffsetX(),modelItem()->shadowOffsetY()) : QSizeF(0,0);
			cache = QPixmap((contentsRect().size()+shadowSize).toSize());
	
			cache.fill(Qt::transparent);
			QPainter textPainter(&cache);
	
			QAbstractTextDocumentLayout::PaintContext pCtx;
	
			#if QT46_SHADOW_ENAB == 0
			if(modelItem()->shadowEnabled())
				renderShadow(&textPainter,&pCtx);
			#endif
	
			// If we're zooming, we want to render the text straight to the painter
			// so it can transform the raw vectors instead of scaling the bitmap.
			// But if we're not zooming, we cache the text with the shadow since it
			// looks better that way when we're crossfading.
			if(!m_zoomEnabled)
				m_text->documentLayout()->draw(&textPainter, pCtx);
			
			QPixmapCache::insert(cacheKey(), cache);
		}
	
		// Draw a rectangular outline in the editor inorder to visually locate empty text blocks
		if(sceneContextHint() == MyGraphicsScene::Editor &&
			m_text->toPlainText().trimmed() == "")
		{
			QPen p = modelItem() ? modelItem()->outlinePen() : QPen(Qt::black,1.5);
			painter->setPen(p);
			painter->setBrush(Qt::NoBrush);
	
			painter->drawRect(QRect(QPoint(0,0),contentsRect().size()));
		}
		else
		{
			if(m_zoomEnabled)
			{
				double xf = (1/m_zoomDestPoint.x());
				double yf = (1/m_zoomDestPoint.y());
				double sx = m_zoomCurSize.x() / m_zoomStartSize.x();
				double sy = m_zoomCurSize.y() / m_zoomStartSize.y();
				painter->save();
				QRect cRect = contentsRect();
				painter->translate(cRect.width()/xf - m_zoomCurSize.x()/xf,cRect.height()/yf - m_zoomCurSize.y()/yf);
				painter->scale(sx,sy);
				painter->drawPixmap(0,0,cache);
				QAbstractTextDocumentLayout::PaintContext pCtx;
				m_text->documentLayout()->draw(painter, pCtx);
				painter->restore();
				
			}
			else
			{
				painter->drawPixmap(0,0,cache);
				if(sceneContextHint() != MyGraphicsScene::Live && modelItem()->zoomEffectEnabled())
				{
					// cache may not contain the actual text, just shadow, since its not live,
					// so render the text
					QAbstractTextDocumentLayout::PaintContext pCtx;
					m_text->documentLayout()->draw(painter, pCtx);
				}
					
			}
		}
	}


	painter->restore();
	
	//qDebug() << "TextBoxContent::paint(): \t \t Elapsed:"<<(((double)total.elapsed())/1000.0)<<" sec";
}

void TextBoxContent::animateZoom()
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


void TextBoxContent::renderShadow(QPainter *painter, QAbstractTextDocumentLayout::PaintContext *pCtx)
{
	AbstractVisualItem *model = modelItem();
	if(qFuzzyIsNull(model->shadowBlurRadius()))
	{
		// render a "cheap" version of the shadow using the shadow text document
		painter->save();

		painter->translate(model->shadowOffsetX(),model->shadowOffsetY());
		m_shadowText->documentLayout()->draw(painter, *pCtx);

		painter->restore();
	}
	else
	{
		double radius = model->shadowBlurRadius();
		double radiusSquared = radius*radius;
		
		// create temporary pixmap to hold a copy of the text
		double blurSize = (int)(radiusSquared*2);
		QSize shadowSize(blurSize,blurSize);
		QPixmap tmpPx(contentsRect().size()+shadowSize);
		tmpPx.fill(Qt::transparent);
		
		// render the text
		QPainter tmpPainter(&tmpPx);
		tmpPainter.save();
		tmpPainter.translate(radiusSquared, radiusSquared);
		m_text->documentLayout()->draw(&tmpPainter, *pCtx);
		tmpPainter.restore();
		
		// blacken the text by applying a color to the copy using a QPainter::CompositionMode_DestinationIn operation. 
		// This produces a homogeneously-colored pixmap.
		QRect rect = QRect(0, 0, tmpPx.width(), tmpPx.height());
		tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
		tmpPainter.fillRect(rect, model->shadowBrush().color());
		tmpPainter.end();

		// blur the colored text
		QImage  orignalImage   = tmpPx.toImage();
		QImage  blurredImage   = ImageFilters::blurred(orignalImage, rect, (int)radius);
		QPixmap blurredPixmap  = QPixmap::fromImage(blurredImage);
		
		// render the blurred text at an offset into the cache
		painter->save();
		painter->translate(model->shadowOffsetX() - radiusSquared,
				model->shadowOffsetY() - radiusSquared);
		painter->drawPixmap(0, 0, blurredPixmap);
		painter->restore();
	}
}

void TextBoxContent::updateTextConstraints(int w)
{
	DEBUG_TSTART();
	if(!m_text)
		return;

	int textWidth = w;
	if(w < 0)
		textWidth = contentsRect().width();

	if(DEBUG_LAYOUT)
		qDebug("updateTextConstraints() BEGIN (width: %d)",textWidth);


	m_text->setTextWidth(textWidth);
	m_shadowText->setTextWidth(textWidth);

	QSizeF sz = m_text->documentLayout()->documentSize();
      	m_textRect = QRect(QPoint(0,0),sz.toSize());

	// Adjust the bounding rect *height* to our document wrapped height, but leave
	// the width alone.
	bool changed = false;
	QRect newRect = contentsRect();
	if(m_textRect.height() > newRect.height())
	{
		// Changed from "!=" comparrison to ">" inorder to not mess up song slide templates
		// - JB 20091011
		newRect.setHeight(m_textRect.height());
		changed = true;
	}

	if(changed)
	{
		AbstractContent::resizeContents(newRect);
	}
	
	//qDebug() << "TextBoxContent::updateTextConstraints(): \t \t Elapsed:"<<(((double)total.elapsed())/1000.0)<<" sec";

}

void TextBoxContent::delayContentsResized()
{
	QTimer::singleShot(0,this,SLOT(contentsResized()));
}

void TextBoxContent::contentsResized()
{
 	DEBUG_TSTART();
 	updateTextConstraints();
 	update();
 	//qDebug() << "TextBoxContent::contentsResized(): \t \t Elapsed:"<<(((double)total.elapsed())/1000.0)<<" sec";
}

void TextBoxContent::resizeContents(const QRect & rect, bool keepRatio)
{
	DEBUG_TSTART();
	updateTextConstraints(rect.width());
	AbstractContent::resizeContents(rect,keepRatio);
	update();
	//qDebug() << "TextBoxContent::resizeContents(): \t \t Elapsed:"<<(((double)total.elapsed())/1000.0)<<" sec";
}
/*
void TextBoxContent::updateCache()
{
    m_cachePixmap = QPixmap(contentsRect().size());
    m_cachePixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter(&m_cachePixmap);
    painter.setRenderHint(QPainter::Antialiasing);

}*/
