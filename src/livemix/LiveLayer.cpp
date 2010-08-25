
#include "LiveLayer.h"

#include "../glvidtex/GLDrawable.h"
#include "../glvidtex/GLWidget.h"



PointEditorWidget::PointEditorWidget(QWidget *parent)
	: QWidget(parent)
{
	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0,0,0,0);
	
	QSpinBox *spin = new QSpinBox(this);
	spin->setSuffix(" px");
	spin->setMinimum(-9999);
	spin->setMaximum(9999);
	connect(spin, SIGNAL(valueChanged(int)), this, SLOT(xValueChanged(int)));
	hbox->addWidget(spin);
	
	x_box = spin;
	
	hbox->addWidget(new QLabel(" x "));
	
	spin = new QSpinBox(this);
	spin->setSuffix(" px");
	spin->setMinimum(-9999);
	spin->setMaximum(9999);
	connect(spin, SIGNAL(valueChanged(int)), this, SLOT(yValueChanged(int)));
	hbox->addWidget(spin);
	
	hbox->addStretch(1);
	
	y_box = spin;
}
	
void PointEditorWidget::setValue(const QPointF& point)
{
	m_point = point;
	x_box->setValue((int)point.x());
	y_box->setValue((int)point.y());
}

void PointEditorWidget::setXMinMax(int a, int b) { x_box->setMinimum(a); x_box->setMaximum(b); }
void PointEditorWidget::setYMinMax(int a, int b) { y_box->setMinimum(a); y_box->setMaximum(b); }
void PointEditorWidget::setSufix(const QString& suffix)
{
	x_box->setSuffix(suffix);
	y_box->setSuffix(suffix);
}

	
void PointEditorWidget::xValueChanged(int v)
{
	m_point = QPointF(v,m_point.y());
	emit valueChanged(m_point);
}

void PointEditorWidget::yValueChanged(int v)
{
	m_point = QPointF(m_point.x(),v);
	emit valueChanged(m_point);
}

//////////////////////////////////////////////////////////////////////////////


SizeEditorWidget::SizeEditorWidget(QWidget *parent)
	: QWidget(parent)
{
	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0,0,0,0);
	
	QSpinBox *spin = new QSpinBox(this);
	spin->setSuffix(" px");
	spin->setMinimum(-9999);
	spin->setMaximum(9999);
	connect(spin, SIGNAL(valueChanged(int)), this, SLOT(wValueChanged(int)));
	hbox->addWidget(spin);
	
	w_box = spin;
	
	hbox->addWidget(new QLabel(" x "));
	
	spin = new QSpinBox(this);
	spin->setSuffix(" px");
	spin->setMinimum(-9999);
	spin->setMaximum(9999);
	connect(spin, SIGNAL(valueChanged(int)), this, SLOT(hValueChanged(int)));
	hbox->addWidget(spin);
	
	hbox->addStretch(1);
	
	h_box = spin;
}
	
void SizeEditorWidget::setValue(const QSizeF& size)
{
	m_size = size;
	w_box->setValue((int)size.width());
	h_box->setValue((int)size.height());
}

void SizeEditorWidget::setWMinMax(int a, int b) { w_box->setMinimum(a); w_box->setMaximum(b); }
void SizeEditorWidget::setHMinMax(int a, int b) { h_box->setMinimum(a); h_box->setMaximum(b); }
void SizeEditorWidget::setSufix(const QString& suffix)
{
	w_box->setSuffix(suffix);
	h_box->setSuffix(suffix);
}

	
void SizeEditorWidget::wValueChanged(int v)
{
	m_size = QSizeF(v,m_size.height());
	emit valueChanged(m_size);
}

void SizeEditorWidget::hValueChanged(int v)
{
	m_size = QSizeF(m_size.width(),v);
	emit valueChanged(m_size);
}

