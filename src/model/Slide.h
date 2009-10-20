#ifndef SLIDE_H
#define SLIDE_H

#include "AbstractItem.h"
#include "TextItem.h"

#include <QList>

class Slide : public QObject
{
	Q_OBJECT
	
	Q_PROPERTY(int slideId READ slideId WRITE setSlideId);
	Q_PROPERTY(int slideNumber READ slideNumber WRITE setSlideNumber);
	Q_PROPERTY(int autoChangeTime READ autoChangeTime WRITE setAutoChangeTime);


public:
	Slide();
	~Slide();
	
	void addItem(AbstractItem *);
	QList<AbstractItem *> itemList();
	int itemCount() { return m_items.size(); }
	AbstractItem * background();
	
	void removeItem(AbstractItem *);
	
// 	AbstractItem * createPicture(QPoint);
// 	TextItem * createText(QPoint);
	
	bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement) const;
	
	ITEM_PROPDEF(SlideId,		int,	slideId);
	ITEM_PROPDEF(SlideNumber,	int,	slideNumber);
	
	// A value of 0 means never change automatically
	ITEM_PROPDEF(AutoChangeTime,	double,	autoChangeTime);
	
	Slide * clone();

signals:
	// Operation = "Add", "Remove", "Change"
	void slideItemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value, QVariant oldValue);
	
private slots:
	void itemChanged(QString fieldName, QVariant value, QVariant);
	
private:
	QList<AbstractItem *> m_items;
	int m_slideNumber;
	int m_slideId;
	double m_autoChangeTime;
};

//Q_DECLARE_METATYPE(Slide);

#endif
