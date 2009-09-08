#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "AbstractVisualItem.h"

class TextItem : public AbstractVisualItem
{
	Q_OBJECT
	
	Q_PROPERTY(QString text READ text WRITE setText);
	
public:
	TextItem();
	~TextItem();
	
	QString text() { return m_text; }
	void setText(QString);
	
	// ::AbstractVisualItem
	
	// class identification
	quint32 itemClass() const { return 0x0001; }
	
	// loading/saving
	bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement) const;

private:
	QString m_text;

};


#endif
