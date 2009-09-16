#include  "BoxItem.h"

#include "items/BoxContent.h"

#include <QGraphicsScene>
#include <QStringList>

BoxItem::BoxItem() : AbstractVisualItem() 
{
}

BoxItem::~BoxItem() {}

#include <assert.h>
AbstractContent * BoxItem::createDelegate(QGraphicsScene *scene)
{
 	BoxContent * box = new BoxContent(scene);
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

