

#include "EditorUtilityWidgets.h"
#include "LiveLayer.h"
#include "LiveScene.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QDirModel>
#include <QCompleter>

#ifndef Q_MAX
   #define Q_MAX(a,b) (a>b?a:b)
#endif


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

//////////////////////////////////////////////////////////////////////////////

PositionWidget::PositionWidget(LiveLayer *layer)
	: QWidget()
	, m_layer(layer)
{
	m_lockAspectRatio = false;
	m_lockToAR = 0.0;
	m_lockValueUpdates = false;
	
	QGridLayout *grid = new QGridLayout(this);
	int row =0;
	
	QHBoxLayout *hbox=0;
	QWidget *box=0;
	
	#define NEW_PANEL {\
		box = new QWidget(this); \
		hbox = new QHBoxLayout(box); \
		hbox->setContentsMargins(0,0,0,0); \
		}
	
	NEW_PANEL;
	
	//////////////////////
	// Fist row: Display type
	
	NEW_PANEL;
	row++;
	
	QComboBox *m_aspectRatioBox = new QComboBox();
	QStringList arNames;
	
	int idx = 0;
	
	QPointF ar = m_layer->desiredAspectRatio();
	
	int count = 0;
	#define AddRatio(name,a,r) \
		arNames << name; \
		m_arList << QPoint(a,r); \
		if(a == (int)ar.x() && r == (int)ar.y()) \
			idx = count; \
		count ++;
		
		
	AddRatio("Manual (Any Ratio)", 0,0);
	AddRatio("User Specified", -1,-1);
	AddRatio("4:3   - Standard Screen", 4,3);
	AddRatio("16:9  - HDTV",16,9);
	AddRatio("16:10 - Widescreen Monitor", 16,10);
	
	#undef AddRatio
	
	m_aspectRatioBox->addItems(arNames);
	connect(m_aspectRatioBox, SIGNAL(activated(int)), this, SLOT(setAspectRatioIdx(int)));
	hbox->addWidget(m_aspectRatioBox);
	
	m_arLeft = new QSpinBox(box);
	m_arLeft->setAlignment(Qt::AlignRight);
 	connect(m_arLeft, SIGNAL(valueChanged(int)), this, SLOT(setARLeft(int)));
	hbox->addWidget(m_arLeft);
	
	hbox->addWidget(new QLabel(":"));
	
	m_arRight = new QSpinBox(box);
	m_arRight->setAlignment(Qt::AlignLeft);
 	connect(m_arRight, SIGNAL(valueChanged(int)), this, SLOT(setARRight(int)));
	hbox->addWidget(m_arRight);
	
	hbox->addStretch(1);
	
	                  // row col rspan cspan
	grid->addWidget(box, row, 1, 1, 3);
	
	
	//////////////////////
	// 2nd row: position
	
	row++;
	grid->addWidget(new QLabel("Position (X-Y):"), row, 0);
	
	NEW_PANEL;
	
	m_posX = new QDoubleSpinBox(box);
	connect(m_posX, SIGNAL(valueChanged(double)), this, SLOT(setLayerX(double)));
	hbox->addWidget(m_posX);
	
	m_posY = new QDoubleSpinBox(box);
	connect(m_posY, SIGNAL(valueChanged(double)), this, SLOT(setLayerY(double)));
	hbox->addWidget(m_posY);
	
	
	grid->addWidget(box, row, 1);
	
	//////////////////////
	// 3nd row: size
	
	row++;
	grid->addWidget(new QLabel("Size (W-H):"), row, 0);
	
	NEW_PANEL;
	
	m_sizeWidth = new QDoubleSpinBox(box);
	connect(m_sizeWidth, SIGNAL(valueChanged(double)), this, SLOT(setLayerWidth(double)));
	hbox->addWidget(m_sizeWidth);
	
	m_sizeHeight = new QDoubleSpinBox(box);
	connect(m_sizeHeight, SIGNAL(valueChanged(double)), this, SLOT(setLayerHeight(double)));
	hbox->addWidget(m_sizeHeight);
	
	grid->addWidget(box, row, 1);
	
	#undef NEW_PANEL
	
