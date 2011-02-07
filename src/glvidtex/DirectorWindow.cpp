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

#include <QCDEStyle>

DirectorWindow::DirectorWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::DirectorWindow)
	, m_players(0)
	, m_collection(0)
	, m_currentGroup(0)
	, m_currentScene(0)
// 	, m_currentDrawable(0)
// 	, m_currentItem(0)
// 	, m_graphicsScene(0)
{
	ui->setupUi(this);
	
	setupUI();
	
	// For some reason, these get unset from NULL between the ctor init above and here, so reset to zero
	m_currentScene = 0;
	m_currentGroup = 0;
// 	m_currentDrawable = 0;
// 	m_currentItem = 0;
	m_collection = 0;
	m_players = 0;
	
	m_vidSendMgr = 0;
// 	m_vidSendMgr = new VideoInputSenderManager();
// 	m_vidSendMgr->setSendingEnabled(true);
	
	readSettings();
	
	bool loadedFile = false;
	
	// Attempt to load the first argument on the command line
	QStringList argList = qApp->arguments();
	if(argList.size() > 1)
		loadedFile = readFile(argList.at(1));
	
	// If no file loaded, create an empty collection 
	if(!loadedFile)
		newFile();
	
	// Connect to any players that have the 'autoconnect' option checked in the PlayerSetupDialog
	foreach(PlayerConnection *con, m_players->players())
		if(!con->isConnected() && con->autoconnect())
			con->connectPlayer();

	setFocusPolicy(Qt::StrongFocus);
}


DirectorWindow::~DirectorWindow()
{
	delete ui;
}

void DirectorWindow::newFile()
{
	if(m_collection)
	{
		if(m_fileName.isEmpty())
			showSaveAsDialog();
		else
			writeFile(m_fileName);
	}
		
	GLSceneGroupCollection *tmp = new GLSceneGroupCollection();
	GLSceneGroup *group = new GLSceneGroup();
	GLScene *scene = new GLScene();
	group->addScene(scene);
	tmp->addGroup(group);
	setCollection(tmp);
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
	dlg.setWindowTitle("Change Canvas Size");
	
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
		
		VideoWidget *vid = new VideoWidget();
		//qDebug() << "DirectorWindow::showPlayerLiveMonitor: Created VideoWidget:"<<vid;
		vid->setVideoSource(rx);
		
		vid->setWindowTitle(QString("Player '%1' - Live Player Monitor").arg(con->name()));
		vid->resize(320,240);
		vid->show();
		
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
	
// 	ui->slidePlayBtn->setText("");
// 	ui->slidePlayBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	
	connect(ui->actionMonitor_Players_Live, SIGNAL(triggered()), this, SLOT(showPlayerLiveMonitor()));
	
	connect(ui->actionAdd_New_Group, SIGNAL(triggered()), this, SLOT(addNewGroup()));
	connect(ui->actionEdit_Selected_Group, SIGNAL(triggered()), this, SLOT(showEditWindow()));
	connect(ui->actionChange_Canvas_Size, SIGNAL(triggered()), this, SLOT(changeCanvasSize()));
	connect(ui->actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(ui->actionOpen_Collection, SIGNAL(triggered()), this, SLOT(showOpenFileDialog()));
	connect(ui->actionPlayer_Setup, SIGNAL(triggered()), this, SLOT(showPlayerSetupDialog()));
	connect(ui->actionRemove_Selected_Group, SIGNAL(triggered()), this, SLOT(deleteGroup()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(showSaveAsDialog()));
	connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFile()));

	connect(ui->groupsListView, SIGNAL(activated(const QModelIndex &)), this, SLOT(groupSelected(const QModelIndex &)));
	connect(ui->groupsListView, SIGNAL(clicked(const QModelIndex &)),   this, SLOT(groupSelected(const QModelIndex &)));

	connect(ui->scenesListView, SIGNAL(activated(const QModelIndex &)), this, SLOT(slideSelected(const QModelIndex &)));
	connect(ui->scenesListView, SIGNAL(clicked(const QModelIndex &)),   this, SLOT(slideSelected(const QModelIndex &)));

// 	connect(ui->sceneListview, SIGNAL(activated(const QModelIndex &)), this, SLOT(drawableSelected(const QModelIndex &)));
// 	connect(ui->sceneListview, SIGNAL(clicked(const QModelIndex &)),   this, SLOT(drawableSelected(const QModelIndex &)));

	
	//ui->drawableGroupBox->setVisible(false);
	//ui->sceneListview->setVisible(false);
	
	//ui->slideTimeLabel->setText("");
}

