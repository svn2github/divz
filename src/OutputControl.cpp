#include "OutputControl.h"

#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>

#include "model/SlideGroupFactory.h"
#include "model/Document.h"
#include "model/SlideGroup.h"
#include "model/Slide.h"

#include "MainWindow.h"
#include "OutputInstance.h"
#include "AppSettings.h"

#define POINTER_STRING(ptr) QString().sprintf("%p",static_cast<void*>(ptr))

OutputControl::OutputControl(QWidget *parent) 
	: QWidget(parent)
	, m_inst(0)
	, m_ctrl(0)
	, m_syncInst(0)
	, m_overlayDoc(0)
	, m_overlaySlide(0)
	, m_outputIsSynced(false)
{
	setupUI();
}

void OutputControl::setupUI()
{
	QVBoxLayout * layout = new QVBoxLayout(this);
	
	// setup first row: type selector, cross fade speed, and black/clear btns
	QHBoxLayout * hbox1 = new QHBoxLayout();
	
	m_syncStatusSelector = new QComboBox();
	m_syncStatusSelector->addItem("Live");
	m_syncStatusSelector->addItem("Synced");
	hbox1->addWidget(m_syncStatusSelector);
	connect(m_syncStatusSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(setIsOutputSynced(int)));
	
	QLabel *label = new QLabel("Fade Speed:");
	hbox1->addWidget(label);
	m_fadeSlider = new QSlider(Qt::Horizontal);
	m_fadeSlider->setMinimum(0);
	m_fadeSlider->setValue(1);
	m_fadeSlider->setMaximum(100);
	m_fadeSlider->setTickInterval(10);
	//m_fadeSlider->setSingleStep(5);
	m_fadeSlider->setTickPosition(QSlider::TicksBelow);
	hbox1->addWidget(m_fadeSlider,1);
	
	QSpinBox *edit = new QSpinBox();
	edit->setSuffix("%");
	edit->setValue(5);
	hbox1->addWidget(edit);
	
	connect(edit, SIGNAL(valueChanged(int)), m_fadeSlider, SLOT(setValue(int)));
	connect(m_fadeSlider, SIGNAL(valueChanged(int)), edit, SLOT(setValue(int)));
	connect(m_fadeSlider, SIGNAL(valueChanged(int)), this, SLOT(setCrossFadeSpeed(int)));
	
	m_blackButton = new QPushButton(QIcon(":/data/stock-media-stop.png"),"&Black");
	m_blackButton->setCheckable(true);
	m_blackButton->setEnabled(false); // enable when view control set
	hbox1->addWidget(m_blackButton);
	
	connect(m_blackButton, SIGNAL(toggled(bool)), this, SLOT(fadeBlackFrame(bool)));
	
	m_clearButton = new QPushButton(QIcon(":/data/stock-media-eject.png"),"&Clear");
	m_clearButton->setCheckable(true);
	m_clearButton->setEnabled(false); 
	hbox1->addWidget(m_clearButton);
	
	connect(m_clearButton, SIGNAL(toggled(bool)), this, SLOT(fadeClearFrame(bool)));
	
	layout->addLayout(hbox1);
	
	
	// setup second row: overlay selector, preview, and on/off btns
	QHBoxLayout * hbox2 = new QHBoxLayout();
	
	QLabel *label2 = new QLabel("Overlay:");
	hbox2->addWidget(label2);
	
	m_overlayDocBox = new QLineEdit(this);
	connect(m_overlayDocBox, SIGNAL(textChanged(const QString&)), this, SLOT(overlayDocumentChanged(const QString&)));
	hbox2->addWidget(m_overlayDocBox,2); // large stretch
	
	QPushButton *btn1 = new QPushButton("&Browse");
	hbox2->addWidget(btn1);
	
	connect(m_overlayDocBox, SIGNAL(textChanged(const QString&)), this, SLOT(overlayDocumentChanged(const QString&)));
	connect(btn1, SIGNAL(clicked()), this, SLOT(overlayBrowseClicked()));
	
	m_overlayPreviewLabel = new QLabel("(No Overlay)");
	hbox2->addWidget(m_overlayPreviewLabel);
	
	m_overlayEnabledBtn = new QPushButton("&Enable Overlay");
	m_overlayEnabledBtn->setCheckable(true);
	m_overlayEnabledBtn->setEnabled(false);
	hbox2->addWidget(m_overlayEnabledBtn);
	
	connect(m_overlayEnabledBtn, SIGNAL(toggled(bool)), this, SLOT(setOverlayEnabled(bool)));
	
	layout->addLayout(hbox2);
	
	// setup stacked widget
	m_stack = new QStackedWidget(this);
	layout->addWidget(m_stack);
	
	// first widget is the live control, to be set by user of this widget
	// ... nothing to do here till setViewControl() called...
	
	// second widget in stack is the sync control widget: 
	m_syncWidget = new QWidget(m_stack);
	
	// setup sync widget rows: sync with, filter on/off, background color, and resize text on/off
	setupSyncWidgetUI();
	m_stack->addWidget(m_syncWidget);
	
	

}

