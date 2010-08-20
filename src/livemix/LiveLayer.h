#ifndef LiveLayer_H
#define LiveLayer_H


#include <QtGui>
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"

class GLDrawable;
class GLWidget;


class LiveLayer : public QObject
{
	Q_OBJECT
public:
	LiveLayer(QObject *parent=0);
	virtual ~LiveLayer();
	
	virtual QString typeName() { return "Generic Layer"; }
	virtual QString instanceName() { return m_instanceName; }
	
	// editorDrawable provides a hint to the setup routines on whether or not to sync visibility
	GLDrawable * drawable(GLWidget *widget);
	
signals:
	void isVisible(bool);

public slots:
	void setIsVisible(bool);
	
protected:
	void changeInstanceName(const QString&);
	
	virtual GLDrawable *createDrawable(GLWidget *widget);
	// If its the first drawable, setup with defaults
	// Otherwise, copy from 'copyFrom'
	virtual void initDrawable(GLDrawable *newDrawable, GLDrawable *copyFrom=0);
	
	QString m_instanceName;
	
	QHash<GLWidget*, GLDrawable*> m_drawables;
};

#endif
