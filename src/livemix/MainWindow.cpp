#include <QtGui>

#include "MainWindow.h"
#include <QCDEStyle>


////////////////////////
LayerControlWidget::LayerControlWidget(LiveLayer *layer)
	: QFrame()
	, m_layer(layer)
{
	setupUI();
}

LayerControlWidget::~LayerControlWidget()
{}

void LayerControlWidget::setupUI()
{
	setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	setLineWidth(2);
	
	QHBoxLayout *layout = new QHBoxLayout(this);
	
	m_nameLabel = new QLabel;
	layout->addWidget(m_nameLabel);
	connect(m_layer, SIGNAL(instanceNameChanged(const QString&)), this, SLOT(instanceNameChanged(const QString&)));
	instanceNameChanged(m_layer->instanceName());
	layout->addStretch(1);
	
	m_opacitySlider = new QSlider(this);
	m_opacitySlider->setOrientation(Qt::Horizontal);
	m_opacitySlider->setMinimum(0);
	m_opacitySlider->setMaximum(100);
	m_opacitySlider->setValue(100);
	connect(m_opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(opacitySliderChanged(int)));
	layout->addWidget(m_opacitySlider);
	
	QSpinBox *box = new QSpinBox(this);
	box->setMinimum(0);
	box->setMaximum(100);
	box->setValue(100);
	connect(m_opacitySlider, SIGNAL(valueChanged(int)), box, SLOT(setValue(int)));
	connect(box, SIGNAL(valueChanged(int)), m_opacitySlider, SLOT(setValue(int)));
	layout->addWidget(box);
	
	layout->addStretch(1);
	
	m_liveButton = new QPushButton("Live");
	m_liveButton->setCheckable(true);
	connect(m_liveButton, SIGNAL(toggled(bool)), m_layer->drawable(), SLOT(setVisible(bool)));
	connect(m_layer->drawable(), SIGNAL(isVisible(bool)), m_liveButton, SLOT(setChecked(bool)));
	layout->addWidget(m_liveButton);
	
}

void LayerControlWidget::instanceNameChanged(const QString& name)
{
	m_nameLabel->setText(QString("<b>%1</b><br>%2").arg(name).arg(m_layer->typeName()));
}

void LayerControlWidget::opacitySliderChanged(int value)
{
	m_layer->drawable()->setOpacity((double)value/100.0);
}

///////////////////////
LiveLayer::LiveLayer(QObject *parent)
	: QObject(parent)
	, m_drawable(0)
	, m_controlWidget(0)
{
}

LiveLayer::~LiveLayer()
{}

GLDrawable* LiveLayer::drawable()
{
	if(!m_drawable)
		setupDrawable();
	return m_drawable;
}

void LiveLayer::setupDrawable()
{
	qDebug() << "LiveLayer::setupDrawable: VIRTUAL - NOTHING DONE";
}

void LiveLayer::changeInstanceName(const QString& name)
{
	m_instanceName = name;
	emit instanceNameChanged(name);
}

///////////////////////
LiveScene::LiveScene(QObject *parent)
	: QObject(parent)
	, m_glWidget(0)
{}

LiveScene::~LiveScene()
{}

QList<LayerControlWidget*> LiveScene::controlWidgets()
{
	QList<LayerControlWidget*> list;
	foreach(LiveLayer *layer, m_layers)
		if(layer->controlWidget())
			list.append(layer->controlWidget());
	return list;
}

void LiveScene::addLayer(LiveLayer *layer)
{
	if(!layer)
		return;
	if(!m_layers.contains(layer))
	{
		m_layers.append(layer);
		if(m_glWidget)
			m_glWidget->addDrawable(layer->drawable());
			
		emit layerAdded(layer);
	}
}

void LiveScene::removeLayer(LiveLayer *layer)
{
	if(!layer)
		return;
	if(m_layers.contains(layer))
	{
		m_layers.removeAll(layer);
		if(m_glWidget)
			m_glWidget->removeDrawable(layer->drawable());
		
		emit layerRemoved(layer);
	}
}

