#include <QtGui>

#include "MainWindow.h"
#include <QCDEStyle>

#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"

#include "VideoSource.h"
#include "../glvidtex/GLWidget.h"
#include "../glvidtex/GLVideoDrawable.h"
#include "../glvidtex/StaticVideoSource.h"


MainWindow::MainWindow()
	: QMainWindow()
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

	setupSampleScene();

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
	
	
	loadLiveScene(scene);
	
	scene->layerList().at(0)->show();
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
	{
		m_currentControlWidget->setIsCurrentWidget(false);
		m_layerViewer->removeDrawable(m_currentControlWidget->layer()->drawable(m_layerViewer));
	}

	LayerControlWidget *widget = dynamic_cast<LayerControlWidget*>(sender());
	if(widget)
	{
		widget->setIsCurrentWidget(true);
		loadLayerProperties(widget->layer());
		m_layerViewer->addDrawable(widget->layer()->drawable(m_layerViewer));
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
	
	QSize size = m_layerViewer->viewport().size().toSize();
	size /= 4;
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
