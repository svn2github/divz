#ifndef SLIDE_H
#define SLIDE_H

#include "AbstractItem.h"

#include <QList>

class Slide : public QObject
{
	Q_OBJECT
	
	Q_PROPERTY(int slideId READ slideId WRITE setSlideId);
	Q_PROPERTY(int slideNumber READ slideNumber WRITE setSlideNumber);
	Q_PROPERTY(double autoChangeTime READ autoChangeTime WRITE setAutoChangeTime);
	
	Q_PROPERTY(double inheritFadeSettings READ inheritFadeSettings WRITE setInheritFadeSettings);
	Q_PROPERTY(double crossFadeSpeed READ crossFadeSpeed WRITE setCrossFadeSpeed);
	Q_PROPERTY(double crossFadeQuality READ crossFadeQuality WRITE setCrossFadeQuality);


public:
	Slide();
	~Slide();
	
	void addItem(AbstractItem *, bool takeOwnership = true);
	QList<AbstractItem *> itemList();
	int itemCount() { return m_items.size(); }
	AbstractItem * background();
	
	void removeItem(AbstractItem *);
	
	
	bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement) const;
        
        virtual QByteArray toByteArray() const;
	virtual void fromByteArray(QByteArray &);
	
	ITEM_PROPDEF(SlideId,		int,	slideId);
	ITEM_PROPDEF(SlideNumber,	int,	slideNumber);
	
	// A value of 0 means never change automatically
	ITEM_PROPDEF(AutoChangeTime,	double,	autoChangeTime);
	
	ITEM_PROPDEF(InheritFadeSettings,bool,	inheritFadeSettings);  
	ITEM_PROPDEF(CrossFadeSpeed,	double,	crossFadeSpeed);    // secs
	ITEM_PROPDEF(CrossFadeQuality,	double,	crossFadeQuality);  // frames
	
	Slide * clone() const;
	
	// guess a change time based on slide contents
	double guessTimeout();

signals:
	// Operation = "Add", "Remove", "Change"
	void slideItemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value, QVariant oldValue);
	
private slots:
	void itemChanged(QString fieldName, QVariant value, QVariant);
	
private:
	void loadByteArray(QByteArray &);
	
	QList<AbstractItem *> m_items;
	QList<AbstractItem *> m_ownedItems;
	int m_slideNumber;
	int m_slideId;
	double m_autoChangeTime;
	bool m_inheritFadeSettings;
	double m_crossFadeSpeed;
	double m_crossFadeQuality;
};

//Q_DECLARE_METATYPE(Slide);

#endif
