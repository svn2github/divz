#include "RichTextRenderer.h"
#include "../ImageFilters.h"

QCache<QString,double> RichTextRenderer::static_autoTextSizeCache;

RichTextRenderer::RichTextRenderer(QObject *parent)
	: QObject(parent)
	, m_textWidth(640)
	, m_outlineEnabled(true)
	, m_outlinePen(Qt::black, 2.0)
	, m_fillEnabled(true)
	, m_fillBrush(Qt::white)
	, m_shadowEnabled(true)
	, m_shadowBlurRadius(16)
	, m_shadowOffsetX(3)
	, m_shadowOffsetY(3)
	, m_updatesLocked(false)
{
// 	qDebug() << "RichTextRenderer::ctor(): \t in thread:"<<QThread::currentThreadId();
	connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(renderText()));
	m_updateTimer.setInterval(50);
	m_updateTimer.setSingleShot(true);
	
	
	// NB QTextDocument doesn't seem to be able to be used from another thread other than the GUI thread under X11 - wierd crashes/freezes if we try to thread it
// // 	// This *should* be sufficient to move the rendering into another thread,
// // 	// since the renderText() is normally called as part of a timer signal
//  	m_killed = false;
// 	
//  	start();
// // 	moveToThread(this);

}

RichTextRenderer::~RichTextRenderer()
{
// 	// shutdown thread
// 	m_killed = true;
// 	quit();
// 	wait();
}

// void RichTextRenderer::run()
// {
// 	while(!m_killed)
// 	{
// 		m_flagMutex.lock();
// 		bool localFlag = m_renderNeeded;
// 		m_flagMutex.unlock();
// 		if(localFlag)
// 		{
// 			renderText();
// 			m_flagMutex.lock();
// 			m_renderNeeded = false;
// 			m_flagMutex.unlock();
// 		}
// 		msleep(50);
// 	}
// }

void RichTextRenderer::setHtml(const QString& html)
{
	if(html == m_html)
	{
		//update();
		return;
	}
		
	m_html = html;
	// dont block calling thread by updating - since we are in a thread ourself, 
	// this should put the update into our thread
	//QTimer::singleShot(0,this,SLOT(update()));
	
	if (Qt::mightBeRichText(html))
	{
		m_doc.setHtml(html);
		m_shadowDoc.setHtml(html);
	}
	else
	{
		m_doc.setPlainText(html);
		m_shadowDoc.setPlainText(html);
	}
	
	if(outlineEnabled())
		setOutlinePen(outlinePen());
		
	update();
// 	m_flagMutex.lock();
// 	m_renderNeeded = true;
// 	m_flagMutex.unlock();
}

bool RichTextRenderer::lockUpdates(bool flag)
{
	bool oldValue = m_updatesLocked;
	m_updatesLocked = flag;
	return oldValue;
}


void RichTextRenderer::changeFontSize(double size)
{
	QTextCursor cursor(&m_doc);
	cursor.select(QTextCursor::Document);

	QTextCharFormat format;
	format.setFontPointSize(size);
	cursor.mergeCharFormat(format);
	cursor.mergeBlockCharFormat(format);

	//setHtml(doc.toHtml());
	update();
}


double RichTextRenderer::findFontSize()
{
	QTextCursor cursor(&m_doc);
	cursor.select(QTextCursor::Document);
	QTextCharFormat format = cursor.charFormat();
	return format.fontPointSize();
}

