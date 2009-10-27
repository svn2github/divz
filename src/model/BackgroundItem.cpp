#include  "BackgroundItem.h"

#include "items/BackgroundContent.h"

#include <QGraphicsScene>
#include <QStringList>

BackgroundItem::BackgroundItem() : AbstractVisualItem()
{
	setFillType(AbstractVisualItem::Solid);
	setOutlineEnabled(false);
	setFillBrush(QBrush(Qt::lightGray));
}

BackgroundItem::~BackgroundItem() {}

#include <assert.h>
AbstractContent * BackgroundItem::createDelegate(QGraphicsScene *scene,QGraphicsItem *parent)
{
 	BackgroundContent * box = new BackgroundContent(scene,parent);
//	SimpleTextContent * textContent = new TextContent(scene);
	assert(box);

// 	AbstractContent * vi = (AbstractContent *)box;
	//printf("Debug: itemId: %d\n", vi->itemId());

	box->syncFromModelItem(this);



	return box;
}

void BackgroundItem::toXml(QDomElement & pe) const
{
	AbstractVisualItem::toXml(pe);

	// Save general item properties
	pe.setTagName("background");
}


AbstractItem * BackgroundItem::clone() const { return AbstractItem::cloneTo(new BackgroundItem()); }