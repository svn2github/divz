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
	
// 	Q_PROPERTY(bool	   	outlineEnabled 	READ outlineEnabled 	WRITE setOutlineEnabled);
// 	Q_PROPERTY(QColor	outlineColor 	READ outlineColor	WRITE setOutlineColor);
// 	Q_PROPERTY(double	outlineWidth	READ outlineWidth	WRITE setOutlineWidth);
// 	
// 	Q_PROPERTY(bool		fillEnabled	READ fillEnabled	WRITE setFillEnabled);
// 	Q_PROPERTY(QColor	fillColor	READ fillColor		WRITE setFillColor);
// 	
// 	Q_PROPERTY(bool		shadowEnabled	READ shadowEnabled	WRITE setShadowEnabled);
// 	Q_PROPERTY(double 	shadowBlurRadius READ shadowBlurRadius	WRITE setShadowBlurRadius);
// 	Q_PROPERTY(double	shadowOffsetX 	READ shadowOffsetX	WRITE setShadowOffsetX);
// 	Q_PROPERTY(double	shadowOffsetY 	READ shadowOffsetY	WRITE setShadowOffsetY);
// 	Q_PROPERTY(QColor	shadowColor	READ shadowColor	WRITE setShadowColor);

	
public:
	Q_INVOKABLE LiveTextLayer(QObject *parent=0);
	~LiveTextLayer();
	
	virtual QString typeName() { return "Text Layer"; }
	
	QString text() { return layerProperty("text").toString(); }
	
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
