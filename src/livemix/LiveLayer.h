#ifndef LiveLayer_H
#define LiveLayer_H


#include <QtGui>

#include "../glvidtex/GLDrawable.h"
class GLWidget;

class QSpinBox;
class QDoubleSpinBox;

class DoubleEditorWidget : public QWidget
{
	Q_OBJECT
public:
	DoubleEditorWidget(QWidget *parent=0);

public slots:
	void setValue(double);
	void setMinMax(double,double);
	void setShowSlider(bool);
	void setSuffix(const QString&);

signals:
	void valueChanged(double);

private slots:
	void sliderValueChanged(int);
	void boxValueChanged(double);

private:
	double m_value;
	QSlider *m_slider;
	QDoubleSpinBox *m_box;

};

class PointEditorWidget : public QWidget
{
	Q_OBJECT
public:
	PointEditorWidget(QWidget *parent=0);

public slots:
	void setValue(const QPointF&);
	void setXMinMax(int,int);
	void setYMinMax(int,int);
	void setSufix(const QString&);
	
	void reset();

signals:
	void valueChanged(const QPointF&);

private slots:
	void xValueChanged(int);
	void yValueChanged(int);

private:
	QPointF m_point;
	QSpinBox *x_box;
	QSpinBox *y_box;
	QPointF m_orig;

};

class SizeEditorWidget : public QWidget
{
	Q_OBJECT
public:
	SizeEditorWidget(QWidget *parent=0);

public slots:
	void setValue(const QSizeF&);
	void setWMinMax(int,int);
	void setHMinMax(int,int);
	void setSufix(const QString&);
	
	void reset();

signals:
	void valueChanged(const QSizeF&);

private slots:
	void wValueChanged(int);
	void hValueChanged(int);


private:
	QSizeF m_size;
	QSpinBox *w_box;
	QSpinBox *h_box;
	QSizeF m_orig;
};
/*
class ColorEditorWidget : public QWidget
{
	Q_OBJECT
public:
	ColorEditorWidget(QWidget *parent=0);

public slots:
	void setValue(const QColor&);

signals:
	void valueChanged(const QColor&);

private slots:
	void rValueChanged(int);
	void gValueChanged(int);
	void bValueChanged(int);

private:
	QColor m_point;
	QSpinBox *r_box;
	QSpinBox *g_box;
	QSpinBox *b_box;
};*/

class LiveLayer : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int id READ id);
	
	Q_PROPERTY(bool isVisible READ isVisible WRITE setVisible);
	Q_PROPERTY(QRectF rect READ rect WRITE setRect);
	Q_PROPERTY(double zIndex READ zIndex WRITE setZIndex);
	Q_PROPERTY(double opacity READ opacity WRITE setOpacity);

	Q_PROPERTY(bool fadeIn READ fadeIn WRITE setFadeIn);
	Q_PROPERTY(int fadeInLength READ fadeInLength WRITE setFadeInLength);

	Q_PROPERTY(bool fadeOut READ fadeOut WRITE setFadeOut);
	Q_PROPERTY(int  fadeOutLength READ fadeOutLength WRITE setFadeOutLength);

	Q_PROPERTY(int showAnimationType READ showAnimationType WRITE setShowAnimationType);
	Q_PROPERTY(int showAnimationLength READ showAnimationLength WRITE setShowAnimationLength);
	Q_PROPERTY(QEasingCurve::Type showAnimationCurve READ showAnimationCurve WRITE setShowAnimationCurve);

	Q_PROPERTY(int hideAnimationType READ hideAnimationType WRITE setHideAnimationType);
	Q_PROPERTY(int hideAnimationLength READ hideAnimationLength WRITE setHideAnimationLength);
	Q_PROPERTY(QEasingCurve::Type hideAnimationCurve READ hideAnimationCurve WRITE setHideAnimationCurve);

	Q_PROPERTY(bool showFullScreen READ showFullScreen WRITE setShowFullScreen);
	Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment);
	Q_PROPERTY(QPointF insetTopLeft READ insetTopLeft WRITE setInsetTopLeft);
	Q_PROPERTY(QPointF insetBottomRight READ insetBottomRight WRITE setInsetBottomRight);

	Q_PROPERTY(double alignedSizeScale READ alignedSizeScale WRITE setAlignedSizeScale);
	
	Q_PROPERTY(int hideOnShowLayerId READ hideOnShowLayerId WRITE setHideOnShowLayerId);
	Q_PROPERTY(int showOnShowLayerId READ showOnShowLayerId WRITE setShowOnShowLayerId);

