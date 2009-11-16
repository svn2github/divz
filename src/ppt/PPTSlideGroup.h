#ifndef PPTSLIDEGROUP_H
#define PPTSLIDEGROUP_H

#include "model/SlideGroup.h"
#include "NativeViewerWin32PPT.h"

#include <QHash>
#include <QPixmap>

/// \brief: PPTSlideGroup represents a PowerPoint file in DViz.
class PPTSlideGroup : public SlideGroup
{
private:
	Q_OBJECT 
	Q_PROPERTY(QString file READ file WRITE setFile);

public:
	PPTSlideGroup();
	
	typedef enum { GroupType = 3 };
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
Q_DECLARE_METATYPE(PPTSlideGroup*);

#endif
