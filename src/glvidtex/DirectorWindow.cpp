#include "DirectorWindow.h"
#include "ui_DirectorWindow.h"
#include "PlayerSetupDialog.h"
#include "EditorWindow.h"
#include "../livemix/EditorUtilityWidgets.h"
#include "RtfEditorWindow.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QApplication>
#include <QGLWidget>

#include "GLDrawables.h"
#include "GLSceneGroup.h"
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
	
	ui->drawableGroupBox->setVisible(false);
	ui->sceneListview->setVisible(false);
	
	ui->itemNameLabel->setText("");
	ui->timeLabel->setText("");
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
	if(!m_collection)
		return;
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
	if(!m_currentScene)
		return;
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
	if(!m_currentDrawable)
		return;
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
	m_currentDrawable->setHidden(btn->isChecked());
}
void DirectorWindow::btnSendToPlayer()
{
	/// TODO
}
void DirectorWindow::btnAddToPlaylist()
{
	if(!m_currentDrawable)
		return;
	GLDrawablePlaylist *playlist = m_currentDrawable->playlist();
	GLPlaylistItem * item = new GLPlaylistItem();
	
	QString userProp = QString(m_currentDrawable->metaObject()->userProperty().name());
	QVariant prop = m_currentDrawable->property(qPrintable(userProp));
	item->setValue(prop);
	
	if(GLTextDrawable *casted = dynamic_cast<GLTextDrawable*>(m_currentDrawable))
	{
		item->setTitle(casted->plainText());
		item->setDuration(5.0); /// just a guess
	}
	else
	if(GLVideoFileDrawable *casted = dynamic_cast<GLVideoFileDrawable*>(m_currentDrawable))
	{
		item->setTitle(QFileInfo(casted->videoFile()).fileName());
		item->setDuration(casted->videoLength());
	}
	else
	if(GLVideoLoopDrawable *casted = dynamic_cast<GLVideoLoopDrawable*>(m_currentDrawable))
	{
		item->setTitle(QFileInfo(casted->videoFile()).fileName());
		item->setDuration(casted->videoLength());
	}
	else
	if(GLImageDrawable *casted = dynamic_cast<GLImageDrawable*>(m_currentDrawable))
	{
		item->setTitle(QFileInfo(casted->imageFile()).fileName());
		item->setDuration(5.0); /// just a guess
	}
	else
	{
		item->setTitle(prop.toString());
		item->setDuration(5.0); /// just a guess
	}
	
	if(!item->duration())
		item->setDuration(15.0);
	
	playlist->addItem(item);
}

void DirectorWindow::playlistTimeChanged(double value)
{
	QString time = "";
	
	double min = value/60;
	double sec = (min - (int)(min)) * 60;
	double ms  = (sec - (int)(sec)) * 60;
	time =  (min<10? "0":"") + QString::number((int)min) + ":" +
		(sec<10? "0":"") + QString::number((int)sec) + "." +
		(ms <10? "0":"") + QString::number((int)ms );
		
	ui->timeLabel->setText(time);
}

void DirectorWindow::playlistItemChanged(GLPlaylistItem *item)
{
	/// TODO send the value of the changed item to the conneted players
	
	if(m_currentItem)
		disconnect(ui->itemLengthBox, 0, m_currentItem, 0);
	
	m_currentItem = item;
	if(item)
	{
		ui->itemLengthBox->setValue(item->duration());
		connect(ui->itemLengthBox, SIGNAL(valueChanged(double)), item, SLOT(setDuration(double)));
		
		ui->playlistView->setCurrentIndex(m_currentDrawable->playlist()->indexOf(item));
	}
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
	
	if(collection->size() > 0)
	{
		QModelIndex idx = collection->index(0, 0);
		ui->collectionListview->setCurrentIndex(idx);
		groupSelected(idx);
	}
}
void DirectorWindow::setGroup(GLSceneGroup *group, GLScene */*currentScene*/)
{
	m_currentGroup = group;
	ui->groupListview->setModel(group);
	
	if(group->size() > 0)
	{
		QModelIndex idx = group->index(0, 0);
		ui->groupListview->setCurrentIndex(idx);
		slideSelected(idx);
	}
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
	
	bool hasUserItems = scene->size();
	
	ui->drawableGroupBox->setVisible(hasUserItems);
	ui->sceneListview->setVisible(hasUserItems);
	
	
	//connect(m_scene, SIGNAL(drawableAdded(GLDrawable*)), this, SLOT(drawableAdded(GLDrawable*)));
	//connect(m_scene, SIGNAL(drawableRemoved(GLDrawable*)), this, SLOT(drawableRemoved(GLDrawable*)));
	
	//if(!list.isEmpty())
	//	list.first()->setSelected(true);
	
	if(scene->size() > 0)
	{
		QModelIndex idx = scene->index(0, 0);
		ui->sceneListview->setCurrentIndex(idx);
		drawableSelected(idx);
	}
}

