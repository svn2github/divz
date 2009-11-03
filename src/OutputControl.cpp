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
#include "model/Output.h"

#include "MainWindow.h"
#include "OutputInstance.h"
#include "AppSettings.h"

#include "itemlistfilters/SlideTextOnlyFilter.h"

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

	
	m_advancedButton = new QPushButton(QIcon(":/data/stock-go-down.png"), "Advanced");
	m_advancedButton->setCheckable(true);
	hbox1->addWidget(m_advancedButton);
	
	connect(m_advancedButton, SIGNAL(toggled(bool)), this, SLOT(setAdvancedWidgetVisible(bool)));
	
	

	layout->addLayout(hbox1);
	
	// Setup advanced widget (shown/hidden with "advanced" button, above)
	m_advancedWidget = new QWidget(this);
	setupAdvancedUI();
	layout->addWidget(m_advancedWidget);
	
	m_advancedWidget->setVisible(false);
	
	
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


void OutputControl::setAdvancedWidgetVisible(bool flag)
{
	m_advancedWidget->setVisible(flag);
	m_advancedButton->setIcon(flag ? QIcon(":/data/stock-go-up.png") : QIcon(":/data/stock-go-down.png"));
	if(m_advancedButton->isChecked() != flag)
		m_advancedButton->setChecked(flag);
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


void OutputControl::setupAdvancedUI()
{

	QVBoxLayout * layout = new QVBoxLayout(m_advancedWidget);
	layout->setMargin(0);
		
	QGridLayout * gridLayout = new QGridLayout();
	QLabel *label;
	int rowNbr = 0;
	
	// row: add seperator line
	rowNbr++;
	QFrame * line_3 = new QFrame();
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        gridLayout->addWidget(line_3,rowNbr,0,1,2);
	
	
	// Row: Sync/live setting
	rowNbr++;
	label = new QLabel("Control Type: ");
	gridLayout->addWidget(label,rowNbr,0);
	
	m_syncStatusSelector = new QComboBox(m_advancedWidget);
	m_syncStatusSelector->addItem("Live");
	m_syncStatusSelector->addItem("Synced");
	
	gridLayout->addWidget(m_syncStatusSelector,rowNbr,1);
	connect(m_syncStatusSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(setIsOutputSynced(int)));
	
	// Row: Output sync source
	rowNbr++;
	
	label = new QLabel("Sync With: ");
	gridLayout->addWidget(label,rowNbr,0);
	
	m_syncWithBox = new QComboBox();
	setupSyncWithBox();
	gridLayout->addWidget(m_syncWithBox,rowNbr,1);
	
	m_syncWithBox->setEnabled(false);
	
	connect(m_syncWithBox, SIGNAL(currentIndexChanged(int)), this, SLOT(syncOutputChanged(int)));
	
	// MUST be created after main window, so allow segfault if no main window
	MainWindow *mw = MainWindow::mw();
	connect(mw, SIGNAL(appSettingsChanged()), this, SLOT(setupSyncWithBox()));
	
	
	// row: add seperator line
	rowNbr++;
	QFrame * line_1 = new QFrame();
        line_1->setFrameShape(QFrame::HLine);
        line_1->setFrameShadow(QFrame::Sunken);
        gridLayout->addWidget(line_1,rowNbr,0,1,2);
        
        // row: add overlay
	rowNbr++;
	
	//label = new QLabel("Text-only Filter: ");
	label = new QLabel("Overlay: ");
	gridLayout->addWidget(label,rowNbr,0);
	
	
	QHBoxLayout * hbox1 = new QHBoxLayout();
	
	m_overlayDocBox = new QLineEdit(this);
	m_overlayDocBox->setReadOnly(true);
	
	//connect(m_overlayDocBox, SIGNAL(textChanged(const QString&)), this, SLOT(overlayDocumentChanged(const QString&)));
	hbox1->addWidget(m_overlayDocBox,2); // large stretch
	
	QPushButton *btn1 = new QPushButton("Browse");
	hbox1->addWidget(btn1);
	
	connect(m_overlayDocBox, SIGNAL(textChanged(const QString&)), this, SLOT(overlayDocumentChanged(const QString&)));
	connect(btn1, SIGNAL(clicked()), this, SLOT(overlayBrowseClicked()));
	
	gridLayout->addLayout(hbox1,rowNbr,1);
	
	// row: add preview & btn below browse
	rowNbr++;
	
	QHBoxLayout * hbox2 = new QHBoxLayout();
	
	hbox2->addStretch(1);
	
	m_overlayPreviewLabel = new QLabel("(No Overlay)");
	hbox2->addWidget(m_overlayPreviewLabel);
	
	hbox2->addStretch(1);
	
	m_overlayEnabledBtn = new QPushButton("&Enable Overlay");
	m_overlayEnabledBtn->setCheckable(true);
	m_overlayEnabledBtn->setEnabled(false);
	hbox2->addWidget(m_overlayEnabledBtn);
	
	connect(m_overlayEnabledBtn, SIGNAL(toggled(bool)), this, SLOT(setOverlayEnabled(bool)));
	
	gridLayout->addLayout(hbox2,rowNbr,1);
	
	// row: add seperator line
	rowNbr++;
	QFrame * line_2 = new QFrame();
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        gridLayout->addWidget(line_2,rowNbr,0,1,2);
	
	
		
	// Row: Resize top textbox
	rowNbr++;
	label = new QLabel("Resize Top Textbox: ");
	gridLayout->addWidget(label,rowNbr,0);
	
	m_textResizeBtn = new QPushButton("Intelli-Size Top Textbox");
	m_textResizeBtn->setCheckable(true);
	//m_textResizeBtn->setEnabled(false);
	gridLayout->addWidget(m_textResizeBtn,rowNbr,1);
	
	connect(m_textResizeBtn, SIGNAL(toggled(bool)), this, SLOT(setTextResizeEnabled(bool)));
//	connect(m_textFilterBtn, SIGNAL(toggled(bool)), m_textResizeBtn, SLOT(setEnabled(bool)));
	
	// Row: Text only background
	rowNbr++;
	label = new QLabel("Text-only Background: ");
	gridLayout->addWidget(label,rowNbr,0);
	
	m_colorPicker = new QtColorPicker();
	m_colorPicker->setStandardColors();
	m_colorPicker->setCurrentColor(Qt::black);
	gridLayout->addWidget(m_colorPicker,rowNbr,1);
	
	connect(m_colorPicker, SIGNAL(colorChanged(const QColor &)), this, SLOT(setTextOnlyBackground(const QColor &)));
//	connect(textFilterBtn, SIGNAL(toggled(bool)), m_colorPicker, SLOT(setEnabled(bool)));
	
	// Row: Text only filter on/off
	rowNbr++;
	
	//label = new QLabel("Text-only Filter: ");
	label = new QLabel("Filters: ");
	gridLayout->addWidget(label,rowNbr,0,Qt::AlignTop);
	
// 	m_textFilterBtn = new QPushButton("Only Show Text");
// 	m_textFilterBtn->setCheckable(true);
// 	gridLayout->addWidget(m_textFilterBtn,rowNbr,1);
// 	
// 	connect(m_textFilterBtn, SIGNAL(toggled(bool)), this, SLOT(setTextOnlyFilterEnabled(bool)));

	m_filterList = new QListWidget();
	
	m_customFilterList = AbstractItemFilter::availableFilters();
	
	setupFilterList();
	
	gridLayout->addWidget(m_filterList,rowNbr,1);
	
	connect(m_filterList, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(filterListItemChanged(QListWidgetItem *)));
	
	
	
	
	layout->addLayout(gridLayout);
	//layout->addStretch(1);
	
	
}

void OutputControl::setupSyncWidgetUI()
{
	
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

void OutputControl::setupFilterList(AbstractItemFilterList selected)
{
	m_filterList->clear();

	for(int i=0; i<m_customFilterList.size(); i++)
	{
		AbstractItemFilter * impl = m_customFilterList[i];
		
		QListWidgetItem *t = new QListWidgetItem();
		t->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
		t->setCheckState(Qt::Unchecked);
		t->setText(impl->filterName());
		t->setData(Qt::UserRole + 100, i);
		
		if(impl->isMandatoryFor(outputInstance()))
		{
			t->setCheckState(Qt::Checked);
			t->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable);
			m_inst->addFilter(impl);
		}
		else
		if(selected.contains(impl))
		{
			t->setCheckState(Qt::Checked);
			m_inst->addFilter(impl);
		}
		
		m_filterList->addItem(t);
	}
}
	
void OutputControl::filterListItemChanged(QListWidgetItem *item)
{
	bool enabled = item->checkState() == Qt::Checked;
	QVariant var = item->data(Qt::UserRole + 100);
	if(var.isValid())
	{
		int idx = var.toInt();
	
		AbstractItemFilter * filter = m_customFilterList[idx];
		
		if(enabled)
			m_inst->addFilter(filter);
		else
			m_inst->removeFilter(filter);
	}
}

void OutputControl::syncOutputChanged(int idx)
{
	if(idx<0)
		return;
		
	int outputId = m_syncWithBox->itemData(idx).toInt();
	//Output * out = AppSettings::outputById(outputId);
	OutputInstance * inst = MainWindow::mw()->outputInst(outputId);
	
	if(!inst)
	{
		qDebug() << "OutputControl::syncOutputChanged(): idx:"<<idx<<", outputId:"<<outputId<<": output doesnt exist.";
	}
	else
	{
		setSyncSource(inst);
	}
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
		
		double iconHeight = 64.0;
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

void OutputControl::setTextResizeEnabled(bool flag)
{
	if(m_inst)
		m_inst->setAutoResizeTextEnabled(flag);
	if(m_textResizeBtn->isChecked() != flag)
		m_textResizeBtn->setChecked(flag);
}

void OutputControl::setTextOnlyFilterEnabled(bool flag)
{
	if(m_inst)
		m_inst->setTextOnlyFilterEnabled(flag);
	//if(m_textFilterBtn->isChecked() != flag)
	//	m_textFilterBtn->setChecked(flag);
	
	int idx;
	for(int i=0;i<m_customFilterList.size();i++)
		if(m_customFilterList[i]->inherits("SlideTextOnlyFilter"))
			idx = i, i = m_customFilterList.size();
			//indexOf(SlideTextOnlyFilter::instance());
	QListWidgetItem * item = m_filterList->item(idx);
	
	if(item)
	{
		bool enabled = item->checkState() == Qt::Checked;
		if(enabled != flag)
		{
			item->setCheckState(Qt::Checked);
			filterListItemChanged(item);
		}
	}
	else
	{
		qDebug() << "OutputControl::setTextOnlyFilterEnabled: ["<<flag<<"]: idx:"<<idx<<", QListWidgetItem was null, unable to update list widget";
		
		if(flag)
			m_inst->addFilter(SlideTextOnlyFilter::instance());
		else
			m_inst->removeFilter(SlideTextOnlyFilter::instance());
		
	}
}

void OutputControl::setTextOnlyBackground(const QColor & color)
{
	AbstractItemFilterList internalList = m_customFilterList;
	
	SlideTextOnlyFilter * textFilter = 0;
	
	foreach(AbstractItemFilter *filter, internalList)
	{
		if(!textFilter && filter->inherits("SlideTextOnlyFilter"))
			textFilter = dynamic_cast<SlideTextOnlyFilter*>(filter);
	}
	
	textFilter->setBackgroundColor(color);
	if(m_inst->hasFilter(textFilter))
	{
		// by removing, then adding again, this forces the output 
		// view to re-apply the filter with the new settings.
		// TODO maybe in future find a more efficient way of applying changes.
		m_inst->removeFilter(textFilter);
		m_inst->addFilter(textFilter);
	}
}

#define POINTER_STRING(ptr) QString().sprintf("%p",static_cast<void*>(ptr))
void OutputControl::setSyncSource(OutputInstance * inst)
{
	if(m_syncInst && m_inst) // && m_outputIsSynced)
	{
		//qDebug() << "OutputControl::setSyncSource: ["<<m_inst->output()->name()<<"] disconnecting from "<<m_syncInst->output()->name();
		disconnect(m_syncInst,0,m_inst,0);
	}
	
	m_syncInst = inst;
	
	if(m_outputIsSynced)
	{
		if(m_syncInst && m_inst)
		{
			connect(m_syncInst, SIGNAL(slideChanged(int)), m_inst, SLOT(setSlide(int)));	
			//qDebug() << "OutputControl::setSyncSource: ["<<m_inst->output()->name()<<"] connecting to "<<m_syncInst->output()->name();
		}
		else
			qDebug() << "OutputControl::setSyncSource(): output is synced but m_syncInst:"<<POINTER_STRING(m_syncInst)<<" or m_inst:"<<POINTER_STRING(m_inst);
	}
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
	
	if(m_ctrl)
		m_ctrl->setEnabled(!m_outputIsSynced);
	
	m_stack->setCurrentWidget( flag ? m_syncWidget : m_ctrl );
	
	m_syncWithBox->setEnabled(flag);
	
	if(m_syncStatusSelector->currentIndex() != (int)flag)
		m_syncStatusSelector->setCurrentIndex((int)flag);
}

void OutputControl::setOutputInstance(OutputInstance * inst)
{
	m_inst = inst;
	setupSyncWithBox();

	// not needed right now
	//connect(m_inst, SIGNAL(slideChanged(int)), this, SLOT(slideChanged(int)));

	// HACK need to default to app settings
	m_fadeSlider->setValue(25);
	
	setupFoldbackSettings();
		
}

void OutputControl::setupFoldbackSettings()
{
	if(m_inst->output()->tags().toLower().indexOf("foldback") >= 0 ||
	   m_inst->output()->name().toLower().indexOf("foldback") >= 0)
	{
		setIsOutputSynced(true);
		setTextOnlyFilterEnabled(true);
		setTextResizeEnabled(true);
		setAdvancedWidgetVisible(true);
		m_fadeSlider->setValue(10);
	}
}

void OutputControl::slideChanged(int)
{
// 	m_blackButton->setChecked(false);
// 	m_clearButton->setChecked(false);
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
	
	// need to re-apply fade speed
	m_fadeSlider->setValue(m_fadeSlider->value());
}

void OutputControl::setCustomFilters(AbstractItemFilterList list)
{
	AbstractItemFilterList selected;
	for(int i=0; i<m_customFilterList.size(); i++)
	{
		QListWidgetItem * item = m_filterList->item(i);
		if(item && item->checkState() == Qt::Checked)
			selected.append(m_customFilterList[i]);
	}
	
	m_inst->removeAllFilters();
	
	m_customFilterList = list;
	
	AbstractItemFilterList internalList = AbstractItemFilter::availableFilters();
	foreach(AbstractItemFilter *filter, internalList)
	{
		bool derivitiveFound = false;
		foreach(AbstractItemFilter *custom, m_customFilterList)
			if(custom->inherits(filter->metaObject()->className()))
				derivitiveFound = true;
		
		if(!derivitiveFound)
			m_customFilterList.append(filter);
	}
	
	setupFilterList(selected);
	
	setupFoldbackSettings();
}

void OutputControl::setCrossFadeSpeed(int value)
{
	double percent = ((double)value) / 100.0;
	double speed = 2000.0 * percent;
	if(speed<1)
		speed = 1;
	double quality = 0.05*speed; // "standard" quality is 10 frames every 250 ms
	double qualityMax = speed<  500 ? 20 :
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
