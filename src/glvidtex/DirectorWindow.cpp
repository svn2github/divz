#include "DirectorWindow.h"
#include "ui_DirectorWindow.h"
#include "PlayerSetupDialog.h"
#include "EditorWindow.h"
#include "../livemix/ExpandableWidget.h"
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
#include <QCleanlooksStyle>

QMap<QString,QRect> DirectorWindow::s_storedSystemWindowsGeometry;

DirectorWindow::DirectorWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::DirectorWindow)
	, m_players(0)
{
	ui->setupUi(this);
	
	setWindowTitle("LiveMix Director");
	
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

	m_camSceneGroup = new GLSceneGroup();
	GLScene *scene = new GLScene();
	GLVideoInputDrawable *vid = new GLVideoInputDrawable();
	scene->addDrawable(vid);
	m_camSceneGroup->addScene(scene);
	
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

					CameraWidget *camera = new CameraWidget(this, rx, con, m_camSceneGroup, ++ index);
					
					addSubwindow(camera);
				}
			}
		}
	}
	
}

DirectorMdiSubwindow *DirectorWindow::addSubwindow(QWidget *widget)
{
	DirectorMdiSubwindow *win = new DirectorMdiSubwindow(widget);
	ui->mdiArea->addSubWindow(win);
	emit subwindowAdded(win);
	return win;
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
				
		ui->mdiArea->addSubWindow(new DirectorMdiSubwindow(vid));
		
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
	
	connect(ui->fadeBlackBtn, SIGNAL(toggled(bool)), this, SLOT(fadeBlack(bool)));
	connect(ui->fadeSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(setFadeSpeedPercent(int)));
	connect(ui->fadeSpeedBox, SIGNAL(valueChanged(double)), this, SLOT(setFadeSpeedTime(double)));
	ui->fadeSpeedSlider->setValue((int)(300. / 3000. * 100));
	
	connect(ui->actionMonitor_Players_Live, SIGNAL(triggered()), this, SLOT(showPlayerLiveMonitor()));
	connect(ui->actionChoose_Output, SIGNAL(triggered()), this, SLOT(chooseOutput()));
	connect(ui->actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(ui->actionPlayer_Setup, SIGNAL(triggered()), this, SLOT(showPlayerSetupDialog()));
	
	connect(ui->actionAdd_Video_Player, SIGNAL(triggered()), this, SLOT(addVideoPlayer()));
	connect(ui->actionAdd_Group_Player, SIGNAL(triggered()), this, SLOT(addGroupPlayer()));
	connect(ui->actionAdd_Overlay, SIGNAL(triggered()), this, SLOT(addOverlay()));
	connect(ui->actionShow_Preview, SIGNAL(triggered()), this, SLOT(showPreviewWin()));
	
	connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
	connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SIGNAL(subwindowActivated(QMdiSubWindow*)));
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
	
	
	QByteArray array = settings.value("DirectorWindow/windows").toByteArray();
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	// s_storedSystemWindowsGeometry contains geometry info for 
	// camera windows and the preview window, indexed by windowtitle()
	s_storedSystemWindowsGeometry.clear();
	QVariantMap sys = map["syswin"].toMap();
	foreach(QString key, sys.keys())
	{
		QRect geom = sys[key].toRect();
		//qDebug() << "DirectorWindow::readSettings(): key:"<<key<<", geom:"<<geom;
		s_storedSystemWindowsGeometry[key] = geom;
	}
	
	// m_storedWindowOptions contains geometry and options for any
	// windows added by the user after program init
	m_storedWindowOptions = map["winopts"].toList();
	
}