int RichTextRenderer::fitToSize(const QSize& size, int minimumFontSize, int maximumFontSize)
{
	int width = size.width();
	int height = size.height();
	
	const QString sizeKey = QString("%1:%2:%3:%4").arg(html()).arg(width).arg(height).arg(minimumFontSize);
	
	// for centering
	qreal boxHeight = -1;
		
	double ptSize = -1;
	if(static_autoTextSizeCache.contains(sizeKey))
	{
		ptSize = *(static_autoTextSizeCache[sizeKey]);
		
		//qDebug()<<"RichTextRenderer::fitToSize(): size search: CACHE HIT: loaded size:"<<ptSize;
		
		// We go thru the much-more-verbose method of creating
		// the document and setting the html, width, merge cursor,
		// etc, just so we can get the document height after
		// setting the font size inorder to use it to center the textbox.
		// If we didnt nead the height, we could just use autoText->setFontSize()
		/*
		QTextDocument doc;
		doc.setTextWidth(width);
		if (Qt::mightBeRichText(html()))
			doc.setHtml(html());
		else
			doc.setPlainText(html());

			*/
		QTextCursor cursor(&m_doc);
		cursor.select(QTextCursor::Document);
		
		QTextCharFormat format;
		format.setFontPointSize(ptSize);
		cursor.mergeCharFormat(format);
		
		boxHeight = m_doc.documentLayout()->documentSize().height();
		
		//setHtml(doc.toHtml());
		update();
	}
	else
	{
		double ptSize = minimumFontSize > 0 ? minimumFontSize : findFontSize();
		double sizeInc = 1;	// how big of a jump to add to the ptSize each iteration
		int count = 0;		// current loop iteration
		int maxCount = 100; 	// max iterations of the search loop
		bool done = false;
		
		double lastGoodSize = ptSize;
		QString lastGoodHtml = html();
		
// 		QTextDocument doc;
		
		qreal heightTmp;
		
// 		doc.setTextWidth(width);
// 		if (Qt::mightBeRichText(html()))
// 			doc.setHtml(html());
// 		else
// 			doc.setPlainText(html());

			
		QTextCursor cursor(&m_doc);
		cursor.select(QTextCursor::Document);
		
		QTextCharFormat format;
			
		while(!done && count++ < maxCount)
		{
			format.setFontPointSize(ptSize);
			cursor.mergeCharFormat(format);
			
			heightTmp = m_doc.documentLayout()->documentSize().height();
			
			if(heightTmp < height &&
			      ptSize < maximumFontSize)
			{
				lastGoodSize = ptSize;
				//lastGoodHtml = html();
				boxHeight = heightTmp;

				sizeInc *= 1.1;
// 				qDebug()<<"size search: "<<ptSize<<"pt was good, trying higher, inc:"<<sizeInc<<"pt";
				ptSize += sizeInc;

			}
			else
			{
// 				qDebug()<<"fitToSize: size search: last good ptsize:"<<lastGoodSize<<", stopping search";
				done = true;
			}
		}
		
		if(boxHeight < 0 && minimumFontSize <= 0) // didnt find a size
		{
			ptSize = 100;
			
			count = 0;
			done = false;
			sizeInc = 1;
			
			//qDebug()<<"RichTextRenderer::fitToSize(): size search: going UP failed, now I'll try to go DOWN";
			
			while(!done && count++ < maxCount)
			{
				
				format.setFontPointSize(ptSize);
				cursor.mergeCharFormat(format);
				
				heightTmp = m_doc.documentLayout()->documentSize().height();
				
				if(heightTmp < height)
				{
					lastGoodSize = ptSize;
					//lastGoodHtml = html();
					boxHeight = heightTmp;
	
					sizeInc *= 1.1;
					//qDebug()<<"size search: "<<ptSize<<"pt was good, trying higher, inc:"<<sizeInc<<"pt";
					ptSize -= sizeInc;
	
				}
				else
				{
					//qDebug()<<"SongSlideGroup::textToSlides(): size search: last good ptsize:"<<lastGoodSize<<", stopping search";
					done = true;
				}
			}
		}

		format.setFontPointSize(lastGoodSize);
		cursor.mergeCharFormat(format);
		
// 		//setHtml(doc.toHtml());
		update();
		
		//qDebug()<<"RichTextRenderer::fitToSize(): size search: caching ptsize:"<<lastGoodSize<<", count: "<<count<<"( minimum size was:"<<minimumFontSize<<")";
		boxHeight = heightTmp;
		//static_autoTextSizeCache[sizeKey] = lastGoodSize;
		
		// We are using a QCache instead of a plain QMap, so that requires a pointer value 
		// Using QCache because the key for the cache could potentially become quite large if there are large amounts of HTML
		// and I dont want to just keep accumlating html in the cache infinitely
		static_autoTextSizeCache.insert(sizeKey, new double(lastGoodSize),1);
	}
	
	return (int)boxHeight;
	
}

