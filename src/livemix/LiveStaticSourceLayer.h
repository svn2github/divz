#ifndef LiveStaticSourceLayer_H
#define LiveStaticSourceLayer_H

#include "LiveLayer.h"

class GLVideoDrawable ;
class GLDrawable;
class GLWidget;
class StaticVideoSource;



class LiveStaticSourceLayer : public LiveLayer
{
	Q_OBJECT
	
	Q_PROPERTY(QString file READ file WRITE setFile)
	
public:
	LiveStaticSourceLayer(QObject *parent=0);
	~LiveStaticSourceLayer();
	
	virtual QString typeName() { return "Static Source"; }

// 	// Used by MainWindow to setup the property editor for this layer
// 	virtual QList<QtPropertyEditorIdPair> createPropertyEditors(QtVariantPropertyManager *manager);

	QString file() { return layerPropertyValue("file").toString(); }

public slots:
// 	// Set a property (emits instancePropertyChanged)
 	virtual void setLayerProperty(const QString& propertyId, const QVariant& value);

	void setFile(const QString&);

protected:
/*	virtual GLDrawable *createDrawable(GLWidget *);
	virtual void setupInstanceProperties(GLDrawable*);*/
	
	virtual GLDrawable *createDrawable(GLWidget *widget);
	// If its the first drawable, setup with defaults
	// Otherwise, copy from 'copyFrom'
	virtual void initDrawable(GLDrawable *drawable, bool isFirstDrawable = false);
	
// 	GLVideoDrawable *videoDrawable() { return m_videoDrawable; }
	
private:
// 	GLVideoDrawable *m_videoDrawable;
	StaticVideoSource *m_staticSource;
};

#endif
