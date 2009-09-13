#include  "TextBoxItem.h"

#include "items/TextBoxContent.h"
//#include "items/SimpleTextContent.h"

#include <QGraphicsScene>
#include <QStringList>

TextBoxItem::TextBoxItem() : TextItem() 
{
}

TextBoxItem::~TextBoxItem() {}

#include <assert.h>
AbstractContent * TextBoxItem::createDelegate(QGraphicsScene *scene)
{
 	TextBoxContent * textContent = new TextBoxContent(scene);
//	SimpleTextContent * textContent = new TextContent(scene);
	assert(textContent);
	
	AbstractContent * vi = (AbstractContent *)textContent;
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

