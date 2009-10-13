#include "Slide.h"

#include "TextItem.h"
#include "TextBoxItem.h"
#include "BoxItem.h"
#include "VideoFileItem.h"
#include "BackgroundItem.h"
#include "ItemFactory.h"

#include <assert.h>
#include <QMetaProperty>

Slide::Slide()  
{
	m_slideId = 0;
	m_slideNumber = 0;
	m_autoChangeTime = 0;
}
Slide::~Slide() 
{
	//qDebug() << "Slide::~Slide() - destorying slide";
	qDeleteAll(m_items);
}

void Slide::setSlideId(int x)     { m_slideId = x; }
void Slide::setSlideNumber(int x)
{
	int old = m_slideNumber;
	m_slideNumber = x; 
	emit slideItemChanged(0,"change","slideNumber",x,old);
}

void Slide::setAutoChangeTime(double x) 
{ 
	double old = m_autoChangeTime;
	m_autoChangeTime = x;	
	emit slideItemChanged(0,"change","autoChangeTime",x,old); 
}

Slide * Slide::clone()
{
	Slide * newSlide = new Slide();
	
	newSlide->setSlideId(ItemFactory::nextId());
	
	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
	const QMetaObject *metaobject = metaObject();
	int count = metaobject->propertyCount();
	for (int i=0; i<count; ++i) 
	{
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		QVariant value = property(name);
		newSlide->setProperty(name,value);
	}
	
	foreach (AbstractItem * content, newSlide->m_items) 
		newSlide->removeItem(content);
	
	foreach(AbstractItem *oldItem, m_items)
	{
		AbstractItem * newItem = oldItem->clone();
			
		newItem->setItemId(ItemFactory::nextId());
		//newItem->setItemName(QString("NewItem%1").arg(bg->itemId()));
		newSlide->addItem(newItem);
	}
	
	return newSlide;
}


AbstractItem * Slide::background()
{
	foreach(AbstractItem *x, m_items)
		if(x->itemClass() == BackgroundItem::ItemClass)
			return x;
	AbstractItem * bg = new BackgroundItem();
	bg->setItemId(ItemFactory::nextId());
	bg->setItemName(QString("BackgroundItem%1").arg(bg->itemId()));
	addItem(bg);
	return bg;
}


QList<AbstractItem *> Slide::itemList() { return m_items; }

void Slide::addItem(AbstractItem *item)
{
	assert(item != NULL);
	emit slideItemChanged(item,"add","",QVariant(),QVariant());
	connect(item,SIGNAL(itemChanged(QString, QVariant, QVariant)),this,SLOT(itemChanged(QString, QVariant, QVariant)));
	m_items.append(item);
}

void Slide::removeItem(AbstractItem *item)
{
	assert(item != NULL);
	disconnect(item,0,this,0);
	emit slideItemChanged(item,"remove","",QVariant(),QVariant());
	m_items.removeAll(item);
}

void Slide::itemChanged(QString s,QVariant v,QVariant old)
{
	AbstractItem * item = dynamic_cast<AbstractItem *>(sender());
	emit slideItemChanged(item,"change",s,v,old);
}

bool Slide::fromXml(QDomElement & pe)
{
	qDeleteAll(m_items);
	m_items.clear();
	
	m_slideNumber = pe.attribute("number").toInt();
	m_slideId = pe.attribute("id").toInt();

	// for each child of 'slide'
	for (QDomElement element = pe.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) 
	{
		//qDebug("Slide::fromXml(): Found an element, tag name=%s", element.tagName().toAscii().constData());
		// create the right kind of content
		AbstractItem * content = 0;
// 		if (element.tagName() == "picture")
// 			content = createPicture(QPoint());
		if (element.tagName() == "text")
			content = new TextItem();
		else 
		if (element.tagName() == "textbox")
			content = new TextBoxItem();
		else 
		if (element.tagName() == "box")
			content = new BoxItem();
		else 
		if (element.tagName() == "videofile")
			content = new VideoFileItem();
		else 
		if (element.tagName() == "background")
			content = new BackgroundItem();
// 		else if (element.tagName() == "webcam")
// 			content = desk->createWebcam(element.attribute("input").toInt(), QPoint());
		if (!content) 
		{
			qWarning("Slide::fromXml: unknown content type '%s'", qPrintable(element.tagName()));
			continue;
		}
	
		addItem(content);
		
		// restore the item, and delete it if something goes wrong
		if (!content->fromXml(element)) 
		{
 			removeItem(content);
			delete content;
			continue;
		}
	}
	
	return true;
}

void Slide::toXml(QDomElement & pe) const
{
	QDomDocument doc = pe.ownerDocument();
	
	pe.setAttribute("number",m_slideNumber);
	pe.setAttribute("id",m_slideId);
	
	foreach (AbstractItem * content, m_items) 
	{
		QDomElement element = doc.createElement("renamed-element");
		pe.appendChild(element);
		content->toXml(element);
	}
}