void DirectorWindow::createUserSubwindows()
{
	foreach(QVariant data, m_storedWindowOptions)
	{
		QVariantMap opts = data.toMap();
		
		QString className = opts["_class"].toString();
		//qDebug() << "DirectorWindow::readSettings(): Class:"<<className<<", opts:"<<opts;
		
		QWidget *widgetPtr = 0;
		if(className == "OverlayWidget")
		{
			OverlayWidget *widget = addOverlay();
			
			widget->loadFromMap(opts);
			
			qDebug() << "DirectorWindow::createUserSubwindows(): Added overlay widget:"<<widget;
			
			widgetPtr = widget;
		}
		else
		if(className == "GroupPlayerWidget")
		{
			GroupPlayerWidget *widget = addGroupPlayer();
			
			widget->loadFromMap(opts);
			
			qDebug() << "DirectorWindow::createUserSubwindows(): Added group player widget:"<<widget;
			
			widgetPtr = widget;
		}
		
		if(QMdiSubWindow *win = windowForWidget(widgetPtr))
		{
			QRect rect = opts["_geom"].toRect();
			qDebug() << "DirectorWindow::createUserSubwindows(): Win:"<<win<<", Widget Ptr:"<<widgetPtr<<", geom:"<<rect;
			win->setGeometry(rect);
		}
		else
		{
			qDebug() << "DirectorWindow::createUserSubwindows(): Can't find window for widget ptr:"<<widgetPtr;
		}
			
		// TODO add more...
	}
		
	//QTimer::singleShot(1000, this, SLOT(applyTiling()));
}

QMdiSubWindow *DirectorWindow::windowForWidget(QWidget *widget)
{
	QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
	foreach(QMdiSubWindow *win, windows)
		if(win->widget() == widget)
			return win;
	return 0;
}

void DirectorWindow::applyTiling()
{	
	QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
	if(!windows.isEmpty())
	{
		windows.first()->setFocus(Qt::OtherFocusReason);
		ui->mdiArea->tileSubWindows();
	}
}

void DirectorWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("DirectorWindow/pos", pos());
	settings.setValue("DirectorWindow/size", size());

	
	//settings.setValue("DirectorWindow/scaleFactor",ui->graphicsView->scaleFactor());
	//qDebug() << "DirectorWindow::writeSettings: scaleFactor: "<<ui->graphicsView->scaleFactor();
	
	settings.setValue("DirectorWindow/players", m_players->toByteArray());
	
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);

	QVariantMap map;
	
	QVariantMap syswins;
	m_storedWindowOptions.clear();
	
	QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
	foreach(QMdiSubWindow *win, windows)
	{
		QWidget *widget = win->widget();
		
// 		if(GLWidget *tmp = dynamic_cast<GLWidget*>(widget))
// 		{
// 			Q_UNUSED(tmp);
// 			// camera window or preview window - just store geometry
			QRect geom = win->geometry();
			qDebug() << "DirectorWindow::writeSettings: System Window: "<<win->windowTitle()<<", Geometry:"<<geom;
			syswins[win->windowTitle()] = geom;
// 		}
// 		else
		if(OverlayWidget *tmp = dynamic_cast<OverlayWidget*>(widget))
		{
			QVariantMap opts = tmp->saveToMap();
			opts["_class"] = "OverlayWidget";
			opts["_geom"]  = win->geometry();
			m_storedWindowOptions << opts;
		}
		else
		if(GroupPlayerWidget *tmp = dynamic_cast<GroupPlayerWidget*>(widget))
		{
			QVariantMap opts = tmp->saveToMap();
			opts["_class"] = "GroupPlayerWidget";
			opts["_geom"]  = win->geometry();
			m_storedWindowOptions << opts;
		}
	}
	
	map["syswin"]  = syswins;
	map["winopts"] = m_storedWindowOptions;
	stream << map;
	 
	settings.setValue("DirectorWindow/windows", array);
}

void DirectorWindow::showEvent(QShowEvent */*event*/)
{
	QTimer::singleShot(5, this, SLOT(showAllSubwindows()));
	QTimer::singleShot(10, this, SLOT(createUserSubwindows()));
}

