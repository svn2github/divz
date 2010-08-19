#include <QtGui>

#include "MainWindow.h"
#include <QCDEStyle>

#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"



////////////////////////
LayerControlWidget::LayerControlWidget(LiveLayer *layer)
	: QFrame()
	, m_layer(layer)
{
	setupUI();
}

LayerControlWidget::~LayerControlWidget()
{}

void LayerControlWidget::mouseReleaseEvent(QMouseEvent*)
{
 	emit clicked();
}

void LayerControlWidget::setIsCurrentWidget(bool flag)
{
	m_isCurrentWidget = flag;
	m_editButton->setStyleSheet(flag ? "background: yellow" : "");
	m_editButton->setChecked(flag);
}

void LayerControlWidget::drawableVisibilityChanged(bool flag)
{
	m_liveButton->setStyleSheet(flag ? "background: red; color: white; font-weight: bold" : "");
	m_liveButton->setChecked(flag);
}

void LayerControlWidget::setupUI()
{
	setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
	setLineWidth(2);

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setContentsMargins(2,2,2,2);

	m_editButton = new QPushButton(QPixmap("../data/stock-edit.png"),"");
	m_editButton->setCheckable(true);
	connect(m_editButton, SIGNAL(clicked()), this, SIGNAL(clicked()));
	layout->addWidget(m_editButton);


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
	connect(m_layer->drawable(), SIGNAL(isVisible(bool)), this, SLOT(drawableVisibilityChanged(bool)));
	if(m_layer->drawable()->isVisible())
		drawableVisibilityChanged(true);
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

QHash<QString,QVariant> LiveLayer::instanceProperties()
{
	if(m_props.isEmpty())
		setupInstanceProperties(drawable());
	return m_props;
}

void LiveLayer::setupInstanceProperties(GLDrawable *drawable)
{
	m_props["rect"] = drawable->rect();
	m_props["zIndex"] = drawable->zIndex();
	m_props["opacity"] = drawable->opacity();
}

QList<QtPropertyEditorIdPair> LiveLayer::createPropertyEditors(QtVariantPropertyManager *manager)
{
	QList<QtPropertyEditorIdPair> list;
	
	GLDrawable *gl = drawable();
	if(!gl)
		return list;
		
	QtVariantProperty *property;

	property = manager->addProperty(QVariant::RectF, tr("Size/Position"));
// 	property->setAttribute(QLatin1String("minimum"), 0);
// 	property->setAttribute(QLatin1String("maximum"), canvas->width());
	property->setValue(gl->rect());
	list << QtPropertyEditorIdPair("rect", property);

	property = manager->addProperty(QVariant::Double, tr("Z Position"));
	property->setAttribute(QLatin1String("minimum"), -1000);
	property->setAttribute(QLatin1String("maximum"), 1000);
	property->setValue(gl->zIndex());
	list << QtPropertyEditorIdPair("zIndex", property);
	
	property = manager->addProperty(QVariant::Double, tr("Opacity"));
	property->setAttribute(QLatin1String("minimum"), 0);
	property->setAttribute(QLatin1String("maximum"), 1);
	property->setValue(gl->opacity());
	list << QtPropertyEditorIdPair("opacity", property);
	
	return list;
}

// Default impl just iterates through all the keys and calls setInstanceProperty()
void LiveLayer::setInstanceProperties(QHash<QString,QVariant> props)
{
	foreach(QString key, props.keys())
	{
		setInstanceProperty(key, props[key]);
	}
}

void LiveLayer::setInstanceProperty(const QString& key, const QVariant& variant)
{
	m_props[key] = variant;
	
	if(GLDrawable *gl = drawable())
	{
		if(m_props.contains(key) && gl->property(qPrintable(key)).isValid())
		{
			gl->setProperty(qPrintable(key), variant);
		}
	}
		
	emit instancePropertyChanged(key,variant);
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
		#ifdef Q_OS_LINUX
			usleep(750 * 1000); // This causes a race condition to manifist itself reliably, which causes a crash every time instead of intermitently.
			// With the crash reproducable, I can now work to fix it.
		#endif
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


// 	VideoDisplayOptionWidget *opts = new VideoDisplayOptionWidget(drawable);
// 	opts->adjustSize();
// 	opts->show();

	m_drawable = drawable;
	m_videoDrawable = drawable;

	setCamera(source);

}


void LiveVideoInputLayer::setupInstanceProperties(GLDrawable *drawable)
{
	LiveLayer::setupInstanceProperties(drawable);
	
	GLVideoDrawable *vid = dynamic_cast<GLVideoDrawable*>(drawable);
	if(!vid)
		return;
	
	QStringList props = QStringList()
		<< "brightness"
		<< "contrast"
		<< "hue"
		<< "saturation"
		<< "flipHorizontal"
		<< "flipVertical"
		<< "cropTopLeft"
		<< "cropBottomRight";
		
	foreach(QString prop, props)
	{
		m_props[prop] = vid->property(qPrintable(prop));
	}
}

QList<QtPropertyEditorIdPair> LiveVideoInputLayer::createPropertyEditors(QtVariantPropertyManager *manager)
{
	QList<QtPropertyEditorIdPair> list = LiveLayer::createPropertyEditors(manager);
		
	QtVariantProperty *property;
	
	VideoSource *source = m_videoDrawable->videoSource();
	
	CameraThread *camera = dynamic_cast<CameraThread*>(source);
	if(camera)
	{
		property = manager->addProperty(QVariant::Bool, tr("Deinterlace"));
		property->setValue(camera->deinterlace());
		list << QtPropertyEditorIdPair("deinterlace", property);
	}
	
	///////////////////////////////////////////
	
	property = manager->addProperty(QVariant::Int, tr("Contrast"));
	property->setAttribute(QLatin1String("minimum"), -100);
	property->setAttribute(QLatin1String("maximum"), 100);
	property->setValue(m_videoDrawable->brightness());
	list << QtPropertyEditorIdPair("contrast", property);
	
	property = manager->addProperty(QVariant::Int, tr("Hue"));
	property->setAttribute(QLatin1String("minimum"), -100);
	property->setAttribute(QLatin1String("maximum"), 100);
	property->setValue(m_videoDrawable->brightness());
	list << QtPropertyEditorIdPair("saturation", property);
	
	property = manager->addProperty(QVariant::Int, tr("Saturation"));
	property->setAttribute(QLatin1String("minimum"), -100);
	property->setAttribute(QLatin1String("maximum"), 100);
	property->setValue(m_videoDrawable->brightness());
	list << QtPropertyEditorIdPair("saturation", property);
	
	
	///////////////////////////////////////////
	
	property = manager->addProperty(QVariant::Bool, tr("Flip Horizontal"));
	property->setValue(m_videoDrawable->flipHorizontal());
	list << QtPropertyEditorIdPair("flipHorizontal", property);
	
	property = manager->addProperty(QVariant::Bool, tr("Flip Vertical"));
	property->setValue(m_videoDrawable->flipHorizontal());
	list << QtPropertyEditorIdPair("flipVertical", property);
	
	///////////////////////////////////////////
		
	property = manager->addProperty(QVariant::PointF, tr("Crop Top-Left"));
	property->setValue(m_videoDrawable->cropTopLeft());
	list << QtPropertyEditorIdPair("cropTopLeft", property);
	
	property = manager->addProperty(QVariant::PointF, tr("Crop Bottom-Right"));
	property->setValue(m_videoDrawable->cropBottomRight());
	list << QtPropertyEditorIdPair("cropBottomRight", property);
	
	///////////////////////////////////////////
	
	return list;
}

void LiveVideoInputLayer::setInstanceProperty(const QString& key, const QVariant& value)
{
	if(key == "deinterlace")
	{
		VideoSource *source = m_videoDrawable->videoSource();
		
		CameraThread *camera = dynamic_cast<CameraThread*>(source);
		if(camera)
		{
			camera->setDeinterlace((bool)value.toInt());
		}
	}
	
	LiveLayer::setInstanceProperty(key,value);

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


void LiveStaticSourceLayer::setupInstanceProperties(GLDrawable *drawable)
{
	LiveLayer::setupInstanceProperties(drawable);
	
	// TODO expose a 'filename' property
	
	/*
	GLVideoDrawable *vid = dynamic_cast<GLVideoDrawable*>(drawable);
	if(!vid)
		return;
	
	QStringList props
		<< "brightness"
		<< "contrast"
		<< "hue"
		<< "saturation"
		<< "flipHorizontal"
		<< "flipVertical"
		<< "cropTopLeft"
		<< "cropBottomRight";
		
	foreach(QString prop, props)
	{
		m_props[prop] = vid->property(qPrintable(prop));
	}*/
}

QList<QtPropertyEditorIdPair> LiveStaticSourceLayer::createPropertyEditors(QtVariantPropertyManager *manager)
{
	QList<QtPropertyEditorIdPair> list = LiveLayer::createPropertyEditors(manager);
		
	GLDrawable *gl = drawable();
	if(!gl)
		return list;
		
	// TODO expose filename property
	
// 	QtVariantProperty *property;
// 	
// 	
// 	VideoSource *source = gl->videoSource();
// 	
// 	CameraThread *camera = dynamic_cast<CameraThread*>(source);
// 	if(camera)
// 	{
// 		property = manager->addProperty(QVariant::Bool, tr("Deinterlace"));
// 		property->setValue(camera->deinterlace());
// 		list << QtPropertyEditorIdPair("deinterlace", property);
// 	}
// 	
// 	///////////////////////////////////////////
// 	
// 	property = manager->addProperty(QVariant::Int, tr("Contrast"));
// 	property->setAttribute(QLatin1String("minimum"), -100);
// 	property->setAttribute(QLatin1String("maximum"), 100);
// 	property->setValue(gl->brightness());
// 	list << QtPropertyEditorIdPair("contrast", property);
// 	
// 	property = manager->addProperty(QVariant::Int, tr("Hue"));
// 	property->setAttribute(QLatin1String("minimum"), -100);
// 	property->setAttribute(QLatin1String("maximum"), 100);
// 	property->setValue(gl->brightness());
// 	list << QtPropertyEditorIdPair("saturation", property);
// 	
// 	property = manager->addProperty(QVariant::Int, tr("Saturation"));
// 	property->setAttribute(QLatin1String("minimum"), -100);
// 	property->setAttribute(QLatin1String("maximum"), 100);
// 	property->setValue(gl->brightness());
// 	list << QtPropertyEditorIdPair("saturation", property);
// 	
// 	
// 	///////////////////////////////////////////
// 	
// 	property = manager->addProperty(QVariant::Bool, tr("Flip Horizontal"));
// 	property->setValue(gl->flipHorizontal());
// 	list << QtPropertyEditorIdPair("flipHorizontal", property);
// 	
// 	property = manager->addProperty(QVariant::Bool, tr("Flip Vertical"));
// 	property->setValue(gl->flipHorizontal());
// 	list << QtPropertyEditorIdPair("flipVertical", property);
// 	
// 	///////////////////////////////////////////
// 		
// 	property = manager->addProperty(QVariant::PointF, tr("Crop Top-Left"));
// 	property->setValue(gl->cropTopLeft());
// 	list << QtPropertyEditorIdPair("cropTopLeft", property);
// 	
// 	property = manager->addProperty(QVariant::PointF, tr("Crop Bottom-Right"));
// 	property->setValue(gl->cropBottomRight());
// 	list << QtPropertyEditorIdPair("cropBottomRight", property);
// 	
// 	///////////////////////////////////////////
	
	return list;
}

void LiveStaticSourceLayer::setInstanceProperty(const QString& key, const QVariant& value)
{
	// TODO load filename from prop and set image
	
// 	if(key == "deinterlace")
// 	{
// 		VideoSource *source = gl->videoSource();
// 		
// 		CameraThread *camera = dynamic_cast<CameraThread*>(source);
// 		if(camera)
// 		{
// 			camera->setDeinterlace((bool)value.toInt());
// 		}
// 	}
	
	LiveLayer::setInstanceProperty(key,value);

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


	m_textSource = new TextVideoSource();
	m_textSource->start();
	
	drawable->setVideoSource(m_textSource);
	
	drawable->setZIndex(1);
	drawable->setObjectName("Text");

	drawable->addShowAnimation(GLDrawable::AnimFade);
	drawable->addShowAnimation(GLDrawable::AnimSlideTop,2500).curve = QEasingCurve::OutElastic;

	drawable->addHideAnimation(GLDrawable::AnimFade);
	drawable->addHideAnimation(GLDrawable::AnimZoom);

	m_drawable = drawable;
	
	setText("<b>Welcome to LiveMix</b>");
}



void LiveTextLayer::setupInstanceProperties(GLDrawable *drawable)
{
	LiveLayer::setupInstanceProperties(drawable);
	
	GLVideoDrawable *vid = dynamic_cast<GLVideoDrawable*>(drawable);
	if(!vid)
		return;
	
	m_props["text"] = text();
}

void LiveTextLayer::setText(const QString& text)
{
	m_textSource->setHtml(text);
	// TODO make font size configurable
	m_textSource->changeFontSize(40);
	QSize size = m_textSource->findNaturalSize();
	m_textSource->setTextWidth(size.width());
	
	// TODO make anchor configurable
	
	QRectF viewport(0,0,1000,750);

	m_drawable->setRect(QRectF(
		qMax(viewport.right()  - size.width()  , 0.0),
		qMax(viewport.bottom() - size.height() , 0.0),
		size.width(),
		size.height()));
	
	// TODO do we need to store both in a member var AND m_props?
	m_props["text"] = text;
	
	m_text = text;
	
	changeInstanceName(text);
}

QList<QtPropertyEditorIdPair> LiveTextLayer::createPropertyEditors(QtVariantPropertyManager *manager)
{
	QList<QtPropertyEditorIdPair> list = LiveLayer::createPropertyEditors(manager);
	
	GLDrawable *gl = drawable();
	if(!gl)
		return list;
		
	QtVariantProperty *property;
	
	///////////////////////////////////////////
	
	property = manager->addProperty(QVariant::String, tr("Text"));
	property->setValue(text());
	list << QtPropertyEditorIdPair("text", property);
	
	return list;
}

void LiveTextLayer::setInstanceProperty(const QString& key, const QVariant& value)
{
	if(key == "text")
	{
		setText(value.toString());
	}
	
	LiveLayer::setInstanceProperty(key,value);

}


///////////////////////



MainWindow::MainWindow()
	: QMainWindow()
	, m_currentScene(0)
	, m_currentControlWidget(0)
	, m_currentLayer(0)
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

	//LiveVideoInputLayer *videoLayer = new LiveVideoInputLayer();
	scene->addLayer(new LiveVideoInputLayer());

	scene->addLayer(new LiveStaticSourceLayer());
	scene->addLayer(new LiveTextLayer());

	scene->layerList().at(0)->drawable()->show();

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

			connect(widget, SIGNAL(clicked()), this, SLOT(liveLayerClicked()));
		}
	}

	foreach(LiveLayer *layer, m_controlWidgetMap.keys())
	{
		if(!foundLayer.contains(layer))
		{
			qDebug() << "MainWindow::updateLayerList(): layer not found, removing control widget"<<layer;
			disconnect(m_controlWidgetMap[layer], 0, this, 0);
			m_layerBase->layout()->removeWidget(m_controlWidgetMap[layer]);
			m_controlWidgetMap.remove(layer);

		}
	}
}

