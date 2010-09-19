#include "OutputControl.h"

#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QActionGroup>
#include <QMenu>

#include "model/SlideTemplateManager.h"
#include "model/SlideGroupFactory.h"
#include "model/Document.h"
#include "model/SlideGroup.h"
#include "model/Slide.h"
#include "model/Output.h"

#include "MainWindow.h"
#include "OutputInstance.h"
#include "AppSettings.h"

#include "itemlistfilters/SlideTextOnlyFilter.h"
#include "itemlistfilters/SlideBackgroundOnlyFilter.h"

#define POINTER_STRING(ptr) QString().sprintf("%p",static_cast<void*>(ptr))

OutputControl::OutputControl(QWidget *parent) 
	: QWidget(parent)
	, m_inst(0)
	, m_ctrl(0)
	, m_syncInst(0)
	, m_overlayDoc(0)
	, m_overlaySlide(0)
	, m_outputIsSynced(false)
	, m_logoMenu(0)
	, m_editWin(0)
	, m_selectedLogo(0)
	, m_zoomSlider(0)
	, m_firstRun(true)
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
	
	m_logoButton = new QPushButton(QIcon(":/data/stock-media-pause.png"),"&Logo");
	m_logoButton->setCheckable(true);
	//m_logoButton->setEnabled(false); 
	hbox1->addWidget(m_logoButton);
	
	connect(m_logoButton, SIGNAL(toggled(bool)), this, SLOT(fadeLogoFrame(bool)));
	
	m_configLogo = new QPushButton(QIcon(":/data/stock-foo.png"),"");
	m_configLogo->setToolTip(tr("Setup Logos"));
	
	setupLogoMenu();
	
	//m_configLogo->setEnabled(false); 
	hbox1->addWidget(m_configLogo);
	
// 	hbox1->addStretch(1);

// // 	// fade speed 
	
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
	
	
// // 	// fade speed row
// // 	QHBoxLayout * hbox2 = new QHBoxLayout();
// 	
// 	QLabel *label = new QLabel("Fade Speed:");
// 	hbox2->addWidget(label);
// 	m_fadeSlider = new QSlider(Qt::Horizontal);
// 	m_fadeSlider->setMinimum(0);
// 	m_fadeSlider->setValue(1);
// 	m_fadeSlider->setMaximum(100);
// 	m_fadeSlider->setTickInterval(10);
// 	//m_fadeSlider->setSingleStep(5);
// 	m_fadeSlider->setTickPosition(QSlider::TicksBelow);
// 	hbox2->addWidget(m_fadeSlider,1);
// 	
// 	QSpinBox *edit = new QSpinBox();
// 	edit->setSuffix("%");
// 	edit->setValue(5);
// 	hbox2->addWidget(edit);
// 	
// 	connect(edit, SIGNAL(valueChanged(int)), m_fadeSlider, SLOT(setValue(int)));
// 	connect(m_fadeSlider, SIGNAL(valueChanged(int)), edit, SLOT(setValue(int)));
// 	connect(m_fadeSlider, SIGNAL(valueChanged(int)), this, SLOT(setCrossFadeSpeed(int)));
// 
// 	layout->addLayout(hbox2);
	
	
	// Setup advanced widget (shown/hidden with "advanced" button, above)
	m_advancedWidget = new QWidget(this);
	setupAdvancedUI();
	layout->addWidget(m_advancedWidget);
	
	m_advancedWidget->setVisible(false);
	
	
	// line right between "quick slide" and fade slider
	QFrame * line = new QFrame();
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	
	layout->addWidget(line);
	
	
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

