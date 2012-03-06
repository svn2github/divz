#include  "TextItem.h"

#include "items/TextContent.h"
//#include "items/SimpleTextContent.h"

#include <QGraphicsScene>
#include <QStringList>
#include <QTextDocument>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QAbstractTextDocumentLayout>

QCache<QString,double> TextItem::static_autoTextSizeCache;

TextItem::TextItem() : AbstractVisualItem() 
{
	m_fontFamily = "Tahoma";
	m_fontSize   = 40;
	m_xTextAlign = Qt::AlignLeft;
	m_yTextAlign = Qt::AlignTop;
}

TextItem::~TextItem() {}

//#define ITEM_PROPSET(className,setterName,typeName,memberName) 
//ITEM_PROPSET(TextItem, Text, QString, text);
void TextItem::setText(QString text)
{
	QString old = m_text;
	m_text = text;
        //qDebug("TextItem::setText: '%s'",text.toAscii().constData());
	setChanged("text",text,old);
}

ITEM_PROPSET(TextItem, FontFamily, QString, fontFamily)
ITEM_PROPSET(TextItem, FontSize, double, fontSize)
ITEM_PROPSET(TextItem, ShapeEnabled, bool, shapeEnabled)
ITEM_PROPSET(TextItem, ShapePoint1, QPointF, shapePoint1)
ITEM_PROPSET(TextItem, ShapePoint2, QPointF, shapePoint2)
ITEM_PROPSET(TextItem, ShapePoint3, QPointF, shapePoint3)
ITEM_PROPSET(TextItem, ShapePoint4, QPointF, shapePoint4)
// ITEM_PROPSET(TextItem, XTextAlign, Qt::Alignment, xTextAlign)
// ITEM_PROPSET(TextItem, YTextAlign, Qt::Alignment, yTextAlign)

// Declare the setters for alignment manually instead of with the macro
// because we need to manually cast Qt::Alignment to (int) because
// QVariant doesnt know how to handle Qt::Alignment nativly
void TextItem::setXTextAlign(Qt::Alignment z)
{
	Qt::Alignment old = m_xTextAlign;
	m_xTextAlign = z;
        //qDebug("TextItem::setText: '%s'",text.toAscii().constData());
	setChanged("xTextAlign",(int)z,(int)old);
}

void TextItem::setYTextAlign(Qt::Alignment z)
{
	Qt::Alignment old = m_yTextAlign;
	m_yTextAlign = z;
        //qDebug("TextItem::setText: '%s'",text.toAscii().constData());
	setChanged("yTextAlign",(int)z,(int)old);
}


QSize TextItem::changeFontSize(double size)
{
	QTextDocument doc;
	if (Qt::mightBeRichText(text()))
		doc.setHtml(text());
	else
		doc.setPlainText(text());

	QTextCursor cursor(&doc);
	cursor.select(QTextCursor::Document);

	QTextCharFormat format;
	format.setFontPointSize(size);
	cursor.mergeCharFormat(format);
	//cursor.mergeBlockCharFormat(format);

	setText(doc.toHtml());
	
	doc.setHtml(text()); // force recalc of documentSize
	
	return doc.size().toSize();
}


double TextItem::findFontSize()
{
	QTextDocument doc;
	if (Qt::mightBeRichText(text()))
		doc.setHtml(text());
	else
		doc.setPlainText(text());

	QTextCursor cursor(&doc);
	cursor.select(QTextCursor::Document);
	QTextCharFormat format = cursor.charFormat();
	return format.fontPointSize();
}