void MainWindow::liveLayerClicked()
{
	if(m_currentControlWidget)
		m_currentControlWidget->setIsCurrentWidget(false);

	LayerControlWidget *widget = dynamic_cast<LayerControlWidget*>(sender());
	if(widget)
	{
		widget->setIsCurrentWidget(true);
		loadLayerProperties(widget->layer());
		m_currentControlWidget = widget;
	}
}

void MainWindow::updateExpandState()
{
	QList<QtBrowserItem *> list = m_propertyEditor->topLevelItems();
	QListIterator<QtBrowserItem *> it(list);
	while (it.hasNext()) 
	{
		QtBrowserItem *item = it.next();
		QtProperty *prop = item->property();
		m_idToExpanded[m_propertyToId[prop]] = m_propertyEditor->isExpanded(item);
	}
}

void MainWindow::loadLayerProperties(LiveLayer *layer)
{
	m_currentLayer = layer;
	updateExpandState();

	QMap<QtProperty *, QString>::ConstIterator itProp = m_propertyToId.constBegin();
	while (itProp != m_propertyToId.constEnd()) 
	{
		delete itProp.key();
		itProp++;
	}
	m_propertyToId.clear();
	m_idToProperty.clear();

	if (!m_currentLayer) 
	{
		//deleteAction->setEnabled(false);
		return;
	}

	//deleteAction->setEnabled(true);

// 	QtVariantProperty *property;
	
	QList<QtPropertyEditorIdPair> list = layer->createPropertyEditors(m_variantManager);
	foreach(QtPropertyEditorIdPair pair, list)
	{
		addProperty(pair.value, pair.id);
	}

// 	property = variantManager->addProperty(QVariant::Double, tr("Position X"));
// 	property->setAttribute(QLatin1String("minimum"), 0);
// 	property->setAttribute(QLatin1String("maximum"), canvas->width());
// 	property->setValue(item->x());
// 	addProperty(property, QLatin1String("xpos"));
// 	
// 	property = variantManager->addProperty(QVariant::Double, tr("Position Y"));
// 	property->setAttribute(QLatin1String("minimum"), 0);
// 	property->setAttribute(QLatin1String("maximum"), canvas->height());
// 	property->setValue(item->y());
// 	addProperty(property, QLatin1String("ypos"));
// 	
// 	property = variantManager->addProperty(QVariant::Double, tr("Position Z"));
// 	property->setAttribute(QLatin1String("minimum"), 0);
// 	property->setAttribute(QLatin1String("maximum"), 256);
// 	property->setValue(item->z());
// 	addProperty(property, QLatin1String("zpos"));
// 	
// 	if (item->rtti() == QtCanvasItem::Rtti_Rectangle) {
// 		QtCanvasRectangle *i = (QtCanvasRectangle *)item;
// 	
// 		property = variantManager->addProperty(QVariant::Color, tr("Brush Color"));
// 		property->setValue(i->brush().color());
// 		addProperty(property, QLatin1String("brush"));
// 	
// 		property = variantManager->addProperty(QVariant::Color, tr("Pen Color"));
// 		property->setValue(i->pen().color());
// 		addProperty(property, QLatin1String("pen"));
// 	
// 		property = variantManager->addProperty(QVariant::Size, tr("Size"));
// 		property->setValue(i->size());
// 		addProperty(property, QLatin1String("size"));
// 	} else if (item->rtti() == QtCanvasItem::Rtti_Line) {
// 		QtCanvasLine *i = (QtCanvasLine *)item;
// 	
// 		property = variantManager->addProperty(QVariant::Color, tr("Pen Color"));
// 		property->setValue(i->pen().color());
// 		addProperty(property, QLatin1String("pen"));
// 	
// 		property = variantManager->addProperty(QVariant::Point, tr("Vector"));
// 		property->setValue(i->endPoint());
// 		addProperty(property, QLatin1String("endpoint"));
// 	} else if (item->rtti() == QtCanvasItem::Rtti_Ellipse) {
// 		QtCanvasEllipse *i = (QtCanvasEllipse *)item;
// 	
// 		property = variantManager->addProperty(QVariant::Color, tr("Brush Color"));
// 		property->setValue(i->brush().color());
// 		addProperty(property, QLatin1String("brush"));
// 	
// 		property = variantManager->addProperty(QVariant::Size, tr("Size"));
// 		property->setValue(QSize(i->width(), i->height()));
// 		addProperty(property, QLatin1String("size"));
// 	} else if (item->rtti() == QtCanvasItem::Rtti_Text) {
// 		QtCanvasText *i = (QtCanvasText *)item;
// 	
// 		property = variantManager->addProperty(QVariant::Color, tr("Color"));
// 		property->setValue(i->color());
// 		addProperty(property, QLatin1String("color"));
// 	
// 		property = variantManager->addProperty(QVariant::String, tr("Text"));
// 		property->setValue(i->text());
// 		addProperty(property, QLatin1String("text"));
// 	
// 		property = variantManager->addProperty(QVariant::Font, tr("Font"));
// 		property->setValue(i->font());
// 		addProperty(property, QLatin1String("font"));
// 	}
}

