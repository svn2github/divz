#include <QtGui>

#include "MainWindow.h"
#include <QCDEStyle>

// #include "qtvariantproperty.h"
// #include "qttreepropertybrowser.h"

#include "VideoSource.h"
#include "../glvidtex/GLWidget.h"
#include "../glvidtex/GLVideoDrawable.h"
#include "../glvidtex/StaticVideoSource.h"
#include "LayerControlWidget.h"
#include "LiveVideoFileLayer.h"
#include "LiveSceneListModel.h"

MainWindow::MainWindow()
	: QMainWindow()
	, m_currentLayerPropsEditor(0)
	, m_currentScene(0)
	, m_currentLayer(0)
	, m_currentKeyFrameRow(-1)
{

	qRegisterMetaType<VideoSource*>("VideoSource*");

	createActions();
	createMenus();
	createToolBars();
	createStatusBar();

	m_mainSplitter = new QSplitter(this);
	//m_splitter->setOrientation(Qt::Vertical);
	setCentralWidget(m_mainSplitter);

	createLeftPanel();
	createCenterPanel();
	createRightPanel();

	//setupSampleScene();
	newFile();

	readSettings();

	setWindowTitle(tr("LiveMix"));
	setUnifiedTitleAndToolBarOnMac(true);
		
	QString lastFile = QSettings().value("last-lmx-file","vid.lmx").toString();
	loadFile(lastFile);
}

void MainWindow::setupSampleScene()
{
	LiveScene *scene = new LiveScene();

	scene->addLayer(new LiveVideoInputLayer());
	scene->addLayer(new LiveStaticSourceLayer());
	scene->addLayer(new LiveTextLayer());
	scene->addLayer(new LiveVideoFileLayer());

	loadLiveScene(scene);

	//scene->layerList().at(2)->setVisible(true);


	//m_layerViewer->addDrawable(scene->layerList().at(1)->drawable(m_layerViewer));

	/*



// 	GLWidget *glOutputWin = new GLWidget();
// 	{
// 		glOutputWin->setWindowTitle("Live");
// 		glOutputWin->resize(1000,750);
// 		// debugging...
// 		//glOutputWin->setProperty("isEditorWidget",true);
//
// 	}

	GLWidget *glEditorWin = new GLWidget();
	{
		glEditorWin->setWindowTitle("Editor");
		glEditorWin->resize(400,300);
		glEditorWin->setProperty("isEditorWidget",true);
	}

	// Setup Editor Background
	if(true)
	{
		QSize size = glEditorWin->viewport().size().toSize();
		size /= 2.5;
		//qDebug() << "MainWindow::createLeftPanel(): size:"<<size;
		QImage bgImage(size, QImage::Format_ARGB32_Premultiplied);
		QBrush bgTexture(QPixmap("squares2.png"));
		QPainter bgPainter(&bgImage);
		bgPainter.fillRect(bgImage.rect(), bgTexture);
		bgPainter.end();

		StaticVideoSource *source = new StaticVideoSource();
		source->setImage(bgImage);
		//source->setImage(QImage("squares2.png"));
		source->start();

		GLVideoDrawable *drawable = new GLVideoDrawable(glEditorWin);
		drawable->setVideoSource(source);
		drawable->setRect(glEditorWin->viewport());
		drawable->setZIndex(-100);
		drawable->setObjectName("StaticBackground");
		drawable->show();

		glEditorWin->addDrawable(drawable);
	}



// 	// setup scene
// 	LiveScene *scene = new LiveScene();
// 	{
// 		//scene->addLayer(new LiveVideoInputLayer());
// 		scene->addLayer(new LiveStaticSourceLayer());
// 		scene->addLayer(new LiveTextLayer());
// 	}

	// add to live output
	{
		//scene->attachGLWidget(glOutputWin);
		//scene->layerList().at(0)->drawable(glOutputWin)->setObjectName("Output");
		//glOutputWin->addDrawable(scene->layerList().at(0)->drawable(glOutputWin));
		scene->layerList().at(0)->setVisible(true);
		scene->layerList().at(1)->setVisible(true);
	}

	// add to editor
	{
		scene->layerList().at(0)->drawable(glEditorWin)->setObjectName("Editor");
		//glEditorWin->addDrawable(scene->layerList().at(1)->drawable(glEditorWin));
		glEditorWin->addDrawable(scene->layerList().at(1)->drawable(glEditorWin));
		m_layerViewer->addDrawable(scene->layerList().at(1)->drawable(m_layerViewer));

	}

	// show windows
	{
		//glOutputWin->show();
		glEditorWin->show();
	}*/




	//m_layerViewer->addDrawable(scene->layerList().at(2)->drawable(m_layerViewer));
}

