#include "DirectorWindow.h"
#include "ui_DirectorWindow.h"
#include "PlayerSetupDialog.h"
#include "EditorWindow.h"
#include "../livemix/EditorUtilityWidgets.h"
#include "RtfEditorWindow.h"
#include "GLEditorGraphicsScene.h"
#include "../qtcolorpicker/qtcolorpicker.h"

#include "FlowLayout.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QApplication>
#include <QGLWidget>

#include "GLDrawables.h"
#include "GLSceneGroup.h"
#include "PlayerConnection.h"

#include "../livemix/VideoWidget.h"
#include "VideoInputSenderManager.h"
#include "VideoReceiver.h"

#include "DrawableDirectorWidget.h"

#include "GLWidget.h"

#include <QCDEStyle>

DirectorWindow::DirectorWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::DirectorWindow)
	, m_players(0)
{
	ui->setupUi(this);
	
	setupUI();
	
	// For some reason, these get unset from NULL between the ctor init above and here, so reset to zero
	m_players = 0;
	
	m_vidSendMgr = 0;
// 	m_vidSendMgr = new VideoInputSenderManager();
// 	m_vidSendMgr->setSendingEnabled(true);
	
	readSettings();
	
	// Connect to any players that have the 'autoconnect' option checked in the PlayerSetupDialog
// 	foreach(PlayerConnection *con, m_players->players())
// 		if(!con->isConnected() && con->autoconnect())
// 			con->connectPlayer();

	chooseOutput();

	setFocusPolicy(Qt::StrongFocus);
}


DirectorWindow::~DirectorWindow()
{
	delete ui;
}

void DirectorWindow::chooseOutput()
{
	if(!ui->mdiArea->subWindowList().isEmpty())
		ui->mdiArea->closeAllSubWindows();
	
	QDialog dlg;
	dlg.setWindowTitle("Choose Output");
	
	QVBoxLayout *vbox = new QVBoxLayout(&dlg);
		
	QHBoxLayout *lay1 = new QHBoxLayout();
	
	QComboBox *sourceBox = new QComboBox();
	QStringList itemList;
	
	QList<PlayerConnection*> playerList = m_players->players();
	qSort(playerList.begin(), playerList.end(), PlayerConnection::sortByUseCount);
	foreach(PlayerConnection *con, playerList)
	{
		itemList << QString("Player: %1").arg(con->name());
		//qDebug() << "DirectorWindow::choseOutput:"<<con->name()<<" useCount:"<<con->useCount();
	}
	sourceBox->addItems(itemList);
	sourceBox->setCurrentIndex(0);
	
	lay1->addWidget(new QLabel("Output:"));
	lay1->addWidget(sourceBox);
	 
	vbox->addLayout(lay1);
	
	QHBoxLayout *buttons = new QHBoxLayout();
	QPushButton *setup = new QPushButton("Setup");
	buttons->addWidget(setup);
	
	buttons->addStretch(1);
	
	QPushButton *ok = new QPushButton("ok");
	buttons->addWidget(ok);
	ok->setDefault(true);
	
	connect(setup, SIGNAL(clicked()), &dlg, SLOT(reject()));
	connect(ok, SIGNAL(clicked()), &dlg, SLOT(accept()));
	
	m_connected = false;
	
	//vbox->addStretch(1);
	vbox->addLayout(buttons);
	dlg.setLayout(vbox);
	dlg.adjustSize();
	if(dlg.exec())
	{
		m_hasVideoInputsList = false;
		
		int idx = sourceBox->currentIndex();
		PlayerConnection *player = playerList.at(idx);
		
		connect(player, SIGNAL(videoInputListReceived(const QStringList&)), this, SLOT(videoInputListReceived(const QStringList&)));
		
		foreach(PlayerConnection *con, m_players->players())
		{
			if(con->isConnected() && con != player)
				con->disconnectPlayer();
		}
		
		if(!player->isConnected())
			player->connectPlayer();
			
		if(player->videoIputsReceived())
			videoInputListReceived(player->videoInputs());
		
		m_connected = true;
		
		showPlayerLiveMonitor(player);
	}
	else
	{
		showPlayerSetupDialog();
	}
}


