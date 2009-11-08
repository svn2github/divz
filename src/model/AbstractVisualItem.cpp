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
	m_isVisible = true;
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
	m_outlinePen.setJoinStyle(Qt::MiterJoin);
	
	m_mirrorEnabled = false;
	m_mirrorOffset = 3;
	
	m_shadowEnabled = false;
	m_shadowBlurRadius = 3;
	m_shadowOffsetX = 3;
	m_shadowOffsetY = 3;
	m_shadowBrush = QBrush(Qt::black);
	
	m_sourceOffsetTL = QPointF(0,0);
	m_sourceOffsetBR = QPointF(0,0);
	
	m_zoomEffectEnabled = false;
	m_zoomAnchorCenter = false;
	m_zoomDirection = ZoomRandom;
	m_zoomSpeed = 30;
	m_zoomLoop = false;
	m_zoomFactor = 1.5;
}

AbstractContent * AbstractVisualItem::createDelegate(QGraphicsScene*,QGraphicsItem*) {return 0;}

//#define PROPSET (a,b,c) void AbstractVisualItem::set##a(b x){m_##b = x;setChanged(c,x);}

ITEM_PROPSET(AbstractVisualItem, Pos,        QPointF, pos);
ITEM_PROPSET(AbstractVisualItem, ContentsRect,QRectF, contentsRect);
ITEM_PROPSET(AbstractVisualItem, SourceOffsetTL,  QPointF, sourceOffsetTL);
ITEM_PROPSET(AbstractVisualItem, SourceOffsetBR,  QPointF, sourceOffsetBR);
// void AbstractVisualItem::setContentsRect(QRectF r)
// {
// 	
//  	//qDebug() << "AbstractVisualItem::setContentsRect: r:" << r;
// // 	qDebug() << "AbstractVisualItem::setContentsRect: current: " << m_contentsRect;
// 	//m_contentsRect.setLeft(0);
// 	
// 	QRectF old = m_contentsRect;
// 	m_contentsRect = QRectF(r.left(), r.top(), r.width(), r.height());
// 	setChanged("contentsRect",r,old);
// }


ITEM_PROPSET(AbstractVisualItem, IsVisible,  		bool,   isVisible);
ITEM_PROPSET(AbstractVisualItem, ZValue,     		double, zValue);
ITEM_PROPSET(AbstractVisualItem, FrameClass, 		quint32,frameClass);
ITEM_PROPSET(AbstractVisualItem, XRotation,  		double, xRotation);
ITEM_PROPSET(AbstractVisualItem, YRotation,  		double, yRotation);
ITEM_PROPSET(AbstractVisualItem, ZRotation,  		double, zRotation);

ITEM_PROPSET(AbstractVisualItem, Opacity,    		double, opacity);

ITEM_PROPSET(AbstractVisualItem, FillType,		FillType,	fillType);
// void AbstractVisualItem::setFillType(FillType x)
// {
// 	//qDebug() << "AbstractVisualItem::setFillType: x="<<x;
// 	FillType oldValue = m_fillType;
// 	m_fillType = x;
// 	setChanged("fillType",x,oldValue);
// }

ITEM_PROPSET(AbstractVisualItem, FillBrush,		QBrush,		fillBrush);
ITEM_PROPSET(AbstractVisualItem, FillImageFile,		QString,	fillImageFile);
ITEM_PROPSET(AbstractVisualItem, FillVideoFile,		QString,	fillVideoFile);

ITEM_PROPSET(AbstractVisualItem, OutlineEnabled,	bool,	outlineEnabled);
ITEM_PROPSET(AbstractVisualItem, OutlinePen,		QPen,	outlinePen);

ITEM_PROPSET(AbstractVisualItem, MirrorEnabled,		bool,	mirrorEnabled);
ITEM_PROPSET(AbstractVisualItem, MirrorOffset,		double,	mirrorOffset);

ITEM_PROPSET(AbstractVisualItem, ShadowEnabled,		bool,	shadowEnabled);
// void AbstractVisualItem::setShadowEnabled(bool flag)
// {
// 	bool oldValue = m_shadowEnabled;
// 	m_shadowEnabled = flag;
// 	qDebug() << "AbstractVisualItem::setShadowEnabled: old:"<<oldValue<<", new:"<<flag;
// 	setChanged("shadowEnabled", flag, oldValue);
// }

ITEM_PROPSET(AbstractVisualItem, ShadowBlurRadius,	double,	shadowBlurRadius);
ITEM_PROPSET(AbstractVisualItem, ShadowOffsetX,		double,	shadowOffsetX);
ITEM_PROPSET(AbstractVisualItem, ShadowOffsetY,		double,	shadowOffsetY);
ITEM_PROPSET(AbstractVisualItem, ShadowBrush,		QBrush,	shadowBrush);

ITEM_PROPSET(AbstractVisualItem, ZoomEffectEnabled,	bool,	zoomEffectEnabled);
ITEM_PROPSET(AbstractVisualItem, ZoomAnchorCenter,	bool,	zoomAnchorCenter);
ITEM_PROPSET(AbstractVisualItem, ZoomDirection,		ZoomEffectDirection,	zoomDirection);
ITEM_PROPSET(AbstractVisualItem, ZoomSpeed,		int,	zoomSpeed);
ITEM_PROPSET(AbstractVisualItem, ZoomLoop,		bool,	zoomLoop);
ITEM_PROPSET(AbstractVisualItem, ZoomFactor,		double,	zoomFactor);
	