int TextItem::fitToSize(const QSize& size, int minimumFontSize, int maximumFontSize)
{
	int width = size.width();
	int height = size.height();
	
	const QString sizeKey = QString("%1:%2:%3:%4").arg(text()).arg(width).arg(height).arg(minimumFontSize);
	
	// for centering
	qreal boxHeight = -1;
		
	double ptSize = -1;
	if(static_autoTextSizeCache.contains(sizeKey))
	{
		ptSize = *(static_autoTextSizeCache[sizeKey]);
		
		//qDebug()<<"TextItem::fitToSize(): size search: CACHE HIT: loaded size:"<<ptSize;
		
		// We go thru the much-more-verbose method of creating
		// the document and setting the html, width, merge cursor,
		// etc, just so we can get the document height after
		// setting the font size inorder to use it to center the textbox.
		// If we didnt nead the height, we could just use autoText->setFontSize()
		
		QTextDocument doc;
		doc.setTextWidth(width);
		if (Qt::mightBeRichText(text()))
			doc.setHtml(text());
		else
			doc.setPlainText(text());

		QTextCursor cursor(&doc);
		cursor.select(QTextCursor::Document);
		
		QTextCharFormat format;
		format.setFontPointSize(ptSize);
		cursor.mergeCharFormat(format);
		
		// force recalc of documentSize() -- see notes on why this is required in the block below
		doc.setHtml(doc.toHtml());
			
		boxHeight = doc.size().height();
		
		setText(doc.toHtml());
		
		// We cant use this because it doesn't call doc.setTextWidth(width)
		//boxHeight = changeFontSize(ptSize).height();
	}
	else
	{
		double ptSize   = minimumFontSize > 0 ? minimumFontSize : findFontSize();
		double sizeInc  = 0.9;	// how big of a jump to add to the ptSize each iteration
		int    count    = 0;	// current loop iteration
		int    maxCount = 100; 	// max iterations of the search loop
		bool   done = false;
		
		double  lastGoodSize = ptSize;
		QString lastGoodHtml = text();
		
		double heightTmp = 0;
		
		QTextDocument doc;
		if (Qt::mightBeRichText(text()))
			doc.setHtml(text());
		else
			doc.setPlainText(text());

		doc.setTextWidth(width);
		
		QTextCursor cursor(&doc);
		cursor.select(QTextCursor::Document);
		
		QTextCharFormat format;
		
		bool foundGood = false;
		while(!done && count++ < maxCount)
		{
			format.setFontPointSize(ptSize);
			cursor.mergeCharFormat(format);
			
			heightTmp = doc.size().height();
			
			if(heightTmp < height &&
			      ptSize < maximumFontSize)
			{
				lastGoodSize = ptSize;
				foundGood = true;
				
				sizeInc *= 1.1;
 				//qDebug()<<"size search: "<<ptSize<<"pt was good, trying higher, inc:"<<sizeInc<<"pt, heightTmp:"<<heightTmp;
				ptSize += sizeInc;

			}
			else
			{
 				//qDebug()<<"fitToSize: size search: last good ptsize:"<<lastGoodSize<<", stopping search, heightTmp:"<<heightTmp;
 				done = true;
			}
		}

		if(!foundGood)// && minimumFontSize <= 0) // didnt find a size
		{
			ptSize  = 100;
			count   = 0;
			done    = false;
			sizeInc = 1;
			
			//qDebug()<<"TextItem::fitToSize(): size search: going UP failed, now I'll try to go DOWN";
			
			while(!done && 
			      count++ < maxCount &&
			      ptSize >= minimumFontSize)
			{
				format.setFontPointSize(ptSize);
				cursor.mergeCharFormat(format);
				
				heightTmp = doc.size().height();
				
				if(heightTmp < height)
				{
					lastGoodSize = ptSize;
					
					sizeInc *= 1.1;
					//qDebug()<<"size search: [down] "<<ptSize<<"pt was good, trying higher, inc:"<<sizeInc<<"pt";
					ptSize -= sizeInc;
	
				}
				else
				{
					//qDebug()<<"fitToSize: size search: [down] size search: last good ptsize:"<<lastGoodSize<<", stopping search";
					done = true;
				}
			}
		}
		
	

		format.setFontPointSize(lastGoodSize);
		cursor.mergeCharFormat(format);
		
		setText(doc.toHtml());
		
		// force recalc of documentSize() - otherwise the docSize is wrong for some reason...
		doc.setHtml(doc.toHtml());
		
		/*
			Note on the above setHtml() call - when we just relied on the doc.size() (below) after calling mergCharFormat(),
			testing found that the height was "off" (too small) - this was found by calling TestItem::findNaturalSize() with the
			same with we use for this method - and it returned a height identical to the height given when we use the
			setHtml() call here - therefore, for some reason, this is required - I'd have to search thru Qt's sources to really
			understand why, but for now, it works..... 
		*/
		

		QSizeF docSize = doc.size();
		boxHeight = docSize.height();
		
		//qDebug()<<"TextItem::fitToSize(): size search: caching ptsize:"<<lastGoodSize<<", count: "<<count<<"( minimum size was:"<<minimumFontSize<<"), docSize:"<<docSize<<", size arg:"<<size;
		//static_autoTextSizeCache[sizeKey] = lastGoodSize;
		
		// We are using a QCache instead of a plain QMap, so that requires a pointer value 
		// Using QCache because the key for the cache could potentially become quite large if there are large amounts of HTML
		// and I dont want to just keep accumlating html in the cache infinitely
		static_autoTextSizeCache.insert(sizeKey, new double(lastGoodSize),1);
	}
	
	return (int)boxHeight;
	
}

QSize TextItem::findNaturalSize(int atWidth)
{
	QTextDocument doc;
	if (Qt::mightBeRichText(text()))
		doc.setHtml(text());
	else
		doc.setPlainText(text());
	
	if(atWidth > 0)
		doc.setTextWidth(atWidth);
		
	QSize firstSize = doc.size().toSize();
	QSize checkSize = firstSize;
	
// 	qDebug() << "TextItem::findNaturalSize: atWidth:"<<atWidth<<", firstSize:"<<firstSize;
	
	#define RUNAWAY_LIMIT 500
	
	int counter = 0;
	int deInc = 10;
	while(checkSize.height() == firstSize.height() &&
	      checkSize.height() > 0 &&
	      counter < RUNAWAY_LIMIT)
	{
		int w = checkSize.width() - deInc;
		doc.setTextWidth(w);
		checkSize = doc.size().toSize();
		
 		//qDebug() << "TextItem::findNaturalSize: w:"<<w<<", checkSize:"<<checkSize<<", counter:"<<counter;
		counter ++;
	}
	
	if(checkSize.width() != firstSize.width())
	{
		int w = checkSize.width() + deInc;
		doc.setTextWidth(w);
		checkSize = doc.size().toSize();
 		//qDebug() << "TextItem::findNaturalSize: Final Size: w:"<<w<<", checkSize:"<<checkSize;
		return checkSize;
	}
	else
	{
 		//qDebug() << "TextItem::findNaturalSize: No Change, firstSize:"<<checkSize;
		return firstSize;
	}
}