void LiveScene::attachGLWidget(GLWidget *glw)
{
	if(m_glWidget)
		detachGLWidget();
		
	m_glWidget = glw;
	
	foreach(LiveLayer *layer, m_layers)
		m_glWidget->addDrawable(layer->drawable());
}
	
void LiveScene::detachGLWidget(bool hideFirst)
{
	if(!m_glWidget)
		return;
		
	if(hideFirst)
	{
		bool foundVisible = false;
		foreach(LiveLayer *layer, m_layers)
		{
			if(layer->drawable()->isVisible())
			{
				foundVisible = true;
				connect(layer->drawable(), SIGNAL(isVisible(bool)), this, SLOT(layerVisibilityChanged(bool)));
				layer->drawable()->hide();
			}
		}
			
		if(foundVisible)
			return;
	}
	
	foreach(LiveLayer *layer, m_layers)
		m_glWidget->removeDrawable(layer->drawable());

	m_glWidget = 0;
}

void LiveScene::layerVisibilityChanged(bool flag)
{
	if(flag)
		return;
	bool foundVisible = false;
	foreach(LiveLayer *layer, m_layers)
		if(layer->drawable()->isVisible())
		{
			foundVisible = true;
			break;
		}
		
	if(!foundVisible)
		detachGLWidget(false);
	
}

///////////////////////

VideoInputControlWidget::VideoInputControlWidget(LiveVideoInputLayer *layer)
	: LayerControlWidget(layer)
	, m_videoLayer(layer)
{
}

VideoInputControlWidget::~VideoInputControlWidget()
{
}

void VideoInputControlWidget::setDeinterlace(bool flag)
{
	m_deinterlace = flag;
	
	QSettings settings;
	settings.setValue("VideoInputControlWidget/deinterlace",(int)m_deinterlace);
	
	if(m_videoLayer->camera())
		m_videoLayer->camera()->setDeinterlace(flag);
}

void VideoInputControlWidget::deviceBoxChanged(int idx)
{
	if(idx < 0 || idx >= m_cameras.size())
		return;
		
	QString camera = m_cameras[idx];
	
	CameraThread * thread = CameraThread::threadForCamera(camera);
	
	if(!thread)
	{
		QMessageBox::critical(this,"Missing Camera",QString("Sorry, cannot connect to %1!").arg(camera));
		setWindowTitle("Camera Error");
	}
	
	// Enable raw VL42 access on linux - note that VideoWidget must be able to handle raw frames
	thread->enableRawFrames(true);
	
	// Default clip rect to compensate for oft-seen video capture 'bugs' (esp. on hauppauge/bttv)
	//thread->setSourceRectAdjust(11,0,-6,-3);
	
	thread->setDeinterlace(m_deinterlace);
	
	//setWindowTitle(camera);
	
	//setVideoSource(m_thread);	
	m_videoLayer->setCamera(thread);
}

void VideoInputControlWidget::setupUI()
{

	QVBoxLayout *layout = new QVBoxLayout(this);
	
	QHBoxLayout *hbox = new QHBoxLayout();
	layout->addLayout(hbox);
	
	m_deviceBox = new QComboBox(this);
	
	hbox->addWidget(m_deviceBox);

	m_cameras = CameraThread::enumerateDevices();
	if(!m_cameras.size())
	{
		QMessageBox::critical(this,tr("No Cameras Found"),tr("Sorry, but no camera devices were found attached to this computer."));
		setWindowTitle("No Camera");
		return;
	}

	QStringList items;
	int counter = 1;
	foreach(QString dev, m_cameras)
		items << QString("Camera # %1").arg(counter++);
	
	m_deviceBox->addItems(items);
	
	connect(m_deviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceBoxChanged(int)));
	
	// load last deinterlace setting
	QSettings settings;
	m_deinterlace = (bool)settings.value("VideoInputControlWidget/deinterlace",false).toInt();
	
	// setup the popup config menu
	QPushButton *configBtn = new QPushButton(QPixmap("../data/stock-preferences.png"),"");
	configBtn->setToolTip("Options");
	
	// Use CDE style to minimize the space used by the button
	// (Could use a custom stylesheet I suppose - later.)
	configBtn->setStyle(new QCDEStyle());
	//configBtn->setStyleSheet("border:1px solid black; padding:0; width: 1em; margin:0");
	
	QAction * action;
	
	// Deinterlace option
	QMenu *configMenu = new QMenu(this);
	action = configMenu->addAction("Deinterlace Video");
	action->setCheckable(true);
	action->setChecked(deinterlace());
	connect(action, SIGNAL(toggled(bool)), this, SLOT(setDeinterlace(bool)));
	
	configBtn->setMenu(configMenu);
	hbox->addWidget(configBtn);
	
	//videoWidget()->setFps(30);
	
	// Start the camera
	deviceBoxChanged(0);
}

