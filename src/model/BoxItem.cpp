#include  "BoxItem.h"

#include "items/BoxContent.h"

#include <QGraphicsScene>
#include <QStringList>

BoxItem::BoxItem() : AbstractVisualItem() 
{
	setFillType(AbstractVisualItem::Solid);
	setOutlineEnabled(true);
	setOutlinePen(QPen(Qt::black,3));
	setFillBrush(QBrush(Qt::white));
}

BoxItem::~BoxItem() {}

#include <assert.h>
AbstractContent * BoxItem::createDelegate(QGraphicsScene *scene,QGraphicsItem *parent)
{
 	BoxContent * box = new BoxContent(scene,parent);
//	SimpleTextContent * textContent = new TextContent(scene);
	assert(box);
	
// 	AbstractContent * vi = (AbstractContent *)box;
	//printf("Debug: itemId: %d\n", vi->itemId());
	
	box->syncFromModelItem(this);
	return box;
}

void BoxItem::toXml(QDomElement & pe) const
{
	AbstractVisualItem::toXml(pe);
	
	// Save general item properties
	pe.setTagName("box");
}


AbstractItem * BoxItem::clone() const { return AbstractItem::cloneTo(new BoxItem()); }