void DirectorWindow::videoInputListReceived(const QStringList& inputs)
{
	if(m_hasVideoInputsList)
		return;
	m_hasVideoInputsList = true;
	int index = 0;
	//qDebug() << "DirectorWindow::videoInputListReceived: "<<inputs;
	foreach(QString con, inputs)
	{
		QStringList opts = con.split(",");
		//qDebug() << "DirectorWindow::videoInputListReceived: Con string: "<<con;
		foreach(QString pair, opts)
		{
			QStringList values = pair.split("=");
			QString name = values[0].toLower();
			QString value = values[1];
			
			//qDebug() << "DirectorWindow::videoInputListReceived: Parsed name:"<<name<<", value:"<<value;
			if(name == "net")
			{
// 				QUrl url(value);
// 				
// 				QString host = url.host();
// 				int port = url.port();

				QStringList url = value.split(":");
				QString host = url[0];
				int port = url.size() > 1 ? url[1].toInt() : 7755;
				
				VideoReceiver *rx = VideoReceiver::getReceiver(host,port);
				
				if(!rx)
				{
					qDebug() << "DirectorWindow::videoInputListReceived: Unable to connect to "<<host<<":"<<port;
					QMessageBox::warning(this,"Video Connection",QString("Sorry, but we were unable to connect to the video transmitter at %1:%2 - not sure why.").arg(host).arg(port));
				}
				else
				{
					m_receivers << QPointer<VideoReceiver>(rx);
					
					qDebug() << "DirectorWindow::videoInputListReceived: Connected to "<<host<<":"<<port<<", creating widget...";
					
// 					VideoWidget *vid = new VideoWidget();
// 					m_currentVideoWidgets << vid;
// 					
// 					vid->setVideoSource(rx);
// 					vid->setOverlayText(QString("# %1").arg(m_currentVideoWidgets.size()));
// 					
// 					rx->setFPS(5);
// 					vid->setFps(5);
// 					vid->setRenderFps(true);

					GLWidget *vid = new GLWidget();
		
					GLVideoDrawable *gld = new GLVideoDrawable();
					gld->setVideoSource(rx);
					vid->addDrawable(gld);
					
					vid->setWindowTitle(QString("Camera %1").arg(++ index));
					gld->setObjectName(qPrintable(vid->windowTitle()));
					vid->resize(320,240);
					//vid->show();
					
					QMdiSubWindow *subWindow = new QMdiSubWindow;
					//subWindow->setStyle(new QCDEStyle());
					//subWindow->setWidget(new QPushButton(item->itemName()));
					subWindow->setWidget(vid);
					subWindow->setAttribute(Qt::WA_DeleteOnClose);
					//subWindow->adjustSize();
					//subWindow->setWindowFlags(Qt::FramelessWindowHint);
				
					ui->mdiArea->addSubWindow(subWindow);
					subWindow->show();
					subWindow->resize(320,270);
					
					connect(this, SIGNAL(closed()), vid, SLOT(deleteLater()));
					
					vid->setProperty("-vid-con-string",con);
		
					//m_videoViewerLayout->addWidget(vid);
					
					connect(vid, SIGNAL(clicked()), this, SLOT(videoInputClicked()));
				}
			}
		}
	}
}

void DirectorWindow::videoInputClicked()
{
	GLWidget *vid = dynamic_cast<GLWidget*>(sender());
	if(!vid)
	{
		qDebug() << "DirectorWindow::videoInputClicked: Sender is not a video widget, ignoring.";
		return;
	}
	//activateVideoInput(vid);
	
	QString con = vid->property("-vid-con-string").toString();
	
// 	if(!m_drawable)
// 		return;
	
	qDebug() << "DirectorWindow::videoInputClicked: Using con string: "<<con;
	
// 	m_drawable->setProperty("videoConnection", con);
// 	
// 	foreach(PlayerConnection *player, m_directorWindow->players()->players())
// 		player->setUserProperty(m_drawable, con, "videoConnection");
}