void DirectorWindow::showAllSubwindows()
{
	QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
	foreach(QMdiSubWindow *win, windows)
		win->show();
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
	if(m_players)
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


void DirectorWindow::addVideoPlayer()
{
}

GroupPlayerWidget *DirectorWindow::addGroupPlayer()
{
	GroupPlayerWidget *vid = new GroupPlayerWidget(this); 

	ui->mdiArea->addSubWindow(new DirectorMdiSubwindow(vid));
	
	return vid;
}

OverlayWidget *DirectorWindow::addOverlay()
{
	OverlayWidget *vid = new OverlayWidget(this); 
	
	ui->mdiArea->addSubWindow(new DirectorMdiSubwindow(vid));
	
	return vid;
}

void DirectorWindow::showPreviewWin()
{
	/// TODO
}

void DirectorWindow::showPropEditor()
{
	if(m_propWin)
	{
		m_propWin->raise();
		m_propWin->show();
	}
	else
	{
		m_propWin = new PropertyEditorWindow(this);
		ui->mdiArea->addSubWindow(new DirectorMdiSubwindow(m_propWin));
	}
}

void DirectorWindow::showPropertyEditor(DirectorSourceWidget *source)
{
	showPropEditor();
	if(m_propWin)
		m_propWin->setSourceWidget(source);
}

void DirectorWindow::showSwitcher()
{
	/// TODO
}



//////////////////////////

GroupPlayerWidget::GroupPlayerWidget(DirectorWindow *d)
	: DirectorSourceWidget(d)
	, m_setGroup(0)
	, m_scene(0)
	, m_collection(0)
	, m_director(d)
{
	m_collection = new GLSceneGroupCollection();
	GLSceneGroup *group = new GLSceneGroup();
	GLScene *scene = new GLScene();
	scene->setSceneName("New Scene");
	group->addScene(scene);
	m_collection->addGroup(group);
	
	QVBoxLayout *vbox = new QVBoxLayout(this);
	m_glw = new GLWidget(this);
	m_glw->setWindowTitle("GroupPlayerWidget");
	vbox->addWidget(m_glw);
	
	QHBoxLayout *hbox = new QHBoxLayout();
	
	m_combo = new QComboBox();
	hbox->addWidget(m_combo);
	hbox->addStretch(1);
	
	QPushButton *newf = new QPushButton(QPixmap(":/data/stock-new.png"), "");
	QPushButton *edit = new QPushButton(QPixmap(":/data/stock-edit.png"), "");
	QPushButton *browse = new QPushButton(QPixmap(":/data/stock-open.png"), "");
	hbox->addWidget(newf);
	hbox->addWidget(edit);
	hbox->addWidget(browse);
	
	vbox->addLayout(hbox);
	
	connect(newf, SIGNAL(clicked()), this, SLOT(newFile()));
	connect(edit, SIGNAL(clicked()), this, SLOT(openEditor()));
	connect(browse, SIGNAL(clicked()), this, SLOT(browse()));
	connect(m_glw, SIGNAL(clicked()), this, SLOT(switchTo()));
	connect(m_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedGroupIndexChanged(int)));
	
	newFile();
}

void GroupPlayerWidget::newFile()
{
	m_collection = new GLSceneGroupCollection();
	GLSceneGroup *group = new GLSceneGroup();
	GLScene *scene = new GLScene();
	scene->setSceneName("New Scene");
	group->addScene(scene);
	m_collection->addGroup(group);
	
	m_combo->setModel(m_collection->at(0));
	
	setWindowTitle("Player - New File");
}

QVariantMap GroupPlayerWidget::saveToMap()
{
	QVariantMap map;
	map["file"] = file();
	map["idx"] = currentIndex();
	return map;
}

void GroupPlayerWidget::loadFromMap(const QVariantMap& map)
{
	loadFile(map["file"].toString());
	setCurrentIndex(map["idx"].toInt());
}

