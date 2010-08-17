#include "TextVideoSource.h"
#include "../ImageFilters.h"

QCache<QString,double> TextVideoSource::static_autoTextSizeCache;

TextVideoSource::TextVideoSource(QObject *parent)
	: VideoSource(parent)
	, m_textWidth(640)
	, m_outlineEnabled(true)
	, m_outlinePen(Qt::black, 2.0)
	, m_fillEnabled(true)
	, m_fillBrush(Qt::white)
	, m_shadowEnabled(true)
	, m_shadowBlurRadius(10)
	, m_shadowOffsetX(3)
	, m_shadowOffsetY(3)
	, m_frameChanged(false)
{
	setImage(QImage());
}

void TextVideoSource::setHtml(const QString& html)
{
	m_html = html;
	// dont block calling thread by updating - since we are in a thread ourself, 
	// this should put the update into our thread
	//QTimer::singleShot(0,this,SLOT(update()));
	update();
}

void TextVideoSource::setImage(const QImage& img)
{
	m_image = img.convertToFormat(QImage::Format_ARGB32);
	m_frame = VideoFrame(m_image,1000/30);
	enqueue(m_frame);
	emit frameReady();
	m_frameChanged = true;
}

void TextVideoSource::run()
{
	while(!m_killed)
	{
//		qDebug() << "TextVideoSource::run()";
		if(m_frameChanged)
		{
			enqueue(m_frame);
			emit frameReady();
		}
// 		msleep(m_frame.holdTime);
		msleep(100);
	}
}



void TextVideoSource::changeFontSize(double size)
{
	QTextDocument doc;
	if (Qt::mightBeRichText(html()))
		doc.setHtml(html());
	else
		doc.setPlainText(html());

	QTextCursor cursor(&doc);
	cursor.select(QTextCursor::Document);

	QTextCharFormat format;
	format.setFontPointSize(size);
	cursor.mergeCharFormat(format);
	cursor.mergeBlockCharFormat(format);

	setHtml(doc.toHtml());
}


double TextVideoSource::findFontSize()
{
	QTextDocument doc;
	if (Qt::mightBeRichText(html()))
		doc.setHtml(html());
	else
		doc.setPlainText(html());

	QTextCursor cursor(&doc);
	cursor.select(QTextCursor::Document);
	QTextCharFormat format = cursor.charFormat();
	return format.fontPointSize();
}

int TextVideoSource::fitToSize(const QSize& size, int minimumFontSize, int maximumFontSize)
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
		
		//qDebug()<<"TextVideoSource::fitToSize(): size search: CACHE HIT: loaded size:"<<ptSize;
		
		// We go thru the much-more-verbose method of creating
		// the document and setting the html, width, merge cursor,
		// etc, just so we can get the document height after
		// setting the font size inorder to use it to center the textbox.
		// If we didnt nead the height, we could just use autoText->setFontSize()
		
		QTextDocument doc;
		doc.setTextWidth(width);
		if (Qt::mightBeRichText(html()))
			doc.setHtml(html());
		else
			doc.setPlainText(html());

			
		QTextCursor cursor(&doc);
		cursor.select(QTextCursor::Document);
		
		QTextCharFormat format;
		format.setFontPointSize(ptSize);
		cursor.mergeCharFormat(format);
		
		boxHeight = doc.documentLayout()->documentSize().height();
		
		setHtml(doc.toHtml());
	}
	else
	{
		double ptSize = minimumFontSize > 0 ? minimumFontSize : findFontSize();
		double sizeInc = 1;	// how big of a jump to add to the ptSize each iteration
		int count = 0;		// current loop iteration
		int maxCount = 100; 	// max iterations of the search loop
		bool done = false;
		
		int lastGoodSize = ptSize;
		QString lastGoodHtml = html();
		
		QTextDocument doc;
		
		int heightTmp;
		
		doc.setTextWidth(width);
		if (Qt::mightBeRichText(html()))
			doc.setHtml(html());
		else
			doc.setPlainText(html());

			
		QTextCursor cursor(&doc);
		cursor.select(QTextCursor::Document);
		
		QTextCharFormat format;
			
		while(!done && count++ < maxCount)
		{
			format.setFontPointSize(ptSize);
			cursor.mergeCharFormat(format);
			
			heightTmp = doc.documentLayout()->documentSize().height();
			
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
			
			//qDebug()<<"TextVideoSource::fitToSize(): size search: going UP failed, now I'll try to go DOWN";
			
			while(!done && count++ < maxCount)
			{
				
				format.setFontPointSize(ptSize);
				cursor.mergeCharFormat(format);
				
				heightTmp = doc.documentLayout()->documentSize().height();
				
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
		
		setHtml(doc.toHtml());
		
		//qDebug()<<"TextVideoSource::fitToSize(): size search: caching ptsize:"<<lastGoodSize<<", count: "<<count<<"( minimum size was:"<<minimumFontSize<<")";
		boxHeight = heightTmp;
		//static_autoTextSizeCache[sizeKey] = lastGoodSize;
		
		// We are using a QCache instead of a plain QMap, so that requires a pointer value 
		// Using QCache because the key for the cache could potentially become quite large if there are large amounts of HTML
		// and I dont want to just keep accumlating html in the cache infinitely
		static_autoTextSizeCache.insert(sizeKey, new double(lastGoodSize),1);
	}
	
	return boxHeight;
	
}

QSize TextVideoSource::findNaturalSize(int atWidth)
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
	
// 	qDebug() << "TextVideoSource::findNaturalSize: atWidth:"<<atWidth<<", firstSize:"<<firstSize;
	
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
		
// 		qDebug() << "TextVideoSource::findNaturalSize: w:"<<w<<", checkSize:"<<checkSize<<", counter:"<<counter;
		counter ++;
	}
	
	if(checkSize.width() != firstSize.width())
	{
		int w = checkSize.width() + deInc;
		doc.setTextWidth(w);
		checkSize = doc.documentLayout()->documentSize().toSize();
// 		qDebug() << "TextVideoSource::findNaturalSize: Final Size: w:"<<w<<", checkSize:"<<checkSize;
		return checkSize;
	}
	else
	{
// 		qDebug() << "TextVideoSource::findNaturalSize: No Change, firstSize:"<<checkSize;
		return firstSize;
	}
}