bool AbstractVisualItem::fromXml(QDomElement & pe)
{
	setBeingLoaded(true);
	
	AbstractItem::fromXml(pe);
	
	// restore content properties
	QDomElement domElement;
	
	// Load image size saved in the rect node
	domElement = pe.firstChildElement("rect");
	qreal x, y, w, h;
	
	// older formats of the file had <x>, <y>, ... elements under <rect>
	if(!domElement.firstChildElement("x").isNull())
	{
		x = domElement.firstChildElement("x").text().toDouble();
		y = domElement.firstChildElement("y").text().toDouble();
		w = domElement.firstChildElement("w").text().toDouble();
		h = domElement.firstChildElement("h").text().toDouble();
	}
	else
	{
		x = domElement.attribute("x").toDouble();
		y = domElement.attribute("y").toDouble();
		w = domElement.attribute("w").toDouble();
		h = domElement.attribute("h").toDouble();
	}
	setContentsRect(QRectF(x, y, w, h));
	
	// Load image size saved in the rect node
	domElement = pe.firstChildElement("source");
	qreal x1,y1,x2,y2;
	x1 = domElement.attribute("x1").toDouble();
	y1 = domElement.attribute("y1").toDouble();
	x2 = domElement.attribute("x2").toDouble();
	y2 = domElement.attribute("y2").toDouble();
	setSourceOffsetTL(QPoint(x1,y1));
	setSourceOffsetBR(QPoint(x2,y2));
	
	// Load position coordinates
	domElement = pe.firstChildElement("pos");
	// older formats of the file had <x> and <y> elements under <pos>
	if(!domElement.firstChildElement("x").isNull())
	{
		x = domElement.firstChildElement("x").text().toDouble();
		y = domElement.firstChildElement("y").text().toDouble();
	}
	else
	{
		x = domElement.attribute("x").toDouble();
		y = domElement.attribute("y").toDouble();
	}
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
			
			//qDebug() << "fromXml:"<<itemName()<<": hasOutline:"<<hasOutline<<", pen: "<<line;
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
			int blur = domElement.attribute("blur").toInt();
			if(!blur || blur<0)
				blur = 3;
			setShadowBlurRadius(blur);
			
			double offsetX = domElement.attribute("x").toDouble();
			setShadowOffsetX(offsetX);
			
			double offsetY = domElement.attribute("y").toDouble();
			setShadowOffsetY(offsetY);
			
			QBrush fill = brushFromXml(domElement);
			setShadowBrush(fill);
		//}
	}
	
	domElement = pe.firstChildElement("zoom");
	if(!domElement.isNull()) 
	{
		setZoomEffectEnabled(domElement.attribute("enabled").toInt());
		setZoomAnchorCenter(domElement.attribute("anchor-center").toInt());
		setZoomDirection((ZoomEffectDirection)domElement.attribute("direction").toInt());
		setZoomSpeed(domElement.attribute("speed").toInt());
		QString str = domElement.attribute("loop");
		setZoomLoop(str.isEmpty() ? true : str.toInt());
		str = domElement.attribute("factor");
		setZoomFactor(str.isEmpty() ? 2 : str.toDouble());
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
	
	QRectF rect = m_contentsRect;
	rectParent.setAttribute("x",QString::number(rect.left()));
	rectParent.setAttribute("y",QString::number(rect.top()));
	rectParent.setAttribute("w",QString::number(rect.width()));
	rectParent.setAttribute("h",QString::number(rect.height()));
	pe.appendChild(rectParent);
	
	// Save item source rect
	rectParent = doc.createElement("source");
	rectParent.setAttribute("x1",QString::number(m_sourceOffsetTL.x()));
	rectParent.setAttribute("y1",QString::number(m_sourceOffsetTL.y()));
	rectParent.setAttribute("x2",QString::number(m_sourceOffsetBR.x()));
	rectParent.setAttribute("y2",QString::number(m_sourceOffsetBR.y()));
	pe.appendChild(rectParent);
	
	
	
	// Save the position
	domElement= doc.createElement("pos");
	domElement.setAttribute("x",pos().x());
	domElement.setAttribute("y",pos().y());
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
 	domElement.setAttribute("blur", shadowBlurRadius());
 	domElement.setAttribute("x", shadowOffsetX());
 	domElement.setAttribute("y", shadowOffsetY());
 	brushToXml(domElement,shadowBrush());
 	pe.appendChild(domElement);
 	
 	domElement = doc.createElement("zoom");
 	domElement.setAttribute("enabled", zoomEffectEnabled());
 	domElement.setAttribute("anchor-center", zoomAnchorCenter());
 	domElement.setAttribute("direction", (int)zoomDirection());
 	domElement.setAttribute("speed", zoomSpeed());
 	domElement.setAttribute("loop", zoomLoop());
 	domElement.setAttribute("factor", zoomFactor());
 	pe.appendChild(domElement);
}

static inline QString qcolorToString(const QColor & color)
{
	//return QString::number((unsigned int)color.rgba());
	
	QStringList rgba;
	rgba << QString::number(color.red());
	rgba << QString::number(color.green());
	rgba << QString::number(color.blue());
	rgba << QString::number(color.alpha());
	return rgba.join(",");

}

static inline QColor qcolorFromString(const QString & string)
{
	if(string.indexOf(",") < 0)
		return QColor(string.toUInt());
	
	QStringList rgba = string.split(",");
	
	return QColor(  rgba[0].toInt(), // r
			rgba[1].toInt(), // g 
			rgba[2].toInt(), // b
			rgba[3].toInt()); // a
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