void OutputControl::overlayBrowseClicked()
{
	QString text = m_overlayDocBox->text();
	if(text.trimmed().isEmpty())
	{
		text = AppSettings::previousPath("overlays");
	}
	
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Overlay Document"), text, tr("DViz Document (*.xml *.dviz *.dvz *.dv);;Any File (*.*)"));
	if(fileName != "")
	{
		overlayDocumentChanged(fileName);
		m_overlayDocBox->setText(fileName);
		AppSettings::setPreviousPath("overlays",QFileInfo(fileName).absolutePath());
	}
}

void OutputControl::setupSyncWidgetUI()
{
	// setup sync widget rows: sync with, filter on/off, background color, and resize text on/off
	//QVBoxLayout * layout = new QVBoxLayout(m_syncWidget);
	
	QGridLayout * layout = new QGridLayout(m_syncWidget);
	
	QLabel *label;
	
	int rowNbr = 0;
	
	// Row 1: Output sync source
	label = new QLabel("Sync With: ");
	layout->addWidget(label,rowNbr,0);
	
	m_syncWithBox = new QComboBox();
	setupSyncWithBox();
	layout->addWidget(m_syncWithBox,rowNbr,1);
	
	connect(m_syncWithBox, SIGNAL(currentIndexChanged(int)), this, SLOT(syncOutputChanged(int)));
	
	// Row 2: Text only filter on/off
	rowNbr++;
	
	label = new QLabel("Text-only Filter: ");
	layout->addWidget(label,rowNbr,0);
	
	m_textFilterBtn = new QPushButton("Only Show Text");
	m_textFilterBtn->setCheckable(true);
	layout->addWidget(m_textFilterBtn,rowNbr,1);
	
	connect(m_textFilterBtn, SIGNAL(toggled(bool)), this, SLOT(setSyncTextOnlyFilterEnabled(bool)));
	
// 	// Row 3: Text only background
//	rowNbr++;
// 	label = new QLabel("Text-only Background: ");
// 	layout->addWidget(label,2,0);
// 	
// 	m_colorPicker = new QtColorPicker();
// 	m_colorPicker->setStandardColors();
// 	m_colorPicker->setCurrentColor(Qt::black);
// 	layout->addWidget(m_colorPicker,2,1);
// 	
	//connect(m_colorPicker, SIGNAL(colorChanged(const QColor &)), this, SLOT(setSyncBackground(const QColor &)));
	//connect(textFilterBtn, SIGNAL(toggled(bool)), m_colorPicker, SLOT(setEnabled(bool)));
	
	// Row 3: REsize top textbox
	rowNbr++;
	label = new QLabel("Resize Top Textbox: ");
	layout->addWidget(label,rowNbr,0);
	
	QPushButton *btn = new QPushButton("Intelli-Size Top Textbox");
	btn->setCheckable(true);
	btn->setEnabled(false);
	layout->addWidget(btn,rowNbr,1);
	
	connect(btn, SIGNAL(toggled(bool)), this, SLOT(setSyncTextResizeEnabled(bool)));
	connect(m_textFilterBtn, SIGNAL(toggled(bool)), btn, SLOT(setEnabled(bool)));
	
	// make the last row expand - i think it should, not tested tet
	rowNbr++;
	layout->setRowStretch(rowNbr,99);
	
	
	// MUST be created after main window, so allow segfault if no main window
	MainWindow *mw = MainWindow::mw();
		
	connect(mw, SIGNAL(appSettingsChanged()), this, SLOT(setupSyncWithBox()));
	
}

