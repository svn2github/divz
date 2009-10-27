#ifndef BOXITEM_H
#define BOXITEM_H

#include "AbstractVisualItem.h"


class BoxItem : public AbstractVisualItem
{
	Q_OBJECT
	
public:
	BoxItem();
	~BoxItem();
	
	// ::AbstractVisualItem
	AbstractContent * createDelegate(QGraphicsScene*scene=0,QGraphicsItem*parent=0);
	
	// ::AbstractItem
	
	// class identification
	quint32 itemClass() const { return 0x0003; }
	
	// loading/saving
        void toXml(QDomElement & parentElement) const;
        
        AbstractItem * clone() const;
};


#endif
