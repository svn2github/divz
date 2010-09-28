

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

PercentPositionWidget::PercentPositionWidget(LiveLayer *layer)
	: QWidget()
	, m_layer(layer)
{
	m_topLock = false;
	m_leftLock = false;
	m_bottomLock = false;
	m_rightLock = false;
	m_lockAspectRatio = false;
	m_editPixels = false;
	m_lockToAR = 0.0;
	m_lockUpdateSizeUI = false;
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
	
	QRadioButton *rb;
	rb = new QRadioButton("Display Percentages");
	rb->setChecked(true);
	connect(rb, SIGNAL(toggled(bool)), this, SLOT(setEditPercent(bool)));
	hbox->addWidget(rb);
	
	rb = new QRadioButton("Display Pixels");
	rb->setChecked(false);
	connect(rb, SIGNAL(toggled(bool)), this, SLOT(setEditPixels(bool)));
	hbox->addWidget(rb);
	
	                  // row col rspan cspan
	grid->addWidget(box, row, 1, 1, 3);
	
	
	//////////////////////
	// 2nd row: Top-left and width
	
	row++;
	grid->addWidget(new QLabel("Top-Left:"), row, 0);
	
	NEW_PANEL;
	
	m_posTop = new QDoubleSpinBox(box);
	connect(m_posTop, SIGNAL(valueChanged(double)), this, SLOT(setLayerTop(double)));
	hbox->addWidget(m_posTop);
	
	m_posLeft = new QDoubleSpinBox(box);
	connect(m_posLeft, SIGNAL(valueChanged(double)), this, SLOT(setLayerLeft(double)));
	hbox->addWidget(m_posLeft);
	
	grid->addWidget(box, row, 1);
	
	grid->addWidget(new QLabel("Width:"), row, 2);
	m_sizeWidth = new QDoubleSpinBox(box);
	connect(m_sizeWidth, SIGNAL(valueChanged(double)), this, SLOT(setLayerWidth(double)));
	//hbox->addWidget(m_sizeWidth);
	grid->addWidget(m_sizeWidth, row, 3);
	
	//////////////////////
	// 3nd row: Bottom-right and height
	
	row++;
	grid->addWidget(new QLabel("Bottom-Right:"), row, 0);
	
	NEW_PANEL;
	
	m_posBottom = new QDoubleSpinBox(box);
	connect(m_posBottom, SIGNAL(valueChanged(double)), this, SLOT(setLayerBottom(double)));
	hbox->addWidget(m_posBottom);
	
	m_posRight = new QDoubleSpinBox(box);
	connect(m_posRight, SIGNAL(valueChanged(double)), this, SLOT(setLayerRight(double)));
	hbox->addWidget(m_posRight);
	
	grid->addWidget(box, row, 1);
	
	grid->addWidget(new QLabel("Height:"), row, 2);
	m_sizeHeight = new QDoubleSpinBox(box);
	connect(m_sizeHeight, SIGNAL(valueChanged(double)), this, SLOT(setLayerHeight(double)));
	//hbox->addWidget(m_sizeHeight);
	grid->addWidget(m_sizeHeight, row, 3);
	
	//////////////////////
	// 4th row: Display type
	
	NEW_PANEL;
	row++;
	
	QCheckBox *cb = new QCheckBox("Lock Aspect Ratio");
	cb->setChecked(m_lockAspectRatio);
	connect(cb, SIGNAL(toggled(bool)), this, SLOT(setLockAR(bool)));
	hbox->addWidget(cb);
	
	m_arLeft = new QSpinBox(box);
 	connect(m_arLeft, SIGNAL(valueChanged(int)), this, SLOT(setARLeft(int)));
	hbox->addWidget(m_arLeft);
	
	hbox->addWidget(new QLabel(":"));
	
	m_arRight = new QSpinBox(box);
 	connect(m_arRight, SIGNAL(valueChanged(int)), this, SLOT(setARRight(int)));
	hbox->addWidget(m_arRight);
	
	hbox->addStretch(1);
	
	                  // row col rspan cspan
	grid->addWidget(box, row, 1, 1, 3);
	
	#undef NEW_PANEL
	
	setEditPixels(false);
	
	// watch for position changes and update UI accordingly
	//connect(m_layer, SIGNAL(layerPropertyChanged(const QString& propertyId, const QVariant& value, const QVariant& oldValue)), this, SLOT(layerPropertyChanged(const QString& propertyId, const QVariant& value, const QVariant& oldValue)));
	
}

