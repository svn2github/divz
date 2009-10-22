#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>

#include "AppSettings.h"
#include "AppSettingsDialog.h"
#include "DocumentSettingsDialog.h"
#include "SlideGroupSettingsDialog.h"

#include "model/SlideGroupFactory.h"

#include "songdb/SongSlideGroup.h"
#include "songdb/SongRecord.h"
#include "songdb/SongBrowser.h"


MainWindow * MainWindow::static_mainWindow = 0;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow),
	m_liveView(0),
	m_docModel(0),
	m_doc(0),
	m_viewControl(0),
	m_editWin(0)
	
{
	static_mainWindow = this;
	
	m_ui->setupUi(this);
	
	m_docModel = new DocumentListModel();
	
	if(!openFile("song-test.xml"))
		actionNew();
	
	// init the editor win AFTER load/new so that editor win starts out with the correct aspect ratio	
	m_editWin = new SlideEditorWindow();
	
	// setup live view before central widget because central widget uses live view in the view control code
	m_liveView = new SlideGroupViewer();
	m_liveView->setWindowFlags(Qt::FramelessWindowHint);
	m_liveView->setBackground(Qt::black);
	m_liveView->setCursor(Qt::BlankCursor);
	connect(m_liveView, SIGNAL(nextGroup()), this, SLOT(nextGroup()));

	setupOutputViews();
	

	setupCentralWidget();

	setupSongList();
	
	//setupMediaBrowser();


	// Restore state
	loadWindowState();

	

	//m_ui->actionEdit_Slide_Group->setEnabled(false);
	QList<QAction*> actionList;

	m_ui->actionOpen->setIcon(QIcon(":data/stock-open.png"));
	actionList << m_ui->actionOpen;
	m_ui->actionOpen->setToolTip(m_ui->actionOpen->text() + "(" + m_ui->actionOpen->shortcut().toString() + ")");
	connect(m_ui->actionOpen, SIGNAL(triggered()), this, SLOT(actionOpen()));
	
	m_ui->actionSave->setIcon(QIcon(":data/stock-save.png"));
	actionList << m_ui->actionSave;
	connect(m_ui->actionSave, SIGNAL(triggered()), this, SLOT(actionSave()));
	
	m_ui->actionSave_As->setIcon(QIcon(":data/stock-save.png"));
	connect(m_ui->actionSave_As, SIGNAL(triggered()), this, SLOT(actionSaveAs()));
	actionList << m_ui->actionSave_As;
	
	m_ui->actionNew->setIcon(QIcon(":data/stock-new.png"));
	connect(m_ui->actionNew, SIGNAL(triggered()), this, SLOT(actionNew()));
	actionList << m_ui->actionNew;

	m_ui->actionNew_Slide_Group->setIcon(QIcon(":data/stock-add.png"));
	connect(m_ui->actionNew_Slide_Group, SIGNAL(triggered()), this, SLOT(actionNewGroup()));
	actionList << m_ui->actionNew_Slide_Group;
	
	m_ui->actionEdit_Slide_Group->setIcon(QIcon(":data/stock-edit.png"));
	m_ui->actionEdit_Slide_Group->setEnabled(false);
	connect(m_ui->actionEdit_Slide_Group, SIGNAL(triggered()), this, SLOT(actionEditGroup()));
	actionList << m_ui->actionEdit_Slide_Group;
	
	m_ui->actionSlide_Group_Properties->setIcon(QIcon(":data/stock-properties.png"));
	m_ui->actionSlide_Group_Properties->setEnabled(false);
	connect(m_ui->actionSlide_Group_Properties, SIGNAL(triggered()), this, SLOT(actionGroupProperties()));
	actionList << m_ui->actionSlide_Group_Properties;
	
	m_ui->actionDelete_Slide_Group->setIcon(QIcon(":data/stock-delete.png"));
	m_ui->actionDelete_Slide_Group->setEnabled(false);
	connect(m_ui->actionDelete_Slide_Group, SIGNAL(triggered()), this, SLOT(actionDelGroup()));
	actionList << m_ui->actionDelete_Slide_Group;
	
	

	m_ui->actionApp_Settings->setIcon(QIcon(":data/stock-preferences.png"));
	connect(m_ui->actionApp_Settings, SIGNAL(triggered()), this, SLOT(actionAppSettingsDialog()));
	actionList << m_ui->actionApp_Settings;
	
	m_ui->actionDoc_Settings->setIcon(QIcon(":data/stock-properties.png"));
	connect(m_ui->actionDoc_Settings, SIGNAL(triggered()), this, SLOT(actionDocSettingsDialog()));
	actionList << m_ui->actionDoc_Settings;

	connect(m_ui->actionAbout_DViz, SIGNAL(triggered()), this, SLOT(actionAboutDviz()));
	connect(m_ui->actionVisit_DViz_Website, SIGNAL(triggered()), this, SLOT(actionDvizWebsite()));
	
	foreach(QAction *action, actionList)
	{
		QString shortcut = action->shortcut().toString();
		if(!shortcut.trimmed().isEmpty())
		{
			QString text = action->text();
			text.replace("&","");
			action->setText(QString("%1 (%2)").arg(text).arg(shortcut));
		}
	}



	//connect(m_groupView, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotListContextMenu(const QPoint &)));

	//connect(m_groupView,SIGNAL(activated(const QModelIndex &)),this,SLOT(groupSetLive(const QModelIndex &)));
	
	m_previewWidget = new SlideGroupViewer(m_ui->dwPreview);
	m_previewWidget->setSceneContextHint(MyGraphicsScene::Preview);
	m_ui->dwPreview->setWidget(m_previewWidget);
	
	
	
	m_liveView->show();
	
	//setLiveGroup(m_doc.groupList().at(0));
	
	connect(m_ui->actionExit,SIGNAL(triggered()), this, SLOT(close()));
	
	
}