public:
	LiveLayer(QObject *parent=0);
	virtual ~LiveLayer();
	
	int id();
	
	void attachGLWidget(GLWidget*);
	void detachGLWidget(GLWidget*);
	
	// Override and return a descritive static type name for your layer
	virtual QString typeName() { return "Generic Layer"; }
	// Use "setInstanceName to change the name here, which emits instanceNameChanged()
	virtual QString instanceName() { return m_instanceName; }

	// Query for layer properties
	virtual QVariantMap layerProperties() { return m_props; }
	virtual QVariant layerProperty(const QString& id) { return m_props[id]; }
	
	// Default impl iterates thru m_props and sets up appropriate editors
	// Caller takes ownership of widget and deletes when done
	virtual QWidget *createLayerPropertyEditors();
	
	virtual void fromByteArray(QByteArray&);
	virtual QByteArray toByteArray();
	
	virtual void loadPropsFromMap(const QVariantMap&, bool onlyApplyIfChanged = false);
	virtual QVariantMap propsToMap();
	
	class AnimParam
	{
	public:
		AnimParam()
			: length(300)
			, curve(QEasingCurve::Linear)
			{}
		
		int length;
		QEasingCurve curve;
	};
	
	// Set/get the currently effective animation parameters used
	// for transitioning from one property to another
	void setAnimParam(const AnimParam&);
	const AnimParam & animParam() { return m_animParam; }
	
	// Enable/disable animations. By default, animations are enabled.
	// Disable for setting properties in bulk or loading props
	bool setAnimEnabled(bool);
	bool animEnabled() { return !m_animationsDisabled; }
		

	// Translated from a perl function I wrote to do basically
	// the same thing for an ERP project a few years back.
	static QString guessTitle(QString field);

	bool isVisible() 		{ return m_isVisible; }
	QRectF rect()			{ return layerProperty("rect").toRectF(); }
	double zIndex()			{ return layerProperty("zIndex").toDouble(); }
	double opacity()		{ return layerProperty("opacity").toDouble(); }


	bool fadeIn() 			{ return layerProperty("fadeIn").toBool(); }
	int fadeInLength() 		{ return layerProperty("fadeInLength").toInt(); }

	bool fadeOut() 			{ return layerProperty("fadeOut").toBool(); }
	int fadeOutLength() 		{ return layerProperty("fadeOutLength").toInt(); }

	int showAnimationType() 	{ return layerProperty("showAnimationType").toInt(); }
	int showAnimationLength() 	{ return layerProperty("showAnimationLength").toInt(); }
	QEasingCurve::Type showAnimationCurve() 	{ return (QEasingCurve::Type)layerProperty("showAnimationCurve").toInt(); }

	int hideAnimationType() 	{ return layerProperty("hideAnimationType").toInt(); }
	int hideAnimationLength() 	{ return layerProperty("hideAnimationLength").toInt(); }
	QEasingCurve::Type hideAnimationCurve() 	{ return (QEasingCurve::Type)layerProperty("hideAnimationCurve").toInt(); }

	bool showFullScreen()		{ return layerProperty("showFullScreen").toBool(); }
	Qt::Alignment alignment()	{ return (Qt::Alignment)layerProperty("alignment").toInt(); }
	QPointF insetTopLeft()		{ return layerProperty("insetTopLeft").toPointF(); }
	QPointF insetBottomRight()	{ return layerProperty("insetBottomRight").toPointF(); }

	double alignedSizeScale()	{ return layerProperty("alignedSizeScale").toDouble(); }
	
	int showOnShowLayerId()	{ return layerProperty("showOnShowLayerId").toInt(); }
	int hideOnShowLayerId()	{ return layerProperty("hideOnShowLayerId").toInt(); }
	
	LiveLayer * hideOnShow() { return m_hideOnShow; }
	LiveLayer * showOnShow() { return m_showOnShow; }
	
	void setHideOnShow(LiveLayer *layer);
	void setShowOnShow(LiveLayer *layer);
	
	class LiveScene *scene() { return m_scene; }
	void setScene(LiveScene *scene);
	


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
	void setZIndex(int z); // for sig/slot compat
	void setOpacity(double o);
	void setOpacity(int);
	void setTransparency(int);

	void setX(int);
	void setY(int);

	void setWidth(int);
	void setHeight(int);

	void setPos(const QPointF& point)	{ setRect(QRectF(point, rect().size())); }
	void setSize(const QSizeF& size)	{ setRect(QRectF(rect().topLeft(),size)); }

	void setFadeIn(bool value) 		{ setLayerProperty("fadeIn", value); }
	void setFadeInLength(int value) 	{ setLayerProperty("fadeInLength", value); }

	void setFadeOut(bool value) 		{ setLayerProperty("fadeOut", value); }
	void setFadeOutLength(int value) 	{ setLayerProperty("fadeOutLength", value); }

	void setShowAnimationType(int value) 	{ setLayerProperty("showAnimationType",   value); }
	void setShowAnimationLength(int value) 	{ setLayerProperty("showAnimationLength", value); }
	void setShowAnimationCurve(QEasingCurve::Type value) 		{ setLayerProperty("showAnimationCurve",  value); }

	void setHideAnimationType(int value) 	{ setLayerProperty("hideAnimationType",   value); }
	void setHideAnimationLength(int value) 	{ setLayerProperty("hideAnimationLength", value); }
	void setHideAnimationCurve(QEasingCurve::Type value) 		{ setLayerProperty("hideAnimationCurve",  value); }

	void setShowFullScreen(bool value)	{ setLayerProperty("showFullScreen", value); }
	void setAlignment(Qt::Alignment value)	{ setLayerProperty("alignment", (int)value); }
	void setInsetTopLeft(QPointF value)	{ setLayerProperty("insetTopLeft", value); }
	void setInsetBottomRight(QPointF value)	{ setLayerProperty("insetBottomRight", value); }

	void setTopInset(int value)	{ setInsetTopLeft(QPointF(insetTopLeft().x(), value)); }
	void setLeftInset(int value)	{ setInsetTopLeft(QPointF(value, insetTopLeft().y())); }
	void setBottomInset(int value)	{ setInsetBottomRight(QPointF(insetBottomRight().x(), value)); }
	void setRightInset(int value)	{ setInsetBottomRight(QPointF(value, insetBottomRight().y())); }

	void setAlignedSizeScale(double value) { setLayerProperty("alignedSizeScale", value); }
	void setAlignedSizeScale(int value) { setAlignedSizeScale((double)value / 100.0); }
	
	void setShowOnShowLayerId(int);
	void setHideOnShowLayerId(int);


	// Internally, tries to set the named property on all the drawables if it has such a property
	// If no prop exists on the drawable, then tries to set the prop on the layer object
	// Either way, sets the prop in m_props and emits layerPropertyChanged()
	// This can be overridden to catch property changes internally, since the property editors
	// in MainWindow use this slot to update the props. Most of the time, this will Do The Right Thing
	// for basic properties, but for some custom ones you might have to intercept it.
	virtual void setLayerProperty(const QString& propertyId, const QVariant& value);

