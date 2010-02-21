#ifndef VIDEOSLIDEGROUP_H
#define VIDEOSLIDEGROUP_H

#include "model/SlideGroup.h"
//#include "NativeViewerPhonon.h"

#include <QHash>
#include <QPixmap>

/// \brief: VideolideGroup represents a Video file in DViz.
class VideoSlideGroup : public SlideGroup
{
private:
	Q_OBJECT 
	Q_PROPERTY(QString file READ file WRITE setFile);

public:
	VideoSlideGroup();
	
	typedef enum { GroupType = 4 };
	int groupType() const { return GroupType; }	
	
	QString file() { return m_file; }
	void setFile(const QString &);
	
	// SlideGroup::
	virtual bool fromXml(QDomElement & parentElement);
	virtual void toXml(QDomElement & parentElement) const;

	void changeBackground(AbstractVisualItem::FillType, QVariant, Slide *);

protected:
	void loadFile();
	void removeAllSlides();
	
	// SlideGroup::
	void fromVariantMap(QVariantMap &);
	void   toVariantMap(QVariantMap &) const;
	
protected slots:
	void aspectRatioChanged(double x);

private:
	QString m_file;
	int m_mtime;

	QHash<int,QPixmap> m_slidePixmapCache;
};
Q_DECLARE_METATYPE(VideoSlideGroup*);

#endif