void OutputControl::setupLogoMenu()
{
	if(m_logoMenu)
		delete m_logoMenu;
		
	m_logoMenu = new QMenu();
	
	
	QActionGroup * actionGroup = new QActionGroup(this);
	QAction * action;
	QAction * selectedAction = 0;
	
	QSize iconSize(48,0);
	QRect scene = MainWindow::mw()->standardSceneRect();
	qreal a = (qreal)scene.height() / (qreal)scene.width();
	iconSize.setHeight((int)(iconSize.width() * a));
	
	bool logoNotSet = !m_selectedLogo; 
	
	Document * doc = SlideTemplateManager::instance()->templateDocument(SlideTemplateManager::Logo);
	for(int i=0;i<doc->numGroups();i++)
	{
	
		SlideGroup * group = doc->at(i);
		action = m_logoMenu->addAction(group->assumedName());
			
		QPixmap icon;
	
		SlideGroupFactory *factory = SlideGroupFactory::factoryForType(group->groupType());
		if(!factory)
			factory = SlideGroupFactory::factoryForType(SlideGroup::GroupType);
		
		if(factory)
		{
			icon = factory->generatePreviewPixmap(group,iconSize,scene);
			action->setIcon(icon);
		}
		action->setCheckable(true);
		//action->setChecked(showVerseNumbers());
		//connect(action, SIGNAL(toggled(bool)), this, SLOT(setShowVerseNumbers(bool)));
		action->setData(group->groupId());
		
		if(logoNotSet)
		{
			m_selectedLogo = group;
			selectedAction = action;
		}
		else
		if(m_selectedLogo == group)
			selectedAction = action;
		
		
		actionGroup->addAction(action);
	}
	
	connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(logoActionTriggered(QAction*)));
	
	if(selectedAction)
		selectedAction->setChecked(true);
	
	m_logoMenu->addSeparator();
	
	action = m_logoMenu->addAction("Edit Selected Logo...");
	connect(action, SIGNAL(triggered()), this, SLOT(editLogo()));
	
	action = m_logoMenu->addAction("Add New Logo...");
	connect(action, SIGNAL(triggered()), this, SLOT(newLogo()));
	
	m_configLogo->setMenu(m_logoMenu);
}


void OutputControl::editLogo()
{
// 	if(selectedGroup())
// 		MainWindow::mw()->editGroup(selectedGroup());
		
 	SlideGroup * tmpl = m_selectedLogo;
 	if(!tmpl)
 		return;
 		
 	//MainWindow::mw()->editGroup(tmpl);
	
	if(m_editWin)
	{
		m_editWin->show();
		m_editWin->raise();
		m_editWin->setSlideGroup(tmpl);
	}
	else
	if((m_editWin = MainWindow::mw()->openSlideEditor(tmpl)) != 0)
		connect(m_editWin, SIGNAL(closed()), this, SLOT(logoEditorClosed()));
		
}


void OutputControl::logoEditorClosed()
{
	
// 	if(m_editWin)
// 		disconnect(m_editWin, 0, this, 0);
	
	SlideTemplateManager::instance()->templateDocument(SlideTemplateManager::Logo)->save();
	
  	//QTimer::singleShot(1000,m_editWin,SLOT(deleteLater()));
  	//m_editWin->deleteL
//  	m_editWin = 0;
	
	
	setupLogoMenu();
	
	//raise();
	//setFocus();
	//qDebug() << "SongEditorWindow::editorWindowClosed(): setting focus to self:"<<this<<", m_editWin="<<m_editWin;
}


void OutputControl::newLogo()
{
	Slide * slide = new Slide();
	SlideGroup *g = new SlideGroup();
	g->addSlide(slide);
	SlideTemplateManager::instance()->templateDocument(SlideTemplateManager::Logo)->addGroup(g);
	
	m_selectedLogo = g;
	editLogo();
}

