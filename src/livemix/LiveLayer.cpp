
#include "LiveLayer.h"
#include "LiveScene.h"
#include "../glvidtex/GLDrawable.h"
#include "../glvidtex/GLWidget.h"
#include <QPropertyAnimation>

//////////////////////////////////////////////////////////////////////////////

		
PropertyChangeListener::PropertyChangeListener(QObject *source, const char *changeSignal, QObject *receiver, const char *receiverSlot, QVariant value, QString prop)
	: QObject(receiver)
	, m_value(value)
	, m_property(prop)
{
// 	qDebug() << "PropertyChangeListener: connecting "<<source<<"::"<<changeSignal<<" to "<<receiver<<"::"<<receiverSlot<<", value:"<<value<<", prop:"<<prop;
	switch(value.type())
	{
		case QVariant::Int:
			connect(this, SIGNAL(value(int)), receiver, receiverSlot);
			break;
		case QVariant::Bool:
			connect(this, SIGNAL(value(bool)), receiver, receiverSlot);
			break;
		case QVariant::Double:
			connect(this, SIGNAL(value(double)), receiver, receiverSlot);
			break;
		case QVariant::String:
			connect(this, SIGNAL(value(const QString&)), receiver, receiverSlot);
			break;
		case QVariant::Size:
			connect(this, SIGNAL(value(const QSize&)), receiver, receiverSlot);
			break;
		case QVariant::Point:
			connect(this, SIGNAL(value(const QPoint&)), receiver, receiverSlot);
			break;
		case QVariant::SizeF:
			connect(this, SIGNAL(value(const QSizeF&)), receiver, receiverSlot);
			break;
		case QVariant::PointF:
			connect(this, SIGNAL(value(const QPointF&)), receiver, receiverSlot);
			break;
		default:
			qDebug() << "PropertyChangeListener: Unable to handle value type:"<<value.type();
			break;
	};
	
	QString signal(changeSignal);
	if(signal.indexOf("QString"))
	{
		connect(source, changeSignal, this, SLOT(receiver(const QString&, const QVariant&)));
	}
	else
	{
		connect(source, changeSignal, this, SLOT(receiver(const QVariant&)));
	}
}

void PropertyChangeListener::receiver(const QString& prop, const QVariant& data)
{
// 	qDebug() << "PropertyChangeListener::receiver: prop:"<<prop<<", m_property:"<<m_property<<", value:"<<data;
	if(prop == m_property)
		receiver(data);
}

void PropertyChangeListener::receiver(const QVariant& data)
{
// 	qDebug() << "PropertyChangeListener::receiver: value:"<<data;
	switch(data.type())
	{
		case QVariant::Int:
			emit value(data.toInt());
			break;
		case QVariant::Bool:
			emit value(data.toBool());
			break;
		case QVariant::Double:
			emit value(data.toDouble());
			break;
		case QVariant::String:
			emit value(data.toString());
			break;
		case QVariant::Size:
			emit value(data.toSize());
			break;
		case QVariant::Point:
			emit value(data.toPoint());
			break;
		case QVariant::SizeF:
			emit value(data.toSizeF());
			break;
		case QVariant::PointF:
			emit value(data.toPointF());
			break;
		default:
			qDebug() << "PropertyChangeListener::receiver: Unable to handle value type:"<<data.type()<<", variant:"<<data;
			break;
	};
}


//////////////////////////////////////////////////////////////////////////////

BrowseDialogLauncher::BrowseDialogLauncher(QObject *attached, const char *slot, QVariant value)
	: QObject(attached)
	, m_attached(attached)
	, m_value(value)
	, m_settingsKey("default")
	, m_title("Browse")
	, m_filter("Any File (*.*)")
{
	connect(this, SIGNAL(setValue(const QString&)), attached, slot);
}


void BrowseDialogLauncher::setTitle(const QString& value)
{
	m_title = value;
}

void BrowseDialogLauncher::setSettingsKey(const QString& value)
{
	m_settingsKey = value;
}

void BrowseDialogLauncher::setFilter(const QString& value)
{
	m_filter = value;
}
	
void BrowseDialogLauncher::browse()
{
	QString text = m_value.toString();
	QString settingsPath = QString("BrowseDialogLauncher/%1").arg(m_settingsKey);
	
	if(text.trimmed().isEmpty())
	{
		text = QSettings().value(settingsPath,"").toString();
	}

	QString fileName = QFileDialog::getOpenFileName(dynamic_cast<QWidget*>(m_attached), m_title, text, m_filter);
	if(fileName != "")
	{
		emit setValue(fileName);
		QSettings().setValue(settingsPath,QFileInfo(fileName).absolutePath());
	}
}

//////////////////////////////////////////////////////////////////////////////

