#ifndef OutputViewItem_H
#define OutputViewItem_H

#include "AbstractVisualItem.h"


class OutputViewItem : public AbstractVisualItem
{
	Q_OBJECT
	
	Q_PROPERTY(int outputId READ outputId WRITE setOutputId);
	Q_PROPERTY(int outputPort READ outputPort WRITE setOutputPort);
	
public:
	OutputViewItem();
	~OutputViewItem();
	
	int outputId() { return m_outputId; }
	void setOutputId(int);
	
	// Output Port is declared as a property and automatically set
	// in the toByteArray method and initalized from the byte array
	// by AbstractItem::fromByteArray().
	// Output Port is used by OutputViewContent to connect to an output
	// on the controlling PC if loaded in the network viewer.
	int outputPort() { return m_outputPort; }
	void setOutputPort(int);
	
	// ::AbstractVisualItem
	AbstractContent * createDelegate(QGraphicsScene*scene=0,QGraphicsItem*parent=0);
	
	// ::AbstractItem
	
	// class identification
	quint32 itemClass() const { return 0x0007; }
	
	// loading/saving
	void toXml(QDomElement & parentElement) const;
	bool fromXml(QDomElement & parentElement);
	
	QByteArray toByteArray() const;
	
	AbstractItem * clone() const;

protected:
	int m_outputId;
	int m_outputPort;
};


#endif