//////////////////////////////////////////////////////////////////////////////
/*
ColorEditorWidget::ColorEditorWidget(QWidget *parent)
	: QWidget(parent)
{
	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0,0,0,0);
	
	QSpinBox *spin = new QSpinBox(this);
	spin->setPrefix("R ");
	spin->setMinimum(0);
	spin->setMaximum(255);
	connect(spin, SIGNAL(valueChanged(int)), this, SLOT(rValueChanged(int)));
	hbox->addWidget(spin);
	
	r_box = spin;
	
	spin = new QSpinBox(this);
	spin->setPrefix("G ");
	spin->setMinimum(0);
	spin->setMaximum(255);
	connect(spin, SIGNAL(valueChanged(int)), this, SLOT(gValueChanged(int)));
	hbox->addWidget(spin);
	
	g_box = spin;
	
	spin = new QSpinBox(this);
	spin->setPrefix("B ");
	spin->setMinimum(0);
	spin->setMaximum(255);
	connect(spin, SIGNAL(valueChanged(int)), this, SLOT(bValueChanged(int)));
	hbox->addWidget(spin);
	
	b_box = spin;
}
	
void ColorEditorWidget::setValue(const QColor& color)
{
	m_color = color;
	r_box->setValue((int)color.r());
	g_box->setValue((int)color.g());
	b_box->setValue((int)color.b());
}

	
void ColorEditorWidget::rValueChanged(int r)
{
	m_color = QColor(r,m_color.g(),m_color.b());
	emit valueChanged(m_color);
}

void ColorEditorWidget::gValueChanged(int g)
{
	m_color = QColor(m_color.r(),g,m_color.b());
	emit valueChanged(m_color);
}

void ColorEditorWidget::bValueChanged(int b)
{
	m_color = QColor(m_color.r(),m_color.g(),b);
	emit valueChanged(m_color);
}*/


//////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////

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



//////////////////////////////////////////////////////////////////////////////

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
		{
			drawable->setAnimationsEnabled(false);
			drawable->show();
		}
		else
			connect(this, SIGNAL(isVisible(bool)), drawable, SLOT(setVisible(bool)));
		
		return drawable;
	}
}