DoubleEditorWidget::DoubleEditorWidget(QWidget *parent)
	: QWidget(parent)
{
	QHBoxLayout *hbox = new QHBoxLayout(this);
	hbox->setContentsMargins(0,0,0,0);


	QDoubleSpinBox *spin = new QDoubleSpinBox(this);
	m_box = spin;
	hbox->addWidget(spin);

	QSlider *slider;
	slider = new QSlider(this);
	slider->setOrientation(Qt::Horizontal);

	connect(spin, SIGNAL(valueChanged(double)), this, SLOT(boxValueChanged(double)));
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
	hbox->addWidget(slider);

	m_slider = slider;
}

void DoubleEditorWidget::setValue(double value)
{
	m_value = value;
	m_box->setValue(value);
	m_slider->setValue((int)value);
}

void DoubleEditorWidget::setMinMax(double a, double b)
{
	   m_box->setMinimum(a);    m_box->setMaximum(b);
	m_slider->setMinimum((int)a); m_slider->setMaximum((int)b);
}

void DoubleEditorWidget::setShowSlider(bool flag)
{
	m_slider->setVisible(flag);
}

void DoubleEditorWidget::setSuffix(const QString& suffix)
{
	m_box->setSuffix(suffix);
}


void DoubleEditorWidget::boxValueChanged(double v)
{
	m_value = v;
	m_slider->setValue((int)v);
	emit valueChanged(v);
}

void DoubleEditorWidget::sliderValueChanged(int v)
{
	double d = (double)v;
	if(m_box->value() != d)
	{
		m_box->setValue(d);
	}
}


//////////////////////////////////////////////////////////////////////////////



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

	QPushButton *undoBtn = new QPushButton(QPixmap("../data/stock-undo.png"), "");
	connect(undoBtn, SIGNAL(clicked()), this, SLOT(reset()));
	hbox->addWidget(undoBtn);
	
	hbox->addStretch(1);
	
	y_box = spin;
}

void PointEditorWidget::setValue(const QPointF& point)
{
	m_point = point;
	x_box->setValue((int)point.x());
	y_box->setValue((int)point.y());
	m_orig = point;
}

void PointEditorWidget::reset()
{
	setValue(m_orig);
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
	
	QPushButton *undoBtn = new QPushButton(QPixmap("../data/stock-undo.png"), "");
	connect(undoBtn, SIGNAL(clicked()), this, SLOT(reset()));
	hbox->addWidget(undoBtn);

	hbox->addStretch(1);

	h_box = spin;
}

void SizeEditorWidget::setValue(const QSizeF& size)
{
	m_size = size;
	w_box->setValue((int)size.width());
	h_box->setValue((int)size.height());
}