LiveVideoInputLayer::LiveVideoInputLayer(QObject *parent)
	: LiveLayer(parent)
{
	m_controlWidget = new LayerControlWidget(this);
}

LiveVideoInputLayer::~LiveVideoInputLayer()
{
	// TODO close camera
}

void LiveVideoInputLayer::setupDrawable()
{
	#ifdef Q_OS_WIN
		QString defaultCamera = "vfwcap://0";
	#else
		QString defaultCamera = "/dev/video0";
	#endif
	
	qDebug() << "LiveVideoInputLayer::setupDrawable: Using default camera:"<<defaultCamera;
	

	CameraThread *source = CameraThread::threadForCamera(defaultCamera);
	if(source)
	{
		source->setFps(30);
		usleep(750 * 1000); // This causes a race condition to manifist itself reliably, which causes a crash every time instead of intermitently. 
		// With the crash reproducable, I can now work to fix it.
		source->enableRawFrames(true);
		//source->setDeinterlace(true);
	}
	m_camera = source;
	
	GLVideoDrawable *drawable = new GLVideoDrawable();
	//drawable->setVideoSource(source);
	drawable->setRect(QRectF(0,0,1000,750));
	
	drawable->addShowAnimation(GLDrawable::AnimFade);
	drawable->addHideAnimation(GLDrawable::AnimFade);
	
	drawable->show();
	drawable->setObjectName(qPrintable(defaultCamera));
		

	VideoDisplayOptionWidget *opts = new VideoDisplayOptionWidget(drawable);
	opts->adjustSize();
	opts->show();
	m_drawable = drawable;
	m_videoDrawable = drawable;
	
	setCamera(source);
	
}

void LiveVideoInputLayer::setCamera(CameraThread *camera)
{
	m_videoDrawable->setVideoSource(camera);
	m_camera = camera;
	changeInstanceName(camera->inputName());
}


///////////////////////
LiveStaticSourceLayer::LiveStaticSourceLayer(QObject *parent)
	: LiveLayer(parent)
{
	m_controlWidget = new LayerControlWidget(this);
}

LiveStaticSourceLayer::~LiveStaticSourceLayer()
{
}

void LiveStaticSourceLayer::setupDrawable()
{
	// add secondary frame
	GLVideoDrawable *drawable = new GLVideoDrawable();
	
	
	StaticVideoSource *source = new StaticVideoSource();
	//source->setImage(QImage("me2.jpg"));
	QImage img("dsc_6645-1.jpg");
	if(img.isNull())
		source->setImage(QImage("../glvidtex/me2.jpg"));
	else
		source->setImage(img);
	//source->setImage(QImage("/opt/qtsdk-2010.02/qt/examples/opengl/pbuffers/cubelogo.png"));
	
	source->start();
	drawable->setVideoSource(source);
	QRectF viewport(0,0,1000,750);
	drawable->setRect(viewport);
	drawable->setZIndex(-1);
	drawable->setObjectName("Static");
	//drawable->setOpacity(0.5);
	//drawable->show();
	
	
// 	#ifdef HAS_QT_VIDEO_SOURCE
// 	// just change enterance anim to match effects
// 	drawable->addShowAnimation(GLDrawable::AnimFade);
 	//drawable->addShowAnimation(GLDrawable::AnimZoom);
// 	#else
	drawable->addShowAnimation(GLDrawable::AnimZoom,2500).curve = QEasingCurve::OutElastic;
// 	#endif
// 	
 	drawable->addHideAnimation(GLDrawable::AnimFade);
 	drawable->addHideAnimation(GLDrawable::AnimZoom,1000);
// 	

	m_drawable = drawable;
	changeInstanceName("File 'me2.jpg'");
	
	
}