#include "ExpandableWidget.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QSlider>

	
QWidget * LiveLayer::generatePropertyEditor(QObject *object, const char *property, const char *slot, PropertyEditorOptions opts)
{
	QWidget *base = new QWidget();
	QHBoxLayout *hbox = new QHBoxLayout(base);
	hbox->setContentsMargins(0,0,0,0);
	
	QVariant prop = object->property(property);
	
	if(opts.value.isValid())
		prop = opts.value;
		
	if(opts.type == QVariant::Invalid)
		opts.type = prop.type();
		
	//qDebug() << "generatePropertyEditor: prop:"<<property<<", opts.type:"<<opts.type<<", variant:"<<(opts.value.isValid() ? opts.value : prop);
	
	if(opts.type == QVariant::Int)
	{
		QSpinBox *spin = new QSpinBox(base);
		if(!opts.suffix.isEmpty())
			spin->setSuffix(opts.suffix);
		spin->setMinimum((int)opts.min);
		spin->setMaximum((int)opts.max);
		
		if(prop.type() == QVariant::Double && opts.doubleIsPercentage)
			spin->setValue((int)(prop.toDouble()*100.0));
		else
			spin->setValue(prop.toInt());
			
		QObject::connect(spin, SIGNAL(valueChanged(int)), object, slot);
		hbox->addWidget(spin);
		
		if(!opts.noSlider)
		{
			QSlider *slider;
			slider = new QSlider(base);
			slider->setOrientation(Qt::Horizontal);
			slider->setMinimum((int)opts.min);
			slider->setMaximum((int)opts.max);
			
			if(prop.type() == QVariant::Double && opts.doubleIsPercentage)
				slider->setValue((int)(prop.toDouble()*100.0));
			else
				slider->setValue(prop.toInt());
				
			QObject::connect(spin, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
			QObject::connect(slider, SIGNAL(valueChanged(int)), spin, SLOT(setValue(int)));
			hbox->addWidget(slider);
		}
	}
	else
	if(opts.type == QVariant::Bool)
	{
		QCheckBox *box = new QCheckBox();
		delete base;
		
		if(opts.text.isEmpty())
			opts.text = guessTitle(property);
			
		box->setText(opts.text);
		
		QObject::connect(box, SIGNAL(toggled(bool)), object, slot);
		box->setChecked( prop.toBool() );
		
		return box;
	}
	else
	if(opts.type == QVariant::String)
	{
		QLineEdit *box = new QLineEdit();
		delete base;
		
		QObject::connect(box, SIGNAL(textChanged(const QString&)), object, slot);
		box->setText( prop.toString() );
		
		return box;
	}
	else
	if(opts.type == QVariant::SizeF)
	{
		SizeEditorWidget *editor = new SizeEditorWidget();
		delete base;
		
		QSizeF size = prop.toSizeF();
		editor->setValue(size);
		
		connect(editor, SIGNAL(valueChanged(const QSizeF&)), object, slot);
		
		return editor;
	}
	else
	if(opts.type == QVariant::PointF)
	{
		PointEditorWidget *editor = new PointEditorWidget();
		delete base;
		
		QPointF point = prop.toPointF();
		editor->setValue(point);
		
		connect(editor, SIGNAL(valueChanged(const QPointF&)), object, slot);
		
		return editor;
	}
	else
	{
		qDebug() << "LiveLayer::generatePropertyEditor(): No editor for type: "<<opts.type;
	}
	
	
	return base;
}


	
QWidget * LiveLayer::createLayerPropertyEditors()
{
	QWidget * base = new QWidget();
	QVBoxLayout *blay = new QVBoxLayout(base);
	blay->setContentsMargins(0,0,0,0);
	
	ExpandableWidget *groupGeom = new ExpandableWidget("Geometry",base);
	blay->addWidget(groupGeom);
	
	QWidget *groupGeomContainer = new QWidget;
	QFormLayout *formLayout = new QFormLayout(groupGeomContainer);
	formLayout->setContentsMargins(3,3,3,3);
	
	groupGeom->setWidget(groupGeomContainer);
	
// 	QHBoxLayout *hbox;
// 	QSpinBox *spin;

// 	<< "showFullScreen"
// 			<< "alignment"
// 			<< "insetTopLeft"
// 			<< "insetBottomRight";

	PropertyEditorOptions opts;
	
	opts.reset();
	//opts.value = rect().topLeft();
	//formLayout->addRow(tr("&Full Screen:"), generatePropertyEditor(this, "showFullScreen", SLOT(setShowFullScreen(bool)), opts));
	
	QStringList showAsList = QStringList()
		<< "Full Screen"//0
		<< "--------------------"//1
		<< "Absolute Position"//2
		<< "--------------------"//3
		<< "Centered on Screen"//4
		<< "Aligned Top Left"//5
		<< "Aligned Top Center"//6
		<< "Aligned Top Right"//7
		<< "Aligned Center Right"//8
		<< "Aligned Center Left"//9
		<< "Aligned Bottom Left"//10
		<< "Aligned Bottom Center"//11
		<< "Aligned Bottom Right";//12
		
	QComboBox *showAsBox = new QComboBox();
	showAsBox->addItems(showAsList);
	//showAsBox->setCurrentIndex(
	
	int idx = 0;
	if(showFullScreen())
	{
		idx = 0;
	}
	else
	{
		Qt::Alignment align = alignment();
		
		if((align & Qt::AlignAbsolute) == Qt::AlignAbsolute)
		{
			idx = 2;
		}
		else
		if((align & Qt::AlignHCenter) == Qt::AlignHCenter &&
		   (align & Qt::AlignVCenter) == Qt::AlignVCenter)
		{
			idx = 4;
		}
		else
		if((align & Qt::AlignTop)  == Qt::AlignTop &&
		   (align & Qt::AlignLeft) == Qt::AlignLeft)
		{
			idx = 5;
		}
		else
		if((align & Qt::AlignTop)  == Qt::AlignTop &&
		   (align & Qt::AlignHCenter) == Qt::AlignHCenter)
		{
			idx = 6;
		}
		else
		if((align & Qt::AlignTop)  == Qt::AlignTop &&
		   (align & Qt::AlignRight) == Qt::AlignRight)
		{
			idx = 7;
		}
		else
		if((align & Qt::AlignVCenter) == Qt::AlignVCenter &&
		   (align & Qt::AlignRight) == Qt::AlignRight)
		{
			idx = 8;
		}
		else
		if((align & Qt::AlignVCenter) == Qt::AlignVCenter &&
		   (align & Qt::AlignLeft) == Qt::AlignLeft)
		{
			idx = 9;
		}
		else
		if((align & Qt::AlignBottom) == Qt::AlignBottom &&
		   (align & Qt::AlignLeft) == Qt::AlignLeft)
		{
			idx = 10;
		}
		else
		if((align & Qt::AlignBottom) == Qt::AlignBottom &&
		   (align & Qt::AlignHCenter) == Qt::AlignHCenter)
		{
			idx = 11;
		}
		else
		if((align & Qt::AlignBottom) == Qt::AlignBottom &&
		   (align & Qt::AlignRight) == Qt::AlignRight)
		{
			idx = 12;
		}
	}
	if(idx >=0 && idx <= 12)
		showAsBox->setCurrentIndex(idx);
	
	connect(showAsBox, SIGNAL(activated(const QString&)), this, SLOT(setShowAsType(const QString&)));
	
	formLayout->addRow(tr("&Show As:"), showAsBox);
	
	opts.reset();
	opts.type = QVariant::Int;
	opts.min = -500;
	opts.max =  500;
	
	opts.value = insetTopLeft().x();
	formLayout->addRow(tr("&Inset Left:"), generatePropertyEditor(this, "insetLeft", SLOT(setLeftInset(int)), opts));
	
	opts.value = insetTopLeft().y();
	formLayout->addRow(tr("&Inset Top:"), generatePropertyEditor(this, "insetTop", SLOT(setTopInset(int)), opts));
	
	opts.value = insetBottomRight().x();
	formLayout->addRow(tr("&Inset Right:"), generatePropertyEditor(this, "insetRight", SLOT(setRightInset(int)), opts));
	
	opts.value = insetBottomRight().y();
	formLayout->addRow(tr("&Inset Bottom:"), generatePropertyEditor(this, "insetBottom", SLOT(setBottomInset(int)), opts));
	
	opts.reset();
	opts.value = rect().topLeft();
	formLayout->addRow(tr("&Position:"), generatePropertyEditor(this, "pos", SLOT(setPos(const QPointF&)), opts));
	
	opts.reset();
	opts.value = rect().size();
	formLayout->addRow(tr("&Size:"), generatePropertyEditor(this, "size", SLOT(setSize(const QSizeF&)), opts));
	
	
	opts.reset();
	opts.noSlider = true;
	opts.type = QVariant::Int;
	formLayout->addRow(tr("&Z Value:"), generatePropertyEditor(this, "zIndex", SLOT(setZIndex(int)), opts));
	
	opts.reset();
	opts.suffix = "%";
	opts.min = 0;
	opts.max = 100;
	opts.type = QVariant::Int;
	opts.doubleIsPercentage = true;
	formLayout->addRow(tr("&Opacity:"), generatePropertyEditor(this, "opacity", SLOT(setOpacity(int)), opts));
	
	groupGeom->setExpanded(true);
	
	/////////////////////////////////////////
	
	ExpandableWidget *groupAnim = new ExpandableWidget("Show/Hide Effects",base);
	blay->addWidget(groupAnim);
	
	QWidget *groupAnimContainer = new QWidget;
	QGridLayout *animLayout = new QGridLayout(groupAnimContainer);
	animLayout->setContentsMargins(3,3,3,3);
	
	groupAnim->setWidget(groupAnimContainer);
	
	opts.reset();
	opts.suffix = " ms";
	opts.min = 10;
	opts.max = 8000;
	
	int row = 0;
	animLayout->addWidget(generatePropertyEditor(this, "fadeIn", SLOT(setFadeIn(bool)), opts), row, 0);
	animLayout->addWidget(generatePropertyEditor(this, "fadeInLength", SLOT(setFadeInLength(int)), opts), row, 1);
	
	row++;
	animLayout->addWidget(generatePropertyEditor(this, "fadeOut", SLOT(setFadeOut(bool)), opts), row, 0);
	animLayout->addWidget(generatePropertyEditor(this, "fadeOutLength", SLOT(setFadeOutLength(int)), opts), row, 1);
	
	opts.reset();
	
	groupAnim->setExpanded(false);
	
	/////////////////////////////////////////
	
	ExpandableWidget *groupAnimAdvanced = new ExpandableWidget("Advanced Effects",base);
	blay->addWidget(groupAnimAdvanced);
	
	QWidget *groupAnimAdvancedContainer = new QWidget;
	QFormLayout *animAdvancedLayout = new QFormLayout(groupAnimAdvancedContainer);
	animAdvancedLayout->setContentsMargins(3,3,3,3);
	
	groupAnimAdvanced->setWidget(groupAnimAdvancedContainer);
	
	opts.reset();
	opts.suffix = " ms";
	opts.min = 10;
	opts.max = 8000;
	
	
	QStringList animTypes = QStringList()
		<< "(None)"
		<< "Zoom In/Out"
		<< "Slide From/To Top"
		<< "Slide From/To Bottom"
		<< "Slide From/To Left"
		<< "Slide From/To Right";
		
	QComboBox *showBox = new QComboBox();
	showBox->addItems(animTypes);
	idx = (int)showAnimationType();
	showBox->setCurrentIndex(!idx?idx:idx-1);
	connect(showBox, SIGNAL(activated(int)), this, SLOT(setShowAnim(int)));
	
	QComboBox *hideBox = new QComboBox();
	hideBox->addItems(animTypes);
	idx = (int)hideAnimationType();
	hideBox->setCurrentIndex(!idx?idx:idx-1);
	connect(hideBox, SIGNAL(activated(int)), this, SLOT(setHideAnim(int)));
	
	animAdvancedLayout->addRow(tr("&Show Animation:"), showBox);
	animAdvancedLayout->addRow(tr("&Show Anim Length:"), generatePropertyEditor(this, "showAnimationLength", SLOT(setShowAnimationLength(int)), opts));
	
	animAdvancedLayout->addRow(tr("&Hide Animation:"), hideBox);
	animAdvancedLayout->addRow(tr("&Hide Anim Length:"), generatePropertyEditor(this, "hideAnimationLength", SLOT(setHideAnimationLength(int)), opts));
	
	groupAnimAdvanced->setExpanded(false);
	
	row++;
	animLayout->addWidget(groupAnimAdvanced, row, 0,1, 2);
	
	/////////////////////////////////////////
	
	
	return base;
}

void LiveLayer::setShowAnim(int x)
{
	setShowAnimationType((GLDrawable::AnimationType)(x == 0 ? x : x + 1));
}

void LiveLayer::setHideAnim(int x)
{
	setHideAnimationType((GLDrawable::AnimationType)(x == 0 ? x : x + 1));
}

void LiveLayer::setShowAsType(const QString& text)
{
// 	<< "Full Screen"
// 	<< "--------------------"
// 	<< "Absolute Position"
// 	<< "--------------------"
// 	<< "Centered on Screen"
// 	<< "Aligned Top Left"
// 	<< "Aligned Top Center"
// 	<< "Aligned Top Right"
// 	<< "Aligned Center Right"
// 	<< "Aligned Center Left"
// 	<< "Aligned Bottom Left"
// 	<< "Aligned Bottom Center"
// 	<< "Aligned Bottom Right"
	qDebug() << "LiveLayer::setShowAsType(): text:"<<text;
	if(text.indexOf("---") >= 0)
		return;
		
	if(text.indexOf("Full Screen") >= 0)
	{
		setShowFullScreen(true);
	}
	else
	{
		setShowFullScreen(false);
		
		if(text.indexOf("Absolute") >= 0)
		{
			setAlignment(Qt::AlignAbsolute);
		}
		else
		if(text.indexOf("Centered") >= 0)
		{
			setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
		else
		if(text.indexOf("Top Left") >= 0)
		{
			setAlignment(Qt::AlignTop | Qt::AlignLeft);
		}
		else
		if(text.indexOf("Top Center") >= 0)
		{
			setAlignment(Qt::AlignTop | Qt::AlignHCenter);
		}
		else
		if(text.indexOf("Top Right") >= 0)
		{
			setAlignment(Qt::AlignTop | Qt::AlignRight);
		}
		else
		if(text.indexOf("Center Right") >= 0)
		{
			setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		}
		else
		if(text.indexOf("Center Left") >= 0)
		{
			setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		}
		else
		if(text.indexOf("Bottom Left") >= 0)
		{
			setAlignment(Qt::AlignBottom | Qt::AlignLeft);
		}
		else
		if(text.indexOf("Bottom Center") >= 0)
		{
			setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
		}
		else
		if(text.indexOf("Bottom Right") >= 0)
		{
			setAlignment(Qt::AlignBottom | Qt::AlignRight);
		}
		else
		{
			qDebug() << "LiveLayer::setShowAsType(): Unknown type:"<<text;
		}
	}
}

void LiveLayer::setX(int value)
{
	QRectF r = rect();
	r = QRectF(value, r.y(), r.width(), r.height());
	setRect(r);
}

void LiveLayer::setY(int value)
{
	QRectF r = rect();
	r = QRectF(r.x(), value, r.width(), r.height());
	setRect(r);
}

void LiveLayer::setWidth(int value)
{
	QRectF r = rect();
	r = QRectF(r.x(), r.y(), value, r.height());
	setRect(r);
}

void LiveLayer::setHeight(int value)
{
	QRectF r = rect();
	r = QRectF(r.x(), r.y(), r.width(), value);
	setRect(r);
}

void LiveLayer::setVisible(bool flag)
{
	// Implemented using sig/slot instead of just calling each if the drawables setVisible
	// directly so that it can be conditionally connected based on the GLWidget that
	// the drawable is connected to - see the connect() statement in LiveLayer::drawable()	
	//if(flag != m_isVisible)
		emit isVisible(flag);
	m_isVisible = flag;
}

void LiveLayer::setRect(const QRectF& rect) 	{ setLayerProperty("rect", rect); }
void LiveLayer::setZIndex(double z)	 	{ setLayerProperty("zIndex", z);  }
void LiveLayer::setZIndex(int z)	 	{ setLayerProperty("zIndex", z);  } // for sig slot compat
void LiveLayer::setOpacity(double o)		{ setLayerProperty("opacity", o); }
void LiveLayer::setOpacity(int o)		{ setLayerProperty("opacity", ((double)o)/100.0); }
void LiveLayer::setTransparency(int o)		{ setLayerProperty("opacity", (100.0-(double)o)/100.0); }

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
		
// 	qDebug() << "LiveLayer::setLayerProperty: id:"<<propertyId<<", value:"<<value;
	
	QVariant oldValue = m_props[propertyId].value;
	m_props[propertyId].value = value;
	
	if(value != oldValue)
		layerPropertyWasChanged(propertyId, value, oldValue);
	
	if(m_drawables.isEmpty())
		return;
		
	if(propertyId.indexOf("fadeIn")    > -1 ||
	   propertyId.indexOf("fadeOut")   > -1 ||
	   propertyId.indexOf("Animation") > -1)
	{
		foreach(GLWidget *widget, m_drawables.keys())
		{
			applyAnimationProperties(m_drawables[widget]);
		}
	}
	else
	{	
		GLDrawable *drawable = m_drawables[m_drawables.keys().first()];
		
		if(drawable->metaObject()->indexOfProperty(qPrintable(propertyId)) >= 0)
		{
			applyDrawableProperty(propertyId, value);	
		}
		else
		if(metaObject()->indexOfProperty(qPrintable(propertyId)) >= 0)
		{
			setProperty(qPrintable(propertyId), value);
		}
	}
}


void LiveLayer::applyDrawableProperty(const QString& propertyId, const QVariant& value)
{
	foreach(GLWidget *widget, m_drawables.keys())
	{
		m_drawables[widget]->setProperty(qPrintable(propertyId), value);
	}
}

void LiveLayer::applyAnimationProperties(GLDrawable *drawable)
{
	drawable->resetAllAnimations();
	
	if(m_props["fadeIn"].value.toBool())
		drawable->addShowAnimation(GLDrawable::AnimFade,m_props["fadeInLength"].value.toInt());
		
	if(m_props["fadeOut"].value.toBool())
		drawable->addHideAnimation(GLDrawable::AnimFade,m_props["fadeOutLength"].value.toInt());
	
	GLDrawable::AnimationType type;
	 
	type = (GLDrawable::AnimationType)m_props["showAnimationType"].value.toInt();
	if(type != GLDrawable::AnimNone)
		drawable->addShowAnimation(type,m_props["showAnimationLength"].value.toInt()).curve = (QEasingCurve::Type)m_props["showAnimationCurve"].value.toInt();
		
	type = (GLDrawable::AnimationType)m_props["hideAnimationType"].value.toInt();
	if(type != GLDrawable::AnimNone)
		drawable->addHideAnimation(type,m_props["hideAnimationLength"].value.toInt()).curve = (QEasingCurve::Type)m_props["hideAnimationCurve"].value.toInt();
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

	QStringList generalProps = QStringList()
			<< "rect"
			<< "zIndex"
			<< "opacity"
			<< "showFullScreen"
			<< "alignment"
			<< "insetTopLeft"
			<< "insetBottomRight";
			
	if(isFirstDrawable)
	{
		loadLayerPropertiesFromObject(drawable, generalProps);
			
		m_isVisible = drawable->isVisible();
		
		m_props["fadeIn"].value = 1;
		m_props["fadeInLength"].value = 300;
		
		m_props["fadeOut"].value = 1;
		m_props["fadeOutLength"].value = 300;
		
		m_props["showAnimationType"].value = GLDrawable::AnimNone;
		m_props["showAnimationLength"].value = 2500;
		m_props["showAnimationCurve"].value = QEasingCurve::OutElastic;
		
		m_props["hideAnimationType"].value = GLDrawable::AnimNone;
		m_props["hideAnimationLength"].value = 300;
		m_props["hideAnimationCurve"].value = QEasingCurve::Linear;
	}
	else
	{
		applyLayerPropertiesToObject(drawable, generalProps);
		applyAnimationProperties(drawable);
		
		drawable->setVisible(m_isVisible);
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
