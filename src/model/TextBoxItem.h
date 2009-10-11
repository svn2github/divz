#ifndef TEXTBOXITEM_H
#define TEXTBOXITEM_H

#include "AbstractVisualItem.h"
#include "TextItem.h"


class TextBoxItem : public TextItem
{
	Q_OBJECT
	
public:
	TextBoxItem();
	~TextBoxItem();
	
	// ::AbstractVisualItem
	AbstractContent * createDelegate(QGraphicsScene*scene=0,QGraphicsItem*parent=0);
	
	// ::AbstractItem
	
	// class identification
	typedef enum { ItemClass = 0x0002 };
	quint32 itemClass() const { return ItemClass; }
	
	// loading/saving
        void toXml(QDomElement & parentElement) const;
        
        AbstractItem * clone();
};


#endif