protected slots:
	// needed to offset the small list of options into the proper enum value
	void setShowAnim(int);
	void setHideAnim(int);

	// Translate the 'show as' combo box into a set of alignment flags and showFullScreen boolean value
	void setShowAsType(const QString&);
	
	void setShowOnShow(int);
	void setHideOnShow(int);

protected:
	// Returns a GLDrawable for the specified GLWidget. If none exists,
	// then calls createDrawable() internally, followed by initDrawable()
	virtual GLDrawable * drawable(GLWidget *widget);

	
	class PropertyEditorOptions
	{
	public:
		PropertyEditorOptions()
		{
			reset();
		};

		void reset()
		{
			text = "";
			suffix = "";
			noSlider = false;
			min = -9999;
			max =  9999;
			type = QVariant::Invalid;
			value = QVariant();
			doubleIsPercentage = false;
			defaultValue = QVariant();
			stringIsFile = false;
			fileTypeFilter = "";
		}

		QString text;
		QString suffix;
		bool noSlider;
		double min;
		double max;
		QVariant::Type type;
		QVariant value;
		bool doubleIsPercentage;
		QVariant defaultValue;
		bool stringIsFile;
		QString fileTypeFilter;
	};

	QWidget * generatePropertyEditor(QObject *object, const char *property, const char *slot, PropertyEditorOptions opts = PropertyEditorOptions());

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

	// Apply the animation properties to the drawable
	void applyAnimationProperties(GLDrawable *);

	// Helper routine to loop thru all drawables and set 'prop' to 'value'
	void applyDrawableProperty(const QString& prop, const QVariant& value);

	// "pretty" name for this instance, like "SeasonsLoop3.mpg" or some other content-identifying string
	QString m_instanceName;

	// Maintained as a member var rather than in m_props due to different method of setting the value
	bool m_isVisible;

	// Cache for the drawables for this layer
	QHash<GLWidget*, GLDrawable*> m_drawables;

	// The properties for this layer - to properly set in sub-classes, use setProperty
	QVariantMap m_props;

private:
	void setVisibleGeometryFields(QStringList list = QStringList());
	
	QFormLayout * m_geomLayout;
	QHash<QString,QWidget*> m_propWidget;
	
	QHash<QString,bool> m_propSetLock;
	
	LiveScene * m_scene;
	LiveLayer * m_hideOnShow;
	LiveLayer * m_showOnShow;
	
	QList<LiveLayer*> m_sortedLayerList;
	bool m_lockVsibleSetter;
	
	QList<GLWidget*>  m_glWidgets;
	
	bool m_animationsDisabled;
	AnimParam m_animParam;
	
	int m_layerId;
	bool m_layerIdLoaded;
};

class ObjectValueSetter : public QObject
{
	Q_OBJECT
public:
	ObjectValueSetter(QObject *attached, const char *slot, QVariant value);
	
public slots:
	void executeSetValue();
	
signals:
	void setValue(int);
	void setValue(double);
	void setValue(const QString&);

private:
	QVariant m_value;
};

class BrowseDialogLauncher : public QObject
{
	Q_OBJECT
public:
	BrowseDialogLauncher(QObject *attached, const char *slot, QVariant value);
	
public slots:
	void browse();
	
	void setTitle(const QString&);
	void setSettingsKey(const QString&);
	void setFilter(const QString&);
	
signals:
	void setValue(const QString&);

private:
	QObject * m_attached;
	QVariant m_value;
	QString m_settingsKey;
	QString m_title;
	QString m_filter;
};

#endif