MainWindow::~MainWindow()
{
	// done in close event now
	//actionSave();
	
	clearAllOutputs();
	
	
	delete m_ui;
	
	delete m_liveView;
	m_liveView = 0;
	
	delete m_docModel;
	delete m_doc;
	delete m_editWin;
}

void MainWindow::loadWindowState()
{
	QSettings settings;
	QSize sz = settings.value("mainwindow/size").toSize();
	if(sz.isValid())
		resize(sz);
	QPoint p = settings.value("mainwindow/pos").toPoint();
	if(!p.isNull())
		move(p);
	restoreState(settings.value("mainwindow/state").toByteArray());
	m_splitter->restoreState(settings.value("mainwindow/splitter_state").toByteArray());
	m_splitter2->restoreState(settings.value("mainwindow/splitter2_state").toByteArray());
	m_songBrowser->restoreState(settings.value("mainwindow/songbrowser_state").toByteArray());

}

void MainWindow::saveWindowState()
{
	QSettings settings;
	settings.setValue("mainwindow/size",size());
	settings.setValue("mainwindow/pos",pos());
	settings.setValue("mainwindow/state",saveState());
	settings.setValue("mainwindow/splitter_state",m_splitter->saveState());
	settings.setValue("mainwindow/splitter2_state",m_splitter2->saveState());
	settings.setValue("mainwindow/songbrowser_state",m_songBrowser->saveState());
}

void MainWindow::clearAllOutputs()
{
	m_liveView->clear();
	m_previewWidget->clear();
	qDebug() << "MainWindow::clearAllOutputs: Releasing preview slides\n";
	
	
	if(m_viewControl)
		m_viewControl->releaseSlideGroup();
}

void MainWindow::actionOpen()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select DViz File"), m_doc->filename(), tr("DViz XML File (*.xml);;Any File (*.*)"));
	if(fileName != "")
	{
		if(openFile(fileName))
		{
			return;
		}
		else
		{
			QMessageBox::critical(this,"File Does Not Exist","Sorry, but the file you chose does not exist. Please try again.");
		}
	}
}

bool MainWindow::actionSave()
{
	if(m_doc->filename().isEmpty())
		return actionSaveAs();
	else
		saveFile();
	return true;
		
}

bool MainWindow::actionSaveAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Choose a Filename"), m_doc->filename(), tr("DViz XML File (*.xml);;Any File (*.*)"));
	if(fileName != "")
	{
		saveFile(fileName);
		return true;
	}
	
	return false;
}

