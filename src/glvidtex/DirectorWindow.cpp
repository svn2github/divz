#include "DirectorWindow.h"
#include "ui_DirectorWindow.h"
#include "PlayerSetupDialog.h"
#include "EditorWindow.h"
#include <QFileInfo>

#include <QFileDialog>
#include <QApplication>
#include <QGLWidget>

#include "GLSceneGroup.h"
#include "GLDrawable.h"
#include "PlayerConnection.h"

DirectorWindow::DirectorWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::DirectorWindow)
	, m_players(0)
	, m_collection(0)
	, m_currentGroup(0)
	, m_currentScene(0)
	, m_currentDrawable(0)
	, m_currentItem(0)
	, m_graphicsScene(0)
{
	ui->setupUi(this);
	setupUI();
	readSettings();
	
	m_currentScene = 0;
	m_currentGroup = 0;
	m_currentDrawable = 0;
	m_currentItem = 0;
	m_collection = 0;
	m_players = 0;
	
	bool loadedFile = false;
	
	QStringList argList = qApp->arguments();
	if(argList.size() > 1)
		loadedFile = readFile(argList.at(1));
	
	if(!loadedFile)
	{
		GLSceneGroupCollection *tmp = new GLSceneGroupCollection();
		GLSceneGroup *group = new GLSceneGroup();
		GLScene *scene = new GLScene();
		group->addScene(scene);
		tmp->addGroup(group);
		setCollection(tmp);
	}
}


DirectorWindow::~DirectorWindow()
{
	delete ui;
}


