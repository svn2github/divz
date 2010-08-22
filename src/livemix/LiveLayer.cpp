
#include "LiveLayer.h"

#include "../glvidtex/GLDrawable.h"
#include "../glvidtex/GLWidget.h"

LiveLayerProperty::LiveLayerProperty(
		const QString&  _id, 
		const QVariant& _value, 
		const QString&  _title, 
		double _min, 
		double _max)
	: id(_id) 
	, title(_title.isEmpty() ? LiveLayer::guessTitle(_id) : _title)
	, value(_value)
	, min(_min)
	, max(_max)
{}


// Translated from a perl function I wrote to do basically
// the same thing for an ERP project a few years back.
QString LiveLayer::guessTitle(QString field)
{
	static QRegExp rUpperCase = QRegExp("([a-z])([A-Z])");
	static QRegExp rFirstLetter = QRegExp("([a-z])");
	static QRegExp rLetterNumber = QRegExp("([a-z])([0-9])");
	//static QRegExp rUnderScore 
	//$name =~ s/([a-z])_([a-z])/$1.' '.uc($2)/segi;
	
	QString tmp = field;
	tmp.replace(rUpperCase,"\\1 \\2");
	if(tmp.indexOf(rFirstLetter) == 0)
	{
		QChar x = tmp.at(0);
		tmp.remove(0,1);
		tmp.prepend(QString(x).toUpper());
	}
	
	tmp.replace(rLetterNumber,"\\1 #\\2");
	
	return tmp;
}



///////////////////////

LiveLayer::LiveLayer(QObject *parent)
	: QObject(parent)
{
}

LiveLayer::~LiveLayer()
{}

// Returns a GLDrawable for the specified GLWidget. If none exists,
// then calls createDrawable() internally, followed by initDrawable()
GLDrawable* LiveLayer::drawable(GLWidget *widget)
{
	if(m_drawables.contains(widget))
	{
// 		qDebug() << "LiveLayer::drawable: widget:"<<widget<<", cache hit";
		return m_drawables[widget];
	}
	else
	{
		GLDrawable *drawable = createDrawable(widget);
		
		bool wasEmpty = m_drawables.isEmpty();
		m_drawables[widget] = drawable;
		
		if(wasEmpty)
		{
// 			qDebug() << "LiveLayer::drawable: widget:"<<widget<<", cache miss, first drawable";
			initDrawable(drawable, true);
		}
		else
		{
// 			qDebug() << "LiveLayer::drawable: widget:"<<widget<<", cache miss, copy from first";
			initDrawable(drawable, false);
		}
		
		if(widget->property("isEditorWidget").toInt())
			drawable->show();
		else
			connect(this, SIGNAL(isVisible(bool)), drawable, SLOT(setVisible(bool)));
		
		return drawable;
	}
}

void LiveLayer::setupLayerPropertyEditors(QtVariantPropertyManager */*manager*/)
{
	/// TODO
}

void LiveLayer::setVisible(bool flag)
{
	// Implemented using sig/slot instead of just calling each if the drawables setVisible
	// directly so that it can be conditionally connected based on the GLWidget that
	// the drawable is connected to - see the connect() statement in LiveLayer::drawable()	
	emit isVisible(flag);
	m_isVisible = flag;
}

void LiveLayer::setRect(const QRectF& rect) 	{ setLayerProperty("rect", rect); }
void LiveLayer::setZIndex(double z)	 	{ setLayerProperty("zIndex", z);  }
void LiveLayer::setOpacity(double o)		{ setLayerProperty("opacity", o); }

// Internally, tries to set the named property on all the drawables if it has such a property
// If no prop exists on the drawable, then tries to set the prop on the layer object
// Either way, sets the prop in m_props and emits layerPropertyChanged() 
// This can be overridden to catch property changes internally, since the property editors 
// in MainWindow use this slot to update the props. Most of the time, this will Do The Right Thing
// for basic properties, but for some custom ones you might have to intercept it.
void LiveLayer::setLayerProperty(const QString& propertyId, const QVariant& value)
{
	if(!m_props.contains(propertyId))
		return;
		
	QVariant oldValue = m_props[propertyId].value;
	m_props[propertyId].value = value;
	
	if(value != oldValue)
		layerPropertyWasChanged(propertyId, value, oldValue);
	
	if(m_drawables.isEmpty())
		return;
		
	GLDrawable *drawable = m_drawables[m_drawables.keys().first()];
	
	const char *asciiPropId = qPrintable(propertyId);
	if(drawable->metaObject()->indexOfProperty(asciiPropId) >= 0)
	{
		foreach(GLWidget *widget, m_drawables.keys())
		{
			m_drawables[widget]->setProperty(asciiPropId, value);
		}
	}
	else
	if(metaObject()->indexOfProperty(asciiPropId) >= 0)
	{
		setProperty(asciiPropId, value);
	}
}

void LiveLayer::setInstanceName(const QString& name)
{
	m_instanceName = name;
	emit instanceNameChanged(name);
}

// just emits layerPropertyChanged
void LiveLayer::layerPropertyWasChanged(const QString& propertyId, const QVariant& value, const QVariant& oldValue)
{
	emit layerPropertyChanged(propertyId, value, oldValue);
}

// The core of the layer - create a new drawable instance for the specified context.
// drawable() will call initDrawable() on it to set it up as needed
GLDrawable *LiveLayer::createDrawable(GLWidget */*widget*/)
{
 	qDebug() << "LiveLayer::createDrawable: Nothing created.";
	return 0;
}

// If its the first drawable, setup with defaults and load m_props[] with appros values
// If not first drawable, load drawable with values from m_props[]
void LiveLayer::initDrawable(GLDrawable *drawable, bool isFirstDrawable)
{
// 	qDebug() << "LiveLayer::initDrawable: drawable:"<<drawable<<", copyFrom:"<<copyFrom;
	if(isFirstDrawable)
	{
		loadLayerPropertiesFromObject(drawable, QStringList() 
			<< "rect"
			<< "zIndex"
			<< "opacity");
			
	}
	else
	{
		applyLayerPropertiesToObject(drawable, QStringList()
			<< "rect"
			<< "zIndex"
			<< "opacity");
	}
}


// Helper function - given a list of property names, load the props using QObject::property() from the drawable into m_props
void LiveLayer::loadLayerPropertiesFromObject(const QObject *object, const QStringList& list)
{
	foreach(QString key, list)
	{
		QVariant value = object->property(qPrintable(key));
		m_props[key] = LiveLayerProperty(key,value);
	}
}

// Helper function - attempts to apply the list of props in 'list' (or all props in m_props if 'list' is empty) to drawable
// 'Attempts' means that if drawable->metaObject()->indexOfProperty() returns <0, then setProperty is not called
void LiveLayer::applyLayerPropertiesToObject(QObject *object, QStringList list)
{
	if(list.isEmpty())
		foreach(QString key, m_props.keys())
			list << key;
	
	foreach(QString key, list)
	{
		if(m_props.contains(key))
		{
			const char *asciiPropId = qPrintable(key);
			if(object->metaObject()->indexOfProperty(asciiPropId) >= 0)
			{
				object->setProperty(asciiPropId, m_props[key].value);
			}
		}
	}
	
}
