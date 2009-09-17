#ifndef VIDEOFILEITEM_H
#define VIDEOFILEITEM_H

#include "AbstractVisualItem.h"


class VideoFileItem : public AbstractVisualItem
{
	Q_OBJECT
	
public:
        VideoFileItem();
        ~VideoFileItem();

        QString filename() { return m_filename; }
        void setFilename(QString);
	
	// ::AbstractVisualItem
	AbstractContent * createDelegate(QGraphicsScene*);
	
	// ::AbstractItem
	
	// class identification
        quint32 itemClass() const { return 0x0004; }
	
	// loading/saving
        bool fromXml(QDomElement & parentElement);
        void toXml(QDomElement & parentElement) const;

protected:
        QString m_filename;
};


#endif
