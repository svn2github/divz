#ifndef SLIDE_H
#define SLIDE_H

#include "AbstractItem.h"
#include "TextItem.h"

#include <QList>

class Slide : public QObject
{
	Q_OBJECT

public:
	Slide();
	~Slide();
	
	void addItem(AbstractItem *);
	QList<AbstractItem *> itemList();
	
	void removeItem(AbstractItem *);
	
// 	AbstractItem * createPicture(QPoint);
// 	TextItem * createText(QPoint);
	
	bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement, QDomDocument & doc) const;
	
private:
	QList<AbstractItem *> m_items;
	
	

};



#endif