void MainWindow::loadLiveScene(LiveScene *scene)
{
	if(m_currentScene)
	{
		removeCurrentScene();
		delete m_currentScene;
	}

	m_currentScene = scene;

	// attach to main viewer
	scene->attachGLWidget(m_mainViewer);
	
	m_sceneModel->setLiveScene(scene);
	
	loadKeyFramesToTable();

	// attach to main output
	/// TODO main output
}

void MainWindow::removeCurrentScene()
{
	if(!m_currentScene)
		return;

	disconnect(m_currentScene, 0, this, 0);
	m_currentScene->detachGLWidget(m_mainViewer);

	m_currentScene = 0;

	setCurrentLayer(0);
	
	loadKeyFramesToTable();

}


void MainWindow::deleteCurrentLayer()
{
	LiveLayer *layer = m_currentLayer;
	if(layer)
	{
		setCurrentLayer(0);
		if(m_currentScene)
			m_currentScene->removeLayer(layer);
		delete layer;
		layer = 0;
	}
}


void MainWindow::setCurrentLayer(LiveLayer *layer)
{
	if(m_currentLayer)
	{
//  		qDebug() << "MainWindow::setCurrentLayer(): removing old layer from editor";
		m_currentLayer->detachGLWidget(m_layerViewer);
	}

	m_currentLayer = layer;
	loadLayerProperties(m_currentLayer);
	
	m_deleteLayerAct->setEnabled(layer!=NULL);
		
	if(m_currentLayer)
		m_currentLayer->attachGLWidget(m_layerViewer);
}

void MainWindow::loadLayerProperties(LiveLayer *layer)
{
	if(m_currentLayerPropsEditor)
	{
		m_controlBase->layout()->removeWidget(m_currentLayerPropsEditor);
		m_currentLayerPropsEditor->deleteLater();
		m_currentLayerPropsEditor = 0;
	}

	if(!m_currentLayer)
		return;
		
	QWidget *props = m_currentLayer->createLayerPropertyEditors();

	QVBoxLayout *layout = dynamic_cast<QVBoxLayout*>(m_controlBase->layout());
	while(layout->count() > 0)
	{
		QLayoutItem * item = layout->itemAt(layout->count() - 1);
		layout->removeItem(item);
		delete item;
	}

	layout->addWidget(props);
	layout->addStretch(1);

	m_currentLayerPropsEditor = props;
}