void SizeEditorWidget::reset()
{
	setValue(m_orig);
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

// Translated from a perl function I wrote to do basically
// the same thing for an ERP project a few years back.
QString LiveLayer::guessTitle(QString field)
{
	static const QRegExp rUpperCase = QRegExp("([a-z])([A-Z])");
	static const QRegExp rFirstLetter = QRegExp("([a-z])");
	static const QRegExp rLetterNumber = QRegExp("([a-z])([0-9])");
	//$name =~ s/([a-z])_([a-z])/$1.' '.uc($2)/segi;
	static const QRegExp rUnderScore = QRegExp("([a-z])_([a-z])");

	QString tmp = field;
	tmp.replace(rUnderScore,"\\1 \\2");
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
	, m_scene(0)
	, m_hideOnShow(0)
	, m_showOnShow(0)
	, m_lockVsibleSetter(false)
	, m_animationsDisabled(false)
	, m_layerId(-1)
	, m_layerIdLoaded(false)
{
	m_props["rect"] = QRectF();
	m_props["zIndex"] = -1.;
	m_props["opacity"] = 1.0;
	m_props["showFullScreen"] = true;
	m_props["alignment"] = 0;
	m_props["insetTopLeft"] = QPointF(0,0);
	m_props["insetBottomRight"] = QPointF(0,0);
	m_props["alignedSizeScale"] = 1.0;
	m_props["rotation"] = QVector3D(0,0,0);
	
	m_props["fadeIn"] = 1;
	m_props["fadeInLength"] = 300;

	m_props["fadeOut"] = 1;
	m_props["fadeOutLength"] = 300;

	m_props["showAnimationType"] = GLDrawable::AnimNone;
	m_props["showAnimationLength"] = 2500;
	m_props["showAnimationCurve"] = QEasingCurve::OutElastic;

	m_props["hideAnimationType"] = GLDrawable::AnimNone;
	m_props["hideAnimationLength"] = 300;
	m_props["hideAnimationCurve"] = QEasingCurve::Linear;
}

LiveLayer::~LiveLayer()
{
	QList<GLDrawable *> drawables = m_drawables.values();
	m_drawables.clear();
	
	qDeleteAll(drawables);
	drawables.clear();
	
	m_scene = 0;
	m_hideOnShow = 0;
	m_showOnShow = 0;	
}

int LiveLayer::id()
{
	if(m_layerId < 0)
	{
		QSettings set;
		m_layerId = set.value("livelayer/id-counter",0).toInt() + 1;
		set.setValue("livelayer/id-counter",m_layerId);
	}
	
	return m_layerId;
}

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
 			//qDebug() << "LiveLayer::drawable: widget:"<<widget<<", cache miss, first drawable";
			initDrawable(drawable, true);
		}
		else
		{
 			//qDebug() << "LiveLayer::drawable: widget:"<<widget<<", cache miss, copy from first";
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
#include <QDoubleSpinBox>
#include <QDirModel>
#include <QCompleter>

ObjectValueSetter::ObjectValueSetter(QObject *attached, const char *slot, QVariant value)
	: QObject(attached)
	, m_value(value)
{
	switch(value.type())
	{
		case QVariant::Int:
			connect(this, SIGNAL(setValue(int)), attached, slot);
			break;
		case QVariant::Double:
			connect(this, SIGNAL(setValue(double)), attached, slot);
			break;
		case QVariant::String:
			connect(this, SIGNAL(setValue(const QString&)), attached, slot);
			break;
		default:
			qDebug() << "ObjectValueSetter: No signal for value type: "<<value.type();
			break;
	}
	
	connect(attached, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}
	
void ObjectValueSetter::executeSetValue()
{
	switch(m_value.type())
	{
		case QVariant::Int:
			emit setValue(m_value.toInt());
			break;
		case QVariant::Double:
			emit setValue(m_value.toDouble());
			break;
		case QVariant::String:
			emit setValue(m_value.toString());
			break;
		default:
			qDebug() << "ObjectValueSetter::executeSetValue: No signal for value type: "<<m_value.type();
			break;
	}
}

QWidget * LiveLayer::generatePropertyEditor(QObject *object, const char *property, const char *slot, PropertyEditorOptions opts, const char *changeSignal)
{
	if(changeSignal == NULL &&
		dynamic_cast<LiveLayer*>(object))
		changeSignal = SIGNAL(layerPropertyChanged(const QString&, const QVariant&, const QVariant&));
		
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
		if(changeSignal)
			new PropertyChangeListener(object, changeSignal, spin, SLOT(setValue(int)), spin->value(), property);
			 
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
		
		QPushButton *undoBtn = new QPushButton(QPixmap("../data/stock-undo.png"), "");
		ObjectValueSetter *setter = new ObjectValueSetter(spin, SLOT(setValue(int)), spin->value());
		connect(undoBtn, SIGNAL(clicked()), setter, SLOT(executeSetValue()));
		hbox->addWidget(undoBtn);
		
		if(opts.defaultValue.isValid())
		{
			QPushButton *resetBtn = new QPushButton(QPixmap("../data/stock-close.png"), "");
			ObjectValueSetter *setter = new ObjectValueSetter(spin, SLOT(setValue(int)), opts.defaultValue);
			connect(resetBtn, SIGNAL(clicked()), setter, SLOT(executeSetValue()));
			hbox->addWidget(resetBtn);
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

		connect(box, SIGNAL(toggled(bool)), object, slot);
		box->setChecked( prop.toBool() );
		
		if(changeSignal)
			new PropertyChangeListener(object, changeSignal, box, SLOT(setChecked(bool)), prop, property);

		return box;
	}
	else
	if(opts.type == QVariant::String)
	{
		QLineEdit *box = new QLineEdit();
		
		QObject::connect(box, SIGNAL(textChanged(const QString&)), object, slot);
		box->setText( prop.toString() );
		
		if(changeSignal)
			new PropertyChangeListener(object, changeSignal, box, SLOT(setText(const QString&)), prop, property);
		
		if(opts.stringIsFile)
		{
			
			QCompleter *completer = new QCompleter(box);
			QDirModel *dirModel = new QDirModel(completer);
			completer->setModel(dirModel);
			//completer->setMaxVisibleItems(10);
			completer->setCompletionMode(QCompleter::PopupCompletion);
			completer->setCaseSensitivity(Qt::CaseInsensitive);
			completer->setWrapAround(true);
			box->setCompleter(completer);
			
			hbox->addWidget(box);
			
			QPushButton *browseButton = new QPushButton(QPixmap("../data/stock-open.png"), "");
			BrowseDialogLauncher *setter = new BrowseDialogLauncher(box, SLOT(setText(const QString&)), box->text());
			connect(browseButton, SIGNAL(clicked()), setter, SLOT(browse()));
			
			if(!opts.fileTypeFilter.isEmpty())
				setter->setFilter(opts.fileTypeFilter);
			
			hbox->addWidget(browseButton);
			
			return base;
		}
		else
		{
			delete base;
		}

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
		if(changeSignal)
			new PropertyChangeListener(object, changeSignal, editor, SLOT(setValue(const QSizeF&)), prop, property);

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
		if(changeSignal)
			new PropertyChangeListener(object, changeSignal, editor, SLOT(setValue(const QPointF&)), prop, property);

		return editor;
	}
	else
	if(opts.type == QVariant::Double)
	{
		DoubleEditorWidget *editor = new DoubleEditorWidget();
		delete base;

		editor->setShowSlider(!opts.noSlider);

		editor->setValue(prop.toDouble());

		connect(editor, SIGNAL(valueChanged(double)), object, slot);
		if(changeSignal)
			new PropertyChangeListener(object, changeSignal, editor, SLOT(setValue(double)), prop, property);

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

	ExpandableWidget *groupGeom = new ExpandableWidget("Position and Display",base);
	blay->addWidget(groupGeom);

	QWidget *groupGeomContainer = new QWidget;
	QFormLayout *formLayout = new QFormLayout(groupGeomContainer);
	formLayout->setContentsMargins(3,3,3,3);
	//formLayout->setContentsMargins(0,0,0,0);

	m_geomLayout = formLayout;

	groupGeom->setWidget(groupGeomContainer);
	
	PropertyEditorOptions opts;

	opts.reset();

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
	opts.defaultValue = 0;

	opts.value = insetTopLeft().x();
	formLayout->addRow(tr("&Inset Left:"), m_propWidget["insetLeft"] = generatePropertyEditor(this, "insetLeft", SLOT(setLeftInset(int)), opts));

	opts.value = insetTopLeft().y();
	formLayout->addRow(tr("&Inset Top:"), m_propWidget["insetTop"] = generatePropertyEditor(this, "insetTop", SLOT(setTopInset(int)), opts));

	opts.value = insetBottomRight().x();
	formLayout->addRow(tr("&Inset Right:"), m_propWidget["insetRight"] = generatePropertyEditor(this, "insetRight", SLOT(setRightInset(int)), opts));

	opts.value = insetBottomRight().y();
	formLayout->addRow(tr("&Inset Bottom:"), m_propWidget["insetBottom"] = generatePropertyEditor(this, "insetBottom", SLOT(setBottomInset(int)), opts));

	opts.reset();
	opts.suffix = "%";
	opts.min = 0;
	opts.max = 5000;
	opts.defaultValue = 100;
	opts.type = QVariant::Int;
	opts.doubleIsPercentage = true;
	formLayout->addRow(tr("&Scale Size:"), m_propWidget["sizeScale"] = generatePropertyEditor(this, "alignedSizeScale", SLOT(setAlignedSizeScale(int)), opts));
	
	opts.reset();
	opts.value = rect().topLeft();
	formLayout->addRow(tr("&Position:"), m_propWidget["pos"] = generatePropertyEditor(this, "pos", SLOT(setPos(const QPointF&)), opts));

	opts.reset();
	opts.value = rect().size();
	formLayout->addRow(tr("&Size:"), m_propWidget["size"] = generatePropertyEditor(this, "size", SLOT(setSize(const QSizeF&)), opts));


	opts.reset();
	opts.noSlider = true;
	opts.type = QVariant::Int;
	opts.defaultValue = 0;
	formLayout->addRow(tr("&Z Value:"), generatePropertyEditor(this, "zIndex", SLOT(setZIndex(int)), opts));
	
	opts.reset();
	opts.suffix = " deg";
	opts.min = -360.0;
	opts.max =  360.0;
	opts.defaultValue = 0;
	opts.type = QVariant::Int;
	opts.value = rotation().x();
	formLayout->addRow(tr("&X Rotation:"), generatePropertyEditor(this, "xRotation", SLOT(setXRotation(int)), opts));
	
	opts.value = rotation().y();
	formLayout->addRow(tr("&Y Rotation:"), generatePropertyEditor(this, "yRotation", SLOT(setYRotation(int)), opts));
	
	opts.value = rotation().z();
	formLayout->addRow(tr("&Z Rotation:"), generatePropertyEditor(this, "zRotation", SLOT(setZRotation(int)), opts));

	opts.reset();
	opts.suffix = "%";
	opts.min = 0;
	opts.max = 100;
	opts.defaultValue = 100;
	opts.type = QVariant::Int;
	opts.doubleIsPercentage = true;
	formLayout->addRow(tr("&Opacity:"), generatePropertyEditor(this, "opacity", SLOT(setOpacity(int)), opts));

	// Add layers to a map to sort by zindex
	if(m_scene)
	{
		QMap<double,LiveLayer*> layerMap;
		QList<LiveLayer*> layerList = m_scene->layerList();
		foreach(LiveLayer *layer, layerList)
			if(layer != this)
				layerMap[layer->zIndex()] = layer;
			
		m_sortedLayerList = layerMap.values();
		
		QStringList layerNames;
		layerNames << "(None)";
		int hideIdx=0, showIdx=0, count=1;
		foreach(LiveLayer *layer, m_sortedLayerList)
		{
			layerNames << QString("%1 (%2)").arg(layer->instanceName()).arg(layer->typeName());
			if(layer == m_hideOnShow)
				hideIdx = count;
			if(layer == m_showOnShow)
				showIdx = count;
			count ++;
		}
	
		//qDebug() << "LiveLayer::createLayerPropertyEditors(): hideIdx:"<<hideIdx<<", showIdx:"<<showIdx;
		
		QComboBox *hideOnShowBox = new QComboBox();
		hideOnShowBox->addItems(layerNames);
		hideOnShowBox->setCurrentIndex(hideIdx);
		hideOnShowBox->setEnabled(layerNames.size() > 1);
		connect(hideOnShowBox, SIGNAL(activated(int)), this, SLOT(setHideOnShow(int)));
	
		QComboBox *showOnShowBox = new QComboBox();
		showOnShowBox->addItems(layerNames);
		showOnShowBox->setCurrentIndex(showIdx);
		showOnShowBox->setEnabled(layerNames.size() > 1);
		connect(showOnShowBox, SIGNAL(activated(int)), this, SLOT(setShowOnShow(int)));
	
		formLayout->addRow(tr("&Hide on Show:"), hideOnShowBox);
		formLayout->addRow(tr("&Show on Show:"), showOnShowBox);
	}
	
	//groupGeom->setExpanded(false);
	groupGeom->setExpandedIfNoDefault(false);

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
	opts.defaultValue = 300;

	int row = 0;
	animLayout->addWidget(generatePropertyEditor(this, "fadeIn", SLOT(setFadeIn(bool)), opts), row, 0);
	animLayout->addWidget(generatePropertyEditor(this, "fadeInLength", SLOT(setFadeInLength(int)), opts), row, 1);

	row++;
	animLayout->addWidget(generatePropertyEditor(this, "fadeOut", SLOT(setFadeOut(bool)), opts), row, 0);
	animLayout->addWidget(generatePropertyEditor(this, "fadeOutLength", SLOT(setFadeOutLength(int)), opts), row, 1);

	opts.reset();

	groupAnim->setExpandedIfNoDefault(false);

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
	opts.defaultValue = 2500;
	animAdvancedLayout->addRow(tr("&Show Anim Length:"), generatePropertyEditor(this, "showAnimationLength", SLOT(setShowAnimationLength(int)), opts));

	animAdvancedLayout->addRow(tr("&Hide Animation:"), hideBox);
	opts.defaultValue = 300;
	animAdvancedLayout->addRow(tr("&Hide Anim Length:"), generatePropertyEditor(this, "hideAnimationLength", SLOT(setHideAnimationLength(int)), opts));

	groupAnimAdvanced->setExpandedIfNoDefault(false);

	row++;
	animLayout->addWidget(groupAnimAdvanced, row, 0,1, 2);

	/////////////////////////////////////////

	setShowAsType(showAsBox->itemText(showAsBox->currentIndex()));


	return base;
}

void LiveLayer::setShowAnim(int x)
{
	setShowAnimationType(x == 0 ? x : x + 1);
}

void LiveLayer::setHideAnim(int x)
{
	setHideAnimationType(x == 0 ? x : x + 1);
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
	setVisibleGeometryFields();

	//qDebug() << "LiveLayer::setShowAsType(): text:"<<text;
	if(text.indexOf("---") >= 0)
		return;

	if(text.indexOf("Full Screen") >= 0)
	{
		setShowFullScreen(true);

		setVisibleGeometryFields(QStringList());

	}
	else
	{
		setShowFullScreen(false);

		if(text.indexOf("Absolute") >= 0)
		{
			setAlignment(Qt::AlignAbsolute);
			setVisibleGeometryFields(QStringList() << "pos" << "size");
		}
		else
		if(text.indexOf("Centered") >= 0)
		{
			setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			setVisibleGeometryFields(QStringList() << "sizeScale");
		}
		else
		if(text.indexOf("Top Left") >= 0)
		{
			setAlignment(Qt::AlignTop | Qt::AlignLeft);
			setVisibleGeometryFields(QStringList() << "insetTop" << "insetLeft" << "sizeScale");
		}
		else
		if(text.indexOf("Top Center") >= 0)
		{
			setAlignment(Qt::AlignTop | Qt::AlignHCenter);
			setVisibleGeometryFields(QStringList() << "insetTop" << "sizeScale");
		}
		else
		if(text.indexOf("Top Right") >= 0)
		{
			setAlignment(Qt::AlignTop | Qt::AlignRight);
			setVisibleGeometryFields(QStringList() << "insetTop" << "insetRight" << "sizeScale");
		}
		else
		if(text.indexOf("Center Right") >= 0)
		{
			setAlignment(Qt::AlignVCenter | Qt::AlignRight);
			setVisibleGeometryFields(QStringList() << "insetRight" << "sizeScale");
		}
		else
		if(text.indexOf("Center Left") >= 0)
		{
			setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
			setVisibleGeometryFields(QStringList() << "insetLeft" << "sizeScale");
		}
		else
		if(text.indexOf("Bottom Left") >= 0)
		{
			setAlignment(Qt::AlignBottom | Qt::AlignLeft);
			setVisibleGeometryFields(QStringList() << "insetBottom" << "insetLeft" << "sizeScale");
		}
		else
		if(text.indexOf("Bottom Center") >= 0)
		{
			setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
			setVisibleGeometryFields(QStringList() << "insetBottom" << "sizeScale");
		}
		else
		if(text.indexOf("Bottom Right") >= 0)
		{
			setAlignment(Qt::AlignBottom | Qt::AlignRight);
			setVisibleGeometryFields(QStringList() << "insetBottom" << "insetRight" << "sizeScale");
		}
		else
		{
			qDebug() << "LiveLayer::setShowAsType(): Unknown type:"<<text;
		}
	}
}

void LiveLayer::setVisibleGeometryFields(QStringList list)
{
	QHash<QString,bool> found;
	foreach(QString field, list)
	{
		QWidget *widget = m_propWidget[field];
		if(!widget)
		{
			qDebug() << "LiveLayer::setVisibleGeometryFields: Cannot find requested field:"<<field;
		}
		else
		{
			widget->setVisible(true);

			QWidget *label = m_geomLayout->labelForField(widget);
			if(label)
				label->setVisible(true);

			found[field] = true;
		}
	}

	foreach(QString key, m_propWidget.keys())
	{
		if(!found.contains(key))
		{
			QWidget *widget = m_propWidget[key];
			widget->setVisible(false);

			QWidget *label = m_geomLayout->labelForField(widget);
			if(label)
				label->setVisible(false);
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
	if(m_lockVsibleSetter)
		return;
	m_lockVsibleSetter = true;
	// Implemented using sig/slot instead of just calling each if the drawables setVisible
	// directly so that it can be conditionally connected based on the GLWidget that
	// the drawable is connected to - see the connect() statement in LiveLayer::drawable()
	//if(flag != m_isVisible)
		emit isVisible(flag);
	m_isVisible = flag;
	
	if(m_hideOnShow)
		if(m_hideOnShow->isVisible() == flag)
			m_hideOnShow->setVisible(!flag);

	if(m_showOnShow)
		if(m_showOnShow->isVisible() != flag)
			m_showOnShow->setVisible(flag);
		
	m_lockVsibleSetter = false;
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
	//if(!m_props.contains(propertyId))
	//	return;
		
	// Prevent recursions that may be triggered by a property setter in turn calling setLayerProperty(), 
	// which would just loop back and call that property setter again for that property - recursion.
	if(m_propSetLock[propertyId])
		return;
		
	m_propSetLock[propertyId] = true;

 	//if(propertyId != "rect")
 	//qDebug() << "LiveLayer::setLayerProperty: id:"<<propertyId<<", value:"<<value;

	QVariant oldValue = m_props[propertyId];
	m_props[propertyId] = value;

	if(value != oldValue)
		layerPropertyWasChanged(propertyId, value, oldValue);

	if(m_drawables.isEmpty())
	{
		//qDebug() << "LiveLayer::setLayerProperty: id:"<<propertyId<<", drawables empty, unlocking and returning";
		m_propSetLock[propertyId] = false;
		return;
	}

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
			//qDebug() << "LiveLayer::setLayerProperty: id:"<<propertyId<<", applying to drawables";
			applyDrawableProperty(propertyId, value);
		}
		else
		if(metaObject()->indexOfProperty(qPrintable(propertyId)) >= 0)
		{
			//qDebug() << "LiveLayer::setLayerProperty: id:"<<propertyId<<", applying to self";
			// could cause recursion if the property setter calls this method again, hence the m_propSetLock[] usage
			setProperty(qPrintable(propertyId), value);
		}
		else
		{
			qDebug() << "LiveLayer::setLayerProperty: id:"<<propertyId<<", not found in any meta object!";
		}
	}
	
	m_propSetLock[propertyId] = false;
}


void LiveLayer::applyDrawableProperty(const QString& propertyId, const QVariant& value)
{
// 	m_animationsDisabled = true;
	foreach(GLWidget *widget, m_drawables.keys())
	{
		if(m_animationsDisabled || 
			propertyId == "aspectRatioMode" || // yes, I know, hack - need to be some way for layers to specify what props dont need to be animated - or to ask a prop if its an enum.......
			(value.type() == QVariant::Bool && propertyId != "showFullScreen"))
		{
			m_drawables[widget]->setProperty(qPrintable(propertyId), value);
		}
		else
		{
			if(propertyId == "alignment")
			{
				m_drawables[widget]->setAlignment((Qt::Alignment)value.toInt(), true, m_animParam.length, m_animParam.curve);
			}
			else
			if(propertyId == "showFullScreen")
			{
				m_drawables[widget]->setShowFullScreen(value.toBool(), true, m_animParam.length, m_animParam.curve);
			}
			else
			{
				QPropertyAnimation *animation = new QPropertyAnimation(m_drawables[widget], propertyId.toAscii());
				animation->setDuration(m_animParam.length);
				animation->setEasingCurve(m_animParam.curve);
				animation->setEndValue(value);
				animation->start(QAbstractAnimation::DeleteWhenStopped);
			}
		}
	}
}

void LiveLayer::applyAnimationProperties(GLDrawable *drawable)
{
	drawable->resetAllAnimations();

	if(m_props["fadeIn"].toBool())
		drawable->addShowAnimation(GLDrawable::AnimFade,m_props["fadeInLength"].toInt());

	if(m_props["fadeOut"].toBool())
		drawable->addHideAnimation(GLDrawable::AnimFade,m_props["fadeOutLength"].toInt());

	GLDrawable::AnimationType type;

	type = (GLDrawable::AnimationType)m_props["showAnimationType"].toInt();
	if(type != GLDrawable::AnimNone)
		drawable->addShowAnimation(type,m_props["showAnimationLength"].toInt()).curve = (QEasingCurve::Type)m_props["showAnimationCurve"].toInt();

	type = (GLDrawable::AnimationType)m_props["hideAnimationType"].toInt();
	if(type != GLDrawable::AnimNone)
		drawable->addHideAnimation(type,m_props["hideAnimationLength"].toInt()).curve = (QEasingCurve::Type)m_props["hideAnimationCurve"].toInt();
}

void LiveLayer::setInstanceName(const QString& name)
{
	m_instanceName = name;
	setObjectName(qPrintable(m_instanceName));
	emit instanceNameChanged(name);
}

// just emits layerPropertyChanged
void LiveLayer::layerPropertyWasChanged(const QString& propertyId, const QVariant& value, const QVariant& oldValue)
{
	//qDebug() << "LiveLayer::layerPropertyWasChanged: "<<this<<": property:"<<propertyId<<", value:"<<value<<", was:"<<oldValue;
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
void LiveLayer::initDrawable(GLDrawable *drawable, bool /*isFirstDrawable*/)
{
 	//qDebug() << "LiveLayer::initDrawable: drawable:"<<drawable;
	bool animEnabled = setAnimEnabled(false);
	
	QStringList generalProps = QStringList()
			<< "rect"
			<< "zIndex"
			<< "opacity"
			<< "showFullScreen"
			<< "alignment"
			<< "insetTopLeft"
			<< "insetBottomRight"
			<< "alignedSizeScale"
			<< "rotation";
		
	//qDebug() << "LiveLayer::initDrawable: drawable:"<<drawable<<", props list:"<<generalProps;
	applyLayerPropertiesToObject(drawable, generalProps);
	applyAnimationProperties(drawable);
	
	setAnimEnabled(animEnabled);

	//qDebug() << "LiveLayer::initDrawable: now setting visible:"<<m_isVisible<<", rect:"<<drawable->rect();
	drawable->setVisible(m_isVisible);
	
}


// Helper function - given a list of property names, load the props using QObject::property() from the drawable into m_props
void LiveLayer::loadLayerPropertiesFromObject(const QObject *object, const QStringList& list)
{
	foreach(QString key, list)
	{
		m_props[key] = object->property(qPrintable(key));
	}
}

// Helper function - attempts to apply the list of props in 'list' (or all props in m_props if 'list' is empty) to drawable
// 'Attempts' means that if drawable->metaObject()->indexOfProperty() returns <0, then setProperty is not called
void LiveLayer::applyLayerPropertiesToObject(QObject *object, QStringList list)
{
	//qDebug() << "LiveLayer::applyLayerPropertiesToObject(): "<<object<<", list:"<<list;
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
				//qDebug() << "LiveLayer::applyLayerPropertiesToObject(): "<<object<<", prop:"<<key<<", setting to "<<m_props[key];
				object->setProperty(asciiPropId, m_props[key]);
			}
			else
			{
				qDebug() << "LiveLayer::applyLayerPropertiesToObject(): "<<object<<", prop:"<<key<<", cannot find index of prop on object, not setting";
			}
		}
		else
		{
			qDebug() << "LiveLayer::applyLayerPropertiesToObject(): "<<object<<", prop:"<<key<<", m_props doesnt contain it, not setting";
		}
	}
}


void LiveLayer::fromByteArray(QByteArray& array)
{
	bool animEnabled = setAnimEnabled(false);
	
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	//qDebug() << "LiveScene::fromByteArray(): "<<map;
	if(map.isEmpty())
	{
		qDebug() << "Error: LiveLayer::fromByteArray(): Map is empty, unable to load scene.";
		return;
	}
	
	loadPropsFromMap(map);
	
	setAnimEnabled(animEnabled);
	
	
}

void LiveLayer::loadPropsFromMap(const QVariantMap& map, bool onlyApplyIfChanged)
{
	bool vis = false;
	
	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
	const QMetaObject *metaobject = metaObject();
	int count = metaobject->propertyCount();
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		QVariant value = map[name];
		
		//if(name == "aspectRatioMode")
		//qDebug() << "LiveLayer::loadPropsFromMap():"<<this<<": i:"<<i<<", count:"<<count<<", prop:"<<name<<", value:"<<value;
		
		// Hold setting visiblility flag till last so that way any properties that affect
		// animations are set BEFORE animations start!
		if(QString(name) == "isVisible")
		{
			vis = value.toBool();
		}
		else
		if(QString(name) == "id")
		{
			// m_layerId is only set ONCE by this method, overwriting any ID assigned at creation time
			if(!m_layerIdLoaded && value.isValid())
			{
				m_layerIdLoaded = true;
				m_layerId = value.toInt();
			}
		}
		else
		{
			
			if(value.isValid())
			{
				if(onlyApplyIfChanged)
				{
					if(property(name) != value)
						setProperty(name,value);
				}
				else
				{
					//qDebug() << "LiveLayer::loadPropsFromMap():"<<this<<": i:"<<i<<", count:"<<count<<", prop:"<<name<<", value:"<<value<<" (calling set prop)";
					setProperty(name,value);
				}
			}
			//else
				//qDebug() << "LiveLayer::loadPropsFromMap: Unable to load property for "<<name<<", got invalid property from map";
		}
	}
	
	//qDebug() << "LiveLayer::fromByteArray():"<<this<<": *** Setting visibility to "<<vis;
	if(!onlyApplyIfChanged || isVisible() != vis)
		setVisible(vis);
}

QByteArray LiveLayer::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	stream << propsToMap();
	return array;
}