void PercentPositionWidget::layerPropertyChanged(const QString& /*propertyId*/, const QVariant& /*value*/, const QVariant& /*oldValue*/)
{

}

void PercentPositionWidget::layerTopChanged(double value)
{
	m_posTop->setValue(m_editPixels ? value * m_layer->scene()->canvasSize().height() : value * 100.);
	updateSizeUI();
}

void PercentPositionWidget::layerLeftChanged(double value)
{
	m_posLeft->setValue(m_editPixels ? value * m_layer->scene()->canvasSize().width() : value * 100.);
	updateSizeUI();
}
	
void PercentPositionWidget::layerBottomChanged(double value)
{
	m_posBottom->setValue(m_editPixels ? value * m_layer->scene()->canvasSize().height() : value * 100.);
	updateSizeUI();
}

void PercentPositionWidget::layerRightChanged(double value)
{
	m_posRight->setValue(m_editPixels ? value * m_layer->scene()->canvasSize().width() : value * 100.);
	updateSizeUI();
}

	
void PercentPositionWidget::setLayerTop(double value)
{
	if(m_lockValueUpdates)
	{
		//qDebug() << "PercentPositionWidget::setLayerTop(): m_lockValueUpdates, returning";
		return;
	}
	m_lockValueUpdates = true;
		
	value = m_editPixels ? value / m_layer->scene()->canvasSize().height() : value / 100.;
	//qDebug() << "PercentPositionWidget::setLayerTop(): value: "<<value;
	
	m_layer->setTopPercent(value);
	
	if(m_lockAspectRatio)
	{
		double wp = widthFromHeight(m_layer->bottomPercent() - value);
		layerLeftChanged(m_layer->rightPercent() - wp);
	}
	else
	{
		updateSizeUI();
	}
	m_lockValueUpdates = false;
}

void PercentPositionWidget::setLayerLeft(double value)
{
	if(m_lockValueUpdates)
		return;
	m_lockValueUpdates = true;
		
	value = m_editPixels ? value / m_layer->scene()->canvasSize().width() : value / 100.;
	m_layer->setLeftPercent(value);
	if(m_lockAspectRatio)
	{
		double hp = heightFromWidth(m_layer->rightPercent() - value);
	// 	m_layer->setTopPercent(m_lyaer->bottomPercent() - hp);
	// 	m_posTop->setValue(m_editPixels ? hp * m_layer->scene()->canvasSize().height() : hp);
		layerTopChanged(m_layer->bottomPercent() - hp);
	}
	else
	{
		updateSizeUI();
	}
	m_lockValueUpdates = false;
}

void PercentPositionWidget::setLayerBottom(double value)
{
	if(m_lockValueUpdates)
		return;
	m_lockValueUpdates = true;
		
	value = m_editPixels ? value / m_layer->scene()->canvasSize().height() : value / 100.;
	m_layer->setBottomPercent(value);
	if(m_lockAspectRatio)
	{
		double newBottom = value;
		double newHeight = newBottom - m_layer->topPercent();
		double newHeightPx = newHeight * m_layer->scene()->canvasSize().height();
		double newWidthPx = widthFromHeight(newHeightPx);
		double newWidth = newWidthPx / m_layer->scene()->canvasSize().width();
		double newRight = m_layer->leftPercent() + newWidth;
		qDebug() << "PercentPositionWidget::setLayerBottom: newBottom:"<<newBottom<<"\n"
			<< "\t newHeight:"<<newHeight<<"\n"
			<< "\t newHeightPx:"<<newHeightPx<<"\n"
			<< "\t newWidthPx:"<<newWidthPx<<"\n"
			<< "\t newWidth:"<<newWidth<<"\n"
			<< "\t newRight:"<<newRight<<"\n"
			<< "\t canvas size:"<<m_layer->scene()->canvasSize();
		layerRightChanged(newRight);
	}
	else
	{
		updateSizeUI();
	}
	m_lockValueUpdates = false;
	
	
}

