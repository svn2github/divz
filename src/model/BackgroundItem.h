#ifndef BACKGROUNDITEM_H
#define BACKGROUNDITEM_H

#include "AbstractVisualItem.h"


class BackgroundItem : public AbstractVisualItem
{
	Q_OBJECT

public:
	
	
public:
	BackgroundItem();
	~BackgroundItem();
	
	// ::AbstractVisualItem
	AbstractContent * createDelegate(QGraphicsScene*scene=0,QGraphicsItem*parent=0);
	
	// ::AbstractItem
	enum { ItemClass = 0x0005 };
	// class identification
	quint32 itemClass() const { return ItemClass; }
	
	// loading/saving
        void toXml(QDomElement & parentElement) const;
};


#endif