///////////////////////

LiveTextLayer::LiveTextLayer(QObject *parent)
	: LiveLayer(parent)
{
	m_controlWidget = new LayerControlWidget(this);
}

LiveTextLayer::~LiveTextLayer()
{
}

void LiveTextLayer::setupDrawable()
{
	// add secondary frame
	// add text overlay frame
	GLVideoDrawable *drawable = new GLVideoDrawable();
	
	
	TextVideoSource *source = new TextVideoSource();
	source->start();
	source->setHtml("<b>Welcome to LiveMix</b>");
	source->changeFontSize(40);
	QSize size = source->findNaturalSize();
	source->setTextWidth(size.width());
	//qDebug() << "New html: "<<source->html();
	//source->setImage(QImage("/opt/qtsdk-2010.02/qt/examples/opengl/pbuffers/cubelogo.png"));
	
	drawable->setVideoSource(source);
	//drawable->setRect(glw->viewport());
	//qDebug() << "Text Size: "<<size;
	
	QRectF viewport(0,0,1000,750);
	
	drawable->setRect(QRectF(
		qMax(viewport.right()  - size.width()  , 0.0),
		qMax(viewport.bottom() - size.height() , 0.0),
		size.width(),
		size.height()));
	drawable->setZIndex(1);
	//drawable->setOpacity(0.5);
	drawable->setObjectName("Text");
	
	drawable->addShowAnimation(GLDrawable::AnimFade);
	drawable->addShowAnimation(GLDrawable::AnimSlideTop,2500).curve = QEasingCurve::OutElastic;
 	
 	drawable->addHideAnimation(GLDrawable::AnimFade);
 	drawable->addHideAnimation(GLDrawable::AnimZoom);
 	
	m_drawable = drawable;
	changeInstanceName("Welcome to LiveMix");
}

///////////////////////



MainWindow::MainWindow()
	: QMainWindow()
	, m_currentScene(0)
{
	
	
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
	
	setupSampleScene();
	
	readSettings();
	
	setWindowTitle(tr("LiveMix"));
	setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::setupSampleScene()
{
	LiveScene *scene = new LiveScene();
	
	LiveVideoInputLayer *videoLayer = new LiveVideoInputLayer();
	scene->addLayer(videoLayer);
	
	scene->addLayer(new LiveStaticSourceLayer());
	scene->addLayer(new LiveTextLayer());
	
	loadLiveScene(scene);
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
		qDebug() << "MainWindow::updateLayerList(): No current scene";
		foreach(LiveLayer *layer, m_controlWidgetMap.keys())
		{
			m_layerBase->layout()->removeWidget(m_controlWidgetMap[layer]);
			m_controlWidgetMap.remove(layer);
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
			qDebug() << "MainWindow::updateLayerList(): control map has layer already"<<layer;
			foundLayer.append(layer);
		}
		else
		{
			LayerControlWidget *widget = layer->controlWidget();
			m_layerBase->layout()->addWidget(widget);
			widget->show();
			
			qDebug() << "MainWindow::updateLayerList(): adding control widget for layer"<<layer<<", widget:"<<widget;
			
			m_controlWidgetMap[layer] = widget;
			foundLayer.append(layer);
		}
	}
	
	foreach(LiveLayer *layer, m_controlWidgetMap.keys())
	{
		if(!foundLayer.contains(layer))
		{
			qDebug() << "MainWindow::updateLayerList(): layer not found, removing control widget"<<layer;
			m_layerBase->layout()->removeWidget(m_controlWidgetMap[layer]);
			m_controlWidgetMap.remove(layer);
		}
	}
}

