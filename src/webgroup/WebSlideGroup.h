#ifndef WebSlideGroup_H
#define WebSlideGroup_H

#include "model/SlideGroup.h"

class NativeViewerWebKit;

#include <QHash>
#include <QPixmap>
#include <QUrl>

/// \brief: VideolideGroup represents a Video file in DViz.
class WebSlideGroup : public SlideGroup
{
private:
	Q_OBJECT 
	Q_PROPERTY(QUrl url READ url WRITE setUrl);

public:
	WebSlideGroup();
	
	typedef enum { GroupType = 6 };
	int groupType() const { return GroupType; }	
	
	QUrl url() { return m_url; }
	void setUrl(const QUrl &);
	
	QString cacheFile();
	
	// SlideGroup::
	virtual bool fromXml(QDomElement & parentElement);
	virtual void toXml(QDomElement & parentElement) const;

	void changeBackground(AbstractVisualItem::FillType, QVariant, Slide *);

	NativeViewerWebKit * nativeViewer() { return m_native; }
	void setNativeViewer(NativeViewerWebKit*);

protected:
	void createSnapshot();
	void removeAllSlides();
	
	// SlideGroup::
	void fromVariantMap(QVariantMap &);
	void   toVariantMap(QVariantMap &) const;
	
protected slots:
	void aspectRatioChanged(double x);

private:
	QUrl m_url;
	int m_mtime;

	QHash<int,QPixmap> m_slidePixmapCache;
	NativeViewerWebKit * m_native;
};
Q_DECLARE_METATYPE(WebSlideGroup*);

#endif
