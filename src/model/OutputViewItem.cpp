#include  "OutputViewItem.h"

#include "items/OutputViewContent.h"
#include "AppSettings.h"
#include "model/Output.h"

#include <QGraphicsScene>
#include <QStringList>

OutputViewItem::OutputViewItem() : AbstractVisualItem() 
{
// 	setFillType(AbstractVisualItem::Solid);
// 	setOutlineEnabled(true);
// 	setOutlinePen(QPen(Qt::black,3));
// 	setFillBrush(QBrush(Qt::white));
	m_outputId = -1;
	m_outputPort = -1;
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
//ITEM_PROPSET(OutputViewItem, OutputPort, int, outputPort)
void OutputViewItem::setOutputPort(int port)
{
	m_outputPort = port;
	qDebug() << "OutputViewItem::setOutputPort(): port:"<<port;
}

QByteArray OutputViewItem::toByteArray() const
{
	// we must! What does this damage though by overriding the constness? ...
	OutputViewItem *item = const_cast<OutputViewItem*>(this);
	{
		item->m_outputPort = AppSettings::outputById(m_outputId)->port();
		qDebug() << "OutputViewItem::toByteArray(): Storing port#:"<<item->m_outputPort;
	}
	
	return AbstractItem::toByteArray();
}

AbstractItem * OutputViewItem::clone() const { return AbstractItem::cloneTo(new OutputViewItem()); }