// 	setEditPixels(false);


	#define CHANGE_SPINBOX_PIXELS(spin) \
		spin->setMinimum(-Q_MAX(m_layer->scene()->canvasSize().width(),m_layer->scene()->canvasSize().height()) * 10); \
		spin->setMaximum( Q_MAX(m_layer->scene()->canvasSize().width(),m_layer->scene()->canvasSize().height()) * 10); \
		spin->setSuffix(" px"); \
		spin->setDecimals(0); \
		spin->setAlignment(Qt::AlignRight);


	m_lockValueUpdates = true;
	CHANGE_SPINBOX_PIXELS(m_posY);
	CHANGE_SPINBOX_PIXELS(m_posX);
	CHANGE_SPINBOX_PIXELS(m_sizeWidth);
	CHANGE_SPINBOX_PIXELS(m_sizeHeight);
	
	QRectF r = m_layer->rect();
	m_posY->setValue(r.y());
	m_posX->setValue(r.x());
	m_sizeWidth->setValue(r.width());
	m_sizeHeight->setValue(r.height());
	
	m_aspectRatioBox->setCurrentIndex(idx);
	setAspectRatioIdx(idx);
	
	
	m_lockValueUpdates = false;
	
	
	#undef CHANGE_SPINBOX_PIXELS
		
	
	// watch for position changes and update UI accordingly
	//connect(m_layer, SIGNAL(layerPropertyChanged(const QString& propertyId, const QVariant& value, const QVariant& oldValue)), this, SLOT(layerPropertyChanged(const QString& propertyId, const QVariant& value, const QVariant& oldValue)));
	
}

void PositionWidget::setAspectRatioIdx(int idx)
{
	if(idx<0 || idx>=m_arList.size())
		return;
		
	QPoint ar = m_arList[idx];
	
	m_arLeft->setEnabled(idx == 1);
	m_arRight->setEnabled(idx == 1);
	
	m_lockAspectRatio = idx > 0;
	
	if(idx > 1)
	{
		m_arLeft->setValue(ar.x());
		m_arRight->setValue(ar.y());
		
		m_layer->setDesiredAspectRatio(QPointF(ar.x(),ar.y()));
	}

}

void PositionWidget::layerPropertyChanged(const QString& /*propertyId*/, const QVariant& /*value*/, const QVariant& /*oldValue*/)
{

}


	
void PositionWidget::setLayerY(double value)
{
	if(m_lockValueUpdates)
	{
		//qDebug() << "PositionWidget::setLayerTop(): m_lockValueUpdates, returning";
		return;
	}
	m_lockValueUpdates = true;
		
	m_layer->setY(value);
	
	m_lockValueUpdates = false;
}

void PositionWidget::setLayerX(double value)
{
	if(m_lockValueUpdates)
		return;
	m_lockValueUpdates = true;
	
	m_layer->setX(value);
	
	m_lockValueUpdates = false;
}
	
void PositionWidget::setLayerWidth(double value)
{
	if(m_lockValueUpdates)
		return;
	m_lockValueUpdates = true;
	
	QRectF r = m_layer->rect();
		
	if(m_lockAspectRatio)
	{
		double newHeight = heightFromWidth(value);
		m_layer->setRect(r.x(), r.y(), value, newHeight);
		m_sizeHeight->setValue(newHeight);
	}
	else
	{
		m_layer->setRect(r.x(), r.y(), value, r.height());
	}
	
	
	m_lockValueUpdates = false;
}

void PositionWidget::setLayerHeight(double value)
{
	if(m_lockValueUpdates)
		return;
	m_lockValueUpdates = true;

	QRectF r = m_layer->rect();
		
	if(m_lockAspectRatio)
	{
		double newWidth = widthFromHeight(value);
		m_layer->setRect(r.x(), r.y(), newWidth, value);
		m_sizeWidth->setValue(newWidth);
	}
	else
	{
		m_layer->setRect(r.x(), r.y(), r.width(), value);
	}


	
	
	m_lockValueUpdates = false;
}
	
double PositionWidget::heightFromWidth(double value)
{
	double ar = ((double)m_arLeft->value()) / ((double)m_arRight->value());
	return value/ar;
}
	
double PositionWidget::widthFromHeight(double value)
{
	double ar = ((double)m_arLeft->value()) / ((double)m_arRight->value());
	return value*ar;
}

void PositionWidget::setARLeft(int /*val*/)
{
	if(m_lockValueUpdates)
		return;
	m_lockValueUpdates = true;
		
// 	// AR = height/width
// 	double ar = ((double)m_arLeft->value()) / ((double)val);
	
	// AR left = width ratio, so update with respective to height
	// eg width for height using new AR
	
	QRectF r = m_layer->rect();
	double newHeight = heightFromWidth(r.width());
	m_layer->setRect(r.x(), r.y(), r.width(), newHeight);
	m_sizeHeight->setValue(newHeight);
	
	m_lockValueUpdates = false;
}

void PositionWidget::setARRight(int /*val*/)
{
	if(m_lockValueUpdates)
		return;
	m_lockValueUpdates = true;
	
	QRectF r = m_layer->rect();
	double newHeight = heightFromWidth(r.width());
	m_layer->setRect(r.x(), r.y(), r.width(), newHeight);
	m_sizeHeight->setValue(newHeight);
		
	m_lockValueUpdates = false;
}
