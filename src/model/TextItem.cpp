#include  "TextItem.h"

#include "items/TextContent.h"
//#include "items/SimpleTextContent.h"

#include <QGraphicsScene>
#include <QStringList>

TextItem::TextItem() : AbstractVisualItem() 
{
	m_fontFamily = "Tahoma";
	m_fontSize   = 40;
}

TextItem::~TextItem() {}

//#define ITEM_PROPSET(className,setterName,typeName,memberName) 
//ITEM_PROPSET(TextItem, Text, QString, text);
void TextItem::setText(QString text)
{
	m_text = text;
        qDebug("TextItem::setText: '%s'",text.toAscii().constData());
	setChanged("text",text);
}

ITEM_PROPSET(TextItem, FontFamily, QString, fontFamily)
ITEM_PROPSET(TextItem, FontSize, double, fontSize)
ITEM_PROPSET(TextItem, ShapeEnabled, bool, shapeEnabled)
ITEM_PROPSET(TextItem, ShapePoint1, QPointF, shapePoint1)
ITEM_PROPSET(TextItem, ShapePoint2, QPointF, shapePoint2)
ITEM_PROPSET(TextItem, ShapePoint3, QPointF, shapePoint3)
ITEM_PROPSET(TextItem, ShapePoint4, QPointF, shapePoint4)

#include <assert.h>
AbstractContent * TextItem::createDelegate(QGraphicsScene *scene)
{
 	TextContent * textContent = new TextContent(scene);
//	SimpleTextContent * textContent = new TextContent(scene);
	assert(textContent);
	
	AbstractContent * vi = (AbstractContent *)textContent;
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
		setFontSize(domElement.attribute("font-size").toDouble());
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
}

