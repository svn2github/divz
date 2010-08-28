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

MainWindow::MainWindow()
	: QMainWindow()
	, m_currentLayerPropsEditor(0)
	, m_currentScene(0)
	, m_currentControlWidget(0)
	, m_currentLayer(0)
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
		removeCurrentScene();

	m_currentScene = scene;

	// attach to main viewer
	scene->attachGLWidget(m_mainViewer);

	// attach to main output
	/// TODO main output

	// Load layers into list
	connect(scene, SIGNAL(layerAdded(LiveLayer*)),   this, SLOT(updateLayerList()));
	connect(scene, SIGNAL(layerRemoved(LiveLayer*)), this, SLOT(updateLayerList()));
	updateLayerList();
}

void MainWindow::removeCurrentScene()
{
	if(!m_currentScene)
		return;

	disconnect(m_currentScene, 0, this, 0);
	m_currentScene->detachGLWidget();

	m_currentScene = 0;

	updateLayerList();

}

void MainWindow::updateLayerList()
{
	if(!m_currentScene)
	{
		//qDebug() << "MainWindow::updateLayerList(): No current scene";
		foreach(LiveLayer *layer, m_controlWidgetMap.keys())
		{
			m_layerBase->layout()->removeWidget(m_controlWidgetMap[layer]);
			m_controlWidgetMap.remove(layer);
			m_controlWidgetMap[layer]->deleteLater();
			m_controlWidgetMap[layer] = 0;
		}
		return;
	}

	QList<LiveLayer*> layers = m_currentScene->layerList();

	/// TODO resort layer list on screen by Z order

	QList<LiveLayer*> foundLayer;

	foreach(LiveLayer *layer, layers)
	{
		if(m_controlWidgetMap.contains(layer))
		{
// 			qDebug() << "MainWindow::updateLayerList(): control map has layer already"<<layer;
			foundLayer.append(layer);
		}
		else
		{
			LayerControlWidget *widget = new LayerControlWidget(layer);
			m_layerBase->layout()->addWidget(widget);
			widget->show();

// 			qDebug() << "MainWindow::updateLayerList(): adding control widget for layer"<<layer<<", widget:"<<widget;

			m_controlWidgetMap[layer] = widget;
			foundLayer.append(layer);

			connect(widget, SIGNAL(clicked()), this, SLOT(liveLayerClicked()));
		}
	}

	foreach(LiveLayer *layer, m_controlWidgetMap.keys())
	{
		if(!foundLayer.contains(layer))
		{
// 			qDebug() << "MainWindow::updateLayerList(): layer not found, removing control widget"<<layer;
			disconnect(m_controlWidgetMap[layer], 0, this, 0);
			m_layerBase->layout()->removeWidget(m_controlWidgetMap[layer]);
			m_controlWidgetMap.remove(layer);
			m_controlWidgetMap[layer]->deleteLater();
			m_controlWidgetMap[layer] = 0;

		}
	}
}

void MainWindow::liveLayerClicked()
{
	LayerControlWidget *widget = dynamic_cast<LayerControlWidget*>(sender());
	if(m_currentControlWidget == widget)
		return;

// 	qDebug() << "MainWindow::liveLayerClicked(): m_layerViewer:"<<m_layerViewer;
	if(m_currentControlWidget)
	{
// 		qDebug() << "MainWindow::liveLayerClicked(): removing old layer from editor";
		m_currentControlWidget->setIsCurrentWidget(false);
		m_layerViewer->removeDrawable(m_currentControlWidget->layer()->drawable(m_layerViewer));
	}

	if(widget)
	{
// 		qDebug() << "MainWindow::liveLayerClicked(): adding clicked layer to editor";
		widget->setIsCurrentWidget(true);
		loadLayerProperties(widget->layer());
		m_layerViewer->addDrawable(widget->layer()->drawable(m_layerViewer));
		m_currentControlWidget = widget;
	}
}