QSize RichTextRenderer::findNaturalSize(int atWidth)
{
	QTextDocument doc;
	if(atWidth > 0)
		doc.setTextWidth(atWidth);
	if (Qt::mightBeRichText(html()))
		doc.setHtml(html());
	else
		doc.setPlainText(html());
	
	QSize firstSize = doc.documentLayout()->documentSize().toSize();
	QSize checkSize = firstSize;
	
// 	qDebug() << "RichTextRenderer::findNaturalSize: atWidth:"<<atWidth<<", firstSize:"<<firstSize;
	
	#define RUNAWAY_LIMIT 500
	
	int counter = 0;
	int deInc = 10;
	while(checkSize.height() == firstSize.height() &&
	      checkSize.height() > 0 &&
	      counter < RUNAWAY_LIMIT)
	{
		int w = checkSize.width() - deInc;
		doc.setTextWidth(w);
		checkSize = doc.documentLayout()->documentSize().toSize();
		
// 		qDebug() << "RichTextRenderer::findNaturalSize: w:"<<w<<", checkSize:"<<checkSize<<", counter:"<<counter;
		counter ++;
	}
	
	if(checkSize.width() != firstSize.width())
	{
		int w = checkSize.width() + deInc;
		doc.setTextWidth(w);
		checkSize = doc.documentLayout()->documentSize().toSize();
// 		qDebug() << "RichTextRenderer::findNaturalSize: Final Size: w:"<<w<<", checkSize:"<<checkSize;
		return checkSize;
	}
	else
	{
// 		qDebug() << "RichTextRenderer::findNaturalSize: No Change, firstSize:"<<checkSize;
		return firstSize;
	}
}

void RichTextRenderer::update()
{
 	if(m_updateTimer.isActive())
 		m_updateTimer.stop();
 	//qDebug() << "RichTextRenderer::update(): \t in thread:"<<QThread::currentThreadId();
	m_updateTimer.start();
}

void RichTextRenderer::renderText()
{
// 	m_flagMutex.lock();
// 	if(!m_renderNeeded)
// 		return;
// 	m_renderNeeded = false;
// 	m_flagMutex.unlock();
	
// 	qDebug()<<itemName()<<"TextBoxWarmingThread::run(): htmlCode:"<<htmlCode;
	//qDebug() << "RichTextRenderer::renderText(): HTML:"<<html();
	//qDebug() << "RichTextRenderer::update(): Update Start...";
//  	qDebug() << "RichTextRenderer::renderText(): \t in thread:"<<QThread::currentThreadId();
	if(m_updateTimer.isActive())
		m_updateTimer.stop();
		
	QTime renderTime;
	renderTime.start();
	
	// Setup the shadow text formatting if enabled
	if(shadowEnabled())
	{
		if(shadowBlurRadius() <= 0.05)
		{
			QTextCursor cursor(&m_shadowDoc);
			cursor.select(QTextCursor::Document);
	
			QTextCharFormat format;
			format.setTextOutline(Qt::NoPen);
			format.setForeground(shadowBrush());
	
			cursor.mergeCharFormat(format);
		}
	}
	
			
	QSizeF shadowSize = shadowEnabled() ? QSizeF(shadowOffsetX(),shadowOffsetY()) : QSizeF(0,0);
	QSizeF docSize = m_doc.size();
	QSize sumSize = (docSize + shadowSize).toSize();
	//qDebug() << "RichTextRenderer::update(): textWidth: "<<textWidth<<", shadowSize:"<<shadowSize<<", docSize:"<<docSize<<", sumSize:"<<sumSize;
	QImage cache(sumSize,QImage::Format_ARGB32); //_Premultiplied);
	memset(cache.scanLine(0),0,cache.byteCount());
	//cache.fill(Qt::black);
	
	QPainter textPainter(&cache);
	//textPainter.fillRect(cache.rect(),Qt::transparent);
	
	QAbstractTextDocumentLayout::PaintContext pCtx;

	//m_shadowEnabled = false;
	if(shadowEnabled())
	{
		if(shadowBlurRadius() <= 0.05)
		{
			// render a "cheap" version of the shadow using the shadow text document
			textPainter.save();

			textPainter.translate(shadowOffsetX(),shadowOffsetY());
			m_shadowDoc.documentLayout()->draw(&textPainter, pCtx);

			textPainter.restore();
		}
		else
		{
			double radius = shadowBlurRadius();
			
			// create temporary pixmap to hold a copy of the text
			QSizeF blurSize = ImageFilters::blurredSizeFor(m_doc.size(), (int)radius);
			//qDebug() << "Blur size:"<<blurSize<<", doc:"<<doc.size()<<", radius:"<<radius;
			QImage tmpImage(blurSize.toSize(),QImage::Format_ARGB32_Premultiplied);
			memset(tmpImage.scanLine(0),0,tmpImage.byteCount());
			
			// render the text
			QPainter tmpPainter(&tmpImage);
			
			tmpPainter.save();
			tmpPainter.translate(radius, radius);
			m_doc.documentLayout()->draw(&tmpPainter, pCtx);
			tmpPainter.restore();
			
			// blacken the text by applying a color to the copy using a QPainter::CompositionMode_DestinationIn operation. 
			// This produces a homogeneously-colored pixmap.
			QRect rect = tmpImage.rect();
			tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
			tmpPainter.fillRect(rect, shadowBrush().color());
			tmpPainter.end();

			// blur the colored text
			ImageFilters::blurImage(tmpImage, (int)radius);
			
			// render the blurred text at an offset into the cache
			textPainter.save();
			textPainter.translate(shadowOffsetX() - radius,
					      shadowOffsetY() - radius);
			textPainter.drawImage(0, 0, tmpImage);
			textPainter.restore();
		}
	}
	
	m_doc.documentLayout()->draw(&textPainter, pCtx);
	
	textPainter.end();
	
	m_image = cache.convertToFormat(QImage::Format_ARGB32);
	emit textRendered(m_image);
	
	//qDebug() << "RichTextRenderer::renderText(): Render finished, elapsed:"<<renderTime.elapsed()<<"ms";
}