void MainWindow::addProperty(QtVariantProperty *property, const QString &id)
{
	m_propertyToId[property] = id;
	m_idToProperty[id] = property;
	QtBrowserItem *item = m_propertyEditor->addProperty(property);
	if (m_idToExpanded.contains(id))
		m_propertyEditor->setExpanded(item, m_idToExpanded[id]);
}

void MainWindow::valueChanged(QtProperty *property, const QVariant &value)
{
	if (!m_propertyToId.contains(property))
		return;
	
	if (!m_currentLayer)
		return;
	
	QString id = m_propertyToId[property];
	
	m_currentLayer->setInstanceProperty(id, value);
	
// 	if (id == QLatin1String("xpos")) {
// 		currentItem->setX(value.toDouble());
// 	} else if (id == QLatin1String("ypos")) {
// 		currentItem->setY(value.toDouble());
// 	} else if (id == QLatin1String("zpos")) {
// 		currentItem->setZ(value.toDouble());
// 	} else if (id == QLatin1String("text")) {
// 		if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
// 		QtCanvasText *i = (QtCanvasText *)currentItem;
// 		i->setText(qVariantValue<QString>(value));
// 		}
// 	} else if (id == QLatin1String("color")) {
// 		if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
// 		QtCanvasText *i = (QtCanvasText *)currentItem;
// 		i->setColor(qVariantValue<QColor>(value));
// 		}
// 	} else if (id == QLatin1String("brush")) {
// 		if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle ||
// 			currentItem->rtti() == QtCanvasItem::Rtti_Ellipse) {
// 		QtCanvasPolygonalItem *i = (QtCanvasPolygonalItem *)currentItem;
// 		QBrush b = i->brush();
// 		b.setColor(qVariantValue<QColor>(value));
// 		i->setBrush(b);
// 		}
// 	} else if (id == QLatin1String("pen")) {
// 		if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle ||
// 			currentItem->rtti() == QtCanvasItem::Rtti_Line) {
// 		QtCanvasPolygonalItem *i = (QtCanvasPolygonalItem *)currentItem;
// 		QPen p = i->pen();
// 		p.setColor(qVariantValue<QColor>(value));
// 		i->setPen(p);
// 		}
// 	} else if (id == QLatin1String("font")) {
// 		if (currentItem->rtti() == QtCanvasItem::Rtti_Text) {
// 		QtCanvasText *i = (QtCanvasText *)currentItem;
// 		i->setFont(qVariantValue<QFont>(value));
// 		}
// 	} else if (id == QLatin1String("endpoint")) {
// 		if (currentItem->rtti() == QtCanvasItem::Rtti_Line) {
// 		QtCanvasLine *i = (QtCanvasLine *)currentItem;
// 		QPoint p = qVariantValue<QPoint>(value);
// 		i->setPoints(i->startPoint().x(), i->startPoint().y(), p.x(), p.y());
// 		}
// 	} else if (id == QLatin1String("size")) {
// 		if (currentItem->rtti() == QtCanvasItem::Rtti_Rectangle) {
// 		QtCanvasRectangle *i = (QtCanvasRectangle *)currentItem;
// 		QSize s = qVariantValue<QSize>(value);
// 		i->setSize(s.width(), s.height());
// 		} else if (currentItem->rtti() == QtCanvasItem::Rtti_Ellipse) {
// 		QtCanvasEllipse *i = (QtCanvasEllipse *)currentItem;
// 		QSize s = qVariantValue<QSize>(value);
// 		i->setSize(s.width(), s.height());
// 		}
// 	}
// 	canvas->update();
}



