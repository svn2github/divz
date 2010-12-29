#include "GLRectDrawable.h"

GLRectDrawable::GLRectDrawable(QObject *parent)
	: GLImageDrawable("",parent)
	, m_fillColor(Qt::white)
	, m_borderColor(Qt::black)
	, m_borderWidth(3.0)
	, m_cornerRounding(0)
{
	foreach(CornerItem *corner, m_cornerItems)
		corner->setDefaultLeftOp(CornerItem::Scale);
		
	renderImage();
}

void GLRectDrawable::setFillColor(QColor c)
{
	m_fillColor = c;
	renderImage();
}

void GLRectDrawable::setBorderColor(QColor c)
{
	m_borderColor = c;
	renderImage();
}

void GLRectDrawable::setBorderWidth(double d)
{
	m_borderWidth = d;
	renderImage();
}

void GLRectDrawable::setCornerRounding(double d)
{
	m_cornerRounding = d;
	renderImage();
}

void GLRectDrawable::renderImage()
{
	QImage image(rect().size().toSize(),QImage::Format_ARGB32);
	memset(image.scanLine(0),0,image.byteCount());
	QPainter painter(&image);
	if(m_borderWidth > 0.0)
		painter.setPen(QPen(m_borderColor,m_borderWidth));
	else
		painter.setPen(QPen());
	painter.setBrush(m_fillColor);
	QRect target = image.rect();
	int v = m_borderWidth / 2;
	target = target.adjusted(v,v,-v,-v);
	painter.drawRect(target);
	painter.end();
	
	setImage(image);
}

// Reimpl rather than rely on GLVideoDrawable for speed on older hardware AND for speed in the editor
void GLRectDrawable::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
	painter->setOpacity(opacity());
	if(m_borderWidth > 0.0)
		painter->setPen(QPen(m_borderColor,m_borderWidth));
	else
		painter->setPen(QPen());
	painter->setBrush(m_fillColor);
	QRectF target = QRectF(QPointF(0,0),rect().size());
	double v = m_borderWidth / 2;
	target = target.adjusted(v,v,-v,-v);
	painter->drawRect(target);
}

