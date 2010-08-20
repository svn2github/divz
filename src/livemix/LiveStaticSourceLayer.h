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
public:
	LiveStaticSourceLayer(QObject *parent=0);
	~LiveStaticSourceLayer();
	
	virtual QString typeName() { return "Static Source"; }

	// Used by MainWindow to setup the property editor for this layer
	virtual QList<QtPropertyEditorIdPair> createPropertyEditors(QtVariantPropertyManager *manager);

public slots:
	// Set a property (emits instancePropertyChanged)
	virtual void setInstanceProperty(const QString&, const QVariant&);

protected:
	virtual GLDrawable *createDrawable(GLWidget *);
	virtual void setupInstanceProperties(GLDrawable*);
	
	GLVideoDrawable *videoDrawable() { return m_videoDrawable; }
	
private:
	GLVideoDrawable *m_videoDrawable;
	StaticVideoSource *m_staticSource;
};

#endif
