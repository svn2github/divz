#include "Slide.h"

#include "TextItem.h"
#include "TextBoxItem.h"
#include "BoxItem.h"
#include "VideoFileItem.h"
#include "ItemFactory.h"

#include <assert.h>

Slide::Slide()  {}
Slide::~Slide() {

	qDeleteAll(m_items);
}

QList<AbstractItem *> Slide::itemList() { return m_items; }

void Slide::addItem(AbstractItem *item)
{
	assert(item != NULL);
	m_items.append(item);
}

void Slide::removeItem(AbstractItem *item)
{
	assert(item != NULL);
	m_items.removeAll(item);
}

bool Slide::fromXml(QDomElement & pe)
{
	qDeleteAll(m_items);
	m_items.clear();

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
		
	
// 		// restore the background element of the desk
// 		if (element.firstChildElement("set-as-background").isElement()) 
// 		{
// 			if (desk->m_backContent) {
// 				qWarning("XmlRead::readContent: only 1 element with <set-as-background/> allowed");
// 				continue;
// 			}
// 			desk->setBackContent(content);
// 		}
	}
	
	return true;
}

void Slide::toXml(QDomElement & pe, QDomDocument & doc) const
{
	foreach (AbstractItem * content, m_items) 
	{
		QDomElement element = doc.createElement("renamed-element");
		pe.appendChild(element);
		content->toXml(element);
	
// 		// add a flag to the background element
// 		if (desk->m_backContent == content) 
// 		{
// 			QDomElement bgEl = doc.createElement("set-as-background");
// 			element.appendChild(bgEl);
// 		}
	}

}
