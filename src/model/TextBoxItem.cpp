#include  "TextBoxItem.h"

#include "items/TextBoxContent.h"
//#include "items/SimpleTextContent.h"

#include <QGraphicsScene>
#include <QStringList>

TextBoxItem::TextBoxItem() : TextItem() 
{
	setFillType(AbstractVisualItem::Solid);
	setOutlineEnabled(true);
	setOutlinePen(QPen(Qt::black,1.5));
	setFillBrush(QBrush(Qt::white));
}

TextBoxItem::~TextBoxItem() {}

#include <assert.h>
AbstractContent * TextBoxItem::createDelegate(QGraphicsScene*scene,QGraphicsItem*parent)
{
 	TextBoxContent * textContent = new TextBoxContent(scene,parent);
//	SimpleTextContent * textContent = new TextContent(scene);
	assert(textContent);
	
//	AbstractContent * vi = (AbstractContent *)textContent;
	//printf("Debug: itemId: %d\n", vi->itemId());
	
	textContent->syncFromModelItem(this);
	return textContent;
}

void TextBoxItem::toXml(QDomElement & pe) const
{
	TextItem::toXml(pe);
	
	// Save general item properties
	pe.setTagName("textbox");
}

