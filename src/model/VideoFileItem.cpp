#include  "VideoFileItem.h"

#include "items/VideoFileContent.h"

#include <QGraphicsScene>
#include <QStringList>

VideoFileItem::VideoFileItem() : AbstractVisualItem()
{
}

VideoFileItem::~VideoFileItem() {}

ITEM_PROPSET(VideoFileItem, Filename, QString, filename)

#include <assert.h>
AbstractContent * VideoFileItem::createDelegate(QGraphicsScene *scene)
{
        VideoFileContent * delegate = new VideoFileContent(scene);
        assert(delegate);
        delegate->syncFromModelItem(this);
        return delegate;
}

bool VideoFileItem::fromXml(QDomElement & pe)
{
        setBeingLoaded(true);

        // restore content properties
        QString text = pe.firstChildElement("filename").text();
        setFilename(text);

        setBeingLoaded(false);

        AbstractVisualItem::fromXml(pe);

        return true;
}

void VideoFileItem::toXml(QDomElement & pe) const
{
	AbstractVisualItem::toXml(pe);
	
	// Save general item properties
        pe.setTagName("videofile");

        QDomDocument doc = pe.ownerDocument();
        QDomElement domElement;
        QDomText text;
        QString valueStr;

        domElement= doc.createElement("filename");
        pe.appendChild(domElement);
        text = doc.createTextNode(m_filename);
        domElement.appendChild(text);
}

