#include "AbstractItem.h"

AbstractItem::AbstractItem() :
	  m_isBeingLoaded(false)
	, m_isChanged(false)
{}

ITEM_PROPSET(AbstractItem, ItemId,   quint32, itemId);
ITEM_PROPSET(AbstractItem, ItemName, QString, itemName);

void AbstractItem::setChanged(QString name, QVariant value)
{
	if(!isBeingLoaded())
	{
		emit itemChanged(name,value);
		m_isChanged = true;
	}
}

void AbstractItem::clearIsChanged() { m_isChanged = false; }

void AbstractItem::setBeingLoaded(bool flag) { m_isBeingLoaded = flag; }



bool AbstractItem::fromXml(QDomElement & pe)
{
	setBeingLoaded(true);
	
	// restore content properties
	QString text = pe.firstChildElement("item-name").text();
	setItemName(text);
	
	quint32 id = pe.firstChildElement("item-id").text().toInt();
	setItemId(id);
	
	setBeingLoaded(false);
	
	return true;
}

void AbstractItem::toXml(QDomElement & pe) const
{
	// Save general item properties
	pe.setTagName("item");
	QDomDocument doc = pe.ownerDocument();
	QDomElement domElement;
	QDomText text;
	QString valueStr;
	
	valueStr.setNum(itemId());
	domElement= doc.createElement("item-id");
	pe.appendChild(domElement);
	text = doc.createTextNode(valueStr);
	domElement.appendChild(text);
	
	domElement= doc.createElement("item-name");
	pe.appendChild(domElement);
	text = doc.createTextNode(itemName());
	domElement.appendChild(text);

}

