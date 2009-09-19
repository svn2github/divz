#include "AbstractVisualItem.h"
//#include "frames/FrameFactory.h"
#include <QTransform>
#include <QDebug>
#ifdef UNIX
    #include <execinfo.h>
     #include <stdio.h>
     #include <stdlib.h>
     
     /* Obtain a backtrace and print it to stdout. */
     static void
     print_trace (void)
     {
       void *array[10];
       size_t size;
       char **strings;
       size_t i;
     
       size = backtrace (array, 10);
       strings = backtrace_symbols (array, size);
     
       printf ("Obtained %zd stack frames.\n", size);
     
       for (i = 0; i < size; i++)
          printf ("%s\n", strings[i]);
     
       free (strings);
     }
#endif

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
	
	m_fillType = None;
	m_fillBrush = QBrush(Qt::white);
	m_fillImageFile = "";
	m_fillVideoFile = "";
	
	m_outlineEnabled = false;
	m_outlinePen = QPen(Qt::black,1);
	
	m_mirrorEnabled = false;
	m_mirrorOffset = 3;
	
	m_shadowEnabled = false;
	m_shadowOffset = 3;
	m_shadowBrush = QBrush(Qt::black);
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


ITEM_PROPSET(AbstractVisualItem, IsVisible,  		bool,   isVisible);
ITEM_PROPSET(AbstractVisualItem, ZValue,     		double, zValue);
ITEM_PROPSET(AbstractVisualItem, FrameClass, 		quint32,frameClass);
ITEM_PROPSET(AbstractVisualItem, XRotation,  		double, xRotation);
ITEM_PROPSET(AbstractVisualItem, YRotation,  		double, yRotation);
ITEM_PROPSET(AbstractVisualItem, ZRotation,  		double, zRotation);

ITEM_PROPSET(AbstractVisualItem, Opacity,    		double, opacity);

ITEM_PROPSET(AbstractVisualItem, FillType,		FillType,	fillType);
ITEM_PROPSET(AbstractVisualItem, FillBrush,		QBrush,		fillBrush);
ITEM_PROPSET(AbstractVisualItem, FillImageFile,		QString,	fillImageFile);
ITEM_PROPSET(AbstractVisualItem, FillVideoFile,		QString,	fillVideoFile);

ITEM_PROPSET(AbstractVisualItem, OutlineEnabled,	bool,	outlineEnabled);
ITEM_PROPSET(AbstractVisualItem, OutlinePen,		QPen,	outlinePen);

ITEM_PROPSET(AbstractVisualItem, MirrorEnabled,		bool,	mirrorEnabled);
ITEM_PROPSET(AbstractVisualItem, MirrorOffset,		double,	mirrorOffset);

ITEM_PROPSET(AbstractVisualItem, ShadowEnabled,		bool,	shadowEnabled);
ITEM_PROPSET(AbstractVisualItem, ShadowOffset,		double,	shadowOffset);
ITEM_PROPSET(AbstractVisualItem, ShadowBrush,		QBrush,	shadowBrush);


