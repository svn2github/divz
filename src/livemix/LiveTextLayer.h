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
public:
	LiveTextLayer(QObject *parent=0);
	~LiveTextLayer();
	
	virtual QString typeName() { return "Text Layer"; }
	
	QString text() { return m_text; }
	
	// Used by MainWindow to setup the property editor for this layer
	virtual QList<QtPropertyEditorIdPair> createPropertyEditors(QtVariantPropertyManager *manager);

public slots:
	// Calls setInstanceProperty internally
	void setText(const QString& text);
	
	// Set a property (emits instancePropertyChanged)
	virtual void setInstanceProperty(const QString&, const QVariant&);

protected:
	virtual GLDrawable *createDrawable(GLWidget *);
	virtual void setupInstanceProperties(GLDrawable*);
	
	GLVideoDrawable *videoDrawable() { return m_videoDrawable; }
	
private:
	GLVideoDrawable *m_videoDrawable;
	TextVideoSource *m_textSource;
	QString m_text; 
};

#endif
