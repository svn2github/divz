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


public:
	Slide();
	~Slide();
	
	void addItem(AbstractItem *);
	QList<AbstractItem *> itemList();
	
	void removeItem(AbstractItem *);
	
// 	AbstractItem * createPicture(QPoint);
// 	TextItem * createText(QPoint);
	
	bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement) const;
	
	ITEM_PROPDEF(SlideId,		int,	slideId);
	ITEM_PROPDEF(SlideNumber,	int,	slideNumber);
	
private:
	QList<AbstractItem *> m_items;
	int m_slideNumber;
	int m_slideId;
};

//Q_DECLARE_METATYPE(Slide);

#endif