void DirectorWindow::setupUI()
{
	m_graphicsScene = new QGraphicsScene();
	ui->graphicsView->setScene(m_graphicsScene);
	//ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	ui->graphicsView->setBackgroundBrush(Qt::black);
	m_graphicsScene->setSceneRect(QRectF(0,0,1000.,750.));
	
	
	connect(ui->actionAdd_New_Group, SIGNAL(triggered()), this, SLOT(addNewGroup()));
	connect(ui->actionEdit_Selected_Group, SIGNAL(triggered()), this, SLOT(showEditWindow()));
	connect(ui->actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(ui->actionOpen_Collection, SIGNAL(triggered()), this, SLOT(showOpenFileDialog()));
	connect(ui->actionPlayer_Setup, SIGNAL(triggered()), this, SLOT(showPlayerSetupDialog()));
	connect(ui->actionRemove_Selected_Group, SIGNAL(triggered()), this, SLOT(deleteGroup()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(showSaveAsDialog()));

	connect(ui->collectionListview, SIGNAL(activated(const QModelIndex &)), this, SLOT(groupSelected(const QModelIndex &)));
	connect(ui->collectionListview, SIGNAL(clicked(const QModelIndex &)),   this, SLOT(groupSelected(const QModelIndex &)));

	connect(ui->groupListview, SIGNAL(activated(const QModelIndex &)), this, SLOT(slideSelected(const QModelIndex &)));
	connect(ui->groupListview, SIGNAL(clicked(const QModelIndex &)),   this, SLOT(slideSelected(const QModelIndex &)));

	connect(ui->sceneListview, SIGNAL(activated(const QModelIndex &)), this, SLOT(drawableSelected(const QModelIndex &)));
	connect(ui->sceneListview, SIGNAL(clicked(const QModelIndex &)),   this, SLOT(drawableSelected(const QModelIndex &)));

	connect(ui->playlistView, SIGNAL(activated(const QModelIndex &)), this, SLOT(itemSelected(const QModelIndex &)));
	connect(ui->playlistView, SIGNAL(clicked(const QModelIndex &)),   this, SLOT(itemSelected(const QModelIndex &)));

	connect(ui->itemLengthBox, SIGNAL(valueChanged(double)), this, SLOT(itemLengthChanged(double)));
	connect(ui->hideBtn, SIGNAL(clicked()), this, SLOT(btnHideDrawable()));
	connect(ui->sendToPlayerBtn, SIGNAL(clicked()), this, SLOT(btnSendToPlayer()));
	connect(ui->addToPlaylistBtn, SIGNAL(clicked()), this, SLOT(btnAddToPlaylist()));
}

void DirectorWindow::closeEvent(QCloseEvent */*event*/)
{
	writeSettings();
	saveFile();
}

void DirectorWindow::readSettings()
{
	QSettings settings;
	QPoint pos = settings.value("DirectorWindow/pos", QPoint(10, 10)).toPoint();
	QSize size = settings.value("DirectorWindow/size", QSize(900,700)).toSize();
	move(pos);
	resize(size);

	ui->mainSplitter->restoreState(settings.value("DirectorWindow/MainSplitter").toByteArray());
	ui->sceneSplitter->restoreState(settings.value("DirectorWindow/SceneSplitter").toByteArray());
	ui->drawableSplitter->restoreState(settings.value("DirectorWindow/DrawableSplitter").toByteArray());

	qreal scaleFactor = (qreal)settings.value("DirectorWindow/scaleFactor", 1.).toDouble();
	//qDebug() << "DirectorWindow::readSettings: scaleFactor: "<<scaleFactor;
	ui->graphicsView->setScaleFactor(scaleFactor);
}

void DirectorWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("DirectorWindow/pos", pos());
	settings.setValue("DirectorWindow/size", size());

	settings.setValue("DirectorWindow/MainSplitter",ui->mainSplitter->saveState());
	settings.setValue("DirectorWindow/SceneSplitter",ui->sceneSplitter->saveState());
	settings.setValue("DirectorWindow/DrawableSplitter",ui->drawableSplitter->saveState());

	settings.setValue("DirectorWindow/scaleFactor",ui->graphicsView->scaleFactor());
}

void DirectorWindow::groupSelected(const QModelIndex &idx)
{
	if(idx.isValid())
		setGroup(m_collection->at(idx.row()));
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

void DirectorWindow::drawableSelected(const QModelIndex &idx)
{
	if(idx.isValid())
		setCurrentDrawable(m_currentScene->at(idx.row()));
}
void DirectorWindow::currentDrawableChanged(const QModelIndex &idx,const QModelIndex &)
{
	if(idx.isValid())
		drawableSelected(idx);
}

void DirectorWindow::itemSelected(const QModelIndex &idx)
{
	if(idx.isValid())
		setCurrentItem(m_currentDrawable->playlist()->at(idx.row()));
}
void DirectorWindow::currentItemChanged(const QModelIndex &idx,const QModelIndex &)
{
	if(idx.isValid())
		itemSelected(idx);
}

void DirectorWindow::itemLengthChanged(double len)
{
	if(!m_currentItem)
		return;
	
	m_currentItem->setDuration(len);
}

void DirectorWindow::btnHideDrawable()
{
	if(!m_currentDrawable)
		return;
	QPushButton *btn = dynamic_cast<QPushButton *>(sender());
	if(!btn)
		return;
	m_currentDrawable->setVisible(btn->isChecked());
}
void DirectorWindow::btnSendToPlayer()
{
	/// TODO
}
void DirectorWindow::btnAddToPlaylist()
{
	/// TODO
}

void DirectorWindow::playlistTimeChanged(double)
{
	/// TODO update label
}
void DirectorWindow::playlistItemChanged(GLPlaylistItem *)
{
	/// TODO update selected item in ui->playlistView
	/// TODO send the value of the changed item to the conneted players
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
	/// TODO: create dialog api:
	// 	dlg.setPlayerList(m_players);
	dlg.exec();
}

void DirectorWindow::showEditWindow()
{
	if(!m_currentGroup)
		return;
	
	EditorWindow *edit = new EditorWindow();
	edit->setGroup(m_currentGroup, m_currentScene);
	edit->show();
}
void DirectorWindow::addNewGroup()
{
	GLSceneGroup *group = new GLSceneGroup();
	m_collection->addGroup(group);
	setGroup(group);
}
void DirectorWindow::deleteGroup()
{
	if(!m_currentGroup)
		return;
	
	if (QMessageBox::question(0, tr("Delete Group"), tr("Are you SURE you want to delete this group?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
		return;
	
	m_collection->removeGroup(m_currentGroup);
	
// 	if(m_collection && m_collection->size() > 0)
// 		setGroup(m_collection->at(0));
// 	else
		setGroup(0);
	
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
	ui->collectionListview->setModel(collection);
}
void DirectorWindow::setGroup(GLSceneGroup *group, GLScene */*currentScene*/)
{
	m_currentGroup = group;
	ui->groupListview->setModel(group);
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
	}
	
	//qDebug() << "DirectorWindow::setCurrentScene: "<<scene<<", size:"<<scene->rowCount(QModelIndex());
	m_currentScene = scene;

	ui->sceneListview->setModel(scene);
	scene->setListOnlyUserItems(true);
	
	GLDrawableList list = m_currentScene->drawableList();
	
	// removeItem() before calling clear() because clear() deletes the drawables
	QList<QGraphicsItem*> itemlist = m_graphicsScene->items();
	foreach(QGraphicsItem *item, itemlist)
	{
		GLDrawable *gld = dynamic_cast<GLDrawable*>(item);	
		if(gld)
			m_graphicsScene->removeItem(gld);
	}
	
	m_graphicsScene->clear();
	
	foreach(GLDrawable *drawable, list)
	{
		drawable->setSelected(false);
		m_graphicsScene->addItem(drawable);
	}
	
	//connect(m_scene, SIGNAL(drawableAdded(GLDrawable*)), this, SLOT(drawableAdded(GLDrawable*)));
	//connect(m_scene, SIGNAL(drawableRemoved(GLDrawable*)), this, SLOT(drawableRemoved(GLDrawable*)));
	
	//if(!list.isEmpty())
	//	list.first()->setSelected(true);
}
void DirectorWindow::setCurrentDrawable(GLDrawable *draw)
{
	if(m_currentDrawable)
	{
		disconnect(m_currentDrawable->playlist(), 0, this, 0);
		m_currentDrawable = 0;
	}
	
	m_currentDrawable = draw;
	ui->playlistView->setModel(draw->playlist());
	
	connect(draw->playlist(), SIGNAL(currentItemChanged(GLPlaylistItem*)), this, SLOT(playlistItemChanged(GLPlaylistItem *)));
	connect(draw->playlist(), SIGNAL(playerTimeChanged(double)), this, SLOT(playlistTimeChanged(double)));
	
	/// TODO create item property editor (and connect signals..?)
}

void DirectorWindow::setCurrentItem(GLPlaylistItem *item)
{
	if(!item)
		return;
	
	m_currentDrawable->playlist()->playItem(item);
	m_currentItem = item;
}
