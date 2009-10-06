#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>

#include <QTableView>

#include <QLabel>

#include "AppSettings.h"
#include "AppSettingsDialog.h"
#include "DocumentSettingsDialog.h"

#include "model/SlideGroupFactory.h"

#include "songdb/SongSlideGroup.h"
#include "songdb/SongRecord.h"

MainWindow * MainWindow::static_mainWindow = 0;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow),
	m_liveView(0)
{
	static_mainWindow = this;
	
	m_ui->setupUi(this);
	

	if(QFile("test.xml").exists())
	{
		m_doc.load("test.xml");
		//r.readSlide(m_slide);
		setWindowTitle("test.xml - DViz");
	}
	else
	{
		Slide * slide = new Slide();
		SlideGroup *g = new SlideGroup();
		g->addSlide(slide);
		m_doc.addGroup(g);
		//m_scene->setSlide(slide);

		setWindowTitle("New Show - DViz");
	}
	
	m_docModel.setDocument(&m_doc);
	
	// setup live view before central widget because central widget uses live view in the view control code
	m_liveView = new SlideGroupViewer();
	m_liveView->setWindowFlags(Qt::FramelessWindowHint);
	m_liveView->view()->setBackgroundBrush(Qt::black);

	setupOutputViews();
	

	setupCentralWidget();

	// Restore state
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


	

	//m_ui->actionEdit_Slide_Group->setEnabled(false);

	connect(m_ui->actionNew_Slide_Group, SIGNAL(triggered()), this, SLOT(actionNewGroup()));
	connect(m_ui->actionEdit_Slide_Group, SIGNAL(triggered()), this, SLOT(actionEditGroup()));
	connect(m_ui->actionDelete_Slide_Group, SIGNAL(triggered()), this, SLOT(actionDelGroup()));

	connect(m_ui->actionApp_Settings, SIGNAL(triggered()), this, SLOT(actionAppSettingsDialog()));
	connect(m_ui->actionDoc_Settings, SIGNAL(triggered()), this, SLOT(actionDocSettingsDialog()));

	connect(m_ui->actionAbout_DViz, SIGNAL(triggered()), this, SLOT(actionAboutDviz()));
	connect(m_ui->actionVisit_DViz_Website, SIGNAL(triggered()), this, SLOT(actionDvizWebsite()));



	//connect(m_groupView, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotListContextMenu(const QPoint &)));

	//connect(m_groupView,SIGNAL(activated(const QModelIndex &)),this,SLOT(groupSetLive(const QModelIndex &)));
	
	m_previewWidget = new SlideGroupViewer(m_ui->dwPreview);
	m_previewWidget->scene()->setContextHint(MyGraphicsScene::Preview);
	m_ui->dwPreview->setWidget(m_previewWidget);
	
	
	
	m_liveView->show();
	
	//setLiveGroup(m_doc.groupList().at(0));
	
	connect(m_ui->actionExit,SIGNAL(activated()), qApp, SLOT(quit()));
	
	
	setupSongList();
}


MainWindow::~MainWindow()
{
	m_doc.save("test.xml");
	
	QSettings settings;
	settings.setValue("mainwindow/size",size());
	settings.setValue("mainwindow/pos",pos());
	settings.setValue("mainwindow/state",saveState());
	settings.setValue("mainwindow/splitter_state",m_splitter->saveState());
	settings.setValue("mainwindow/splitter2_state",m_splitter2->saveState());

	delete m_ui;
	
	delete m_liveView;
	m_liveView = 0;
}

void MainWindow::setupSongList()
{
// 	QTableView * m_songList;
// 	QLineEdit * m_songSearch;
// 	QComboBox * m_searchOpt;
	QVBoxLayout *vbox = new QVBoxLayout();
	
	QHBoxLayout *hbox = new QHBoxLayout();
	QLabel *label = new QLabel("Search:");
	m_songSearch = new QLineEdit(m_ui->tabSongs);
	//m_searchOpt = new QComboBox(m_ui->tabSongs);
	//QPushButton * searchBtn = new QPushButton(m_ui->tabSongs);
	hbox->addWidget(label);
	hbox->addWidget(m_songSearch);
	//hbox->addWidget(m_SearchOpt);
	//hbox->addWidget(searchBtn);
	connect(m_songSearch, SIGNAL(textChanged(const QString &)), this, SLOT(songFilterChanged(const QString &)));

	m_songList = new QTableView(m_ui->tabSongs);
	
	m_songTableModel = new QSqlTableModel(0,SongRecord::db());
	m_songTableModel->setTable("songs");
	
	m_songTableModel->select();
	//tbl->removeColumn(0); // don't show the ID
	m_songTableModel->removeColumn(0); //, Qt::Horizontal, tr("SongID"));
	m_songTableModel->setHeaderData(1, Qt::Horizontal, tr("Number"));
	m_songTableModel->setHeaderData(2, Qt::Horizontal, tr("Title"));
	m_songTableModel->removeColumn(3); //, Qt::Horizontal, tr("Text"));
	m_songTableModel->removeColumn(4); //, Qt::Horizontal, tr("Author"));
	m_songTableModel->removeColumn(5); //, Qt::Horizontal, tr("Copyright"));
	m_songTableModel->removeColumn(6); //6, Qt::Horizontal, tr("Last Used"));
	
	m_songProxyModel = new QSortFilterProxyModel(this);
	m_songProxyModel->setSourceModel(m_songTableModel);
	m_songProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);

	m_songList->setAlternatingRowColors(true);
	m_songList->setModel(m_songProxyModel);
	m_songList->setSortingEnabled(true);
	
	vbox->addLayout(hbox);
	vbox->addWidget(m_songList);
	m_ui->tabSongs->setLayout(vbox);
	connect(m_songList, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(songDoubleClicked(const QModelIndex &)));
}

