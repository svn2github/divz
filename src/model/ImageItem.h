#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include "AbstractVisualItem.h"


class ImageItem : public AbstractVisualItem
{
	Q_OBJECT
	
public:
	ImageItem();
	~ImageItem();
	
	// ::AbstractVisualItem
	AbstractContent * createDelegate(QGraphicsScene*scene=0,QGraphicsItem*parent=0);
	
	// ::AbstractItem
	
	// class identification
	typedef enum { ItemClass = 0x0006 };
	quint32 itemClass() const { return ItemClass; }
	
	// loading/saving
        void toXml(QDomElement & parentElement) const;
        
        AbstractItem * clone();
};


#endif
