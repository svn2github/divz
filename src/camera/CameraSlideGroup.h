#ifndef CAMERASLIDEGROUP_H
#define CAMERASLIDEGROUP_H

#include "model/SlideGroup.h"
// #include "NativeViewerPhonon.h"
#include "NativeViewerCamera.h"

#include <QHash>
#include <QPixmap>

/// \brief: CameraSlideGroup represents a Camera object in DViz.
class CameraSlideGroup : public SlideGroup
{
private:
	Q_OBJECT 
	Q_PROPERTY(QString device READ device WRITE setDevice);

public:
	CameraSlideGroup();
	
	typedef enum { GroupType = 6 };
	int groupType() const { return GroupType; }	
	
	QString device() { return m_device; }
	void setDevice(const QString &);
	
	// SlideGroup::
	virtual bool fromXml(QDomElement & parentElement);
	virtual void toXml(QDomElement & parentElement) const;

	void changeBackground(AbstractVisualItem::FillType, QVariant, Slide *);

	NativeViewerCamera * nativeViewer(int outputId) { return m_viewers[outputId]; }
	void addNativeViewer(int outputId, NativeViewerCamera* viewer) { m_viewers[outputId]=viewer; }
	void removeNativeViewer(int outputId) { m_viewers.remove(outputId); }
	QHash<int,NativeViewerCamera*> nativeViewers() { return m_viewers; }

protected:
	void loadFile();
	void removeAllSlides();
	
	// SlideGroup::
	void fromVariantMap(QVariantMap &);
	void   toVariantMap(QVariantMap &) const;
	
protected slots:
	void aspectRatioChanged(double x);

private:
	QString m_device;
	int m_mtime;

	QHash<int,QPixmap> m_slidePixmapCache;
	//NativeViewerPhonon * m_native;
	QHash<int,NativeViewerCamera*> m_viewers;
};
Q_DECLARE_METATYPE(CameraSlideGroup*);

#endif