void DirectorWindow::setCurrentDrawable(GLDrawable *gld)
{
	if(m_currentDrawable)
	{
		if(m_currentDrawable->playlist()->isPlaying())
			m_currentDrawable->playlist()->stop();
		
		disconnect(m_currentDrawable->playlist(), 0, this, 0);
		m_currentDrawable = 0;
	}
	
	m_currentDrawable = gld;
	ui->playlistView->setModel(gld->playlist());
	
	connect(gld->playlist(), SIGNAL(currentItemChanged(GLPlaylistItem*)), this, SLOT(playlistItemChanged(GLPlaylistItem *)));
	connect(gld->playlist(), SIGNAL(playerTimeChanged(double)), this, SLOT(playlistTimeChanged(double)));
	
	connect(ui->playBtn,  SIGNAL(clicked()), gld->playlist(), SLOT(play()));
	connect(ui->pauseBtn, SIGNAL(clicked()), gld->playlist(), SLOT(stop()));
	
	QString itemName = gld->itemName();
	QString typeName;
	
	// Fun loop to clear out a QFormLayout - why doesn't QLayout just have a removeAll() or clear() method?
	QFormLayout *form = ui->itemPropLayout;
	while(form->count() > 0)
	{
		QLayoutItem * item = form->itemAt(form->count() - 1);
		form->removeItem(item);
		if(QWidget *widget = item->widget())
		{
			//qDebug() << "DirectorWindow::setCurrentDrawable: Deleting "<<widget<<" from form, total:"<<form->count()<<" items remaining.";
			form->removeWidget(widget);
			if(QWidget *label = form->labelForField(widget))
			{
				form->removeWidget(label);
				delete label;
			}
			delete widget;
		}
		delete item;
	}
	
	PropertyEditorFactory::PropertyEditorOptions opts;
	opts.reset();	
	
	if(GLVideoInputDrawable *item = dynamic_cast<GLVideoInputDrawable*>(gld))
	{
		// show device selection box
		// show deinterlace checkbox
		Q_UNUSED(item);

		typeName = "Video Input Item";
	}
	else
	if(GLVideoLoopDrawable *item = dynamic_cast<GLVideoLoopDrawable*>(gld))
	{
		opts.reset();
		opts.stringIsFile = true;
		opts.fileTypeFilter = tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif);;Any File (*.*)");

		ui->itemPropLayout->addRow(tr("&File:"), PropertyEditorFactory::generatePropertyEditor(item, "videoFile", SLOT(setVideoFile(const QString&)), opts));

		typeName = "Video Loop Item";
	}
	else
	if(GLVideoFileDrawable *item = dynamic_cast<GLVideoFileDrawable*>(gld))
	{
		PropertyEditorFactory::PropertyEditorOptions opts;
		opts.stringIsFile = true;
		opts.fileTypeFilter = tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif);;Any File (*.*)");
		
		ui->itemPropLayout->addRow(tr("&File:"), PropertyEditorFactory::generatePropertyEditor(item, "videoFile", SLOT(setVideoFile(const QString&)), opts));

		typeName = "Video Item";
	}
	else
	if(GLTextDrawable *item = dynamic_cast<GLTextDrawable*>(gld))
	{
		opts.reset();
		
		QWidget *edit = PropertyEditorFactory::generatePropertyEditor(gld, "plainText", SLOT(setPlainText(const QString&)), opts);
		
		QLineEdit *line = dynamic_cast<QLineEdit*>(edit);
		if(line)
			connect(gld, SIGNAL(plainTextChanged(const QString&)), line, SLOT(setText(const QString&)));
		
		QWidget *base = new QWidget();
		
		RtfEditorWindow *dlg = new RtfEditorWindow(item, base);
		QPushButton *btn = new QPushButton("&Advanced...");
		connect(btn, SIGNAL(clicked()), dlg, SLOT(show()));
		
		QHBoxLayout *hbox = new QHBoxLayout(base);
		hbox->setContentsMargins(0,0,0,0);
		hbox->addWidget(new QLabel("Text:"));
		hbox->addWidget(edit);
		hbox->addWidget(btn);
		
		ui->itemPropLayout->addRow(base); 

		typeName = "Text Item";
	}
	else
	if(GLImageDrawable *item = dynamic_cast<GLImageDrawable*>(gld))
	{
		PropertyEditorFactory::PropertyEditorOptions opts;
		opts.stringIsFile = true;
		opts.fileTypeFilter = tr("Image Files (*.png *.jpg *.bmp *.svg *.xpm *.gif);;Any File (*.*)");
		ui->itemPropLayout->addRow(tr("&Image:"), PropertyEditorFactory::generatePropertyEditor(item, "imageFile", SLOT(setImageFile(const QString&)), opts));
		
		typeName = "Image Item";
	} 
	
	
	ui->itemNameLabel->setText(itemName.isEmpty() ? QString("<b>%1</b>").arg(typeName) : QString("<b>%1</b> (%2)").arg(itemName).arg(typeName));
}

void DirectorWindow::setCurrentItem(GLPlaylistItem *item)
{
	if(!item)
		return;
	
	m_currentDrawable->playlist()->playItem(item);
	m_currentItem = item;
}