void PercentPositionWidget::setLayerRight(double value)
{
	if(m_lockValueUpdates)
		return;
	m_lockValueUpdates = true;
		
	value = m_editPixels ? value / m_layer->scene()->canvasSize().width() : value/100.;
	m_layer->setRightPercent(value);
	
	if(m_lockAspectRatio)
	{
		double hp = heightFromWidth((value - m_layer->leftPercent()) * m_layer->scene()->canvasSize().width()) / m_layer->scene()->canvasSize().height();
		layerBottomChanged(m_layer->leftPercent() + hp);
	}
	else
	{
		updateSizeUI();
	}
	
	m_lockValueUpdates = false;
}
	
void PercentPositionWidget::setLayerWidth(double value)
{
	if(m_lockValueUpdates)
		return;
	m_lockValueUpdates = true;
	
	qDebug() << "PercentPositionWidget::setLayerWidth: value: "<<value;
	double perc = m_editPixels ? value / m_layer->scene()->canvasSize().width() : value / 100.;
	m_layer->setRightPercent(m_layer->leftPercent() + perc);
	qDebug() << "PercentPositionWidget::setLayerWidth: perc: "<<perc;
	
	m_posRight->setValue(m_layer->rightPercent() * (m_editPixels ? m_layer->scene()->canvasSize().width() : 100.));
	
	if(m_lockAspectRatio)
	{
		double hp = heightFromWidth(perc);
		double nv = m_editPixels ? hp * m_layer->scene()->canvasSize().height() : hp * 100.;
		qDebug() << "PercentPositionWidget::setLayerWidth: hp: "<<hp<<", nv: "<<nv;
		if(m_sizeHeight->value() != nv)
			m_sizeHeight->setValue(nv);
	}
	
	updateSizeUI();
	
	m_lockValueUpdates = false;
}

void PercentPositionWidget::setLayerHeight(double value)
{
	if(m_lockValueUpdates)
		return;
	m_lockValueUpdates = true;
		
	qDebug() << "PercentPositionWidget::setLayerHeight: value: "<<value;
	double perc = m_editPixels ? value / m_layer->scene()->canvasSize().height() : value / 100.;
	m_layer->setBottomPercent(m_layer->topPercent() + perc);
	qDebug() << "PercentPositionWidget::setLayerHeight: perc: "<<perc;
	
	m_posRight->setValue(m_layer->bottomPercent() * (m_editPixels ? m_layer->scene()->canvasSize().height() : 100.));
	
	if(m_lockAspectRatio)
	{
		double wp = widthFromHeight(perc);
		double nv = m_editPixels ? wp * m_layer->scene()->canvasSize().width() : wp * 100.;
		qDebug() << "PercentPositionWidget::setLayerHeight: wp: "<<wp<<", nv: "<<nv;
		if(m_sizeWidth->value() != nv)
			m_sizeWidth->setValue(nv);
	}
	
	updateSizeUI();
	
	m_lockValueUpdates = false;
}

void PercentPositionWidget::setLockAR(bool flag)
{
	m_lockAspectRatio = flag;
	m_sizeWidth->setReadOnly(flag);
	m_sizeHeight->setReadOnly(flag);
}
	
double PercentPositionWidget::heightFromWidth(double value)
{
	double ar = ((double)m_arLeft->value()) / ((double)m_arRight->value());
	return value/ar;
}
	
double PercentPositionWidget::widthFromHeight(double value)
{
	double ar = ((double)m_arLeft->value()) / ((double)m_arRight->value());
	return value*ar;
}

#define CHANGE_SPINBOX_PERCENT(spin) \
	spin->setMinimum(-200); \
	spin->setMaximum(200); \
	spin->setSuffix("%"); \
	spin->setDecimals(3); \
	spin->setAlignment(Qt::AlignRight);

#ifndef Q_MAX
   #define Q_MAX(a,b) (a>b?a:b)
#endif

#define CHANGE_SPINBOX_PIXELS(spin) \
	spin->setMinimum(-Q_MAX(m_layer->scene()->canvasSize().width(),m_layer->scene()->canvasSize().height()) * 2); \
	spin->setMaximum( Q_MAX(m_layer->scene()->canvasSize().width(),m_layer->scene()->canvasSize().height()) * 2); \
	spin->setSuffix(" px"); \
	spin->setDecimals(0); \
	spin->setAlignment(Qt::AlignRight);