void GroupPlayerWidget::openEditor()
{
	int idx = m_combo->currentIndex();
	
	EditorWindow *edit = new EditorWindow();
	edit->setGroup(m_collection->at(0), 
		       m_collection->at(0)->at(idx));
	edit->setCanvasSize(m_collection->canvasSize());
	edit->show();
	edit->setAttribute(Qt::WA_DeleteOnClose, true);
	
	connect(edit, SIGNAL(destroyed()), this, SLOT(saveFile()));
}

void GroupPlayerWidget::browse()
{
	QSettings settings;
	QString curFile = m_collection->fileName();
	if(curFile.trimmed().isEmpty())
		curFile = settings.value("last-collection-file").toString();

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Group Collection File"), curFile, tr("GLDirector File (*.gld);;Any File (*.*)"));
	if(fileName != "")
	{
		settings.setValue("last-collection-file",fileName);
		if(!loadFile(fileName))
		{
			QMessageBox::critical(this,tr("File Does Not Exist"),tr("Sorry, but the file you chose does not exist. Please try again."));
		}
	}
}

QString GroupPlayerWidget::file() { return m_collection->fileName(); }

bool GroupPlayerWidget::loadFile(QString fileName)
{
	if(m_collection->readFile(fileName))
	{
		setWindowTitle(QString("Player - %1").arg(QFileInfo(fileName).fileName()));
		m_combo->setModel(m_collection->at(0));
		return true;
	}
	return false;
}


void GroupPlayerWidget::saveFile()
{
	QString curFile = m_collection->fileName();
	
	QSettings settings;
	
	if(curFile.trimmed().isEmpty())
	{
		curFile = settings.value("last-collection-file").toString();

		QString fileName = QFileDialog::getSaveFileName(this, tr("Choose a Filename"), curFile, tr("GLDirector File (*.gld);;Any File (*.*)"));
		if(fileName != "")
		{
			QFileInfo info(fileName);
			settings.setValue("last-collection-file",fileName);
			
			curFile = fileName;
		}
	}
	
	if(!curFile.isEmpty())
	{
		m_collection->writeFile(curFile);
	}
	
	int idx = m_combo->currentIndex();
	m_combo->setModel(m_collection->at(0));
	selectedGroupIndexChanged(idx);
}

// DirectorSourceWidget::	
bool GroupPlayerWidget::switchTo()
{
	int idx = m_combo->currentIndex();
	
	GLSceneGroup *group = m_collection->at(0); 
	GLScene *scene = group->at(idx);
	
	foreach(PlayerConnection *player, m_director->players()->players())
 	{
 		if(player->isConnected())
 		{
 			//if(player->lastGroup() != group)
 				player->setGroup(group, scene);
 			m_setGroup = group;
			//player->setUserProperty(gld, con, "videoConnection");
		}
	}
	
	return true;
}


GroupPlayerWidget::~GroupPlayerWidget()
{
	// TODO...?
}


void GroupPlayerWidget::selectedGroupIndexChanged(int idx)
{
	GLSceneGroup *group = m_collection->at(0); 
	GLScene *scene = group->at(idx);
	
	if(m_scene)
		m_scene->detachGLWidget();
		
	if(scene)
	{
		scene->setGLWidget(m_glw);
		m_scene = scene;
	}
	
// 	foreach(PlayerConnection *player, m_director->players()->players())
// 	{
// 		if(player->isConnected())
// 		{
// 			if(player->lastGroup() == m_setGroup)
// 				player->setGroup(m_setGroup, scene);
// 		}
// 	}
}

//////////////////////////