void DirectorWindow::showPlayerLiveMonitor()
{
	// Compile list of connected player names for the combo box selection
	QComboBox *sourceBox = new QComboBox();
	QStringList itemList;
	QList<PlayerConnection*> players;
	foreach(PlayerConnection *con, m_players->players())
		if(con->isConnected())
		{
			itemList << QString("Player: %1").arg(con->name());
			players << con;
		}
	
	// Dont prompt the user if only one player is connected
	if(players.size() == 1)
	{
		showPlayerLiveMonitor(players.first());
		return;
	}
	
	// Create the dialog box layout
	sourceBox->addItems(itemList);
	QDialog dlg;
	dlg.setWindowTitle("Choose Player");
	
	QVBoxLayout *vbox = new QVBoxLayout(&dlg);
	
	QFormLayout *form = new QFormLayout();
	 
	form->addRow("Player to View:", sourceBox);
	 
	QHBoxLayout *buttons = new QHBoxLayout();
	buttons->addStretch(1);
	QPushButton *cancel = new QPushButton("Cancel");
	buttons->addWidget(cancel);
	
	QPushButton *ok = new QPushButton("ok");
	buttons->addWidget(ok);
	ok->setDefault(true);
	
	connect(cancel, SIGNAL(clicked()), &dlg, SLOT(reject()));
	connect(ok, SIGNAL(clicked()), &dlg, SLOT(accept()));
	
	vbox->addLayout(form);
	vbox->addStretch(1);
	vbox->addLayout(buttons);
	dlg.setLayout(vbox);
	dlg.adjustSize();
	
	if(dlg.exec())
	{
		int idx = sourceBox->currentIndex();
		if(idx < 0 || idx >= players.size())
		{
			qDebug() << "Player selection error";
			return;	
		}
		
		PlayerConnection *con = players[idx];
		
		showPlayerLiveMonitor(con);
		
	}
}

void DirectorWindow::showPlayerLiveMonitor(PlayerConnection *con)
{
	QString host = con->host();
	int port = 9978;
	VideoReceiver *rx = VideoReceiver::getReceiver(host,port);
			
	if(!rx)
	{
		qDebug() << "DirectorWindow::showPlayerLiveMonitor: Unable to connect to "<<host<<":"<<port;
		QMessageBox::warning(this,"Video Connection",QString("Sorry, but we were unable to connect to the video transmitter at %1:%2 - not sure why.").arg(host).arg(port));
	}
	else
	{
		m_receivers << QPointer<VideoReceiver>(rx);
		
		qDebug() << "DirectorWindow::showPlayerLiveMonitor: Connected to "<<host<<":"<<port<<", creating widget...";
		
		//VideoWidget *vid = new VideoWidget();
		//qDebug() << "DirectorWindow::showPlayerLiveMonitor: Created VideoWidget:"<<vid;
		//vid->setVideoSource(rx);
		
		GLWidget *vid = new GLWidget();
		
		GLVideoDrawable *gld = new GLVideoDrawable();
		gld->setVideoSource(rx);
		vid->addDrawable(gld);
		
		vid->setWindowTitle(QString("Live - %1").arg(con->name()));
		gld->setObjectName(qPrintable(vid->windowTitle()));
		vid->resize(320,240);
		//vid->show();
		
		QMdiSubWindow *subWindow = new QMdiSubWindow;
		//subWindow->setStyle(new QCDEStyle());
		//subWindow->setWidget(new QPushButton(item->itemName()));
		subWindow->setWidget(vid);
		subWindow->setAttribute(Qt::WA_DeleteOnClose);
		//subWindow->adjustSize();
		//subWindow->setWindowFlags(Qt::FramelessWindowHint);
	
		ui->mdiArea->addSubWindow(subWindow);
		subWindow->show();
		subWindow->resize(320,270);
		
		connect(this, SIGNAL(closed()), vid, SLOT(deleteLater()));
	}
}


