#ifndef LiveVideoInputLayer_H
#define LiveVideoInputLayer_H

#include "LiveLayer.h"

class GLVideoDrawable ;
class CameraThread;
class GLDrawable;
class GLWidget;
class VideoSource;

class LiveVideoInputLayer : public LiveLayer
{
	Q_OBJECT
	
	Q_PROPERTY(bool deinterlace READ deinterlace WRITE setDeinterlace);
	
public:
	LiveVideoInputLayer(QObject *parent=0);
	~LiveVideoInputLayer();
	
	virtual QString typeName() { return "Video Input"; }
	
	bool deinterlace() { return layerPropertyValue("deinterlace").toBool(); }

public slots:
	// Set a property (emits instancePropertyChanged)
// 	virtual void setInstanceProperty(const QString&, const QVariant&);
	void setDeinterlace(bool);
	
	virtual void setLayerProperty(const QString& propertyId, const QVariant& value);

signals:
	void videoSourceChanged(VideoSource*);
	
protected:
	virtual GLDrawable *createDrawable(GLWidget *widget);
	// If its the first drawable, setup with defaults
	// Otherwise, copy from 'copyFrom'
	virtual void initDrawable(GLDrawable *drawable, bool isFirstDrawable = false);
	virtual QWidget *createLayerPropertyEditors();
	
	void setCamera(CameraThread*);
	CameraThread *camera() { return m_camera; }
	
private:
	CameraThread *m_camera;
};

#endif