OverlayWidget::OverlayWidget(DirectorWindow *d)
	: DirectorSourceWidget(d)
	, m_setGroup(0)
	, m_scene(0)
	, m_collection(0)
	, m_director(d)
{
	
	QVBoxLayout *vbox = new QVBoxLayout(this);
	m_glw = new GLWidget(this);
	m_glw->setWindowTitle("OverlayWidget");
	vbox->addWidget(m_glw);
	
	QHBoxLayout *hbox = new QHBoxLayout();
	
	m_combo = new QComboBox();
	hbox->addWidget(m_combo);
	hbox->addStretch(1);
	
	QPushButton *show = new QPushButton(QPixmap(":/data/stock-media-play.png"), "");
	QPushButton *hide = new QPushButton(QPixmap(":/data/stock-media-stop.png"), "");
	
	QPushButton *newf = new QPushButton(QPixmap(":/data/stock-new.png"), "");
	QPushButton *edit = new QPushButton(QPixmap(":/data/stock-edit.png"), "");
	QPushButton *browse = new QPushButton(QPixmap(":/data/stock-open.png"), "");
	
	hbox->addWidget(show);
	hbox->addWidget(hide);
	hbox->addWidget(newf);
	hbox->addWidget(edit);
	hbox->addWidget(browse);
	
	vbox->addLayout(hbox);
	
	connect(m_glw, SIGNAL(clicked()), this, SLOT(clicked()));
	connect(show, SIGNAL(clicked()), this, SLOT(showOverlay()));
	connect(hide, SIGNAL(clicked()), this, SLOT(hideOverlay()));
	connect(newf, SIGNAL(clicked()), this, SLOT(newFile()));
	connect(edit, SIGNAL(clicked()), this, SLOT(openEditor()));
	connect(browse, SIGNAL(clicked()), this, SLOT(browse()));
	connect(m_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedGroupIndexChanged(int)));
	
	newFile();
}

QVariantMap OverlayWidget::saveToMap()
{
	QVariantMap map;
	map["file"] = file();
	map["idx"] = currentIndex();
	return map;
}

void OverlayWidget::loadFromMap(const QVariantMap& map)
{
	loadFile(map["file"].toString());
	setCurrentIndex(map["idx"].toInt());
}

void OverlayWidget::newFile()
{
	m_collection = new GLSceneGroupCollection();
	GLSceneGroup *group = new GLSceneGroup();
	GLScene *scene = new GLScene();
	scene->setSceneName("New Scene");
	group->addScene(scene);
	m_collection->addGroup(group);
	
	m_combo->setModel(m_collection->at(0));
	
	setWindowTitle("Overlay - New File");
}

void OverlayWidget::openEditor()
{
	int idx = m_combo->currentIndex();
	
	EditorWindow *edit = new EditorWindow();
	edit->setGroup(m_collection->at(0), 
		       m_collection->at(0)->at(idx));
	edit->setCanvasSize(m_collection->canvasSize());
	edit->show();
	edit->setAttribute(Qt::WA_DeleteOnClose, true);
	
	connect(edit, SIGNAL(destroyed()), this, SLOT(saveFile()));
}

void OverlayWidget::browse()
{
	QSettings settings;
	QString curFile = m_collection->fileName();
	if(curFile.trimmed().isEmpty())
		curFile = settings.value("last-collection-file").toString();

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Group Collection File"), curFile, tr("GLDirector File (*.gld);;Any File (*.*)"));
	if(fileName != "")
	{
		settings.setValue("last-collection-file",fileName);
		if(!loadFile(fileName))
		{
			QMessageBox::critical(this,tr("File Does Not Exist"),tr("Sorry, but the file you chose does not exist. Please try again."));
		}
	}
}

QString OverlayWidget::file() { return m_collection->fileName(); }

bool OverlayWidget::loadFile(QString fileName)
{
	if(m_collection->readFile(fileName))
	{
		setWindowTitle(QString("Overlay - %1").arg(QFileInfo(fileName).fileName()));
		m_combo->setModel(m_collection->at(0));
		return true;
	}
	return false;
}