#include <assert.h>
AbstractContent * TextItem::createDelegate(QGraphicsScene *scene, QGraphicsItem*parent)
{
 	TextContent * textContent = new TextContent(scene,parent);
//	SimpleTextContent * textContent = new TextContent(scene);
	assert(textContent);
	
//	AbstractContent * vi = (AbstractContent *)textContent;
	//printf("Debug: itemId: %d\n", vi->itemId());
	
	textContent->syncFromModelItem(this);
	return textContent;
}

bool TextItem::fromXml(QDomElement & pe)
{
	setBeingLoaded(true);
	
	// restore content properties
	QString text = pe.firstChildElement("text").text();
	setText(text);
	
	// load default font
	QDomElement domElement;
	domElement = pe.firstChildElement("default-font");
	if (domElement.isElement()) 
	{
		setFontFamily(domElement.attribute("font-family"));
		//setFontSize(domElement.attribute("font-size").toDouble());
// 		QFont font;
// 		font.setFamily(domElement.attribute("font-family"));
// 		font.setPointSize(domElement.attribute("font-size").toInt());
// 		m_text->setDefaultFont(font);
	}
	
	// load shape
	domElement = pe.firstChildElement("shape");
	if (domElement.isElement()) 
	{
		bool shapeEnabled = domElement.attribute("enabled").toInt();
		setShapeEnabled(shapeEnabled);
		
		domElement = domElement.firstChildElement("control-points");
		if (shapeEnabled && domElement.isElement()) 
		{
			QList<QPointF> points;
			
			QStringList strPoint;
			strPoint = domElement.attribute("one").split(" ");
			setShapePoint1(QPointF(strPoint.at(0).toFloat(), strPoint.at(1).toFloat()));
			strPoint = domElement.attribute("two").split(" ");
			setShapePoint2(QPointF(strPoint.at(0).toFloat(), strPoint.at(1).toFloat()));
			strPoint = domElement.attribute("three").split(" ");
			setShapePoint3(QPointF(strPoint.at(0).toFloat(), strPoint.at(1).toFloat()));
			strPoint = domElement.attribute("four").split(" ");
			setShapePoint4(QPointF(strPoint.at(0).toFloat(), strPoint.at(1).toFloat()));
		}
	}
	
	domElement = pe.firstChildElement("text-align");
	if (domElement.isElement()) 
	{
		Qt::Alignment x = (Qt::Alignment)domElement.attribute("x").toInt();
		Qt::Alignment y = (Qt::Alignment)domElement.attribute("y").toInt();
		setXTextAlign(x);
		setXTextAlign(y);
	}
		
	
	
	setBeingLoaded(false);
	
	AbstractVisualItem::fromXml(pe);
	
	return true;
}

void TextItem::toXml(QDomElement & pe) const
{
	AbstractVisualItem::toXml(pe);
	
	// Save general item properties
	pe.setTagName("text");
	QDomDocument doc = pe.ownerDocument();
	QDomElement domElement;
	QDomText text;
	QString valueStr;
	
	domElement= doc.createElement("text");
	pe.appendChild(domElement);
	text = doc.createTextNode(m_text);
	domElement.appendChild(text);
	
	// save default font
	domElement = doc.createElement("default-font");
	domElement.setAttribute("font-family", fontFamily());
	domElement.setAttribute("font-size", fontSize());
	pe.appendChild(domElement);
	
	// save shape and control points
	QDomElement shapeElement = doc.createElement("shape");
	shapeElement.setAttribute("enabled", shapeEnabled());
	pe.appendChild(shapeElement);
	if (shapeEnabled()) 
	{
// 		QList<QPointF> cp = m_shapeEditor->controlPoints();
		domElement = doc.createElement("control-points");
		shapeElement.appendChild(domElement);
		domElement.setAttribute("one", QString::number(shapePoint1().x())
			+ " " + QString::number(shapePoint1().y()));
		domElement.setAttribute("two", QString::number(shapePoint2().x())
			+ " " + QString::number(shapePoint2().y()));
		domElement.setAttribute("three", QString::number(shapePoint3().x())
			+ " " + QString::number(shapePoint3().y()));
		domElement.setAttribute("four", QString::number(shapePoint4().x())
			+ " " + QString::number(shapePoint4().y()));
	}
	
	QDomElement alignElement = doc.createElement("text-align");
	alignElement.setAttribute("x", (int)xTextAlign());
	alignElement.setAttribute("y", (int)yTextAlign());
	pe.appendChild(alignElement);
}