void DirectorWindow::closeEvent(QCloseEvent */*event*/)
{
	writeSettings();
	saveFile();
	emit closed();
}

void DirectorWindow::readSettings()
{
	QSettings settings;
	QPoint pos = settings.value("DirectorWindow/pos", QPoint(10, 10)).toPoint();
	QSize size = settings.value("DirectorWindow/size", QSize(900,700)).toSize();
	move(pos);
	resize(size);

	ui->mainSplitter->restoreState(settings.value("DirectorWindow/MainSplitter2").toByteArray());
	ui->sceneSplitter->restoreState(settings.value("DirectorWindow/SceneSplitter2").toByteArray());
	//ui->drawableSplitter->restoreState(settings.value("DirectorWindow/DrawableSplitter").toByteArray());

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

	settings.setValue("DirectorWindow/MainSplitter2",ui->mainSplitter->saveState());
	settings.setValue("DirectorWindow/SceneSplitter2",ui->sceneSplitter->saveState());
	//settings.setValue("DirectorWindow/DrawableSplitter",ui->drawableSplitter->saveState());

	//settings.setValue("DirectorWindow/scaleFactor",ui->graphicsView->scaleFactor());
	//qDebug() << "DirectorWindow::writeSettings: scaleFactor: "<<ui->graphicsView->scaleFactor();
	
	settings.setValue("DirectorWindow/players", m_players->toByteArray());
}