void MainWindow::actionNew()
{
	if(m_doc)
	{
		clearAllOutputs();
		
		m_docModel->releaseDocument();
		delete m_doc;
	}
		
	m_doc = new Document();
	
	Slide * slide = new Slide();
	SlideGroup *g = new SlideGroup();
	g->addSlide(slide);
	m_doc->addGroup(g);
	//m_scene->setSlide(slide);

	m_docModel->setDocument(m_doc);
	
	setWindowTitle("New File - DViz");
}

bool MainWindow::openFile(const QString & file)
{
	if(!QFile(file).exists())
		return false;
	
	double oldAspect = 0;
	
	if(m_doc)
	{
		oldAspect = m_doc->aspectRatio();
		if(!m_doc->filename().isEmpty())
			m_doc->save();
		
		clearAllOutputs();
		
		m_docModel->releaseDocument();
		delete m_doc;
	}
	
	m_doc = new Document(file);
		
	//m_doc->load(file);
	//r.readSlide(m_slide);
	setWindowTitle(QFileInfo(file).fileName() + " - DViz");
	
	//qDebug()<<"MainWindow::open(): "<<file<<", oldAspect:"<<oldAspect<<", new:"<<m_doc->aspectRatio();
	if(oldAspect != m_doc->aspectRatio())
	{
		//qDebug()<<"MainWindow::open(): emitting aspectRatioChanged()"; 
		emit aspectRatioChanged(m_doc->aspectRatio());
	}
		
// 	qDebug() << "MainWindow::open(): m_docModel->setDocument() - start";
// 	printf("MainWindow::open(): m_docModel ptr: %p\n",m_docModel);
	m_docModel->setDocument(m_doc);
//	qDebug() << "MainWindow::open(): m_docModel->setDocument() - end";
	
	return true;
}

void MainWindow::saveFile(const QString & file)
{
	m_doc->save(file.isEmpty() ? m_doc->filename() : file);
	
	setWindowTitle(QFileInfo(m_doc->filename()).fileName() + " - DViz");
	
	saveWindowState();
	
}

void MainWindow::setupSongList()
{
	QVBoxLayout * baseLayout = new QVBoxLayout(m_ui->tabSongs);
	baseLayout->setContentsMargins(0,0,0,0);
	
	m_songBrowser = new SongBrowser();
	baseLayout->addWidget(m_songBrowser);
	
	connect(m_songBrowser, SIGNAL(songSelected(SongRecord*)), this, SLOT(songSelected(SongRecord*)));
}

void MainWindow::songSelected(SongRecord *song)
{
	SongSlideGroup *group = new SongSlideGroup();
	group->setSong(song);
	m_doc->addGroup(group);
}

void MainWindow::setupOutputViews()
{
    	Output *out = AppSettings::outputs().at(0);
	if(out && out->name() == "Live")
	{
		Output::OutputType x = out->outputType();
		if(x == Output::Screen || x == Output::Custom)
		{
			QRect geom;
			if(x == Output::Screen)
			{
				int screenNum = out->screenNum();
				QDesktopWidget *d = QApplication::desktop();
				geom = d->screenGeometry(screenNum);
			}
			else
			{
				geom = out->customRect();
				m_liveView->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
			}

			m_liveView->resize(geom.width(),geom.height());
			m_liveView->move(geom.left(),geom.top());
		}
		else
		//if(x == Output::Network)
		{
			qDebug("Warning: Output to network not supported yet. Still to be written.");
		}
	}
	else
	{
		QRect geom = QApplication::desktop()->availableGeometry();
		m_liveView->resize(geom.width(),geom.height());
		m_liveView->move(0,0);
		m_liveView->setWindowTitle("Live");
		qDebug("Debug: Screen 0 was null or not named Live, setting to default geometry.");

	}
	
	m_liveView->setVisible(true);
}


void MainWindow::groupsDropped(QList<SlideGroup*> list)
{
	QModelIndex idx = m_docModel->indexForGroup(list.first());
	m_groupView->setCurrentIndex(idx);
}


