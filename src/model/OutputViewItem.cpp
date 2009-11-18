#include  "OutputViewItem.h"

#include "items/OutputViewContent.h"

#include <QGraphicsScene>
#include <QStringList>

OutputViewItem::OutputViewItem() : AbstractVisualItem() 
{
// 	setFillType(AbstractVisualItem::Solid);
// 	setOutlineEnabled(true);
// 	setOutlinePen(QPen(Qt::black,3));
// 	setFillBrush(QBrush(Qt::white));
}

OutputViewItem::~OutputViewItem() {}

#include <assert.h>
AbstractContent * OutputViewItem::createDelegate(QGraphicsScene *scene,QGraphicsItem *parent)
{
 	OutputViewContent * box = new OutputViewContent(scene,parent);
//	SimpleTextContent * textContent = new TextContent(scene);
	assert(box);
	
// 	AbstractContent * vi = (AbstractContent *)box;
	//printf("Debug: itemId: %d\n", vi->itemId());
	
	
	box->syncFromModelItem(this);
	return box;
}

void OutputViewItem::toXml(QDomElement & pe) const
{
	AbstractVisualItem::toXml(pe);
	
	// Save general item properties
	pe.setTagName("outputview");
	pe.setAttribute("outputid",m_outputId);
}


bool OutputViewItem::fromXml(QDomElement & pe) 
{
	if(!AbstractVisualItem::fromXml(pe))
		return false;
	
	m_outputId = pe.attribute("outputid",0).toInt();
	
	return true;
}

ITEM_PROPSET(OutputViewItem, OutputId, int, outputId)


AbstractItem * OutputViewItem::clone() const { return AbstractItem::cloneTo(new OutputViewItem()); }