void MainWindow::songDoubleClicked(const QModelIndex &idx)
{
	QSqlRecord record = m_songTableModel->record(idx.row());
	SongRecord *song = SongRecord::fromSqlRecord(record);
	SongSlideGroup *group = new SongSlideGroup();
	group->setSong(song);
	m_doc.addGroup(group);
}

void MainWindow::songFilterChanged(const QString &text)
{
	QRegExp regExp(text, Qt::CaseInsensitive, QRegExp::Wildcard);
	m_songProxyModel->setFilterRegExp(regExp);
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
}


void MainWindow::groupsDropped(QList<SlideGroup*> list)
{
	QModelIndex idx = m_docModel.indexForGroup(list.first());
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
	
	m_groupView->setModel(&m_docModel);
	m_groupView->setContextMenuPolicy(Qt::ActionsContextMenu);
	m_groupView->insertAction(0,m_ui->actionEdit_Slide_Group);
	m_groupView->insertAction(0,m_ui->actionNew_Slide_Group);
	m_groupView->insertAction(0,m_ui->actionDelete_Slide_Group);
	
	connect(&m_docModel, SIGNAL(groupsDropped(QList<SlideGroup*>)), this, SLOT(groupsDropped(QList<SlideGroup*>)));
	
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
	event->accept();
	m_liveView->hide();
	close();
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
	double ar = m_doc.aspectRatio();
	DocumentSettingsDialog *d = new DocumentSettingsDialog(&m_doc,this);
	d->exec();
	if(ar != m_doc.aspectRatio())
		emit aspectRatioChanged(m_doc.aspectRatio());
	
}


void MainWindow::groupSelected(const QModelIndex &idx)
{
	SlideGroup *s = m_docModel.groupFromIndex(idx);
        //qDebug() << "MainWindow::groupSelected(): selected group#:"<<s->groupNumber()<<", title:"<<s->groupTitle();
	//openSlideEditor(s);
	previewSlideGroup(s);
	m_ui->actionEdit_Slide_Group->setEnabled(true);
}

void MainWindow::previewSlideGroup(SlideGroup *s)
{
	m_previewWidget->setSlideGroup(s);
}

void MainWindow::setLiveGroup(SlideGroup *s)
{
	//SlideGroup *s = m_docModel.groupFromIndex(idx);
        //qDebug() << "MainWindow::groupSelected(): groupSetLive group#:"<<s->groupNumber()<<", title:"<<s->groupTitle();
	//openSlideEditor(s);
	//m_previewWidget->clear();
	
	SlideGroup * g = m_liveView->slideGroup();
	
	if(!m_liveView->isVisible())
		m_liveView->show();
		
	if(g && g->groupType() != s->groupType())
	{
		SlideGroupFactory *factory = SlideGroupFactory::factoryForType(s->groupType());
		if(!factory)
			factory = SlideGroupFactory::factoryForType(SlideGroup::Generic);
		
		if(factory)
		{
			m_outputTabs->removeTab(m_outputTabs->indexOf(m_viewControl));
			m_viewControl = factory->newViewControl();
			m_viewControl->setOutputView(m_liveView);
			m_outputTabs->addTab(m_viewControl,"Live");
		}
	}
	
	m_viewControl->setSlideGroup(s);
	m_liveView->setSlideGroup(s);
	
}

void MainWindow::groupDoubleClicked(const QModelIndex &idx)
{
	SlideGroup *g = m_docModel.groupFromIndex(idx);
        //qDebug() << "MainWindow::groupSelected(): double-clicked group#:"<<g->groupNumber()<<", title:"<<g->groupTitle();
	setLiveGroup(g);
}

void MainWindow::editGroup(SlideGroup *g)
{
	statusBar()->showMessage(QString("Loading %1...").arg(g->groupTitle().isEmpty() ? QString("Group %1").arg(g->groupNumber()) : g->groupTitle()));
	openSlideEditor(g);
	//m_previewWidget->clear();
	statusBar()->clearMessage();
}

void MainWindow::actionEditGroup()
{
	QModelIndex idx = m_groupView->currentIndex();
	SlideGroup *s = m_docModel.groupFromIndex(idx);
	editGroup(s);
}

void MainWindow::actionNewGroup()
{
	Slide * slide = new Slide();
	SlideGroup *g = new SlideGroup();
	g->addSlide(slide);
	m_doc.addGroup(g);
}

void MainWindow::actionDelGroup()
{
	QModelIndex idx = m_groupView->currentIndex();
	SlideGroup *s = m_docModel.groupFromIndex(idx);
	deleteGroup(s);
}

void MainWindow::deleteGroup(SlideGroup *s)
{
	m_doc.removeGroup(s);
}


void MainWindow::openSlideEditor(SlideGroup *g)
{
	m_editWin.setSlideGroup(g);
	m_editWin.show();
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
		aspectRatio = m_doc.aspectRatio();
	
	int height = 768;
	return QRect(0,0,aspectRatio * height,height);
}