void MainWindow::setupCentralWidget()
{

	m_splitter = new QSplitter(this);
	//m_splitter->setOrientation(Qt::Vertical);
	setCentralWidget(m_splitter);

	// left side
	m_groupView = new QListView(this);
	
	m_groupView->setViewMode(QListView::ListMode);
	//m_groupView->setViewMode(QListView::IconMode);
	m_groupView->setMovement(QListView::Free);
	m_groupView->setWordWrap(true);
	m_groupView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_groupView->setDragEnabled(true);
	m_groupView->setAcceptDrops(true);
	m_groupView->setDropIndicatorShown(true);
	m_groupView->setEditTriggers(QAbstractItemView::EditKeyPressed); 
	
	m_groupView->setModel(m_docModel);
	m_groupView->setContextMenuPolicy(Qt::ActionsContextMenu);
	m_groupView->insertAction(0,m_ui->actionEdit_Slide_Group);
	m_groupView->insertAction(0,m_ui->actionSlide_Group_Properties);
	m_groupView->insertAction(0,m_ui->actionNew_Slide_Group);
	m_groupView->insertAction(0,m_ui->actionDelete_Slide_Group);
	
	
	connect(m_docModel, SIGNAL(groupsDropped(QList<SlideGroup*>)), this, SLOT(groupsDropped(QList<SlideGroup*>)));
	
	connect(m_groupView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(groupSelected(const QModelIndex &)));
	connect(m_groupView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(groupDoubleClicked(const QModelIndex &)));
	
	m_splitter->addWidget(m_groupView);
	
	


	// right side
	m_splitter2 = new QSplitter(m_splitter);
	m_splitter2->setOrientation(Qt::Vertical);
	m_splitter->addWidget(m_splitter2);

	QWidget *topright = new QWidget(m_splitter);
	QVBoxLayout * layout = new QVBoxLayout(topright);
	layout->setMargin(0);
	m_splitter2->addWidget(topright);

	// output controls at top
	m_outputList = new QListWidget();
	m_outputList->setMinimumSize(QSize(1,20));
	layout->addWidget(m_outputList);

	m_btnSendOut = new QPushButton("Send to Output");
	layout->addWidget(m_btnSendOut);

	// live view control at bottom
	m_outputTabs = new QTabWidget();
	m_splitter2->addWidget(m_outputTabs);
	
	SlideGroupFactory *factory;// = SlideGroupFactory::factoryForType(g->groupType());
	//if(!factory)
		factory = SlideGroupFactory::factoryForType(SlideGroup::Generic);
	
	if(factory)
	{
		m_viewControl = factory->newViewControl();
		
		m_viewControl->setOutputView(m_liveView);
		
		m_outputTabs->addTab(m_viewControl,"Live");
	}
	

	setupOutputList();
	setupOutputControl();
}

void MainWindow::setupOutputList()
{
	QList<Output*> allOut = AppSettings::outputs();

	QList<Output*> outputs;
	foreach(Output *x, allOut)
		if(x->isEnabled())
			outputs << x;

	QListWidget * tbl = m_outputList;
	tbl->clear();

	//tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
	//connect(tbl, SIGNAL(cellClicked(int,int)), this, SLOT(slotOutputListCellActivated(int,int)));

	//tbl->setColumnCount(1);
	//tbl->setHorizontalHeaderLabels(QStringList() << "Name");
	//tbl->setRowCount(outputs.size());

	QListWidgetItem *prototype = new QListWidgetItem();
	// setup your prototype
	prototype->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);

	foreach(Output*out, outputs)
	{
		QListWidgetItem *t = prototype->clone();
		t->setText(out->name());
		t->setCheckState(Qt::Checked);
		tbl->addItem(t);
	}

}

void MainWindow::setupOutputControl()
{

}


void MainWindow::actionAboutDviz()
{
	QString ver = "";

#ifdef VER
	ver = QString(", Ver %1").arg(VER);
#endif

	QMessageBox msgBox(QMessageBox::NoIcon,"About DViz",QString("DViz%1\n\"Tasty Breakfast\" Release\n(c) 2009 Josiah Bryan").arg(ver));
	msgBox.exec();
}