void MainWindow::createLeftPanel()
{
	m_leftSplitter = new QSplitter(this);
	m_leftSplitter->setOrientation(Qt::Vertical);
	
	m_layerListView = new QListView(m_leftSplitter);
	m_layerListView->setViewMode(QListView::ListMode);
	//m_layerListView->setViewMode(QListView::IconMode);
	m_layerListView->setMovement(QListView::Free);
	m_layerListView->setWordWrap(true);
	m_layerListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_layerListView->setDragEnabled(true);
	m_layerListView->setAcceptDrops(true);
	m_layerListView->setDropIndicatorShown(true);

	connect(m_layerListView, SIGNAL(activated(const QModelIndex &)), this, SLOT(layerSelected(const QModelIndex &)));
	connect(m_layerListView, SIGNAL(clicked(const QModelIndex &)),   this, SLOT(layerSelected(const QModelIndex &)));

	// deleting old selection model per http://doc.trolltech.com/4.5/qabstractitemview.html#setModel
	QItemSelectionModel *m = m_layerListView->selectionModel();

	m_sceneModel = new LiveSceneListModel();
	m_layerListView->setModel(m_sceneModel);
	connect(m_sceneModel, SIGNAL(repaintList()), this, SLOT(repaintLayerList()));
	connect(m_sceneModel, SIGNAL(layersDropped(QList<LiveLayer*>)), this, SLOT(layersDropped(QList<LiveLayer*>)));

	if(m)
	{       
		delete m;
		m=0;
	}

	QItemSelectionModel *currentSelectionModel = m_layerListView->selectionModel();
	connect(currentSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(currentChanged(const QModelIndex &, const QModelIndex &)));
	
	//parentLyout->addWidget(m_layerListView);
	
	
	m_leftSplitter->addWidget(m_layerListView);
	
	QWidget *tableBase = new QWidget(m_leftSplitter);
	
	QVBoxLayout *layout = new QVBoxLayout(tableBase);
	
	QHBoxLayout *btnLayout = new QHBoxLayout();
	m_keyNewBtn = new QPushButton("Create Frame",tableBase);
	m_keyDelBtn = new QPushButton("Delete Frame",tableBase);
	m_keyUpdateBtn = new QPushButton("Update Frame",tableBase);
	
	btnLayout->addWidget(m_keyNewBtn);
	btnLayout->addWidget(m_keyDelBtn);
	btnLayout->addWidget(m_keyUpdateBtn);
	layout->addLayout(btnLayout);
	
	m_keyDelBtn->setDisabled(true); // enabled when a row is selected
	m_keyUpdateBtn->setDisabled(true);
	
	connect(m_keyNewBtn, SIGNAL(clicked()), this, SLOT(createKeyFrame()));
	connect(m_keyDelBtn, SIGNAL(clicked()), this, SLOT(deleteKeyFrame()));
	connect(m_keyUpdateBtn, SIGNAL(clicked()), this, SLOT(updateKeyFrame()));
	
	
	m_timelineTable = new QTableWidget(tableBase);
	m_timelineTable->verticalHeader()->setVisible(false);
	m_timelineTable->horizontalHeader()->setVisible(false);
	m_timelineTable->setColumnCount(3);
	m_timelineTable->setHorizontalHeaderLabels(QStringList() << "#" << "Description"<<"Show");
	m_timelineTable->setRowCount(0);
	m_timelineTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_timelineTable->resizeColumnsToContents();
	m_timelineTable->resizeRowsToContents();
	connect(m_timelineTable, SIGNAL(cellClicked(int,int)), this, SLOT(slotTimelineTableCellActivated(int,int)));
	connect(m_timelineTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(slotTimelineTableCellEdited(QTableWidgetItem*)));
	
	layout->addWidget(m_timelineTable);
	m_leftSplitter->addWidget(tableBase);
	
	m_mainSplitter->addWidget(m_leftSplitter);

}

void MainWindow::slotTimelineTableCellActivated(int row,int)
{
	m_keyDelBtn->setEnabled(true);
	m_keyUpdateBtn->setEnabled(true);
	m_currentKeyFrameRow = row;
}

void MainWindow::createKeyFrame()
{
	if(!m_currentScene)
		return;
		
	QPixmap pixmap = QPixmap::fromImage(m_mainViewer->grabFrameBuffer());
	m_currentScene->createAndAddKeyFrame(pixmap);
	
	loadKeyFramesToTable();
}

void MainWindow::deleteKeyFrame()
{
	if(m_currentKeyFrameRow<0)
		return;
	if(!m_currentScene)
		return;
		
	m_currentScene->removeKeyFrame(m_currentKeyFrameRow);
	m_currentKeyFrameRow = -1;
	m_keyDelBtn->setEnabled(false);
	m_keyUpdateBtn->setEnabled(false);
	
	loadKeyFramesToTable();
}

void MainWindow::updateKeyFrame()
{
	if(m_currentKeyFrameRow<0)
		return;
	if(!m_currentScene)
		return;
	
	QPixmap pixmap = QPixmap::fromImage(m_mainViewer->grabFrameBuffer());
	m_currentScene->updateKeyFrame(m_currentKeyFrameRow, pixmap);
	
	loadKeyFramesToTable();
}

void MainWindow::slotTimelineTableCellEdited(QTableWidgetItem *item)
{
	if(!item)
		return;
	
	if(!m_currentScene)
		return;
		
	if(item->column() != 1)
		return;
	
	QList<LiveScene::KeyFrame> frames = m_currentScene->keyFrames();
	int row = item->row();
	if(row < 0 || row >frames.size())
		return;
	
	m_currentScene->setKeyFrameName(row,item->text());
	
	//qDebug() << "MainWindow::slotTimelineTableCellEdited(): row:"<<row<<", new text:"<<m_currentScene->keyFrames().at(row).frameName<<", text:"<<item->text();
	
}