void MainWindow::loadLayerProperties(LiveLayer *layer)
{
	m_currentLayer = layer;
	
	if (!m_currentLayer)
	{
		//deleteAction->setEnabled(false);
		return;
	}

	if(m_currentLayerPropsEditor)
	{
		m_controlBase->layout()->removeWidget(m_currentLayerPropsEditor);
		m_currentLayerPropsEditor->deleteLater();
		m_currentLayerPropsEditor = 0;
	}

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
	m_leftSplitter = new QSplitter(m_mainSplitter);
	m_leftSplitter->setOrientation(Qt::Vertical);

	m_layerViewer = new GLWidget(m_leftSplitter);
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

	GLVideoDrawable *drawable = new GLVideoDrawable(m_layerViewer);
	drawable->setVideoSource(source);
	drawable->setRect(m_layerViewer->viewport());
	drawable->setZIndex(-100);
	drawable->setObjectName("StaticBackground");
	drawable->show();

	m_layerViewer->addDrawable(drawable);


	m_leftSplitter->addWidget(m_layerViewer);

 	m_controlArea = new QScrollArea(m_leftSplitter);
 	m_controlArea->setWidgetResizable(true);
 	m_controlBase = new QWidget(m_controlArea);

 	QVBoxLayout *layout = new QVBoxLayout(m_controlBase);
 	layout->setContentsMargins(0,0,0,0);

 	m_controlArea->setWidget(m_controlBase);
 	m_leftSplitter->addWidget(m_controlArea);

// 	m_variantManager = new QtVariantPropertyManager(this);
//
// 	connect(m_variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
// 			      this, SLOT(valueChanged(QtProperty *, const QVariant &)));
//
// 	QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);

// 	canvas = new QtCanvas(800, 600);
// 	canvasView = new CanvasView(canvas, this);
// 	setCentralWidget(canvasView);

// 	QDockWidget *dock = new QDockWidget(this);
// 	addDockWidget(Qt::RightDockWidgetArea, dock);

// 	m_propertyEditor = new QtTreePropertyBrowser(m_leftSplitter);
// 	m_propertyEditor->setFactoryForManager(m_variantManager, variantFactory);
//	m_leftSplitter->addWidget(m_propertyEditor);

// 	m_currentItem = 0;

// 	connect(canvasView, SIGNAL(itemClicked(QtCanvasItem *)),
// 		this, SLOT(itemClicked(QtCanvasItem *)));
// 	connect(canvasView, SIGNAL(itemMoved(QtCanvasItem *)),
// 		this, SLOT(itemMoved(QtCanvasItem *)));

}

void MainWindow::createCenterPanel()
{
	m_layerArea = new QScrollArea(m_mainSplitter);
	m_layerArea->setWidgetResizable(true);

 	QWidget *baseParent = new QWidget(m_layerArea);
 	QVBoxLayout *parentLayout = new QVBoxLayout(baseParent);
 	parentLayout->setContentsMargins(0,0,0,0);

 	m_layerBase = new QWidget(baseParent);
 	(void)new QVBoxLayout(m_layerBase);
 	parentLayout->addWidget(m_layerBase);
 	parentLayout->addStretch(1);
 	//m_layerArea->setWidget(m_layerBase);
 	//m_mainSplitter->addWidget(m_layerArea);
 	m_layerArea->setWidget(baseParent);
 	m_mainSplitter->addWidget(m_layerArea);

// 	QWidget *baseParent = new QWidget(m_mainSplitter);
// 	QVBoxLayout *parentLayout = new QVBoxLayout(baseParent);
// 	parentLayout->setContentsMargins(0,0,0,0);
//
// 	m_layerBase = new QWidget(baseParent);
// 	QVBoxLayout *layout = new QVBoxLayout(m_layerBase);
// 	layout->setContentsMargins(2,2,2,2);
//
// 	parentLayout->addWidget(m_layerBase);
// 	parentLayout->addStretch(1);
// 	//m_layerArea->setWidget(m_layerBase);
// 	//m_mainSplitter->addWidget(m_layerArea);
// 	m_mainSplitter->addWidget(baseParent);

}

void MainWindow::createRightPanel()
{
	QWidget *base = new QWidget(m_mainSplitter);
	QVBoxLayout *layout = new QVBoxLayout(base);

	m_mainViewer = new GLWidget(base);
	layout->addWidget(m_mainViewer);
	layout->addStretch(1);

	m_mainSplitter->addWidget(base);
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
	m_leftSplitter->restoreState(settings.value("mainwindow/left_splitter").toByteArray());

}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("mainwindow/pos", pos());
	settings.setValue("mainwindow/size", size());

	settings.setValue("mainwindow/main_splitter",m_mainSplitter->saveState());
	settings.setValue("mainwindow/left_splitter",m_leftSplitter->saveState());
}

void MainWindow::newFile()
{
	LiveScene *old = m_currentScene;
	
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
			QFile file(fileName);
			if (!file.open(QIODevice::ReadOnly)) 
			{
				QMessageBox::critical(0, tr("Loading error"), tr("Unable to read file %1").arg(fileName));
				return;
			}

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
		else
		{
			QMessageBox::critical(this,tr("File Does Not Exist"),tr("Sorry, but the file you chose does not exist. Please try again."));
		}
	}
	
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