bool AbstractVisualItem::fromXml(QDomElement & pe)
{
	setBeingLoaded(true);
	
	AbstractItem::fromXml(pe);
	
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
	
	domElement = pe.firstChildElement("fill");
	if(!domElement.isNull()) 
	{
		AbstractVisualItem::FillType t = (AbstractVisualItem::FillType)domElement.attribute("type").toInt();
		setFillType(t);
		
		QBrush fill = brushFromXml(domElement);
		setFillBrush(fill);
		
		setFillImageFile(domElement.attribute("image"));
		setFillVideoFile(domElement.attribute("video"));
		
		//qDebug() << "fromXml: "<<itemName()<<": fillVideoFile
	}
	
 	domElement = pe.firstChildElement("outline");
	if(!domElement.isNull()) 
	{
		bool hasOutline = domElement.attribute("enabled").toInt();
 		setOutlineEnabled(hasOutline);
 		
// 		if(hasOutline)
// 		{
			QPen line = penFromXml(domElement);
			
			qDebug() << "fromXml:"<<itemName()<<": hasOutline:"<<hasOutline<<", pen: "<<line;
 			setOutlinePen(line);
// 		}
	}
	
	domElement = pe.firstChildElement("mirror");
	if(!domElement.isNull()) 
	{
		bool flag = domElement.attribute("enabled").toInt();
		setMirrorEnabled(flag);
// 		if(flag)
// 		{
			double offset = domElement.attribute("offset").toDouble();
			setMirrorOffset(offset);
// 		}
	}
	
	domElement = pe.firstChildElement("shadow");
	if(!domElement.isNull()) 
	{
		bool flag = domElement.attribute("enabled").toInt();
		setShadowEnabled(flag);
// 		if(flag)
// 		{
			double offset = domElement.attribute("offset").toDouble();
			setMirrorOffset(offset);
			
			QBrush fill = brushFromXml(domElement);
			setShadowBrush(fill);
		//}
	}
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
 	
 	domElement = doc.createElement("fill");
 	domElement.setAttribute("type", fillType());
 	brushToXml(domElement,fillBrush());
 	domElement.setAttribute("image", fillImageFile());
 	domElement.setAttribute("video", fillVideoFile());
 	pe.appendChild(domElement);
 	
 	domElement = doc.createElement("outline");
 	domElement.setAttribute("enabled", outlineEnabled());
 	penToXml(domElement,outlinePen());
 	pe.appendChild(domElement);
 	
 	domElement = doc.createElement("mirror");
 	domElement.setAttribute("enabled", mirrorEnabled());
 	domElement.setAttribute("offset", mirrorOffset());
 	pe.appendChild(domElement);
 	
 	domElement = doc.createElement("shadow");
 	domElement.setAttribute("enabled", shadowEnabled());
 	domElement.setAttribute("offset", shadowOffset());
 	brushToXml(domElement,shadowBrush());
 	pe.appendChild(domElement);

}

static QString qcolorToString(const QColor & color)
{
	return QString::number(color.rgba()); //QString("%1,%2,%3,%4").arg(color.redF()).arg(color.greenF()).arg(color.blueF()).arg(color.alphaF());
}

static QColor qcolorFromString(const QString & string)
{
// 	QStringList list = string.split(",");
// 	QColor c = QColor(list[0].toInt(),list[1].toInt(),list[2].toInt(),list[3].toInt());
// 	qDebug()<<"qcolorFromString(): string:"<<string<<", list:"<<list<<", color:"<<c;
	return QColor(string.toUInt());
	//return c;
}

void AbstractVisualItem::penToXml(QDomElement & domElement, QPen pen)
{
	brushToXml(domElement,pen.brush());
	domElement.setAttribute("pen-cap",   (int)pen.capStyle());
	domElement.setAttribute("pen-color", qcolorToString(pen.color()));
	domElement.setAttribute("pen-join",  (int)pen.joinStyle());
	domElement.setAttribute("pen-width", pen.widthF());
}

QPen AbstractVisualItem::penFromXml(QDomElement & domElement)
{
	QPen pen;
	QBrush brush = brushFromXml(domElement);
	pen.setBrush(brush);
	pen.setCapStyle((Qt::PenCapStyle)domElement.attribute("pen-cap").toInt());
	pen.setColor(qcolorFromString(domElement.attribute("pen-color")));
	pen.setJoinStyle((Qt::PenJoinStyle)domElement.attribute("pen-join").toInt());
	pen.setWidthF((double)domElement.attribute("pen-width").toDouble());
	return pen;
}