void MainWindow::actionDvizWebsite()
{
	QMessageBox msgBox(QMessageBox::Information,"Visit DViz Website","For more info, go to http://code.google.com/p/dviz");
	msgBox.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if(m_doc->filename().isEmpty())
	{
		switch(QMessageBox::question(this,"File Not Saved","This file has not yet been saved - do you want to give it a file name? Click 'Save' to save the document to a file, click 'Discard' to continue closing and loose all changes, or click 'Cancel' to cancel closing and return to the program.",
			QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel))
		{
			case QMessageBox::Save:
				if(!actionSaveAs())
					event->ignore();
				else
				{
					event->accept();
					m_liveView->hide();
				}
				return;
			case QMessageBox::Discard:
				event->accept();
				m_liveView->hide();
				return;
			default:
				event->ignore();
				return;
		};
	}
	else
	if(actionSave())
	{
		event->accept();
		m_liveView->hide();
		//close();
	} 
	else 
	{
		event->ignore();
	}

	//deleteLater();
}


/*
void MainWindow::slotListContextMenu(const QPoint &pos)
{
	QModelIndex idx = m_groupView->currentIndex();
	SlideGroup *s = m_docModel.groupFromIndex(idx);
	qDebug() << "MainWindow::slotListContextMenu(): selected group#:"<<s->groupNumber()<<", title:"<<s->groupTitle()<<", pos:"<<pos<<", mapToGlobal:"<<mapToGlobal(pos);

	QMenu contextMenu(tr("Context menu"), this);
	contextMenu.addAction(new QAction(tr("Hello"), this));
	contextMenu.exec(mapToGlobal(pos));
}
*/

void MainWindow::actionAppSettingsDialog()
{
	//OutputSetupDialog *d = new OutputSetupDialog(this);
	AppSettingsDialog *d = new AppSettingsDialog(this);
	d->exec();

	emit appSettingsChanged();
	setupOutputViews();
	setupOutputList();
	setupOutputControl();
}

void MainWindow::actionDocSettingsDialog()
{
	//OutputSetupDialog *d = new OutputSetupDialog(this);
	double ar = m_doc->aspectRatio();
	DocumentSettingsDialog *d = new DocumentSettingsDialog(m_doc,this);
	d->exec();
	if(ar != m_doc->aspectRatio())
		emit aspectRatioChanged(m_doc->aspectRatio());
	
}


void MainWindow::groupSelected(const QModelIndex &idx)
{
	SlideGroup *s = m_docModel->groupFromIndex(idx);
        //qDebug() << "MainWindow::groupSelected(): selected group#:"<<s->groupNumber()<<", title:"<<s->groupTitle();
	//openSlideEditor(s);
	previewSlideGroup(s);
	m_ui->actionEdit_Slide_Group->setEnabled(true);
	m_ui->actionDelete_Slide_Group->setEnabled(true);
	m_ui->actionSlide_Group_Properties->setEnabled(true);
	
}

void MainWindow::nextGroup()
{
	QModelIndex idx = m_groupView->currentIndex();
	int nextRow = idx.row() + 1;
	if(nextRow >= m_doc->numGroups())
		nextRow = 0;
		
	SlideGroup *nextGroup = m_docModel->groupAt(nextRow);
	setLiveGroup(nextGroup);
	
	QModelIndex nextIdx = m_docModel->indexForGroup(nextGroup);
	m_groupView->setCurrentIndex(nextIdx);
}

void MainWindow::previewSlideGroup(SlideGroup *s)
{
	//qDebug() << "MainWindow::previewSlideGroup: Loading preview slide\n";
	m_previewWidget->setSlideGroup(s);
}

void MainWindow::setLiveGroup(SlideGroup *newGroup, Slide *currentSlide)
{
	//SlideGroup *s = m_docModel->groupFromIndex(idx);
        //qDebug() << "MainWindow::groupSelected(): groupSetLive group#:"<<s->groupNumber()<<", title:"<<s->groupTitle();
	//openSlideEditor(s);
	//m_previewWidget->clear();
	
	SlideGroup * oldGroup = m_liveView->slideGroup();
	
	if(!m_liveView->isVisible())
		m_liveView->show();
	
	//qDebug() << "MainWindow::setLiveGroup(): newGroup->groupType():"<<newGroup->groupType()<<", SlideGroup::Generic:"<<SlideGroup::Generic;
	if((oldGroup && oldGroup->groupType() != newGroup->groupType()) || newGroup->groupType() != SlideGroup::Generic)
	{
		SlideGroupFactory *factory = SlideGroupFactory::factoryForType(newGroup->groupType());
		if(!factory)
		{
			//qDebug() << "MainWindow::setLiveGroup(): Factory fell thu for request, going to generic control";
			factory = SlideGroupFactory::factoryForType(SlideGroup::Generic);
		}
		
		if(factory)
		{
			//qDebug() << "MainWindow::setLiveGroup(): got new factory, initalizing";
			m_outputTabs->removeTab(m_outputTabs->indexOf(m_viewControl));
			if(m_viewControl)
			{
				delete m_viewControl;
				m_viewControl = 0;
			}	
			
			m_viewControl = factory->newViewControl();
			m_viewControl->setOutputView(m_liveView);
			m_outputTabs->addTab(m_viewControl,"Live");
		}
	}
	
	//qDebug() << "MainWindow::setLiveGroup: Loading into view control";
	m_viewControl->setSlideGroup(newGroup,currentSlide);
	//qDebug() << "MainWindow::setLiveGroup: Loading into LIVE output";
	m_liveView->setSlideGroup(newGroup,currentSlide);
	//qDebug() << "MainWindow::setLiveGroup: Loading into LIVE output (done)";
	m_viewControl->setFocus(Qt::OtherFocusReason);
	
}