void OutputControl::logoActionTriggered(QAction *action)
{
	int id = action->data().toInt();
	m_selectedLogo = SlideTemplateManager::instance()->findTemplate(SlideTemplateManager::Logo,id);
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
	QFrame * line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        gridLayout->addWidget(line,rowNbr,0,1,2);
        
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
	line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        gridLayout->addWidget(line,rowNbr,0,1,2);
	
	
		
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
	
// 	// Row: Text only background
// 	rowNbr++;
// 	label = new QLabel("Text-only Background: ");
// 	gridLayout->addWidget(label,rowNbr,0);
// 	
// 	m_colorPicker = new QtColorPicker();
// 	m_colorPicker->setStandardColors();
// 	m_colorPicker->setCurrentColor(Qt::black);
// 	gridLayout->addWidget(m_colorPicker,rowNbr,1);
// 	
// 	connect(m_colorPicker, SIGNAL(colorChanged(const QColor &)), this, SLOT(setTextOnlyBackground(const QColor &)));
// //	connect(textFilterBtn, SIGNAL(toggled(bool)), m_colorPicker, SLOT(setEnabled(bool)));
	
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
	
	// row: add seperator line
	rowNbr++;
	line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        gridLayout->addWidget(line,rowNbr,0,1,2);
	
	// Zoom / icon size
	rowNbr++;
	
	label = new QLabel("Icon Size:");
	gridLayout->addWidget(label,rowNbr,0);
	
	m_zoomSlider = new QSlider(Qt::Horizontal);
	m_zoomSlider->setMinimum(16);
	m_zoomSlider->setMaximum(480);
	m_zoomSlider->setTickInterval(16);
	m_zoomSlider->setSingleStep(16);
	m_zoomSlider->setPageStep(32);
	m_zoomSlider->setTickPosition(QSlider::TicksBelow);
	/*hbox->addWidget(m_zoomSlider,1);*/
	gridLayout->addWidget(m_zoomSlider,rowNbr,1);
	
	// ListMode & Wrap Mode
	rowNbr++;
	
	m_listModeCheckbox = new QCheckBox("List Mode");
	gridLayout->addWidget(m_listModeCheckbox,rowNbr,0);
	
	m_wrapModeCheckbox = new QCheckBox("Wrapping Enabled");
	gridLayout->addWidget(m_wrapModeCheckbox,rowNbr,1);
	
	layout->addLayout(gridLayout);
// 	
// 	connect(m_zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setIconSize(int)));
	
	
// 	gridLayout->addWidget(label,rowNbr,1);
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

void OutputControl::setupFilterList(AbstractItemFilterList selected, AbstractItemFilterList defaultEnabled)
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
		
		// TODO - fix bug wherein Song filter gets "stuck" when going from a song to regular group
		// HACK - ignore isMandatoryFor() until we can fix song filter bug
		
		// TODO - maybe? isMandatoryFor() is now officially buggy by design - not sure if it should be *fixed* or if the API should be changed.
		// See, right now, if isMandatoryFor() checks the outputInstance()->slideGroup() - it will be using the PREVIOUS slide group, not the new one.
		
// 		if(impl->isMandatoryFor(outputInstance()))
// 		{
// 			t->setCheckState(Qt::Checked);
// 			t->setFlags(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable);
// 			m_inst->addFilter(impl);
// 		}
// 		else
 		if(selected.contains(impl) 
//  			|| impl->isMandatoryFor(outputInstance()) 
 			|| defaultEnabled.contains(impl))
 		{
// 			qDebug() << "Enabling filter "<<impl->metaObject()->className()<<" because selected is "<<selected.contains(impl)
// 				//<<" || is madatory is "<<impl->isMandatoryFor(outputInstance())
// 				<<" || defaultEnabled contains is "<<defaultEnabled.contains(impl);
			
			t->setCheckState(Qt::Checked);
			m_inst->addFilter(impl);
			
			if(defaultEnabled.contains(impl))
			{
// 				qDebug()<<"Updating userRole+101";
				t->setData(Qt::UserRole + 101, true);
			}
		}
		else
		{
			//qDebug() << "**NOT** Enabling filter "<<impl->metaObject()->className()<<" for output "<< (outputInstance() ? outputInstance()->output()->name() : QString("[null outputInstance()]"))<<" because selected is "<<selected.contains(impl)<<" || is madatory is "<<impl->isMandatoryFor(outputInstance());
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
		if(idx>0)
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
			factory = SlideGroupFactory::factoryForType(SlideGroup::GroupType);
		
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
		if(m_customFilterList[i]->metaObject()->className() == "SlideTextOnlyFilter")
			idx = i, i = m_customFilterList.size();
			//indexOf(SlideTextOnlyFilter::instance());
	QListWidgetItem * item = m_filterList->item(idx);
	
	if(item)
	{
		bool enabled = item->checkState() == Qt::Checked;
		if(enabled != flag)
		{
			//qDebug() << "Enabling text only filter!";
			item->setCheckState(Qt::Checked);
			//filterListItemChanged(item);
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
			//connect(m_syncInst, SIGNAL(slideGroupChanged(SlideGroup*,Slide*)), m_inst, SLOT(setSlideGroup(SlideGroup*,Slide*)));
			connect(m_syncInst, SIGNAL(slideChanged(Slide*)), m_inst, SLOT(setSlide(Slide*)));
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
	
	
// 	qDebug() << "OutputControl::setIsOutputSynced: flag:"<<flag;
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
	setTextResizeEnabled(false);
// 	if(m_inst->output()->tags().toLower().indexOf("foldback") >= 0 ||
// 	   m_inst->output()->name().toLower().indexOf("foldback") >= 0)
	if(m_inst->output()->tags().toLower().indexOf("live") < 0 &&
	   m_inst->output()->name().toLower().indexOf("live") < 0)
	{
		//setIsOutputSynced(true);
		if(m_outputIsSynced || m_firstRun)
		{
			m_firstRun = false;
			setIsOutputSynced(true);
	// 		setTextOnlyFilterEnabled(true);
	// 		setTextResizeEnabled(true);
			setAdvancedWidgetVisible(true);
			m_fadeSlider->setValue(0);
		}
	}
}

void OutputControl::slideChanged(int)
{
// 	m_blackButton->setChecked(false);
// 	m_clearButton->setChecked(false);
}

void OutputControl::fadeBlackFrame(bool flag) 
{
	//if(m_ctrl)
		//m_ctrl->fadeBlackFrame(flag);
	if(m_inst)
	{
			
		m_inst->fadeBlackFrame(flag);
		
		if(m_ctrl)
		{
			SlideGroup *group = m_inst->slideGroup();
			
			if(group)
			{
				if(flag)
				{
					m_timerWasActiveBeforeFade = m_ctrl->timerState() == SlideGroupViewControl::Running;
					if(m_timerWasActiveBeforeFade)
						m_ctrl->toggleTimerState(SlideGroupViewControl::Stopped);
				}
				else
				{
					if(m_timerWasActiveBeforeFade)
						m_ctrl->toggleTimerState(SlideGroupViewControl::Running);
				}
			}
		}
	}
		
	if(m_blackButton->isChecked() != flag)
		m_blackButton->setChecked(flag);
		
}

void OutputControl::fadeLogoFrame(bool flag)
{
	if(!m_selectedLogo)
		return;
		
	if(flag)
	{
		SlideGroup * group = m_inst->slideGroup();
		
		m_inst->setSlideGroup(m_selectedLogo);
	}
	else
	{
		if(m_ctrl && m_ctrl->slideGroup())
			m_inst->setSlideGroup(m_ctrl->slideGroup());
		else
			fadeBlackFrame(true);
	}
	
	if(m_logoButton->isChecked() != flag)
		m_logoButton->setChecked(flag);
}

void OutputControl::fadeClearFrame(bool flag) 
{
// 	if(m_ctrl)
// 		m_ctrl->fadeClearFrame(flag);

	int idx;
	for(int i=0;i<m_customFilterList.size();i++)
		if(m_customFilterList[i]->inherits("SlideBackgroundOnlyFilter"))
			idx = i, i = m_customFilterList.size();
			//indexOf(SlideTextOnlyFilter::instance());
	QListWidgetItem * item = m_filterList->item(idx);
	
	if(item)
	{
		bool enabled = item->checkState() == Qt::Checked;
		if(enabled != flag)
		{
			//qDebug() << "Enabling clear frame (background only filter)";
			item->setCheckState(flag ? Qt::Checked : Qt::Unchecked);
			//filterListItemChanged(item);
		}
	}
	else
// 	if(m_inst)
	{
		if(flag)
			m_inst->addFilter(SlideBackgroundOnlyFilter::instance());
		else
			m_inst->removeFilter(SlideBackgroundOnlyFilter::instance());
	}
		
	if(m_clearButton->isChecked() != flag)
		m_clearButton->setChecked(flag);
}

void OutputControl::setViewControl(SlideGroupViewControl *ctrl)
{
	//qDebug() << "OutputControl::setViewControl: ["<<m_inst->output()->name()<<"]: old ctrl:"<<POINTER_STRING(m_ctrl)<<", new ctrl:"<<POINTER_STRING(ctrl)<<", btns:"<<POINTER_STRING(m_blackButton)<<","<<POINTER_STRING(m_clearButton);
	if(m_ctrl)
	{
// 		if(m_zoomSlider)
// 		{
// 			disconnect(m_zoomSlider,0,m_ctrl,0);
// 			disconnect(m_ctrl,0,m_zoomSlider,0);
// 		}
		m_stack->removeWidget(m_ctrl);
	}
	
	m_ctrl = ctrl;
	
	m_stack->addWidget(m_ctrl);
	
	//qDebug() << "OutputControl::setViewControl: m_ctrl:"<<POINTER_STRING(ctrl);
	setIsOutputSynced(m_outputIsSynced);
	
	m_blackButton->setEnabled(true);
	m_clearButton->setEnabled(true);
	
	// need to re-apply fade speed
	m_fadeSlider->setValue(m_fadeSlider->value());
	
	if(m_zoomSlider)
	{
		if(ctrl->canSetIconSize())
		{
			connect(m_zoomSlider, SIGNAL(valueChanged(int)), ctrl, SLOT(setIconSize(int)));
			connect(ctrl, SIGNAL(iconSizeChanged(int)), m_zoomSlider, SLOT(setValue(int)));
		
			m_zoomSlider->setValue(ctrl->iconSize());
		}
		m_zoomSlider->setEnabled(ctrl->canSetIconSize());
		
		
		connect(m_listModeCheckbox, SIGNAL(toggled(bool)), ctrl, SLOT(setListModeEnabled(bool)));
		m_listModeCheckbox->setChecked(ctrl->listModeEnabled());
		
		connect(m_wrapModeCheckbox, SIGNAL(toggled(bool)), ctrl, SLOT(setWrapModeEnabled(bool)));
		m_wrapModeCheckbox->setChecked(ctrl->wrapModeEnabled());
	}
	
// 	if(m_inst)
// 	{
// 		if(flag)
// 			m_inst->addFilter(SlideBackgroundOnlyFilter::instance());
// 		else
// 			m_inst->removeFilter(SlideBackgroundOnlyFilter::instance());
// 	}
// 		
// 	if(m_clearButton->isChecked() != flag)
}

void OutputControl::setCustomFilters(AbstractItemFilterList list)
{
// 	qDebug() << "OutputControl::setCustomFilters(): start"; 
	AbstractItemFilterList selected;
	for(int i=0; i<m_customFilterList.size(); i++)
	{
		AbstractItemFilter * impl = m_customFilterList[i];
		QListWidgetItem * item = m_filterList->item(i);
		
		QVariant var = item->data(Qt::UserRole + 101);
		bool defaultEnabled = false;
		if(var.isValid())
			defaultEnabled = var.toBool();
// 		else
// 			qDebug() << "* Note: userRole+101 is not valid for classname "<<impl->metaObject()->className();
		
		if(item && item->checkState() == Qt::Checked
			//&& ! impl->isMandatoryFor(outputInstance())
			&& ! defaultEnabled)
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
	
	
	// The API has been changed for setupFilterList() to add a 2nd param with the list of
	// filters to be enabled by default.
	// This is because we are making the assumption that the list given to this method ('list'),
	// is desired (by the programmer) to default to 'enabled', rather than just options 
	// for the user to pick from. Note that we are not enabling ALL availableFilters() (below),
	// just the ones in the list passed in the argument variable called 'list'
	setupFilterList(selected,list);
	
	setupFoldbackSettings();
	
// 	qDebug() << "OutputControl::setCustomFilters(): end";
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
