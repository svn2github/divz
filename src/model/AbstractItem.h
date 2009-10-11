#ifndef ABSTRACTITEM_H
#define ABSTRACTITEM_H

#include <QObject>
#include <QVariant>
#include <QDomElement>
#include <QPen>
#include <QBrush>

#define ITEM_PROPSET(className,setterName,typeName,memberName) \
	void className::set##setterName(typeName newValue) { \
		typeName oldValue = m_##memberName; \
		m_##memberName = newValue; \
		setChanged(#memberName,newValue,oldValue); \
	}
#define ITEM_PROPDEF(setterName,typeName,memberName) void set##setterName(typeName value); typeName memberName() const { return m_##memberName; }
#define V_ITEM_PROPDEF(setterName,typeName,memberName) virtual void set##setterName(typeName value); virtual typeName memberName() const { return m_##memberName; }

#define DMARK __FILE__":"#__LINE__
#include <QDebug>

class AbstractItem : public QObject
{
	Q_OBJECT
	
	Q_PROPERTY(quint32 itemId   READ itemId   WRITE setItemId);
	Q_PROPERTY(QString itemName READ itemName WRITE setItemName);

public:
	AbstractItem();
// 	~AbstractItem();
	
	// class and type are NOT pure virtual so that clone() can work as expected
	
	enum { ItemClass = 0x0 };
	
	// class identification
	virtual quint32 itemClass() const { return ItemClass; }
	
	// Type of item - Visual items should inherit AbstractVisualItem, NonVisual should inherit AbstractNonVisualItem
	typedef enum ItemType { Abstract, Visual, NonVisual };
	virtual ItemType itemType() const { return Abstract; }
	
	// Unique ID of an item 
	quint32 itemId() const { return m_itemId; }
	void setItemId(quint32);
	
	// loading/saving
	virtual bool fromXml(QDomElement & parentElement);
        virtual void toXml(QDomElement & parentElement) const;
	
	// Item name for human reference - not meant for content
	QString	itemName() const { return m_itemName; }
	void setItemName(QString);
	
	// Return true of any of the properties have been changed
	bool isChanged() { return m_isChanged; }
	
	bool isBeingLoaded() { return m_isBeingLoaded; }
	
	AbstractItem * clone();

signals:
	void itemChanged(QString fieldName, QVariant value, QVariant oldValue);
	
protected:
	void clearIsChanged();
	void setChanged(QString fieldName, QVariant value, QVariant oldValue);
	void setBeingLoaded(bool);
	
	AbstractItem * cloneTo(AbstractItem *);
	
private:
	
	// Fields
	qint32		m_itemClass;
	ItemType	m_itemType;
	
	quint32		m_itemId; 	// unique integer identifier for an item - unique to the scene...? (should scope be larger than just the scene)
	QString		m_itemName; 	// identification in the editor, e.x. "TextBox_4"
	
	bool		m_isChanged;	// true if any of the properties have been changed using their write accessors after object creation
	
	bool		m_isBeingLoaded; // true if being loaded (fromXml) - prevents itemChanged() signal from being fired by setChanged()
	
};

#endif