void PercentPositionWidget::setEditPixels(bool flag)
{
	if(flag)
	{
		m_lockValueUpdates = true;
		m_editPixels = true;
		CHANGE_SPINBOX_PIXELS(m_posTop);
		CHANGE_SPINBOX_PIXELS(m_posLeft);
		CHANGE_SPINBOX_PIXELS(m_posBottom);
		CHANGE_SPINBOX_PIXELS(m_posRight);
		CHANGE_SPINBOX_PIXELS(m_sizeWidth);
		CHANGE_SPINBOX_PIXELS(m_sizeHeight);
		
		double w = m_layer->scene()->canvasSize().width();
		double h = m_layer->scene()->canvasSize().height();
		
		m_posTop->setValue(m_layer->topPercent() * h);
		m_posLeft->setValue(m_layer->leftPercent() * w);
		m_posBottom->setValue(m_layer->bottomPercent() * h);
		m_posRight->setValue(m_layer->rightPercent() * w);
		
		m_sizeWidth->setValue((m_layer->rightPercent() - m_layer->leftPercent()) * w);
		m_sizeHeight->setValue((m_layer->bottomPercent() - m_layer->topPercent()) * h);
		
		updateSizeUI();
		
		m_lockValueUpdates = false;
	}
	else
	{
		setEditPercent(true);
	}
}

void PercentPositionWidget::setEditPercent(bool flag)
{
	if(flag)
	{
		m_lockValueUpdates = true;
		m_editPixels = false;
		CHANGE_SPINBOX_PERCENT(m_posTop);
		CHANGE_SPINBOX_PERCENT(m_posLeft);
		CHANGE_SPINBOX_PERCENT(m_posBottom);
		CHANGE_SPINBOX_PERCENT(m_posRight);
		CHANGE_SPINBOX_PERCENT(m_sizeWidth);
		CHANGE_SPINBOX_PERCENT(m_sizeHeight);
		
		qDebug() << "PercentPositionWidget::setEditPercent: tlbr: "
			<< m_layer->topPercent()
			<< m_layer->leftPercent()
			<< m_layer->bottomPercent()
			<< m_layer->rightPercent();
			
		m_posTop->setValue(m_layer->topPercent() * 100.);
		m_posLeft->setValue(m_layer->leftPercent() * 100.);
		m_posBottom->setValue(m_layer->bottomPercent() * 100.);
		m_posRight->setValue(m_layer->rightPercent() * 100.);
		
		m_sizeWidth->setValue((m_layer->rightPercent() - m_layer->leftPercent()) * 100.);
		m_sizeHeight->setValue((m_layer->bottomPercent() - m_layer->topPercent()) * 100.);
		
		updateSizeUI();
		
		m_lockValueUpdates = false;
	}
	else
	{
		setEditPixels(true);
	}
}
	
void PercentPositionWidget::setARLeft(int /*val*/)
{
	if(m_lockValueUpdates)
		return;
	m_lockValueUpdates = true;
		
// 	// AR = height/width
// 	double ar = ((double)m_arLeft->value()) / ((double)val);
	
	// AR left = width ratio, so update with respective to height
	// eg width for height using new AR
	
	double wp = widthFromHeight(m_layer->bottomPercent() - m_layer->topPercent());
	double nv = m_editPixels ? wp * m_layer->scene()->canvasSize().width() : wp * 100.;
	qDebug() << "PercentPositionWidget::setARLeft: wp: "<<wp<<", nv: "<<nv;
	if(m_sizeWidth->value() != nv)
	{
		m_sizeWidth->setValue(nv);
		
		// set the percent on the layer manually rather than waiting for the slot to do it, since we've disabled value updates
		m_layer->setRightPercent(m_layer->leftPercent() + wp);
	}
		
	m_lockValueUpdates = false;
}