void MainWindow::createLeftPanel()
{
	m_leftSplitter = new QSplitter(m_mainSplitter);
	m_leftSplitter->setOrientation(Qt::Vertical);

	m_layerViewer = new GLWidget(m_leftSplitter);
	m_leftSplitter->addWidget(m_layerViewer);

// 	m_controlArea = new QScrollArea(m_leftSplitter);
// 	m_controlBase = new QWidget(m_controlArea);
// 	(void)new QVBoxLayout(m_controlBase);
// 	m_controlArea->setWidget(m_controlBase);
// 	m_leftSplitter->addWidget(m_controlArea);

	m_variantManager = new QtVariantPropertyManager(this);
	
	connect(m_variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
			      this, SLOT(valueChanged(QtProperty *, const QVariant &)));
	
	QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);
	
// 	canvas = new QtCanvas(800, 600);
// 	canvasView = new CanvasView(canvas, this);
// 	setCentralWidget(canvasView);
	
// 	QDockWidget *dock = new QDockWidget(this);
// 	addDockWidget(Qt::RightDockWidgetArea, dock);
	
	m_propertyEditor = new QtTreePropertyBrowser(m_leftSplitter);
	m_propertyEditor->setFactoryForManager(m_variantManager, variantFactory);
	m_leftSplitter->addWidget(m_propertyEditor);
	