void OutputControl::setupSyncWithBox()
{
	m_syncWithBox->clear();
	QList<Output*> allOut = AppSettings::outputs();
	
	foreach(Output *out, allOut)
	{
		if(out->isEnabled() && (m_inst ? out->id() != m_inst->output()->id() : true))
		{
			m_syncWithBox->addItem(out->name(), QVariant(out->id()));
		}
	}
}

void OutputControl::syncOutputChanged(int idx)
{
	int outputId = m_syncWithBox->itemData(idx).toInt();
	//Output * out = AppSettings::outputById(outputId);
	OutputInstance * inst = MainWindow::mw()->outputInst(outputId);
	
	setSyncSource(inst);
}

void OutputControl::overlayDocumentChanged(const QString& filename)
{
	if(m_overlayDoc)
		delete m_overlayDoc;
		
	m_overlayDoc = new Document(filename);
	setOverlayDocument(m_overlayDoc);
}


bool OutputControl_group_num_compare(SlideGroup *a, SlideGroup *b)
{
	return (a && b) ? a->groupNumber() < b->groupNumber() : true;
}

bool OutputControl_slide_num_compare(Slide *a, Slide *b)
{
	return (a && b) ? a->slideNumber() < b->slideNumber() : true;
}


void OutputControl::setOverlayDocument(Document *doc)
{
	m_overlayDoc = doc;
	
	int sz = m_overlayDoc->numGroups();
	
	if(sz > 0)
	{
		// Sort the groups so that our "first group" matches the sort order the user sees in the editor
		QList<SlideGroup*> glist = m_overlayDoc->groupList();
		qSort(glist.begin(), glist.end(), OutputControl_group_num_compare);
		
		SlideGroup *g = glist[0];
		
		QPixmap icon;
	
		SlideGroupFactory *factory = SlideGroupFactory::factoryForType(g->groupType());
		if(!factory)
			factory = SlideGroupFactory::factoryForType(SlideGroup::Generic);
		
		double iconHeight = 32.0;
		if(factory)
			icon = factory->generatePreviewPixmap(g,QSize(doc->aspectRatio() * iconHeight,iconHeight),MainWindow::mw()->standardSceneRect(doc->aspectRatio()));
			
		m_overlayPreviewLabel->setPixmap(icon);
		
		m_overlayEnabledBtn->setEnabled(true);
		
		if(g->numSlides() > 0)
		{
			// subway mexican sweetshop mcdonalds
			QList<Slide*> slist = g->slideList();
			qSort(slist.begin(), slist.end(), OutputControl_slide_num_compare);
			Slide * slide = slist[0];
			
			if(!slide)
			{
				qDebug("setOverlayDocument: No slide at 0");
			}
			else
			{
				m_overlaySlide = slide;
				if(m_inst)
					m_inst->setOverlaySlide(slide);
				else
					qDebug("setOverlayDocument: No m_inst set to setOverlaySlide on.");
			}
		}
		else
		{
			qDebug("setOverlayDocument: No slides in group 0");
		}

	}
	else
	{
		qDebug("setOverlayDocument: No groups in document");
	}
}


void OutputControl::setOverlayEnabled(bool flag)
{
	if(m_inst)
		m_inst->setOverlayEnabled(flag);
}

void OutputControl::setSyncTextResizeEnabled(bool flag)
{
	if(m_inst)
		m_inst->setAutoResizeTextEnabled(flag);
}

void OutputControl::setSyncTextOnlyFilterEnabled(bool flag)
{
	if(m_inst)
		m_inst->setTextOnlyFilterEnabled(flag);
	if(m_textFilterBtn->isChecked() != flag)
		m_textFilterBtn->setChecked(flag);
}

