#include "AbstractItem.h"
#include <QDebug>
#include <QMetaProperty>

AbstractItem::AbstractItem() :
	  m_isBeingLoaded(false)
	, m_isChanged(false)
{}

ITEM_PROPSET(AbstractItem, ItemId,   quint32, itemId);
ITEM_PROPSET(AbstractItem, ItemName, QString, itemName);

void AbstractItem::setChanged(QString name, QVariant value, QVariant oldValue)
{
	if(!isBeingLoaded())
	{
		if(value != oldValue)
		{
			//qDebug() << "AbstractItem::setChanged: itemName:"<<itemName()<<", name:"<<name<<", value:"<<value;
			emit itemChanged(name,value,oldValue);
			m_isChanged = true;
		}
	}
}

void AbstractItem::clearIsChanged() { m_isChanged = false; }

void AbstractItem::setBeingLoaded(bool flag) { m_isBeingLoaded = flag; }

AbstractItem * AbstractItem::clone()
{
	return cloneTo(new AbstractItem());
}

AbstractItem * AbstractItem::cloneTo(AbstractItem *item)
{
	item->setBeingLoaded(true);

	//qDebug() << "AbstractItem::clone():"<<itemName()<<": Cloning item.";

	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
	const QMetaObject *metaobject = metaObject();
	int count = metaobject->propertyCount();
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		QVariant value = property(name);
		//qDebug() << "AbstractItem::clone():"<<itemName()<<": prop:"<<name<<", value:"<<value;
		item->setProperty(name,value);
	}

	item->setBeingLoaded(false);

	return item;
}

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

