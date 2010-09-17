

#include "EditorUtilityWidgets.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QDirModel>
#include <QCompleter>

//////////////////////////////////////////////////////////////////////////////

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

