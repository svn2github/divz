#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H

#include <QMap>
#include <QString>
#include <QMutex>

#include "AbstractItem.h"

#define ITEM_TEXT 0x0001 

class ItemFactory
{
public:
	// enumerate all available items
	static QList<quint32> classes();

	// create an item
	static AbstractItem * createItem(quint32 itemClass);
	
	static quint32 nextId();

private:
	quint32 m_idCounter;
	
public:
	ItemFactory();
	~ItemFactory();
};

#endif
