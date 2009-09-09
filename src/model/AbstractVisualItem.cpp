#include "AbstractVisualItem.h"
//#include "frames/FrameFactory.h"
#include <QTransform>
#include <QDebug>

AbstractVisualItem::AbstractVisualItem() 
{
	m_pos = QPointF();
	m_contentsRect = QRectF(0,0,0,0);
	m_isVisible = false;
	m_zValue = 0;
	m_frameClass = 0;
	m_xRotation = 0;
	m_yRotation = 0;
	m_zRotation = 0;
	m_opacity = 1;
}

AbstractContent * AbstractVisualItem::createDelegate(QGraphicsScene*) {return 0;}

//#define PROPSET (a,b,c) void AbstractVisualItem::set##a(b x){m_##b = x;setChanged(c,x);}

ITEM_PROPSET(AbstractVisualItem, Pos,        QPointF, pos);
//ITEM_PROPSET(AbstractVisualItem, ContentsRect,QRectF, contentsRect);

void AbstractVisualItem::setContentsRect(QRectF r)
{
	
// 	qDebug() << "AbstractVisualItem::setContentsRect: new: " << r;
// 	qDebug() << "AbstractVisualItem::setContentsRect: current: " << m_contentsRect;
	//m_contentsRect.setLeft(0);
	
	m_contentsRect = QRectF(r.left(), r.top(), r.width(), r.height());
	setChanged("contentsRect",r);
}


ITEM_PROPSET(AbstractVisualItem, IsVisible,  bool,    isVisible);
ITEM_PROPSET(AbstractVisualItem, ZValue,     double,  zValue);
ITEM_PROPSET(AbstractVisualItem, FrameClass, quint32, frameClass);
ITEM_PROPSET(AbstractVisualItem, XRotation,  double,  xRotation);
ITEM_PROPSET(AbstractVisualItem, YRotation,  double,  yRotation);
ITEM_PROPSET(AbstractVisualItem, ZRotation,  double,  zRotation);
ITEM_PROPSET(AbstractVisualItem, Opacity,    double,  opacity);


bool AbstractVisualItem::fromXml(QDomElement & pe)
{
	setBeingLoaded(true);
	
	// restore content properties
	QDomElement domElement;
	
	// Load image size saved in the rect node
	domElement = pe.firstChildElement("rect");
	qreal x, y, w, h;
	x = domElement.firstChildElement("x").text().toDouble();
	y = domElement.firstChildElement("y").text().toDouble();
	w = domElement.firstChildElement("w").text().toDouble();
	h = domElement.firstChildElement("h").text().toDouble();
	setContentsRect(QRectF(x, y, w, h));
	
	// Load position coordinates
	domElement = pe.firstChildElement("pos");
	x = domElement.firstChildElement("x").text().toDouble();
	y = domElement.firstChildElement("y").text().toDouble();
	setPos(QPointF(x, y));
	
	double zvalue = pe.firstChildElement("zvalue").text().toDouble();
	setZValue(zvalue);
	
	double opacity = pe.firstChildElement("opacity").text().toDouble();
	setOpacity(opacity);
	
	bool visible = pe.firstChildElement("visible").text().toInt();
	setIsVisible(visible);
	/*
	bool hasText = pe.firstChildElement("frame-text-enabled").text().toInt();
	setFrameTextEnabled(hasText);
	if (hasText) {
		QString text = pe.firstChildElement("frame-text").text();
		setFrameText(text);
	}*/
	
	quint32 frameClass = pe.firstChildElement("frame-class").text().toInt();
	//setFrame(frameClass ? FrameFactory::createFrame(frameClass) : 0);
	setFrameClass(frameClass);
	
	// restore transformation
	QDomElement te = pe.firstChildElement("transformation");
	if (!te.isNull()) {
		m_xRotation = te.attribute("xRot").toDouble();
		m_yRotation = te.attribute("yRot").toDouble();
		m_zRotation = te.attribute("zRot").toDouble();
		//applyRotations();
	}
// 	domElement = pe.firstChildElement("mirror");
// 	setMirrorEnabled(domElement.attribute("state").toInt());
// 	
	AbstractItem::fromXml(pe);
	
	setBeingLoaded(false);
	
	return true;
}

void AbstractVisualItem::toXml(QDomElement & pe) const
{
	AbstractItem::toXml(pe);
		
	// Save general item properties
	pe.setTagName("visual");
	QDomDocument doc = pe.ownerDocument();
	QDomElement domElement;
	QDomText text;
	QString valueStr;
	
	// Save item position and size
	QDomElement rectParent = doc.createElement("rect");
	QDomElement xElement = doc.createElement("x");
	rectParent.appendChild(xElement);
	QDomElement yElement = doc.createElement("y");
	rectParent.appendChild(yElement);
	QDomElement wElement = doc.createElement("w");
	rectParent.appendChild(wElement);
	QDomElement hElement = doc.createElement("h");
	rectParent.appendChild(hElement);
	
	QRectF rect = m_contentsRect;
	xElement.appendChild(doc.createTextNode(QString::number(rect.left())));
	yElement.appendChild(doc.createTextNode(QString::number(rect.top())));
	wElement.appendChild(doc.createTextNode(QString::number(rect.width())));
	hElement.appendChild(doc.createTextNode(QString::number(rect.height())));
	pe.appendChild(rectParent);
	
	// Save the position
	domElement= doc.createElement("pos");
	xElement = doc.createElement("x");
	yElement = doc.createElement("y");
	valueStr.setNum(pos().x());
	xElement.appendChild(doc.createTextNode(valueStr));
	valueStr.setNum(pos().y());
	yElement.appendChild(doc.createTextNode(valueStr));
	domElement.appendChild(xElement);
	domElement.appendChild(yElement);
	pe.appendChild(domElement);
	
	// Save the stacking position
	domElement= doc.createElement("zvalue");
	pe.appendChild(domElement);
	valueStr.setNum(zValue());
	text = doc.createTextNode(valueStr);
	domElement.appendChild(text);
	
	// Save the opacity
	domElement= doc.createElement("opacity");
	pe.appendChild(domElement);
	valueStr.setNum(opacity());
	text = doc.createTextNode(valueStr);
	domElement.appendChild(text);
	
	// Save the visible state
	domElement= doc.createElement("visible");
	pe.appendChild(domElement);
	valueStr.setNum(isVisible());
	text = doc.createTextNode(valueStr);
	domElement.appendChild(text);
	
	// Save the frame class
	valueStr.setNum(frameClass());
	domElement= doc.createElement("frame-class");
	pe.appendChild(domElement);
	text = doc.createTextNode(valueStr);
	domElement.appendChild(text);
	
// 	domElement= doc.createElement("frame-text-enabled");
// 	pe.appendChild(domElement);
// 	valueStr.setNum(frameTextEnabled());
// 	text = doc.createTextNode(valueStr);
// 	domElement.appendChild(text);
	
// 	if(frameTextEnabled()) {
// 		domElement= doc.createElement("frame-text");
// 		pe.appendChild(domElement);
// 		text = doc.createTextNode(frameText());
// 		domElement.appendChild(text);
// 	}
// 	
// 	// save transformation
// 	const QTransform t = transform();
// 	if (!t.isIdentity()) {
		domElement = doc.createElement("transformation");
		domElement.setAttribute("xRot", m_xRotation);
		domElement.setAttribute("yRot", m_yRotation);
		domElement.setAttribute("zRot", m_zRotation);
		pe.appendChild(domElement);
//	}
// 	domElement = doc.createElement("mirror");
// 	domElement.setAttribute("state", mirrorEnabled());
// 	pe.appendChild(domElement);

}