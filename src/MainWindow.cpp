#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QFile>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);
	
	// Restore state
	QSettings settings;
	QSize sz = settings.value("mainwindow/size").toSize();
	if(sz.isValid())
		resize(sz);
	QPoint p = settings.value("mainwindow/pos").toPoint();
	if(!p.isNull())
		move(p);
	restoreState(settings.value("mainwindow/state").toByteArray());
	
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
	m_ui->listView->setModel(&m_docModel);
	
	connect(m_ui->listView,SIGNAL(activated(const QModelIndex &)),this,SLOT(groupSelected(const QModelIndex &)));
	connect(m_ui->listView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(groupSelected(const QModelIndex &)));
	connect(m_ui->listView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(groupDoubleClicked(const QModelIndex &)));

	m_previewWidget = new SlideGroupViewer(m_ui->dwPreview);
	m_ui->dwPreview->setWidget(m_previewWidget);

}

MainWindow::~MainWindow()
{
	m_doc.save("test.xml");
	
	QSettings settings;
	settings.setValue("mainwindow/size",size());
	settings.setValue("mainwindow/pos",pos());
	settings.setValue("mainwindow/state",saveState());
	
	delete m_ui;
}


void MainWindow::groupSelected(const QModelIndex &idx)
{
	SlideGroup *s = m_docModel.groupFromIndex(idx);
	qDebug() << "MainWindow::groupSelected(): selected group#:"<<s->groupNumber()<<", title:"<<s->groupTitle();
	//openSlideEditor(s);
	m_previewWidget->setSlideGroup(s);
}

void MainWindow::groupDoubleClicked(const QModelIndex &idx)
{
	SlideGroup *g = m_docModel.groupFromIndex(idx);
	qDebug() << "MainWindow::groupSelected(): double-clicked group#:"<<g->groupNumber()<<", title:"<<g->groupTitle();
	statusBar()->showMessage(QString("Loading %1...").arg(g->groupTitle().isEmpty() ? QString("Group %1").arg(g->groupNumber()) : g->groupTitle()));
	openSlideEditor(g);
	statusBar()->clearMessage();
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
