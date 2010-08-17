#include <QtGui>

#include "MainWindow.h"
#include <QCDEStyle>


////////////////////////
LayerControlWidget::LayerControlWidget(LiveLayer *layer)
	: QWidget()
	, m_layer(layer)
{
	setupUI();
}

LayerControlWidget::~LayerControlWidget()
{}

///////////////////////
LiveLayer::LiveLayer(QObject *parent)
	: QObject(parent)
	, m_drawable(0)
	, m_controlWidget(0)
{
	setupDrawable();
}

LiveLayer::~LiveLayer()
{}

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
	
void LiveScene::detachGLWidget()
{
	if(!m_glWidget)
		return;
		
	foreach(LiveLayer *layer, m_layers)
		m_glWidget->removeDrawable(layer->drawable());

	m_glWidget = 0;
}

///////////////////////

LiveVideoInputLayer::LiveVideoInputLayer(QObject *parent)
	: LiveLayer(parent)
{
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
	drawable->setVideoSource(source);
	drawable->setRect(QRectF(0,0,100,750));
	
	drawable->addShowAnimation(GLDrawable::AnimFade);
	drawable->addHideAnimation(GLDrawable::AnimFade);
	
	//drawable->show();
	drawable->setObjectName(qPrintable(defaultCamera));
		

	VideoDisplayOptionWidget *opts = new VideoDisplayOptionWidget(drawable);
	opts->adjustSize();
	opts->show();
	m_drawable = drawable;
	m_videoDrawable = drawable;
}

void LiveVideoInputLayer::setCamera(CameraThread *camera)
{
	m_videoDrawable->setVideoSource(camera);
	m_camera = camera;
}


///////////////////////



MainWindow::MainWindow()
{
	
	
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	
	readSettings();
	
	setWindowTitle(tr("LiveMix"));
	setUnifiedTitleAndToolBarOnMac(true);
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
	QPoint pos = settings.value("pos", QPoint(10, 10)).toPoint();
	QSize size = settings.value("size", QSize(640,480)).toSize();
	move(pos);
	resize(size);
}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("pos", pos());
	settings.setValue("size", size());
}