void OverlayWidget::saveFile()
{
	QString curFile = m_collection->fileName();
	
	QSettings settings;
	
	if(curFile.trimmed().isEmpty())
	{
		curFile = settings.value("last-collection-file").toString();

		QString fileName = QFileDialog::getSaveFileName(this, tr("Choose a Filename"), curFile, tr("GLDirector File (*.gld);;Any File (*.*)"));
		if(fileName != "")
		{
			QFileInfo info(fileName);
			settings.setValue("last-collection-file",fileName);
			
			curFile = fileName;
		}
	}
	
	if(!curFile.isEmpty())
	{
		m_collection->writeFile(curFile);
	}
	
	int idx = m_combo->currentIndex();
	m_combo->setModel(m_collection->at(0));
	selectedGroupIndexChanged(idx);
}

void OverlayWidget::showOverlay()
{
	int idx = m_combo->currentIndex();
	
	GLSceneGroup *group = m_collection->at(0); 
	GLScene *scene = group->at(idx);
	
	foreach(PlayerConnection *player, m_director->players()->players())
		if(player->isConnected())
			player->addOverlay(scene);
}

void OverlayWidget::hideOverlay()
{
	int idx = m_combo->currentIndex();
	
	GLSceneGroup *group = m_collection->at(0); 
	GLScene *scene = group->at(idx);
	
	foreach(PlayerConnection *player, m_director->players()->players())
		if(player->isConnected())
			player->removeOverlay(scene);
}

OverlayWidget::~OverlayWidget()
{
	// TODO...?
}


void OverlayWidget::selectedGroupIndexChanged(int idx)
{
	GLSceneGroup *group = m_collection->at(0); 
	GLScene *scene = group->at(idx);
	
	if(m_scene)
		m_scene->detachGLWidget();
		
	if(scene)
	{
		scene->setGLWidget(m_glw);
		m_scene = scene;
	}
	
// 	foreach(PlayerConnection *player, m_director->players()->players())
// 	{
// 		if(player->isConnected())
// 		{
// 			if(player->lastGroup() == m_setGroup)
// 				player->setGroup(m_setGroup, scene);
// 		}
// 	}
}

void OverlayWidget::clicked()
{
	if(m_dir)
		m_dir->showPropertyEditor(this);
}

//////////////////////////////////////////////////////


DirectorMdiSubwindow::DirectorMdiSubwindow(QWidget *child)
	: QMdiSubWindow()
{
	if(child)
		setWidget(child);
}
	
void DirectorMdiSubwindow::setWidget(QWidget *widget)
{
	//setStyle(new QCDEStyle());
	setStyle(new QCleanlooksStyle());
	//subWindow->setWidget(new QPushButton(item->itemName()));
	QMdiSubWindow::setWidget(widget);
	setAttribute(Qt::WA_DeleteOnClose);
	//subWindow->adjustSize();
	//subWindow->setWindowFlags(Qt::FramelessWindowHint);

	QTimer::singleShot(0, this, SLOT(show()));
	resize(320,270);
	
	m_geom = DirectorWindow::s_storedSystemWindowsGeometry[windowTitle()];
	qDebug() << "DirectorMdiSubwindow::setWidget(): windowTitle:"<<windowTitle()<<", m_geom:"<<m_geom;
}

void DirectorMdiSubwindow::showEvent(QShowEvent *)
{
	//QTimer::singleShot(5, this, SLOT(applyGeom()));
	applyGeom();
}

void DirectorMdiSubwindow::applyGeom()
{
	if(m_geom.isValid())
		setGeometry(m_geom);
		
	qDebug() << "DirectorMdiSubwindow::applyGeometry(): windowTitle:"<<windowTitle()<<", m_geom:"<<m_geom<<", confirming geom:"<<geometry();
} 

	
	//connect((QMdiSubWindow*)this, SIGNAL(closed()), widget, SLOT(deleteLater()));
	
void DirectorMdiSubwindow::moveEvent(QMoveEvent * moveEvent)
{
	QMdiSubWindow::moveEvent(moveEvent);
}

///////////////////////////////////////////////////////////



