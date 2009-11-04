#include "MainWindow.h"
#include ".build/ui_MainWindow.h"
#include <QCloseEvent>
#include "AppSettings.h"
#include "OutputInstance.h"
#include "ConnectDialog.h"
#include "SingleOutputSetupDialog.h"
#include "NetworkClient.h"

#include <QMessageBox>
#include <QApplication>
#include <QSettings>

#define RECONNECT_WAIT_TIME 1000 * 2

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
	
	setWindowTitle("Network Viewer - DViz");
	setWindowIcon(QIcon(":/data/icon-d.png"));
	
	connect(m_ui->actionConnect_To, SIGNAL(triggered()), this, SLOT(slotConnect()));
	connect(m_ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(slotDisconnect()));
	connect(m_ui->actionSetup_Outputs, SIGNAL(triggered()), this, SLOT(slotOutputSetup()));
	connect(m_ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
	
	m_ui->actionDisconnect->setEnabled(false);
	m_ui->actionConnect_To->setEnabled(true);
	
	m_ui->actionConnect_To->setIcon(QIcon(":/data/stock-connect.png"));
	m_ui->actionDisconnect->setIcon(QIcon(":/data/stock-disconnect.png"));
	m_ui->actionSetup_Outputs->setIcon(QIcon(":data/stock-preferences.png"));
	
	m_ui->textEdit->setReadOnly(true);
	// pmp00750
	log("Welcome to the DViz Network Viewer!");
	
	QSettings s;
	bool flag = s.value("viewer/firstrun").toBool();
	if(flag)
	{
		s.setValue("viewer/firstrun",false);
		slotOutputSetup();
	}
}

void MainWindow::openOutput()
{
	//qDebug() << "MainWindow::openOutput()";
	if(m_inst)
		delete m_inst;
		
	QList<Output*> outputs = AppSettings::outputs();
	
	Output * out = outputs.first();
	m_inst = new OutputInstance(out); // start hidden
	// not going to use this signal since it should be handeled from the server
	//connect(inst, SIGNAL(nextGroup()), this, SLOT(nextGroup()));
	
	// start hidden until connected
	m_inst->setVisible(false);
}

void MainWindow::log(const QString& msg, int)
{
	m_ui->textEdit->append(msg);
}

// void MainWindow::slotClientError(const QString& error)
// {
// 	log(QString("[ERROR] %1").arg(error));
// 	QMessageBox::critical(this,"Network Error",QString("%1").arg(error));
// 	slotDisconnect();
// 	//if(m_reconnect)
// 	//	slotReconnect();
// }
// 

void MainWindow::socketError(QAbstractSocket::SocketError socketError)
{
	switch (socketError) 
	{
		case QAbstractSocket::RemoteHostClosedError:
			break;
		case QAbstractSocket::HostNotFoundError:
			QMessageBox::critical(0,"Host Not Found",tr("The host was not found. Please check the host name and port settings."));
			break;
		case QAbstractSocket::ConnectionRefusedError:
			if(m_reconnect)
				QTimer::singleShot(RECONNECT_WAIT_TIME, this, SLOT(slotReconnect()));
			else
				QMessageBox::critical(0,"Connection Refused",
					tr("The connection was refused by the peer. "
						"Make sure the DViz server is running, "
						"and check that the host name and port "
						"settings are correct."));
			break;
		default:
			QMessageBox::critical(0,"Connection Problem",tr("The following error occurred: %1.").arg(m_client->errorString()));
	}
}


void MainWindow::slotConnect()
{
	//qDebug() << "MainWindow::slotConnect()";
	if(m_client)
		slotDisconnect();
	
	ConnectDialog d;
	if(d.exec())
	{
		m_host = d.host();
		m_port = d.port();
		m_reconnect = d.reconnect();
		
		
		slotReconnect();
	}
}

void MainWindow::slotReconnect()
{
	//qDebug() << "MainWindow::slotReconnect()";
	
	// reopen output inorder to reset all settings such as
	// black frame, fade speed, etc
	openOutput();
	
	m_client = new NetworkClient(this);
	m_client->setInstance(m_inst);
	m_client->setLogger(this);
	connect(m_client, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	connect(m_client, SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectChanged(double)));
	connect(m_client, SIGNAL(socketConnected()), this, SLOT(slotConnected()));
	
	if(m_reconnect)
		connect(m_client, SIGNAL(socketDisconnected()), this, SLOT(slotReconnect()));
	
	log(QString("[INFO] Connecting to to %1:%2 ...").arg(m_host).arg(m_port));
	
	m_client->connectTo(m_host,m_port);
	
	m_ui->actionDisconnect->setEnabled(true);
	m_ui->actionConnect_To->setEnabled(false);
}

void MainWindow::aspectChanged(double d)
{
	qDebug() << "MainWindow::aspectChanged(): "<<d;
	m_aspect = d;
	emit aspectRatioChanged(d);
}

void MainWindow::slotConnected()
{
	log("[INFO] Connected!");
	m_inst->setVisible(true);	
}

void MainWindow::slotDisconnect()
{
	//qDebug() << "MainWindow::slotDisconnect()";
	m_ui->actionDisconnect->setEnabled(false);
	m_ui->actionConnect_To->setEnabled(true);
	
	m_inst->setVisible(false);
			
	if(m_client)
	{
		//qDebug() << "MainWindow::slotDisconnect(): Exiting client";
		m_client->exit();
		disconnect(m_client,0,this,0);
		delete m_client;
		m_client = 0;
		log("[INFO] Disconnected");
	}
}

void MainWindow::slotOutputSetup()
{
	SingleOutputSetupDialog d(this);
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

void MainWindow::setLiveGroup(SlideGroup *newGroup, Slide *currentSlide,bool)
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