void MainWindow::groupDoubleClicked(const QModelIndex &idx)
{
	SlideGroup *g = m_docModel->groupFromIndex(idx);
        //qDebug() << "MainWindow::groupSelected(): double-clicked group#:"<<g->groupNumber()<<", title:"<<g->groupTitle();
	setLiveGroup(g);
}

void MainWindow::editGroup(SlideGroup *g, Slide *slide)
{
	statusBar()->showMessage(QString("Loading %1...").arg(g->groupTitle().isEmpty() ? QString("Group %1").arg(g->groupNumber()) : g->groupTitle()));
	openSlideEditor(g,slide);
	//m_previewWidget->clear();
	statusBar()->clearMessage();
}

void MainWindow::actionEditGroup()
{
	QModelIndex idx = m_groupView->currentIndex();
	if(!idx.isValid())
		return;
	SlideGroup *group = m_docModel->groupFromIndex(idx);
	Slide *slide = 0;
	if(m_liveView->slideGroup() == group)
		slide = m_viewControl->selectedSlide();
		
	editGroup(group,slide);
}

void MainWindow::actionGroupProperties()
{
	QModelIndex idx = m_groupView->currentIndex();
	if(!idx.isValid())
		return;
	SlideGroup *s = m_docModel->groupFromIndex(idx);
	SlideGroupSettingsDialog d(s,this);
	d.adjustSize();
	d.exec();
}


void MainWindow::actionNewGroup()
{
	Slide * slide = new Slide();
	SlideGroup *g = new SlideGroup();
	g->addSlide(slide);
	m_doc->addGroup(g);
}

void MainWindow::actionDelGroup()
{
	QModelIndex idx = m_groupView->currentIndex();
	if(!idx.isValid())
		return;
	SlideGroup *s = m_docModel->groupFromIndex(idx);
	deleteGroup(s);
}

void MainWindow::deleteGroup(SlideGroup *s)
{
	m_doc->removeGroup(s);
}


void MainWindow::openSlideEditor(SlideGroup *group,Slide *slide)
{
	if(group->groupType() != SlideGroup::Generic)
	{
		SlideGroupFactory *factory = SlideGroupFactory::factoryForType(group->groupType());
		if(!factory)
			factory = SlideGroupFactory::factoryForType(SlideGroup::Generic);
		
		if(factory)
		{
			AbstractSlideGroupEditor * editor = factory->newEditor();
			editor->setSlideGroup(group,slide);
			editor->show();
			return;
		}
	}
	
	
	// WHY ??????
	// If we setSlideGroup BEFORE but not AFTER, then newslide btn works only after clicking twice (So to speak)
	// If AFTER and not BEFORE, then list is BLANK unless click new btn a few times. WHY????fs
	m_editWin->setSlideGroup(group,slide);
	m_editWin->show();
	m_editWin->setSlideGroup(group,slide);
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) 
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}


QRect MainWindow::standardSceneRect(double aspectRatio)
{
	if(aspectRatio < 0)
	{
		if(m_doc)
			aspectRatio = m_doc->aspectRatio();
		else
			aspectRatio = 4/3;
	}
	
	int height = 768;
	return QRect(0,0,aspectRatio * height,height);
}