CameraWidget::CameraWidget(DirectorWindow* dir, VideoReceiver *rx, QString con, GLSceneGroup *group, int index)
	: DirectorSourceWidget(dir)
	, m_rx(rx)
	, m_con(con)
	, m_camSceneGroup(group)
{
	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setContentsMargins(0,0,0,0);
	
	GLWidget *vid = new GLWidget();
	vbox->addWidget(vid);

	GLVideoDrawable *gld = new GLVideoDrawable();
	gld->setVideoSource(rx);
	vid->addDrawable(gld);
	
	setWindowTitle(QString("Camera %1").arg(index));
	gld->setObjectName(qPrintable(windowTitle()));
	resize(320,240);
	//vid->show();
	
	m_configMenu = new QMenu(this);
	QAction * action;
	QSettings settings;
	m_deinterlace = settings.value(QString("%1/deint").arg(con),false).toBool();
	
	action = m_configMenu->addAction("Deinterlace");
	action->setCheckable(true);
	action->setChecked(m_deinterlace);
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setDeinterlace(bool)));
	
	connect(vid, SIGNAL(clicked()), this, SLOT(switchTo()));
}
	
void CameraWidget::contextMenuEvent(QContextMenuEvent * event)
{
	if(m_configMenu)
		m_configMenu->popup(event->globalPos());
}

CameraWidget::~CameraWidget() {}
	
// DirectorSourceWidget::	
bool CameraWidget::switchTo()
{
	qDebug() << "CameraWidget::switchTo: Using con string: "<<m_con;
	
	GLDrawable *gld = m_camSceneGroup->at(0)->at(0);
	gld->setProperty("videoConnection", m_con);
	
	if(!m_dir->players())
		return false;
		
	foreach(PlayerConnection *player, m_dir->players()->players())
	{
		if(player->isConnected())
		{
			if(player->lastGroup() != m_camSceneGroup)
				player->setGroup(m_camSceneGroup, m_camSceneGroup->at(0));
			player->setUserProperty(gld, m_con, "videoConnection");
		}
	}
	 
	setDeinterlace(m_deinterlace);
	
	return true;
}

void CameraWidget::setDeinterlace(bool flag)
{
	if(!m_dir->players())
 		return;
 		
 	m_deinterlace = flag;
 	
 	GLDrawable *gld = m_camSceneGroup->at(0)->at(0);
 	
 	if(gld->property("videoConnection").toString() != m_con)
 	{
 		qDebug() << "CameraWidget::setDeinterlace("<<flag<<"): Not setting, not currently the live camera";
 		return;
 	}
 	
 	foreach(PlayerConnection *player, m_dir->players()->players())
 	{
 		if(player->isConnected())
 		{
 			if(player->lastGroup() == m_camSceneGroup)
 			{
 				//player->setGroup(m_camSceneGroup, m_camSceneGroup->at(0));
	 			player->setUserProperty(gld, flag, "deinterlace");
	 		}
	 		else
	 		{
	 			qDebug() << "CameraWidget::setDeinterlace("<<flag<<"): Not setting, not currently the live group";
	 		}
	 	}
	 }
}

QVariantMap CameraWidget::saveToMap() { return QVariantMap(); }
void CameraWidget::loadFromMap(const QVariantMap&) {}
	

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


SwitcherWindow::SwitcherWindow(DirectorWindow * dir)
	: QWidget(dir)
	, m_dir(dir)
{
	/// TODO
	
	connect(dir, SIGNAL(subwindowAdded(QMdiSubWindow*)), this, SLOT(subwindowAdded(QMdiSubWindow*)));
	
	// Loop tru all open windows
	// Find DirectorSourceWidgets
	// Set switcher pointer on DirectorSourceWidget
	// Create buttons for all widgets (call subwindowAdded() directly)
	// Insert event filter
	// ....
}
	
void SwitcherWindow::notifyIsLive(DirectorSourceWidget*)
{
	/// TODO
	// DirectorSourceWidget* will call this to notify of live
	// - find button for this DirectorSourceWidget* pointer and change status (background color...?)
}