void DirectorWindow::groupSelected(const QModelIndex &idx)
{
	if(!m_collection)
		return;
	if(idx.isValid())
		setCurrentGroup(m_collection->at(idx.row()));
}
void DirectorWindow::currentGroupChanged(const QModelIndex &idx,const QModelIndex &)
{
	if(idx.isValid())
		groupSelected(idx);
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
void DirectorWindow::slideSelected(const QModelIndex &idx)
{
	if(idx.isValid())
		setCurrentScene(m_currentGroup->at(idx.row()));
}
void DirectorWindow::currentSlideChanged(const QModelIndex &idx,const QModelIndex &)
{
	if(idx.isValid())
		slideSelected(idx);
}

// void DirectorWindow::drawableSelected(const QModelIndex &idx)
// {
// 	if(!m_currentScene)
// 		return;
// 	if(idx.isValid())
// 		setCurrentDrawable(m_currentScene->at(idx.row()));
// }
// void DirectorWindow::currentDrawableChanged(const QModelIndex &idx,const QModelIndex &)
// {
// 	if(idx.isValid())
// 		drawableSelected(idx);
// }

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


void DirectorWindow::showOpenFileDialog()
{
	QSettings settings;
	
	QString curFile = m_fileName;
	if(curFile.trimmed().isEmpty())
		curFile = settings.value("last-collection-file").toString();

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Group Collection File"), curFile, tr("GLDirector File (*.gld);;Any File (*.*)"));
	if(fileName != "")
	{
		settings.setValue("last-collection-file",fileName);
		if(readFile(fileName))
		{
			return;
		}
		else
		{
			QMessageBox::critical(this,tr("File Does Not Exist"),tr("Sorry, but the file you chose does not exist. Please try again."));
		}
	}
}
void DirectorWindow::showSaveAsDialog()
{
	QSettings settings;
	
	QString curFile = m_fileName;
	if(curFile.trimmed().isEmpty())
		curFile = settings.value("last-collection-file").toString();

	QString fileName = QFileDialog::getSaveFileName(this, tr("Choose a Filename"), curFile, tr("GLDirector File (*.gld);;Any File (*.*)"));
	if(fileName != "")
	{
		QFileInfo info(fileName);
		//if(info.suffix().isEmpty())
			//fileName += ".dviz";
		settings.setValue("last-collection-file",fileName);
		writeFile(fileName);
// 		return true;
	}

// 	return false;
}
void DirectorWindow::saveFile()
{
	//m_fileName = "test.gld";
	if(m_fileName.isEmpty())
		showSaveAsDialog();
	else
		writeFile(m_fileName);
}

void DirectorWindow::showPlayerSetupDialog()
{
	PlayerSetupDialog dlg;
	dlg.setPlayerList(m_players);
	dlg.exec();
}

void DirectorWindow::showEditWindow()
{
	if(!m_currentGroup)
		return;
	
	EditorWindow *edit = new EditorWindow();
	edit->setGroup(m_currentGroup, m_currentScene);
	edit->setCanvasSize(m_collection->canvasSize());
	edit->show();
}
void DirectorWindow::addNewGroup()
{
	GLSceneGroup *group = new GLSceneGroup();
	m_collection->addGroup(group);
	setCurrentGroup(group);
}
void DirectorWindow::deleteGroup()
{
	if(!m_currentGroup)
		return;
	
	if (QMessageBox::question(0, tr("Delete Group"), tr("Are you SURE you want to delete this group?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
		return;
	
	m_collection->removeGroup(m_currentGroup);
	
// 	if(m_collection && m_collection->size() > 0)
// 		setCurrentGroup(m_collection->at(0));
// 	else
		setCurrentGroup(0);
	
	m_currentGroup->deleteLater();
}

bool DirectorWindow::readFile(const QString& fileArg)
{
	if(!fileArg.isEmpty())
	{
		m_fileName = fileArg;
		QFile file(fileArg);
		if (!file.open(QIODevice::ReadOnly)) 
		{
			qDebug() << "DirectorWindow: Unable to read collection file: "<<fileArg;
			return false;
		}
		else
		{
			QByteArray array = file.readAll();
			
			GLSceneGroupCollection *collection = new GLSceneGroupCollection();
			collection->fromByteArray(array);
			
			setCollection(collection);
			
			qDebug() << "DirectorWindow: [DEBUG]: Loaded File: "<<fileArg<<", CollectionID: "<<collection->collectionId()<<", array: "<<array.size();
			
			setWindowTitle(QString("%1 - GLDirector").arg(QFileInfo(fileArg).fileName()));
		}
	}
	
	return true;
}
bool DirectorWindow::writeFile(const QString& fileName)
{
	QString fileArg = fileName;
	if(fileArg.isEmpty())
		fileArg = m_fileName;
	
	if(!fileArg.isEmpty())
	{
		m_fileName = fileArg;
		setWindowTitle(QString("%1 - GLDirector").arg(QFileInfo(fileArg).fileName()));
		
		QFile file(fileArg);
		// Open file
		if (!file.open(QIODevice::WriteOnly))
		{
			QMessageBox::warning(0, QObject::tr("File Error"), QObject::tr("Error opening file '%1' for writing").arg(fileArg));
			//throw 0;
			return false;
		}
		else
		{
			
			//QByteArray array;
			//QDataStream stream(&array, QIODevice::WriteOnly);
			//QVariantMap map;
			
			file.write(collection()->toByteArray());
			file.close();
			
			qDebug() << "DirectorWindow: Debug: Saved CollectionID: "<< collection()->collectionId()<<" to file: "<<fileArg;
		}	
	}
	return true;
}

void DirectorWindow::setCollection(GLSceneGroupCollection *collection)
{
	if(m_collection)
	{
		m_collection->deleteLater();
		m_collection = 0;
	}
	
	m_collection = collection;
	ui->groupsListView->setModel(collection);
// 	ui->groupsListView->resizeColumnsToContents();
//  	ui->groupsListView->resizeRowsToContents();
	
	if(collection->size() > 0)
	{
		QModelIndex idx = collection->index(0, 0);
		ui->groupsListView->setCurrentIndex(idx);
		groupSelected(idx);
	}
	
	foreach(PlayerConnection *con, m_players->players())
		con->setCanvasSize(m_collection->canvasSize());
		
// 	if(m_graphicsScene)
// 		m_graphicsScene->setSceneRect(QRectF(QPointF(0,0),m_collection->canvasSize()));
}
void DirectorWindow::setCurrentGroup(GLSceneGroup *group, GLScene */*currentScene*/)
{
	if(m_currentGroup)
		disconnect(m_currentGroup, 0, this, 0);
		
	m_currentGroup = group;
	if(!group)
		return;
		
	ui->scenesListView->setModel(group);
// 	ui->scenesListView->resizeColumnsToContents();
//  	ui->scenesListView->resizeRowsToContents();
	connect(group, SIGNAL(sceneDataChanged()), this, SLOT(sceneDataChanged()));
	
	foreach(PlayerConnection *con, m_players->players())
		con->setGroup(group);
	
	if(group->size() > 0)
	{
		QModelIndex idx = group->index(0, 0);
		ui->scenesListView->setCurrentIndex(idx);
		slideSelected(idx);
	}
}

void DirectorWindow::sceneDataChanged()
{
// 	ui->scenesListView->resizeColumnsToContents();
//  	ui->scenesListView->resizeRowsToContents();
}

void DirectorWindow::setCurrentScene(GLScene *scene)
{
	if(!scene || scene == m_currentScene)
		return;
	
	//qDebug() << "DirectorWindow::setCurrentScene: scene:"<<scene<<", m_currentScene:"<<(void*)m_currentScene;
	
	if(m_currentScene)
	{
		disconnect(m_currentScene, 0, this, 0);
		m_currentScene = 0;
		
		ui->mdiArea->closeAllSubWindows();
	}
	
	m_currentScene = scene;
	
	foreach(PlayerConnection *con, m_players->players())
		con->setScene(scene);

	//ui->sceneListview->setModel(scene);
	scene->setListOnlyUserItems(true);
	qDebug() << "DirectorWindow::setCurrentScene: "<<scene<<": size:"<<scene->size();
	
	for(int i=0; i<scene->size(); i++)
	{
		GLDrawable *item = scene->at(i);
		
 		QMdiSubWindow *subWindow = new QMdiSubWindow;
 		//subWindow->setStyle(new QCDEStyle());
		//subWindow->setWidget(new QPushButton(item->itemName()));
		DrawableDirectorWidget *widget = new DrawableDirectorWidget(item, scene, this); 
		subWindow->setWidget(widget);
 		subWindow->setAttribute(Qt::WA_DeleteOnClose);
 		//subWindow->adjustSize();
 		//subWindow->setWindowFlags(Qt::FramelessWindowHint);
 		
 		ui->mdiArea->addSubWindow(subWindow);
 		subWindow->show();
		
		qDebug() << "DirectorWindow::setCurrentScene: "<<scene<<": Created subwindow for item:"<<item->itemName();
	}
	
	
// 	if(!dynamic_cast<GLEditorGraphicsScene*>(m_currentScene->graphicsScene()))
// 		m_currentScene->setGraphicsScene(new GLEditorGraphicsScene);
// 	m_graphicsScene = dynamic_cast<GLEditorGraphicsScene*>(m_currentScene->graphicsScene());
// 	if(!m_graphicsScene)
// 		qDebug() << "Directorwindow::setCurrentScene: "<<scene<<" Internal Error: No graphics scene returned. Probably will crash now.";
// 	m_graphicsScene->setSceneRect(QRectF(QPointF(0,0),m_collection->canvasSize()));
// 	ui->graphicsView->setScene(m_graphicsScene);
	
// 	bool hasUserItems = scene->size();
// 	
// 	ui->drawableGroupBox->setVisible(hasUserItems);
// 	ui->sceneListview->setVisible(hasUserItems);
	
	/*
	if(scene->size() > 0)
	{
		QModelIndex idx = scene->index(0, 0);
		ui->sceneListview->setCurrentIndex(idx);
		drawableSelected(idx);
	}*/
}

void DirectorWindow::changeCanvasSize()
{
	if(!m_collection)
		return;
	
	QDialog dlg;
	dlg.setWindowTitle("Change Canvas Size");
	
	QVBoxLayout *vbox = new QVBoxLayout(&dlg);
	
		
	QFormLayout *form = new QFormLayout();
	QSpinBox *width = new QSpinBox();
	width->setMinimum(0);
	width->setMaximum(99999);
	
	QSpinBox *height = new QSpinBox();
	height->setMinimum(0);
	height->setMaximum(99999);
	
	QSizeF size = m_collection->canvasSize();
	width->setValue((int)size.width());
	height->setValue((int)size.height()); 
	
	form->addRow("Width:", width);
	form->addRow("Height:", height);
	 
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
		m_collection->setCanvasSize(QSizeF((qreal)width->value(), (qreal)height->value()));
		
		foreach(PlayerConnection *con, m_players->players())
			con->setCanvasSize(m_collection->canvasSize());
			
// 		if(m_graphicsScene)
// 			m_graphicsScene->setSceneRect(QRectF(QPointF(0,0),m_collection->canvasSize()));
		
	}
	
}

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