void MainWindow::loadKeyFramesToTable()
{
	m_timelineTable->clear();
	if(!m_currentScene)
		return;
		
	QList<LiveScene::KeyFrame> frames = m_currentScene->keyFrames();
	
	m_timelineTable->setRowCount(frames.size());
	
	QTableWidgetItem *prototype = new QTableWidgetItem();
	prototype->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

	int row=0;
	foreach(LiveScene::KeyFrame frame, frames)
	{
		QTableWidgetItem *t = prototype->clone();
		if(!frame.pixmap.isNull())
			t->setIcon(QIcon(frame.pixmap.scaled(QSize(32,32),Qt::KeepAspectRatio)));
		else
			t->setText(QString::number(frame.id));
		m_timelineTable->setItem(row,0,t);
		
		t = prototype->clone();
		t->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable);
		t->setText(frame.frameName);
		m_timelineTable->setItem(row,1,t);
		
		QPushButton *btn = new QPushButton("Show");
		btn->setProperty("keyFrameRow", row);
		
		connect(btn, SIGNAL(clicked()), this, SLOT(keyFrameBtnActivated()));
		m_timelineTable->setCellWidget(row,2,btn);
		
		row++;
	}
}

void MainWindow::keyFrameBtnActivated()
{
	QPushButton *btn = dynamic_cast<QPushButton*>(sender());
	if(!btn)
		return;
	QVariant prop = btn->property("keyFrameRow");
	if(!prop.isValid())
		return;

	int row = prop.toInt();
	
	if(!m_currentScene)
		return;
		
	m_currentKeyFrameRow = row;
	
// 	QList<LiveScene::KeyFrame> frames = m_currentScene->keyFrames();
// 	if(row < 0 || row >frames.size())
// 		return;
// 	
// 	LiveScene::keyFrame frame = frames[row];
	
	//qDebug() << "MainWindow::slotTimelineTableCellEdited(): row:"<<item->row()<<", new text:"<<m_currentScene->keyFrames()[item->row()].frameName();
	m_currentScene->applyKeyFrame(row);
		
}

void MainWindow::createCenterPanel()
{
	m_editSplitter = new QSplitter(m_mainSplitter);
	m_editSplitter->setOrientation(Qt::Vertical);

	m_layerViewer = new GLWidget(m_editSplitter);
	m_layerViewer->setProperty("isEditorWidget",true);

	QSize size = m_layerViewer->viewport().size().toSize();
	size /= 2.5;
	qDebug() << "MainWindow::createLeftPanel(): size:"<<size;
	QImage bgImage(size, QImage::Format_ARGB32_Premultiplied);
	QBrush bgTexture(QPixmap("squares2.png"));
	QPainter bgPainter(&bgImage);
	bgPainter.fillRect(bgImage.rect(), bgTexture);
	bgPainter.end();

	StaticVideoSource *source = new StaticVideoSource();
	source->setImage(bgImage);
	//source->setImage(QImage("squares2.png"));
	source->start();
	connect(this, SIGNAL(destroyed()), source, SLOT(deleteLater()));

	GLVideoDrawable *drawable = new GLVideoDrawable(m_layerViewer);
	drawable->setVideoSource(source);
	drawable->setRect(m_layerViewer->viewport());
	drawable->setZIndex(-100);
	drawable->setObjectName("StaticBackground");
	drawable->show();

	m_layerViewer->addDrawable(drawable);
	
	
	m_editSplitter->addWidget(m_layerViewer);
	
	m_controlArea = new QScrollArea(m_editSplitter);
	m_controlArea->setWidgetResizable(true);
	m_controlBase = new QWidget(m_controlArea);
	
	QVBoxLayout *layout = new QVBoxLayout(m_controlBase);
	layout->setContentsMargins(0,0,0,0);
	
	m_controlArea->setWidget(m_controlBase);
	m_editSplitter->addWidget(m_controlArea);
	
	m_mainSplitter->addWidget(m_editSplitter);

}

void MainWindow::layerSelected(const QModelIndex &idx)
{
	LiveLayer *layer = m_sceneModel->itemFromIndex(idx);
	//qDebug() << "SlideEditorWindow::slideSelected(): selected slide#:"<<s->slideNumber();
	if(m_currentLayer != layer)
		setCurrentLayer(layer);
}


void MainWindow::currentChanged(const QModelIndex &idx,const QModelIndex &)
{
	if(idx.isValid())
		layerSelected(idx);
}

void MainWindow::repaintLayerList()
{
	m_layerListView->clearFocus();
	m_layerListView->setFocus();
	m_layerListView->update();
}


void MainWindow::layersDropped(QList<LiveLayer*> list)
{
	QModelIndex idx = m_sceneModel->indexForItem(list.first());
	m_layerListView->setCurrentIndex(idx);
}