void PercentPositionWidget::setARRight(int /*val*/)
{
	if(m_lockValueUpdates)
		return;
	m_lockValueUpdates = true;
	
	double hp = heightFromWidth(m_layer->rightPercent() - m_layer->leftPercent());
	double nv = m_editPixels ? hp * m_layer->scene()->canvasSize().height() : hp * 100.;
	qDebug() << "PercentPositionWidget::setARRight: hp: "<<hp<<", nv: "<<nv;
	if(m_sizeHeight->value() != nv)
	{
		m_sizeHeight->setValue(nv);
		
		// set the percent on the layer manually rather than waiting for the slot to do it, since we've disabled value updates
		m_layer->setBottomPercent(m_layer->topPercent() + hp);
	}
		
	m_lockValueUpdates = false;
}
	
	
void PercentPositionWidget::updateSizeUI()
{
	if(m_lockUpdateSizeUI)
	{
		qDebug() << "PercentPositionWidget::updateSizeUI(): m_lockUpdateSizeUI, returning";
		return;
	}
	m_lockUpdateSizeUI = true;
	
	double w = m_layer->rightPercent()  - m_layer->leftPercent();
	double h = m_layer->bottomPercent() - m_layer->topPercent();
	
	double nv;
	nv = m_editPixels ? h * m_layer->scene()->canvasSize().height() : h * 100.;
	qDebug() << "PercentPositionWidget::updateSizeUI(): checking for height change, currently "<<m_sizeHeight->value()<<", nv is "<<nv;
	if(m_sizeHeight->value() != nv)
	{
		qDebug() << "PercentPositionWidget::updateSizeUI(): height changed, was "<<m_sizeHeight->value()<<", now "<<nv;
		m_sizeHeight->setValue (nv);
	}
	
	nv = m_editPixels ? w * m_layer->scene()->canvasSize().width()  : w * 100.;
	qDebug() << "PercentPositionWidget::updateSizeUI(): checking for width change, currently "<<m_sizeHeight->value()<<", nv is "<<nv;
	if(m_sizeWidth->value() != nv)
	{
		m_sizeWidth->setValue  (nv);
		qDebug() << "PercentPositionWidget::updateSizeUI(): width changed, was "<<m_sizeHeight->value()<<", now "<<nv;
	}
	
	if(m_lockAspectRatio)
	{
		m_lockUpdateSizeUI = false;
		return;
	}
	
	// Calc AR using actual screen size, since percent AR will likely be different than the AR of the pixels
	double arFrac = (h * m_layer->scene()->canvasSize().height())/(w * m_layer->scene()->canvasSize().width());
	
	QString arStr;
	arStr.sprintf("%.02f",arFrac);
	arFrac = arStr.toDouble();
	
	qDebug() << "PercentPositionWidget::updateSizeUI(): ar: "<<arFrac;
	
	int whole = (int)arFrac;
	double decimal = arFrac - (double)whole;
	
	QString numStr = "1";
	QString decStr = QString::number(decimal);
	for(int z=0; z<decStr.length()-2; z++){
		numStr += "0";
	}
	
	int num = numStr.toInt();
	int dec = (int)(decimal*num);
	
	for(int z=2; z<dec+1; z++)
	{
		if(dec%z==0 && num%z==0)
		{
			dec = dec/z;
			num = num/z;
			z=2;
		}
	}
// 	if(String(decimal).indexOf("33") == 0 && String(num).indexOf("100") == 0)
// 	{
// 		decimal = 1;
// 		num = 3;
// 	}
// 	if(String(decimal).indexOf("1666") == 0)
// 	{
// 		decimal = 1;
// 		num = 6;
// 	}
// 	
// 	if(String(decimal).indexOf("66") == 0 && String(num).indexOf("100") == 0)
// 	{
// 		decimal = 2;
// 		num = 3;
// 	}
	
	//return ((whole==0)?"" : whole+" ")+
	//	(isNaN(decimal) ? "" : decimal+"/"+num);
	qDebug() << "PercentPositionWidget::updateSizeUI(): AR num"<<num<<", dec:"<<dec;
	if(num<=99 && dec<=99)
	{
		m_arLeft->setValue(num);
		m_arRight->setValue(dec);
	}
	
	m_lockUpdateSizeUI = false;
	
}
/*

private:
	LiveLayer *m_layer;
	bool m_topLock;
	bool m_leftLock;
	bool m_bottomLock;
	bool m_rightLock;
	bool m_lockAspectRatio;
	bool m_editPixels;
	double m_lockToAR;
	
	QSpinBox *m_arLeft;
	QSpinBox *m_arRight;
	QDoubleSpinBox *m_posTop;
	QDoubleSpinBox *m_posLeft;
	QDoubleSpinBox *m_posBottom;
	QDoubleSpinBox *m_posRight;
	QDoubleSpinBox *m_sizeWidth;
	QDoubleSpinBox *m_sizeHeight;*/


