#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QDesktopWidget>

#include "AppSettings.h"


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{
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


	m_groupView->setModel(&m_docModel);
	//m_groupView->setContextMenuPolicy(Qt::CustomContextMenu);
	m_groupView->insertAction(0,m_ui->actionEdit_Slide_Group);
	m_groupView->insertAction(0,m_ui->actionNew_Slide_Group);
	m_groupView->insertAction(0,m_ui->actionDelete_Slide_Group);

	m_ui->actionEdit_Slide_Group->setEnabled(false);

	connect(m_ui->actionNew_Slide_Group, SIGNAL(triggered()), this, SLOT(actionNewGroup()));
	connect(m_ui->actionEdit_Slide_Group, SIGNAL(triggered()), this, SLOT(actionEditGroup()));
	connect(m_ui->actionDelete_Slide_Group, SIGNAL(triggered()), this, SLOT(actionDelGroup()));

	connect(m_ui->actionSetup_Outputs, SIGNAL(triggered()), this, SLOT(setupOutputs()));



	//connect(m_groupView, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotListContextMenu(const QPoint &)));

	//connect(m_groupView,SIGNAL(activated(const QModelIndex &)),this,SLOT(groupSetLive(const QModelIndex &)));
	connect(m_groupView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(groupSelected(const QModelIndex &)));
	connect(m_groupView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(groupDoubleClicked(const QModelIndex &)));

	m_previewWidget = new SlideGroupViewer(m_ui->dwPreview);
	m_previewWidget->scene()->setContextHint(MyGraphicsScene::Preview);
	m_ui->dwPreview->setWidget(m_previewWidget);
	
	m_liveView = new SlideGroupViewer();
	
	//m_liveMonitor = new OutputViewer(m_liveView, m_ui->dwLive);
	//m_ui->dwLive->setWidget(m_liveMonitor);
	
	
	//resize(2 * geom.width() / 3, 2 * geom.height() / 3);

	m_liveView->setWindowFlags(Qt::FramelessWindowHint);
	m_liveView->view()->setBackgroundBrush(Qt::black);

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
	
	m_liveView->show();
	
	connect(m_ui->actionExit,SIGNAL(activated()), qApp, SLOT(quit()));
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

void MainWindow::setupCentralWidget()
{

	m_splitter = new QSplitter(this);
	//m_splitter->setOrientation(Qt::Vertical);
	setCentralWidget(m_splitter);

	// left side
	m_groupView = new QListView(this);
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
	m_btnSendOut = new QPushButton("Send to Output");
	layout->addWidget(m_btnSendOut);

	m_outputList = new QListWidget();
	layout->addWidget(m_outputList);

	// live view control at bottom
	m_outputTabs = new QTabWidget();
	m_splitter2->addWidget(m_outputTabs);

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

void MainWindow::setupOutputs()
{
	OutputSetupDialog *d = new OutputSetupDialog(this);
	d->show();
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
	m_previewWidget->clear();
	m_liveView->setSlideGroup(s);
	if(!m_liveView->isVisible())
		m_liveView->show();
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
	m_previewWidget->clear();
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
