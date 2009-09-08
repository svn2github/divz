#include  "TextItem.h"

TextItem::TextItem() {}
TextItem::~TextItem() {}

ITEM_PROPSET(TextItem, Text, QString, text);

bool TextItem::fromXml(QDomElement & pe)
{
	setBeingLoaded(true);
	
	// restore content properties
	QString text = pe.firstChildElement("text").text();
	setText(text);
	
	setBeingLoaded(false);
	
	AbstractVisualItem::fromXml(pe);
	
	return true;
}

void TextItem::toXml(QDomElement & pe) const
{
	AbstractVisualItem::toXml(pe);
	
	// Save general item properties
	pe.setTagName("text");
	QDomDocument doc = pe.ownerDocument();
	QDomElement domElement;
	QDomText text;
	QString valueStr;
	
	domElement= doc.createElement("text");
	pe.appendChild(domElement);
	text = doc.createTextNode(m_text);
	domElement.appendChild(text);
}