void MainWindow::createRightPanel()
{
	m_previewSplitter = new QSplitter(m_mainSplitter);
	m_previewSplitter->setOrientation(Qt::Vertical);
	/*
	QWidget *base = new QWidget(m_mainSplitter);
	QVBoxLayout *layout = new QVBoxLayout(base);*/

	m_mainViewer = new GLWidget(m_previewSplitter);
	m_previewSplitter->addWidget(m_mainViewer);
	//layout->addStretch(1);

	m_mainSplitter->addWidget(m_previewSplitter);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	writeSettings();
	event->accept();
}


void MainWindow::about()
{
	QMessageBox::about(this, tr("About LiveMix"),
		tr("<b>LiveMix</b> is an open-source video mixer for live and recorded video."));
}

void MainWindow::createActions()
{
	m_newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
	m_newAct->setShortcuts(QKeySequence::New);
	m_newAct->setStatusTip(tr("Create a new file"));
	connect(m_newAct, SIGNAL(triggered()), this, SLOT(newFile()));

	m_openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	m_openAct->setShortcuts(QKeySequence::Open);
	m_openAct->setStatusTip(tr("Open an existing file"));
	connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

	m_saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
	m_saveAct->setShortcuts(QKeySequence::Save);
	m_saveAct->setStatusTip(tr("Save the document to disk"));
	connect(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

	m_saveAsAct = new QAction(tr("Save &As..."), this);
	m_saveAsAct->setShortcuts(QKeySequence::SaveAs);
	m_saveAsAct->setStatusTip(tr("Save the document under a new name"));
	connect(m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

	//! [0]
	m_exitAct = new QAction(tr("E&xit"), this);
	m_exitAct->setShortcuts(QKeySequence::Quit);
	m_exitAct->setStatusTip(tr("Exit the application"));
	connect(m_exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
	//! [0]

// 	cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
// 	cutAct->setShortcuts(QKeySequence::Cut);
// 	cutAct->setStatusTip(tr("Cut the current selection's contents to the "
// 				"clipboard"));
// 	connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));


	m_aboutAct = new QAction(tr("&About"), this);
	m_aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	m_aboutQtAct = new QAction(tr("About &Qt"), this);
	m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

 	
	m_newCameraLayerAct = new QAction(QIcon("../data/stock-panel-screenshot.png"), tr("New Camera Layer"), this);
	connect(m_newCameraLayerAct, SIGNAL(triggered()), this, SLOT(newCameraLayer()));
	
	m_newVideoLayerAct = new QAction(QIcon("../data/stock-panel-multimedia.png"), tr("New Video Layer"), this);
	connect(m_newVideoLayerAct, SIGNAL(triggered()), this, SLOT(newVideoLayer()));
		
	m_newTextLayerAct = new QAction(QIcon("../data/stock-font.png"), tr("New Text Layer"), this);
	connect(m_newTextLayerAct, SIGNAL(triggered()), this, SLOT(newTextLayer()));
	
	m_newImageLayerAct = new QAction(QIcon("../data/stock-insert-image.png"), tr("New Image Layer"), this);
	connect(m_newImageLayerAct, SIGNAL(triggered()), this, SLOT(newImageLayer()));
	
	m_deleteLayerAct = new QAction(QIcon("../data/action-delete.png"), tr("Delete Current Layer"), this);
	connect(m_deleteLayerAct, SIGNAL(triggered()), this, SLOT(deleteCurrentLayer()));

}

void MainWindow::createMenus()
{
	m_fileMenu = menuBar()->addMenu(tr("&File"));

	m_fileMenu->addAction(m_newAct);
	m_fileMenu->addAction(m_openAct);
	m_fileMenu->addAction(m_saveAct);
	m_fileMenu->addAction(m_saveAsAct);
 	
 	m_fileMenu->addSeparator();

	m_fileMenu->addAction(m_exitAct);

	menuBar()->addSeparator();

	m_helpMenu = menuBar()->addMenu(tr("&Help"));
	m_helpMenu->addAction(m_aboutAct);
	m_helpMenu->addAction(m_aboutQtAct);
}

void MainWindow::createToolBars()
{
	m_fileToolBar = addToolBar(tr("Layer Toolbar"));
	
	m_fileToolBar->addAction(m_newCameraLayerAct);
	m_fileToolBar->addAction(m_newVideoLayerAct);
	m_fileToolBar->addAction(m_newTextLayerAct);
	m_fileToolBar->addAction(m_newImageLayerAct);
	
	m_fileToolBar->addSeparator();
	
	m_fileToolBar->addAction(m_deleteLayerAct);
	
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
	QSettings settings;
	QPoint pos = settings.value("mainwindow/pos", QPoint(10, 10)).toPoint();
	QSize size = settings.value("mainwindow/size", QSize(640,480)).toSize();
	move(pos);
	resize(size);

	m_mainSplitter->restoreState(settings.value("mainwindow/main_splitter").toByteArray());
	m_editSplitter->restoreState(settings.value("mainwindow/left_splitter").toByteArray());

}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("mainwindow/pos", pos());
	settings.setValue("mainwindow/size", size());

	settings.setValue("mainwindow/main_splitter",m_mainSplitter->saveState());
	settings.setValue("mainwindow/left_splitter",m_editSplitter->saveState());
}

void MainWindow::newFile()
{
	LiveScene *old = m_currentScene;
	
	setCurrentLayer(0);
	
	LiveScene *scene = new LiveScene();
	loadLiveScene(scene);
	
	if(old)
		old->deleteLater();
		
	setWindowTitle("New File - LiveMix");
}

void MainWindow::open()
{
	QString curFile = m_currentFile;
	if(curFile.trimmed().isEmpty())
		curFile = QSettings().value("last-livemix-file","").toString();

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select LiveMix File"), curFile, tr("LiveMix File (*.livemix *.lmx);;Any File (*.*)"));
	if(fileName != "")
	{
		QSettings().setValue("last-livemix-file",fileName);
		if(QFile(fileName).exists())
		{
			loadFile(fileName);
		}
		else
		{
			QMessageBox::critical(this,tr("File Does Not Exist"),tr("Sorry, but the file you chose does not exist. Please try again."));
		}
	}
	
}

void MainWindow::loadFile(const QString& fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly)) 
	{
		QMessageBox::critical(0, tr("Loading error"), tr("Unable to read file %1").arg(fileName));
		return;
	}
	
	QSettings().setValue("last-lmx-file",fileName);

	m_currentFile = fileName;
	
	QByteArray array = file.readAll();
	
	LiveScene *old = m_currentScene;

	LiveScene *scene = new LiveScene();
	scene->fromByteArray(array);
	
	loadLiveScene(scene);
	
	if(old)
		old->deleteLater();
		
	setWindowTitle(QString("%1 - LiveMix").arg(QFileInfo(fileName).fileName()));
}

