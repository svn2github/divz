#ifndef LiveLayer_H
#define LiveLayer_H


#include <QtGui>
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"

class GLDrawable;
class GLWidget;

class LiveLayerProperty
{
public:
	LiveLayerProperty() { min=0; max=0; }
	LiveLayerProperty(
			const QString&  _id, 
			const QVariant& _value, 
			const QString&  _title="", 
			double _min=-100, 
			double _max=100);
	
	QString id;
	QString title;
	QVariant value;
	
	// For numerical properties
	double min, max;
};

typedef QHash<QString, LiveLayerProperty> LiveLayerPropertyHash;

class LiveLayer : public QObject
{
	Q_OBJECT
	
	Q_PROPERTY(bool isVisible READ isVisible WRITE setVisible);
	Q_PROPERTY(QRectF rect READ rect WRITE setRect);
	Q_PROPERTY(double zIndex READ zIndex WRITE setZIndex);
	Q_PROPERTY(double opacity READ opacity WRITE setOpacity);
	
public:
	LiveLayer(QObject *parent=0);
	virtual ~LiveLayer();
	
	// Override and return a descritive static type name for your layer
	virtual QString typeName() { return "Generic Layer"; }
	// Use "setInstanceName to change the name here, which emits instanceNameChanged()
	virtual QString instanceName() { return m_instanceName; }
	
	// Returns a GLDrawable for the specified GLWidget. If none exists,
	// then calls createDrawable() internally, followed by initDrawable()
	virtual GLDrawable * drawable(GLWidget *widget);
	
	// Query for layer properties
	virtual LiveLayerPropertyHash layerProperties() { return m_props; }
	virtual LiveLayerProperty layerProperty(const QString& id) { return m_props[id]; }
	virtual QVariant layerPropertyValue(const QString& id) { return m_props[id].value; }
	
	// Default impl iterates thru m_props and sets up appropriate editors
	virtual void setupLayerPropertyEditors(QtVariantPropertyManager*);
	
	// Translated from a perl function I wrote to do basically
	// the same thing for an ERP project a few years back.
	static QString guessTitle(QString field);
	
	bool isVisible() 	{ return m_isVisible; }
	QRectF rect()		{ return layerPropertyValue("rect").toRectF(); }
	double zIndex()		{ return layerPropertyValue("zIndex").toDouble(); }
	double opacity()	{ return layerPropertyValue("opacity").toDouble(); }

signals:
	void isVisible(bool);
	
	// emitted when a property is changed in this layer
	void layerPropertyChanged(const QString& propertyId, const QVariant& value, const QVariant& oldValue);
	
	// emitted by setInstanceName()
	void instanceNameChanged(const QString&);

public slots:
	void setVisible(bool);
	
	void setRect(const QRectF&);
	void setZIndex(double z);
	void setOpacity(double o);
	
	// Internally, tries to set the named property on all the drawables if it has such a property
	// If no prop exists on the drawable, then tries to set the prop on the layer object
	// Either way, sets the prop in m_props and emits layerPropertyChanged() 
	// This can be overridden to catch property changes internally, since the property editors 
	// in MainWindow use this slot to update the props. Most of the time, this will Do The Right Thing
	// for basic properties, but for some custom ones you might have to intercept it.
	virtual void setLayerProperty(const QString& propertyId, const QVariant& value);
	
protected:
	void setInstanceName(const QString&);
	
	// just emits layerPropertyChanged
	void layerPropertyWasChanged(const QString& propertyId, const QVariant& value, const QVariant& oldValue);
	
	// The core of the layer - create a new drawable instance for the specified context.
	// drawable() will call initDrawable() on it to set it up as needed
	virtual GLDrawable *createDrawable(GLWidget *widget);
	
	// If its the first drawable, setup with defaults and load m_props[] with appros values
	// If not first drawable, load drawable with values from m_props[]
	virtual void initDrawable(GLDrawable *drawable, bool isFirstDrawable = false);
	
	// Helper function - given a list of property names, load the props using QObject::property() from the drawable into m_props
	void loadLayerPropertiesFromObject(const QObject *, const QStringList& list);
	
	// Helper function - attempts to apply the list of props in 'list' (or all props in m_props if 'list' is empty) to drawable
	// 'Attempts' means that if drawable->metaObject()->indexOfProperty() returns <0, then setProperty is not called
	void applyLayerPropertiesToObject(QObject *object, QStringList list = QStringList());
	
	// "pretty" name for this instance, like "SeasonsLoop3.mpg" or some other content-identifying string
	QString m_instanceName;
	
	// Maintained as a member var rather than in m_props due to different method of setting the value
	bool m_isVisible;
	
	// Cache for the drawables for this layer
	QHash<GLWidget*, GLDrawable*> m_drawables;
	
	// The properties for this layer - to properly set in sub-classes, use setProperty
	LiveLayerPropertyHash m_props;
};

#endif