//ITEM_PROPSET(RichTextRenderer, TextWidth,	int,	textWidth);
void RichTextRenderer::setTextWidth(int x)
{
	//qDebug() << "RichTextRenderer::setTextWidth: "<<x;
	m_textWidth = x;
	
	m_doc.setTextWidth(x);
	m_shadowDoc.setTextWidth(x);
}

//ITEM_PROPSET(RichTextRenderer, OutlineEnabled,	bool,	outlineEnabled);
//ITEM_PROPSET(RichTextRenderer, OutlinePen,	QPen,	outlinePen);

void RichTextRenderer::setOutlineEnabled(bool flag)
{
	//qDebug() << "RichTextRenderer::setOutlineEnabled: "<<flag;
	m_outlineEnabled = flag;
	setOutlinePen(outlinePen());
}

void RichTextRenderer::setOutlinePen(QPen pen)
{
	//qDebug() << "RichTextRenderer::setOutlinePen: "<<pen;
	m_outlinePen = pen;
	
	// Apply outline pen to the html
	QTextCursor cursor(&m_doc);
	cursor.select(QTextCursor::Document);

	QTextCharFormat format;

	QPen p(Qt::NoPen);
	if(outlineEnabled())
	{
		p = pen;
		p.setJoinStyle(Qt::MiterJoin);
	}

	format.setTextOutline(p);
	//format.setForeground(fillEnabled() ? fillBrush() : Qt::NoBrush); //Qt::white);

	cursor.mergeCharFormat(format);
}


ITEM_PROPSET(RichTextRenderer, FillEnabled,	bool,	fillEnabled);
ITEM_PROPSET(RichTextRenderer, FillBrush,	QBrush,	fillBrush);

ITEM_PROPSET(RichTextRenderer, ShadowEnabled,	bool,	shadowEnabled);
ITEM_PROPSET(RichTextRenderer, ShadowBlurRadius,	double,	shadowBlurRadius);
ITEM_PROPSET(RichTextRenderer, ShadowOffsetX,	double,	shadowOffsetX);
ITEM_PROPSET(RichTextRenderer, ShadowOffsetY,	double,	shadowOffsetY);
ITEM_PROPSET(RichTextRenderer, ShadowBrush,	QBrush,	shadowBrush);
