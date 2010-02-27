#ifndef VIDEOSLIDEGROUP_H
#define VIDEOSLIDEGROUP_H

#include "model/SlideGroup.h"
#include "NativeViewerPhonon.h"

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
	
	typedef enum { GroupType = 5 };
	int groupType() const { return GroupType; }	
	
	QString file() { return m_file; }
	void setFile(const QString &);
	
	// SlideGroup::
	virtual bool fromXml(QDomElement & parentElement);
	virtual void toXml(QDomElement & parentElement) const;

	void changeBackground(AbstractVisualItem::FillType, QVariant, Slide *);

	NativeViewerPhonon * nativeViewer(int outputId) { return m_viewers[outputId]; }
	void addNativeViewer(int outputId, NativeViewerPhonon* viewer) { m_viewers[outputId]=viewer; }
	void removeNativeViewer(int outputId) { m_viewers.remove(outputId); }
	QHash<int,NativeViewerPhonon*> nativeViewers() { return m_viewers; }

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
	//NativeViewerPhonon * m_native;
	QHash<int,NativeViewerPhonon*> m_viewers;
};
Q_DECLARE_METATYPE(VideoSlideGroup*);

#endif
