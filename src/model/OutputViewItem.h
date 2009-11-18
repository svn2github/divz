#ifndef OutputViewItem_H
#define OutputViewItem_H

#include "AbstractVisualItem.h"


class OutputViewItem : public AbstractVisualItem
{
	Q_OBJECT
	
	Q_PROPERTY(int outputId READ outputId WRITE setOutputId);
	
public:
	OutputViewItem();
	~OutputViewItem();
	
	int outputId() { return m_outputId; }
	void setOutputId(int);
	
	// ::AbstractVisualItem
	AbstractContent * createDelegate(QGraphicsScene*scene=0,QGraphicsItem*parent=0);
	
	// ::AbstractItem
	
	// class identification
	quint32 itemClass() const { return 0x0007; }
	
	// loading/saving
	void toXml(QDomElement & parentElement) const;
	bool fromXml(QDomElement & parentElement);
	
	AbstractItem * clone() const;

protected:
	int m_outputId;
};


#endif