QVariantMap LiveLayer::propsToMap()
{
	QVariantMap map;
	// So we dont have to engineer our own method of tracking
	// properties, just assume all inherited objects delcare the relevant
	// properties using Q_PROPERTY macro
	const QMetaObject *metaobject = metaObject();
	int count = metaobject->propertyCount();
	for (int i=0; i<count; ++i)
	{
		QMetaProperty metaproperty = metaobject->property(i);
		const char *name = metaproperty.name();
		QVariant value = property(name);
		
		if(name == "aspectRatioMode")
			qDebug() << "LiveLayer::toByteArray():"<<this<<instanceName()<<": prop:"<<name<<", value:"<<value;
			
		map[name] = value;
	}
	return map;
}

void LiveLayer::setShowOnShowLayerId(int value)
{
	setLayerProperty("showOnShowLayerId", value);
	if(m_scene)
		setShowOnShow(m_scene->layerFromId(value));
}

void LiveLayer::setHideOnShowLayerId(int value)
{
	setLayerProperty("hideOnShowLayerId", value);
	if(m_scene)
		setHideOnShow(m_scene->layerFromId(value));
}
	
void LiveLayer::setHideOnShow(LiveLayer *layer)
{
	m_hideOnShow = layer;
	setLayerProperty("hideOnShowLayerId", layer ? layer->id() : 0);
}

