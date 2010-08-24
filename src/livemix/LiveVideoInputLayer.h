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
	
	Q_PROPERTY(int brightness READ brightness WRITE setBrightness);
	Q_PROPERTY(int contrast READ contrast WRITE setContrast);
	Q_PROPERTY(int hue READ hue WRITE setHue);
	Q_PROPERTY(int saturation READ saturation WRITE setSaturation);
	
	Q_PROPERTY(bool flipHorizontal READ flipHorizontal WRITE setFlipHorizontal);
	Q_PROPERTY(bool flipVertical READ flipVertical WRITE setFlipVertical);
	
	Q_PROPERTY(QPointF cropTopLeft READ cropTopLeft WRITE setCropTopLeft);
	Q_PROPERTY(QPointF cropBottomRight READ cropBottomRight WRITE setCropBottomRight);
	
	Q_PROPERTY(QPointF textureOffset READ textureOffset WRITE setTextureOffset);
	
	Q_PROPERTY(QString alphaMaskFile READ alphaMaskFile WRITE setAlphaMaskFile);
	Q_PROPERTY(QString overlayMaskFile READ overlayMaskFile WRITE setOverlayMaskFile);
	
	Q_PROPERTY(Qt::AspectRatioMode aspectRatioMode READ aspectRatioMode WRITE setAspectRatioMode);
	
public:
	LiveVideoInputLayer(QObject *parent=0);
	~LiveVideoInputLayer();
	
	virtual QString typeName() { return "Video Input"; }
	
	bool deinterlace() { return layerPropertyValue("deinterlace").toBool(); }
	
	int brightness() { return layerPropertyValue("brightness").toInt(); }
	int contrast() { return layerPropertyValue("contrast").toInt(); }
	int hue() { return layerPropertyValue("hue").toInt(); }
	int saturation() { return layerPropertyValue("saturation").toInt(); }
	
	bool flipHorizontal() { return layerPropertyValue("flipHorizontal").toBool(); }
	bool flipVertical() { return layerPropertyValue("flipVertical").toBool(); }
	
	QPointF cropTopLeft() { return layerPropertyValue("cropTopLeft").toPointF(); }
	QPointF cropBottomRight() { return layerPropertyValue("cropBottomRight").toPointF(); }
	
	QPointF textureOffset() { return layerPropertyValue("textureOffset").toPointF(); }
	
	QString alphaMaskFile() { return layerPropertyValue("alphaMaskFile").toString(); }
	QString overlayMaskFile() { return layerPropertyValue("overlayMaskFile").toString(); }
	
	Qt::AspectRatioMode aspectRatioMode() { return (Qt::AspectRatioMode)layerPropertyValue("aspectRatioMode").toInt(); }


public slots:
	// Set a property (emits instancePropertyChanged)
// 	virtual void setInstanceProperty(const QString&, const QVariant&);
	void setDeinterlace(bool);
	
	void setBrightness(int value) 	{ setLayerProperty("brightness", value); }
	void setContrast(int value) 	{ setLayerProperty("contrast", value); }
	void setHue(int value) 		{ setLayerProperty("hue", value); }
	void setSaturation(int value) 	{ setLayerProperty("saturation", value); }
	
	void setFlipHorizontal(bool value) 	{ setLayerProperty("flipHorizontal", value); }
	void setFlipVertical(bool value) 	{ setLayerProperty("flipVertical", value); }
	
	void setCropTopLeft(const QPointF &value)	{ setLayerProperty("cropTopLeft", value); }
	void setCropBottomRight(const QPointF &value)	{ setLayerProperty("cropBottomRight", value); }
	
	void setTextureOffset(const QPointF &value)	{ setLayerProperty("textureOffset", value); }
	
	void setAlphaMaskFile(const QString &value)	{ setLayerProperty("alphaMaskFile", value); }
	void setOverlayMaskFile(const QString &value)	{ setLayerProperty("overlayMaskFile", value); }
	
	void setAspectRatioMode(Qt::AspectRatioMode value)	{ setLayerProperty("aspectRatioMode", (int)value); }
	void setAspectRatioMode(int value)			{ setLayerProperty("aspectRatioMode", value); }
	
	
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