void MainWindow::save(const QString& filename)
{
	if(!m_currentScene)
	{
		QMessageBox::warning(0, tr("Save Error"), tr("No current scene."));
		return;
	}
		
	QString tmp = filename;
	if(tmp.isEmpty())
		tmp = m_currentFile;
	else
		m_currentFile = tmp;
		
	if(tmp.isEmpty())
	{
		saveAs();
		return;
	}
	
	QFile file(tmp);
	// Open file
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(0, tr("File Error"), tr("Error saving writing file '%1'").arg(tmp));
		//throw 0;
		return;
	}
	
	//QByteArray array;
	//QDataStream stream(&array, QIODevice::WriteOnly);
	//QVariantMap map;
	
	file.write(m_currentScene->toByteArray());
	file.close();
}

void MainWindow::saveAs()
{
	QString curFile = m_currentFile;
	if(curFile.trimmed().isEmpty())
		curFile = QSettings().value("last-livemix-file","").toString();

	QString fileName = QFileDialog::getSaveFileName(this, tr("Choose a Filename"), curFile, tr("LiveMix File (*.livemix *.lmx);;Any File (*.*)"));
	if(fileName != "")
	{
		//QFileInfo info(fileName);
		//if(info.suffix().isEmpty())
			//fileName += ".dviz";
		QSettings().setValue("last-livemix-file",fileName);
		save(fileName);
		//return true;
	}

	//return false;
}

void MainWindow::addLayer(LiveLayer *layer)
{
	if(!m_currentScene)
		m_currentScene = new LiveScene();
	m_currentScene->addLayer(layer);
	layer->setZIndex(-m_currentScene->layerList().size());
	//loadLiveScene(m_currentScene);
}

void MainWindow::newCameraLayer()
{
	addLayer(new LiveVideoInputLayer());
}

void MainWindow::newVideoLayer()
{
	addLayer(new LiveVideoFileLayer());
}

void MainWindow::newTextLayer()
{
	addLayer(new LiveTextLayer());
}

void MainWindow::newImageLayer()
{
	addLayer(new LiveStaticSourceLayer());
}