// 	m_currentItem = 0;
	
// 	connect(canvasView, SIGNAL(itemClicked(QtCanvasItem *)),
// 		this, SLOT(itemClicked(QtCanvasItem *)));
// 	connect(canvasView, SIGNAL(itemMoved(QtCanvasItem *)),
// 		this, SLOT(itemMoved(QtCanvasItem *)));

}

void MainWindow::createCenterPanel()
{
//	m_layerArea = new QScrollArea(m_mainSplitter);
// 	QWidget *baseParent = new QWidget(m_layerArea);
// 	QVBoxLayout *parentLayout = new QVBoxLayout(baseParent);
// 	parentLayout->setContentsMargins(0,0,0,0);
// 	m_layerBase = new QWidget(baseParent);
// 	(void)new QVBoxLayout(m_layerBase);
// 	parentLayout->addWidget(m_layerBase);
// 	parentLayout->addStretch(1);
// 	//m_layerArea->setWidget(m_layerBase);
// 	//m_mainSplitter->addWidget(m_layerArea);
// 	m_layerArea->setWidget(baseParent);
// 	m_mainSplitter->addWidget(m_layerArea);
// 	baseParent->show();

	QWidget *baseParent = new QWidget(m_mainSplitter);
	QVBoxLayout *parentLayout = new QVBoxLayout(baseParent);
	parentLayout->setContentsMargins(0,0,0,0);
	
	m_layerBase = new QWidget(baseParent);
	QVBoxLayout *layout = new QVBoxLayout(m_layerBase);
	layout->setContentsMargins(2,2,2,2);
	
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