void LiveLayer::setHideOnShow(int x)
{
	if(x == 0)
	{
		setHideOnShow((LiveLayer*)0);
		return;
	}
	
	if(x<1 || x>m_sortedLayerList.size())
		return;
	setHideOnShow(m_sortedLayerList[x-1]);
}

void LiveLayer::setShowOnShow(LiveLayer *layer)
{
	m_showOnShow = layer;
	setLayerProperty("showOnShowLayerId", layer ? layer->id() : 0);
}

void LiveLayer::setShowOnShow(int x)
{
	if(x == 0)
	{
		setShowOnShow((LiveLayer*)0);
		return;
	}
	
	
	if(x<1 || x>m_sortedLayerList.size())
		return;
	setShowOnShow(m_sortedLayerList[x-1]);
}

void LiveLayer::setScene(LiveScene *scene)
{
	m_scene = scene;
	
	if(hideOnShowLayerId() > 0 &&
	  !hideOnShow())
	  {
	  	LiveLayer *layer = m_scene->layerFromId(hideOnShowLayerId());
	  	if(layer)
	  		setHideOnShow(layer);
	  }
		
	if(showOnShowLayerId() > 0 &&
	  !showOnShow())
	  {
	  	LiveLayer *layer = m_scene->layerFromId(showOnShowLayerId());
	  	if(layer)
			setShowOnShow(layer);
	}
}

void LiveLayer::attachGLWidget(GLWidget *glw)
{
	if(!glw)
		return;
		
	m_glWidgets.append(glw);

	glw->addDrawable(drawable(glw));
}

void LiveLayer::detachGLWidget(GLWidget *glw)
{
	if(!glw)
		return;

	glw->removeDrawable(drawable(glw));

	m_glWidgets.removeAll(glw);
}

bool LiveLayer::setAnimEnabled(bool flag)
{
	bool old = !m_animationsDisabled;
	m_animationsDisabled = !flag;
	return old;
}

void LiveLayer::setAnimParam(const LiveLayer::AnimParam &p)
{
	m_animParam = p;
}