void DirectorWindow::setupUI()
{
// 	m_graphicsScene = new QGraphicsScene();
// 	ui->graphicsView->setScene(m_graphicsScene);
	//ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
// 	ui->graphicsView->setBackgroundBrush(Qt::black);
// 	ui->graphicsView->setAutoResize(true);
	//m_graphicsScene->setSceneRect(QRectF(0,0,1000.,750.));
	
	ui->fadeSpeedSlider->setValue((int)(300. / 3000. * 100));
	connect(ui->fadeBlackBtn, SIGNAL(toggled(bool)), this, SLOT(fadeBlack(bool)));
	connect(ui->fadeSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(setFadeSpeedPercent(int)));
	connect(ui->fadeSpeedBox, SIGNAL(valueChanged(double)), this, SLOT(setFadeSpeedTime(double)));
	
	connect(ui->actionMonitor_Players_Live, SIGNAL(triggered()), this, SLOT(showPlayerLiveMonitor()));
	connect(ui->actionChoose_Output, SIGNAL(triggered()), this, SLOT(chooseOutput()));
	connect(ui->actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(ui->actionPlayer_Setup, SIGNAL(triggered()), this, SLOT(showPlayerSetupDialog()));
	
	connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
	m_windowMapper = new QSignalMapper(this);
	connect(m_windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));
	
	m_closeAct = new QAction(tr("Cl&ose"), this);
	m_closeAct->setStatusTip(tr("Close the active window"));
	connect(m_closeAct, SIGNAL(triggered()),
		ui->mdiArea, SLOT(closeActiveSubWindow()));
	
	m_closeAllAct = new QAction(tr("Close &All"), this);
	m_closeAllAct->setStatusTip(tr("Close all the windows"));
	connect(m_closeAllAct, SIGNAL(triggered()),
		ui->mdiArea, SLOT(closeAllSubWindows()));
	
	m_tileAct = new QAction(tr("&Tile"), this);
	m_tileAct->setStatusTip(tr("Tile the windows"));
	connect(m_tileAct, SIGNAL(triggered()), ui->mdiArea, SLOT(tileSubWindows()));
	
	m_cascadeAct = new QAction(tr("&Cascade"), this);
	m_cascadeAct->setStatusTip(tr("Cascade the windows"));
	connect(m_cascadeAct, SIGNAL(triggered()), ui->mdiArea, SLOT(cascadeSubWindows()));
	
	m_nextAct = new QAction(tr("Ne&xt"), this);
	m_nextAct->setShortcuts(QKeySequence::NextChild);
	m_nextAct->setStatusTip(tr("Move the focus to the next window"));
	connect(m_nextAct, SIGNAL(triggered()),
		ui->mdiArea, SLOT(activateNextSubWindow()));
	
	m_previousAct = new QAction(tr("Pre&vious"), this);
	m_previousAct->setShortcuts(QKeySequence::PreviousChild);
	m_previousAct->setStatusTip(tr("Move the focus to the previous "
					"window"));
	connect(m_previousAct, SIGNAL(triggered()),
		ui->mdiArea, SLOT(activatePreviousSubWindow()));
	
	m_separatorAct = new QAction(this);
	m_separatorAct->setSeparator(true);
	
	updateWindowMenu();
	connect(ui->menuWindow, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
}

void DirectorWindow::closeEvent(QCloseEvent */*event*/)
{
	writeSettings();
	emit closed();
}

void DirectorWindow::readSettings()
{
	QSettings settings;
	QPoint pos = settings.value("DirectorWindow/pos", QPoint(10, 10)).toPoint();
	QSize size = settings.value("DirectorWindow/size", QSize(900,700)).toSize();
	move(pos);
	resize(size);

	
// 	qreal scaleFactor = (qreal)settings.value("DirectorWindow/scaleFactor", 1.).toDouble();
// 	//qDebug() << "DirectorWindow::readSettings: scaleFactor: "<<scaleFactor;
// 	ui->graphicsView->setScaleFactor(scaleFactor);
	
	m_players = new PlayerConnectionList();
	
	connect(m_players, SIGNAL(playerAdded(PlayerConnection *)), this, SLOT(playerAdded(PlayerConnection *)));
	connect(m_players, SIGNAL(playerRemoved(PlayerConnection *)), this, SLOT(playerRemoved(PlayerConnection *)));
	
	m_players->fromByteArray(settings.value("DirectorWindow/players").toByteArray());
}

void DirectorWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("DirectorWindow/pos", pos());
	settings.setValue("DirectorWindow/size", size());

	
	//settings.setValue("DirectorWindow/scaleFactor",ui->graphicsView->scaleFactor());
	//qDebug() << "DirectorWindow::writeSettings: scaleFactor: "<<ui->graphicsView->scaleFactor();
	
	settings.setValue("DirectorWindow/players", m_players->toByteArray());
}

/*

	Ui::DirectorWindow *ui;
	
	PlayerConnectionList *m_players;
	GLSceneGroupCollection *m_collection;
	
	GLSceneGroup *m_currentGroup;
	GLScene *m_currentScene;
	GLDrawable *m_currentDrawable;
	GLPlaylistItem *m_currentItem;

*/

void DirectorWindow::fadeBlack(bool toBlack)
{
	foreach(PlayerConnection *con, m_players->players())
		con->fadeBlack(toBlack);
}

