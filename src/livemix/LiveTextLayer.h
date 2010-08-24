#ifndef LiveTextLayer_H
#define LiveTextLayer_H

#include "LiveLayer.h"

class GLVideoDrawable ;
class GLDrawable;
class GLWidget;
class TextVideoSource;

class LiveTextLayer  : public LiveLayer
{
	Q_OBJECT
	
	Q_PROPERTY(QString text READ text WRITE setText);
	
public:
	LiveTextLayer(QObject *parent=0);
	~LiveTextLayer();
	
	virtual QString typeName() { return "Text Layer"; }
	
	QString text() { return layerPropertyValue("text").toString(); }
	
// 	// Used by MainWindow to setup the property editor for this layer
// 	virtual QList<QtPropertyEditorIdPair> createPropertyEditors(QtVariantPropertyManager *manager);

public slots:
	// Calls setInstanceProperty internally
	void setText(const QString& text);
	
	// Set a property (emits instancePropertyChanged)
	virtual void setLayerProperty(const QString& propertyId, const QVariant& value);

protected:
	virtual GLDrawable *createDrawable(GLWidget *);
	virtual void initDrawable(GLDrawable *drawable, bool isFirstDrawable = false);
	virtual QWidget *createLayerPropertyEditors();
	
private:
	TextVideoSource *m_textSource;
	QString m_text; 
};

#endif
