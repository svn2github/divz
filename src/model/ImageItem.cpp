#include "ImageItem.h"
#include "items/ImageContent.h"

#include <QGraphicsScene>

ImageItem::ImageItem() : AbstractVisualItem() 
{
	setFillType(AbstractVisualItem::Image);
	setOutlineEnabled(false);
	setOutlinePen(QPen(Qt::black,3));
	setFillBrush(QBrush(Qt::white));
}

ImageItem::~ImageItem() {}

AbstractContent * ImageItem::createDelegate(QGraphicsScene *scene,QGraphicsItem *parent)
{
 	ImageContent * box = new ImageContent(scene,parent);
	box->syncFromModelItem(this);
	return box;
}

void ImageItem::toXml(QDomElement & pe) const
{
	AbstractVisualItem::toXml(pe);
	
	// Save general item properties
	pe.setTagName("image");
}


AbstractItem * ImageItem::clone() { return AbstractItem::cloneTo(new ImageItem()); }