void DirectorWindow::setFadeSpeedPercent(int value)
{
	double perc = ((double)value)/100.;
	int ms = (int)(3000 * perc);
	
	double sec = ((double)ms) / 1000.;
	if(ui->fadeSpeedBox->value() != sec)
	{
		bool block = ui->fadeSpeedBox->blockSignals(true);
		ui->fadeSpeedBox->setValue(sec);
		ui->fadeSpeedBox->blockSignals(block);
	}
	
	qDebug() << "DirectorWindow::setFadeSpeedPercent: value:"<<value<<", ms:"<<ms;
	foreach(PlayerConnection *con, m_players->players())
		con->setCrossfadeSpeed(ms);
	
}

void DirectorWindow::setFadeSpeedTime(double sec)
{
	int ms = (int)(sec * 1000.);
	if(ms < 1)
		ms = 1;
	if(ms > 3000)
		ms = 3000;
		
	int perc = (int)((((double)ms) / 3000.) * 100);
	if(ui->fadeSpeedSlider->value() != perc)
	{
		bool block = ui->fadeSpeedSlider->blockSignals(true);
		ui->fadeSpeedSlider->setValue(perc);
		ui->fadeSpeedSlider->blockSignals(block);
	}
	
	qDebug() << "DirectorWindow::setFadeSpeedTime: sec:"<<sec<<", ms:"<<ms;
	foreach(PlayerConnection *con, m_players->players())
		con->setCrossfadeSpeed(ms);
	
}


void DirectorWindow::showPlayerSetupDialog()
{
	PlayerSetupDialog dlg;
	dlg.setPlayerList(m_players);
	dlg.exec();
	
	if(!m_connected)
		chooseOutput();
}

/*void DirectorWindow::showEditWindow()
{
	if(!m_currentGroup)
		return;
	
// 	EditorWindow *edit = new EditorWindow();
// 	edit->setGroup(m_currentGroup, m_currentScene);
// 	edit->setCanvasSize(m_collection->canvasSize());
// 	edit->show();
}
*/

void DirectorWindow::playerAdded(PlayerConnection * con)
{
	if(!con)
		return;
	
// 	connect(con, SIGNAL(playlistTimeChanged(GLDrawable*, double)), this, SLOT(playlistTimeChanged(GLDrawable*, double)));
// 	connect(con, SIGNAL(currentPlaylistItemChanged(GLDrawable*, GLPlaylistItem*)), this, SLOT(playlistItemChanged(GLDrawable*, GLPlaylistItem *)));
}

void DirectorWindow::playerRemoved(PlayerConnection * con)
{
	if(!con)
		return;
	disconnect(con, 0, this, 0);
}


void DirectorWindow::updateMenus()
{
    bool hasMdiChild = (ui->mdiArea->activeSubWindow() != 0);
    m_closeAct->setEnabled(hasMdiChild);
    m_closeAllAct->setEnabled(hasMdiChild);
    m_tileAct->setEnabled(hasMdiChild);
    m_cascadeAct->setEnabled(hasMdiChild);
    m_nextAct->setEnabled(hasMdiChild);
    m_previousAct->setEnabled(hasMdiChild);
    m_separatorAct->setVisible(hasMdiChild);
}

void DirectorWindow::updateWindowMenu()
{
	ui->menuWindow->clear();
	ui->menuWindow->addAction(m_closeAct);
	ui->menuWindow->addAction(m_closeAllAct);
	ui->menuWindow->addSeparator();
	ui->menuWindow->addAction(m_tileAct);
	ui->menuWindow->addAction(m_cascadeAct);
	ui->menuWindow->addSeparator();
	ui->menuWindow->addAction(m_nextAct);
	ui->menuWindow->addAction(m_previousAct);
	ui->menuWindow->addAction(m_separatorAct);
	
	QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
	m_separatorAct->setVisible(!windows.isEmpty());
	
	for (int i = 0; i < windows.size(); ++i) 
	{
		QWidget *widget = windows.at(i);
	
		QString text = tr("%1%2 %3").arg(i < 9 ? "&" : "")
					    .arg(i + 1)
					    .arg(widget->windowTitle());

		QAction *action = ui->menuWindow->addAction(text);
		action->setCheckable(true);
		action->setChecked(widget == ui->mdiArea->activeSubWindow());
		connect(action, SIGNAL(triggered()), m_windowMapper, SLOT(map()));
		m_windowMapper->setMapping(action, windows.at(i));
	}
}

void DirectorWindow::setActiveSubWindow(QWidget *window)
{
	if (!window)
		return;
	ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}
