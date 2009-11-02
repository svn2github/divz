#include "MainWindow.h"
#include ".build/ui_MainWindow.h"
#include <QCloseEvent>
#include "AppSettings.h"
#include "OutputInstance.h"
#include "ConnectDialog.h"
#include "OutputSetupDialog.h"
#include "NetworkClient.h"

#include <QMessageBox>
#include <QApplication>

MainWindow * MainWindow::static_mainWindow = 0;

MainWindow::MainWindow(QWidget *parent) 
	: QMainWindow(parent)
	, m_ui(new Ui::MainWindow)
	, m_inst(0)
	, m_aspect(-1)
	, m_client(0)
{
	static_mainWindow = this;
	m_ui->setupUi(this);
	
	connect(m_ui->actionConnect_To, SIGNAL(triggered()), this, SLOT(slotConnect()));
	connect(m_ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(slotDisconnect()));
	connect(m_ui->actionSetup_Outputs, SIGNAL(triggered()), this, SLOT(slotOutputSetup()));
	connect(m_ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
	
	m_ui->actionDisconnect->setEnabled(false);
	m_ui->actionConnect_To->setEnabled(true);
	
	m_ui->actionConnect_To->setIcon(QIcon(":/data/stock-connect.png"));
	m_ui->actionDisconnect->setIcon(QIcon(":/data/stock-disconnect.png"));
	m_ui->actionSetup_Outputs->setIcon(QIcon(":data/stock-preferences.png"));
	
	QList<Output*> outputs = AppSettings::outputs();
	if(!outputs.isEmpty())
	{
		Output * out = outputs.first();
		m_inst = new OutputInstance(out);
		// not going to use this signal since it should be handeled from the server
		//connect(inst, SIGNAL(nextGroup()), this, SLOT(nextGroup()));
	}
	else
	{
		slotOutputSetup();
	}
	
	m_ui->textEdit->setReadOnly(true);
	// pmp00750
	log("Welcome to the DViz Network Viewer!");
}

void MainWindow::log(const QString& msg, int)
{
	m_ui->textEdit->append(msg);
}

void MainWindow::slotClientError(const QString& error)
{
	log(QString("[ERROR] %1").arg(error));
	QMessageBox::critical(this,"Network Error",QString("%1").arg(error));
	slotDisconnect();
}

void MainWindow::slotConnect()
{
	if(m_client)
		slotDisconnect();
	
	ConnectDialog d;
	if(d.exec())
	{
		m_client = new NetworkClient(this);
		m_client->setInstance(m_inst);
		m_client->setLogger(this);
		connect(m_client, SIGNAL(error(const QString&)), this, SLOT(slotClientError(const QString&)));
		connect(m_client, SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectChanged(double)));
		
		log(QString("[INFO] Connecting to to %1:%2 ...").arg(d.host()).arg(d.port()));
		
		m_client->connectTo(d.host(),d.port());
		
			
		m_ui->actionDisconnect->setEnabled(false);
		m_ui->actionConnect_To->setEnabled(true);
	}
}

void MainWindow::aspectChanged(double d)
{
	m_aspect = d;
	emit aspectRatioChanged(d);
}

void MainWindow::slotDisconnect()
{
	m_ui->actionDisconnect->setEnabled(false);
	m_ui->actionConnect_To->setEnabled(true);
			
	if(m_client)
	{
		m_client->exit();
		disconnect(m_client,0,this,0);
		delete m_client;
		m_client = 0;
		log("[INFO] Disconnected");
	}
}

void MainWindow::slotOutputSetup()
{
	OutputSetupDialog d(this);
	if(d.exec())
		emit appSettingsChanged();
}

void MainWindow::slotExit()
{
	qApp->quit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	slotDisconnect();
	if(m_inst)
		m_inst->close();
}


MainWindow::~MainWindow()
{
	delete m_ui;
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

void MainWindow::setLiveGroup(SlideGroup *newGroup, Slide *currentSlide)
{
	(void)0;
}

QRect MainWindow::standardSceneRect(double aspectRatio)
{
	if(aspectRatio < 0)
	{
		aspectRatio = AppSettings::liveAspectRatio();
	}
	
	int height = 768;
	return QRect(0,0,aspectRatio * height,height);
}