void MainWindow::createLeftPanel()
{
	m_leftSplitter = new QSplitter(m_mainSplitter);
	m_leftSplitter->setOrientation(Qt::Vertical);
	
	m_layerViewer = new GLWidget(m_leftSplitter);
	m_leftSplitter->addWidget(m_layerViewer);
	
	m_controlArea = new QScrollArea(m_leftSplitter);
	m_controlBase = new QWidget(m_controlArea);
	(void)new QVBoxLayout(m_controlBase);
	m_controlArea->setWidget(m_controlBase);
	m_leftSplitter->addWidget(m_controlArea);
}

void MainWindow::createCenterPanel()
{
	//m_layerArea = new QScrollArea(m_mainSplitter);
	
	//m_layerBase = new QWidget(m_layerArea);
	
	QWidget *baseParent = new QWidget(m_mainSplitter);
	QVBoxLayout *parentLayout = new QVBoxLayout(baseParent);
	parentLayout->setContentsMargins(0,0,0,0);
	m_layerBase = new QWidget(baseParent);
	(void)new QVBoxLayout(m_layerBase);
	parentLayout->addWidget(m_layerBase);
	parentLayout->addStretch(1);
	//m_layerArea->setWidget(m_layerBase);
	//m_mainSplitter->addWidget(m_layerArea);
	m_mainSplitter->addWidget(baseParent);
	
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
// 	newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
// 	newAct->setShortcuts(QKeySequence::New);
// 	newAct->setStatusTip(tr("Create a new file"));
// 	connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
// 	
// 	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
// 	openAct->setShortcuts(QKeySequence::Open);
// 	openAct->setStatusTip(tr("Open an existing file"));
// 	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
// 	
// 	saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
// 	saveAct->setShortcuts(QKeySequence::Save);
// 	saveAct->setStatusTip(tr("Save the document to disk"));
// 	connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
// 	
// 	saveAsAct = new QAction(tr("Save &As..."), this);
// 	saveAsAct->setShortcuts(QKeySequence::SaveAs);
// 	saveAsAct->setStatusTip(tr("Save the document under a new name"));
// 	connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
	
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
	
// 	m_actNewCamera= new QAction(tr("New Camera"), this);
// 	connect(m_actNewCamera, SIGNAL(triggered()), this, SLOT(newCamera()));
// 	
// 	m_actNewVideo = new QAction(tr("New Video"), this);
// 	connect(m_actNewVideo, SIGNAL(triggered()), this, SLOT(newVideo()));
// 	
// 	m_actNewMjpeg = new QAction(tr("New MJPEG"), this);
// 	connect(m_actNewMjpeg, SIGNAL(triggered()), this, SLOT(newMjpeg()));
// 	
// 	m_actNewDViz = new QAction(tr("New DViz"), this);
// 	connect(m_actNewDViz, SIGNAL(triggered()), this, SLOT(newDViz()));
// 	
// 	m_actNewOutput = new QAction(tr("New Output"), this);
// 	connect(m_actNewOutput, SIGNAL(triggered()), this, SLOT(newOutput()));
}

void MainWindow::createMenus()
{
	m_fileMenu = menuBar()->addMenu(tr("&File"));

// 	fileMenu->addAction(m_actNewCamera);
// 	fileMenu->addAction(m_actNewVideo);
// 	fileMenu->addAction(m_actNewMjpeg);
// 	fileMenu->addAction(m_actNewDViz);
// 	fileMenu->addAction(m_actNewOutput);
// 	fileMenu->addSeparator();
	
	m_fileMenu->addAction(m_exitAct);

	menuBar()->addSeparator();
	
	m_helpMenu = menuBar()->addMenu(tr("&Help"));
	m_helpMenu->addAction(m_aboutAct);
	m_helpMenu->addAction(m_aboutQtAct);
}

void MainWindow::createToolBars()
{
	m_fileToolBar = addToolBar(tr("Layer Toolbar"));
// 	m_fileToolBar->addAction();
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