bool SwitcherWindow::eventFilter(QObject *, QEvent *)
{
	/// TODO
	// Translate keypress into a specific DirectorSourceWidget* and call switchTo() on that window
	return false;
}
	
void SwitcherWindow::buttonClicked()
{
	/// TODO
	// Figure out which DirectorSourceWidget* goes to this button and call switchTo()
}

void SwitcherWindow::subwindowAdded(QMdiSubWindow*)
{
	/// TODO
	// When window added, add button to layout
	// Connect listern for window destruction to remove button
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


PropertyEditorWindow::PropertyEditorWindow(DirectorWindow *win)
	: m_dir(win)
{
	// Just create a static UI, possibly with dropdown of DirectorSourceWidget* to select from...?
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	
	QScrollArea *controlArea = new QScrollArea(this);
	controlArea->setWidgetResizable(true);
	layout->addWidget(controlArea);
	
	QWidget *controlBase = new QWidget(controlArea);
	
	m_layout = new QVBoxLayout(controlBase);
	m_layout->setContentsMargins(0,0,0,0);
	controlArea->setWidget(controlBase);
	
	setSourceWidget(0);
	
	connect(win, SIGNAL(subwindowActivated(QMdiSubWindow*)), this, SLOT(subwindowActivated(QMdiSubWindow*)));
}
	

void PropertyEditorWindow::setSourceWidget(DirectorSourceWidget* source)
{
	// Core - create the UI for this DirectorSourceWidget* scene() based on user controllable items
	// However, specialcase based on subclass:
	// If it's a CameraWidget then provide different controls than for other widgets
	
	while(m_layout->count() > 0)
	{
		QLayoutItem *item = m_layout->itemAt(m_layout->count() - 1);
		m_layout->removeItem(item);
		if(QWidget *widget = item->widget())
		{
			m_layout->removeWidget(widget);
			delete widget;
		}
			
		delete item;
		item = 0;
	}
	
	m_source = source;
	
	if(!source)
	{
		setWindowTitle(QString("Property Editor"));
		m_layout->addWidget(new QLabel("<i>Select a window to start editing properties.</i>"));
		m_layout->addStretch(1);
		return;
	}
	
	connect(source, SIGNAL(destroyed()), this, SLOT(sourceDestroyed()));
	
	GLScene *scene = source->scene();
	if(scene)
	{
		bool foundWidget = false;
		GLDrawableList list = scene->drawableList();
		foreach(GLDrawable *gld, list)
		{
			if(gld->isUserControllable())
			{
				ExpandableWidget *widget = new ExpandableWidget(gld->itemName().isEmpty() ? "Item" : gld->itemName());
				
				DrawableDirectorWidget *editor = new DrawableDirectorWidget(gld, scene, m_dir);
				widget->setWidget(editor);
				
				m_layout->addWidget(widget);
				
				foundWidget = true;
			}
		}
		
		setWindowTitle(QString("Properties - %1").arg(scene->sceneName()));
		
		if(!foundWidget)
			m_layout->addWidget(new QLabel("<i>No modifiable items found in the active window.</i>"));
		
		m_layout->addStretch(1);
	}
	else
	{
		//setSourceWidget(0);
		m_layout->addWidget(new QLabel("<i>No active scene loaded in the selected window.</i>"));
		m_layout->addStretch(1);
		setWindowTitle(QString("Properties - %1").arg(source->windowTitle()));
	}
	
	
}

void PropertyEditorWindow::sourceDestroyed() { setSourceWidget(0); }
	
void PropertyEditorWindow::subwindowActivated(QMdiSubWindow* win)
{
	if(!win)
		return;
		
	// This should just check for DirectorSourceWidget* subclass and call setSourceWidget()
	if(DirectorSourceWidget *src = dynamic_cast<DirectorSourceWidget*>(win->widget()))
	{
		setSourceWidget(src);
	}
	else
	{
		qDebug() << "Window activated: "<<win<<", widget:"<<win->widget()<<" - not a DirectorSourceWidget";
	}
}