void TextVideoSource::update()
{
// 	qDebug()<<itemName()<<"TextBoxWarmingThread::run(): htmlCode:"<<htmlCode;
	//qDebug() << "TextVideoSource::update(): HTML:"<<html();
	
	QTextDocument doc;
	QTextDocument shadowDoc;
	
	if (Qt::mightBeRichText(html()))
	{
		doc.setHtml(html());
		shadowDoc.setHtml(html());
	}
	else
	{
		doc.setPlainText(html());
		shadowDoc.setPlainText(html());
	}
	
	
	int textWidth = m_textWidth;

	doc.setTextWidth(textWidth);
	shadowDoc.setTextWidth(textWidth);
	
	// Apply outline pen to the html
	QTextCursor cursor(&doc);
	cursor.select(QTextCursor::Document);

	QTextCharFormat format;

	QPen p(Qt::NoPen);
	if(outlineEnabled())
	{
		p = outlinePen();
		p.setJoinStyle(Qt::MiterJoin);
	}

	format.setTextOutline(p);
	format.setForeground(fillEnabled() ? fillBrush() : Qt::NoBrush); //Qt::white);

	cursor.mergeCharFormat(format);
	
	// Setup the shadow text formatting if enabled
	if(shadowEnabled())
	{
		if(shadowBlurRadius() <= 0.05)
		{
			QTextCursor cursor(&shadowDoc);
			cursor.select(QTextCursor::Document);
	
			QTextCharFormat format;
			format.setTextOutline(Qt::NoPen);
			format.setForeground(shadowBrush());
	
			cursor.mergeCharFormat(format);
		}
	}
	
			
	QSizeF shadowSize = shadowEnabled() ? QSizeF(shadowOffsetX(),shadowOffsetY()) : QSizeF(0,0);
	QSizeF docSize = doc.size();
	QSize sumSize = (docSize + shadowSize).toSize();
	//qDebug() << "TextVideoSource::update(): textWidth: "<<textWidth<<", shadowSize:"<<shadowSize<<", docSize:"<<docSize<<", sumSize:"<<sumSize;
	QImage cache(sumSize,QImage::Format_ARGB32_Premultiplied);

	QPainter textPainter(&cache);
	textPainter.fillRect(cache.rect(),Qt::transparent);
	
	QAbstractTextDocumentLayout::PaintContext pCtx;

	if(shadowEnabled())
	{
		if(shadowBlurRadius() <= 0.05)
		{
			// render a "cheap" version of the shadow using the shadow text document
			textPainter.save();

			textPainter.translate(shadowOffsetX(),shadowOffsetY());
			shadowDoc.documentLayout()->draw(&textPainter, pCtx);

			textPainter.restore();
		}
		else
		{
			double radius = shadowBlurRadius();
			double radiusSquared = radius*radius;
			
			// create temporary pixmap to hold a copy of the text
			double blurSize = (int)(radiusSquared*2);
			QSizeF shadowSize(blurSize,blurSize);
			QImage tmpImage((doc.size()+shadowSize).toSize(),QImage::Format_ARGB32);
			
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
			tmpPainter.fillRect(rect, shadowBrush().color());
			tmpPainter.end();

			// blur the colored text
			QImage  blurredImage   = ImageFilters::blurred(tmpImage, rect, (int)radius);
			
			// render the blurred text at an offset into the cache
			textPainter.save();
			textPainter.translate(shadowOffsetX() - radiusSquared,
					      shadowOffsetY() - radiusSquared);
			textPainter.drawImage(0, 0, blurredImage.copy(blurredImage.rect()));
			textPainter.restore();
		}
	}
	
	doc.documentLayout()->draw(&textPainter, pCtx);
	
	textPainter.end();
	
	setImage(cache);
}

ITEM_PROPSET(TextVideoSource, TextWidth,	int,	textWidth);

ITEM_PROPSET(TextVideoSource, OutlineEnabled,	bool,	outlineEnabled);
ITEM_PROPSET(TextVideoSource, OutlinePen,	QPen,	outlinePen);

ITEM_PROPSET(TextVideoSource, FillEnabled,	bool,	fillEnabled);
ITEM_PROPSET(TextVideoSource, FillBrush,	QBrush,	fillBrush);

ITEM_PROPSET(TextVideoSource, ShadowEnabled,	bool,	shadowEnabled);
ITEM_PROPSET(TextVideoSource, ShadowBlurRadius,	double,	shadowBlurRadius);
ITEM_PROPSET(TextVideoSource, ShadowOffsetX,	double,	shadowOffsetX);
ITEM_PROPSET(TextVideoSource, ShadowOffsetY,	double,	shadowOffsetY);
ITEM_PROPSET(TextVideoSource, ShadowBrush,	QBrush,	shadowBrush);
