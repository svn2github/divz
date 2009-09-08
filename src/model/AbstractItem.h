#ifndef ABSTRACTITEM_H
#define ABSTRACTITEM_H

#include <QObject>
#include <QVariant>
#include <QDomElement>

#define ITEM_PROPSET(cn,a,b,c) void cn::set##a(b x){m_##c = x;setChanged(#c,x);}


class AbstractItem : public QObject
{
	Q_OBJECT
	
	Q_PROPERTY(quint32 itemId   READ itemId   WRITE setItemId);
	Q_PROPERTY(QString itemName READ itemName WRITE setItemName);

public:
	AbstractItem();
// 	~AbstractItem();
	
	// class identification
	virtual quint32 itemClass() const = 0;
	enum { Abstract = 0 };
	
	// Type of item - Visual items should inherit AbstractVisualItem, NonVisual should inherit AbstractNonVisualItem
	typedef enum ItemType { Visual, NonVisual };
	virtual ItemType itemType() const = 0;
	
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
	
signals:
	void itemChanged(QString fieldName, QVariant value);
	
protected:
	void clearIsChanged();
	void setChanged(QString fieldName, QVariant value);
	void setBeingLoaded(bool);
	
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