void AbstractVisualItem::brushToXml(QDomElement & domElement, QBrush brush)
{
	// Gradient:
	// - type: QGradient::LinearGradient, QGradient::RadialGradient, QGradient::ConicalGradient
	// - stopList: QVector<QGradientStop> = QPair<qreal,QColor> = QColor<rgba>
	// 	ex:  24|0.0=2,1,2,.5|1.0=255,255,255,255
	// Radial has:
	// - QPointF center, QPointF focal, qreal radius
	// Linear has:
	// - Final stop, start
	// Conical has:
	// - qreal angle, QPointF center
	
	
	domElement.setAttribute("brush-style",   (int)brush.style());
	domElement.setAttribute("brush-color", qcolorToString(brush.color()));
	
	
	const QGradient * g = brush.gradient();
	if(g)
	{
		QGradientStops stopList = g->stops();
		QString s;
		s += QString::number(stopList.size());
		s += "|";
		foreach(QGradientStop stop, stopList)
		{
			s += QString::number(stop.first);
			s += "=";
			s += qcolorToString(stop.second);
			s += "|";
		}
		domElement.setAttribute("grad-stops", s);
		
		domElement.setAttribute("grad-type", (int)g->type());
		
		if(g->type() == QGradient::LinearGradient)
		{
			QLinearGradient * g2 = (QLinearGradient*)g;
			QPointF start = g2->start();
			QPointF stop = g2->finalStop();
			
			domElement.setAttribute("grad-point1", QString("%1,%2").arg(start.x()).arg(start.y()));
			domElement.setAttribute("grad-point2", QString("%1,%2").arg(stop.x()).arg(stop.y()));
		}
		else
		if(g->type() == QGradient::RadialGradient)
		{
			QRadialGradient * g2 = (QRadialGradient*)g;
			QPointF start = g2->center();
			QPointF stop = g2->focalPoint();
			qreal arg = g2->radius();
			
			domElement.setAttribute("grad-point1", QString("%1,%2").arg(start.x()).arg(start.y()));
			domElement.setAttribute("grad-point2", QString("%1,%2").arg(stop.x()).arg(stop.y()));
			domElement.setAttribute("grad-modifier", QString::number(arg));
		}
		else
		if(g->type() == QGradient::ConicalGradient)
		{
			QConicalGradient * g2 = (QConicalGradient*)g;
			QPointF start = g2->center();
			qreal arg = g2->angle();
			
			domElement.setAttribute("grad-point1", QString("%1,%2").arg(start.x()).arg(start.y()));
			domElement.setAttribute("grad-modifier", QString::number(arg));
		}
	}
}

QBrush AbstractVisualItem::brushFromXml(QDomElement & domElement)
{
	QBrush brush;
	
	QString gradStops = domElement.attribute("grad-stops");
	if(!gradStops.isEmpty())
	{
		QGradient::Type t = (QGradient::Type)domElement.attribute("grad-type").toInt();
		
		// Parse the stop list
		QGradientStops stopList;
		QStringList stops = gradStops.split("|");
		foreach(QString stopDef, stops)
		{
			QStringList pair = stopDef.split("=");
			qreal pos = pair[0].toDouble();
			QColor color = qcolorFromString(pair[1]);
			stopList.append(QGradientStop(pos,color));
		}
		
		// Read in the control points
		QString pnt1 = domElement.attribute("grad-point1");
		QString pnt2 = domElement.attribute("grad-point2");
		
		QStringList split;
		split = pnt1.split(",");
		QPointF point1(split[0].toDouble(),split[1].toDouble());
		
		split = pnt2.split(",");
		QPointF point2(split[0].toDouble(),split[1].toDouble());
		
		// And the angle/radius modifier, if needed
		qreal mod = domElement.attribute("grad-modifier").toDouble();
		
		
		// Now, all thats left is to create the appros object and set the stop list, 
		// then set it on the brush
		if(t == QGradient::LinearGradient)
		{
			QLinearGradient g(point1,point2);
			g.setStops(stopList);
			brush = QBrush(g);
		}
		else
		if(t == QGradient::RadialGradient)
		{
			QRadialGradient g(point1,mod,point2);
			g.setStops(stopList);
			brush = QBrush(g);
		}
		else
		if(t == QGradient::ConicalGradient)
		{
			QRadialGradient g(point1,mod);
			g.setStops(stopList);
			brush = QBrush(g);
		}
	}
	
	brush.setColor(qcolorFromString(domElement.attribute("brush-color")));
	brush.setStyle((Qt::BrushStyle)domElement.attribute("brush-style").toInt());
	
	
	return brush;
}