void OutputControl::setSyncSource(OutputInstance * inst)
{
	if(m_syncInst && m_outputIsSynced)
		disconnect(m_syncInst,0,this,0);
	
	m_syncInst = inst;
	
	if(m_outputIsSynced)
		connect(m_syncInst, SIGNAL(slideChanged(int)), m_inst, SLOT(setSlide(int)));	
}

void OutputControl::setIsOutputSynced(bool flag)
{
	m_outputIsSynced = flag;
	
	// disconnect/reconnect lost as needed
	if(!m_syncInst)
		syncOutputChanged(0);
	else
		setSyncSource(m_syncInst);
	
	
	//qDebug() << "OutputControl::setIsOutputSynced: flag:"<<flag;
	//qDebug() << "OutputControl::setViewControl: m_ctrl:"<<POINTER_STRING(m_ctrl)<<", m_syncWidget:"<<POINTER_STRING(m_syncWidget);
	
	m_stack->setCurrentWidget( flag ? m_syncWidget : m_ctrl );
	
	if(m_syncStatusSelector->currentIndex() != (int)flag)
		m_syncStatusSelector->setCurrentIndex((int)flag);
}

void OutputControl::setOutputInstance(OutputInstance * inst)
{
	m_inst = inst;
	setupSyncWithBox();

	if(inst->output()->tags().toLower().indexOf("foldback") >= 0 ||
	   inst->output()->name().toLower().indexOf("foldback") >= 0)
	{
		setIsOutputSynced(true);
		setSyncTextOnlyFilterEnabled(true);
	}
		
	connect(m_inst, SIGNAL(slideChanged(int)), this, SLOT(slideChanged(int)));

	// HACK - later need to default to app settings
	m_fadeSlider->setValue(25);
}

void OutputControl::slideChanged(int)
{
	m_blackButton->setChecked(false);
	m_clearButton->setChecked(false);
}

void OutputControl::fadeBlackFrame(bool flag) 
{
	if(m_ctrl)
		m_ctrl->fadeBlackFrame(flag);
}

void OutputControl::fadeClearFrame(bool flag) 
{
	if(m_ctrl)
		m_ctrl->fadeClearFrame(flag);
}

void OutputControl::setViewControl(SlideGroupViewControl *ctrl)
{
	//qDebug() << "OutputControl::setViewControl: ["<<m_inst->output()->name()<<"]: old ctrl:"<<POINTER_STRING(m_ctrl)<<", new ctrl:"<<POINTER_STRING(ctrl)<<", btns:"<<POINTER_STRING(m_blackButton)<<","<<POINTER_STRING(m_clearButton);
	if(m_ctrl)
		m_stack->removeWidget(m_ctrl);
	
	m_ctrl = ctrl;
	
	m_stack->addWidget(m_ctrl);
	
	//qDebug() << "OutputControl::setViewControl: m_ctrl:"<<POINTER_STRING(ctrl);
	setIsOutputSynced(m_outputIsSynced);
	
	m_blackButton->setEnabled(true);
	m_clearButton->setEnabled(true);
}

void OutputControl::setCrossFadeSpeed(int value)
{
	double percent = ((double)value) / 100.0;
	double speed = 1250.0 * percent;
	if(speed<1)
		speed = 1;
	double quality = 0.05*speed; // "standard" quality is 10 frames every 250 ms
	double qualityMax   = speed<  500 ? 20 :
			    speed< 1000 ? 30 :
			    speed< 2000 ? 40 :
			    speed<=3000 ? 50 : 30;
	quality = qMin(qualityMax,quality);
	if(quality<1)
		quality=1;
	//qDebug() << "OutputControl::setFadeSpeedPreset: value:"<<value<<" ("<<percent<<"), speed:"<<speed<<", quality:"<<quality<<" ( qualityMax:"<<qualityMax<<")";
	m_fadeSlider->setToolTip(QString("%1 % - %2 ms / %3 fames").arg(value).arg(speed).arg(quality));
		
	m_inst->setFadeSpeed(speed);
	m_inst->setFadeQuality(quality);
}

void OutputControl::setIsOutputSynced(int flag) 
{	
	setIsOutputSynced(flag == 1); 
}
