#include "EditorWindow.h"

#include "GLSceneGroup.h"
#include "GLEditorGraphicsScene.h"

#include "GLDrawables.h"

#include "RtfEditorWindow.h"

#include "../livemix/ExpandableWidget.h"
#include "../livemix/EditorUtilityWidgets.h"

#include "../3rdparty/richtextedit/richtexteditor_p.h"

#include "EditorGraphicsView.h"

#include <QApplication>

EditorWindow::EditorWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_canvasSize(1000.,750.)
	, m_graphicsScene(0)
	, m_group(0)
	, m_scene(0)
	, m_currentLayerPropsEditor(0)
	, m_rtfEditor(0)
	, m_currentDrawable(0)
	, m_isStandalone(false)
{
	createUI();
	setCanvasSize(m_canvasSize);
	
	setWindowTitle(tr("GLEditor"));
	setUnifiedTitleAndToolBarOnMac(true);
	
	readSettings();
}

void EditorWindow::setIsStandalone(bool flag)
{
	m_isStandalone = flag;
	
	if(flag && m_fileName.isEmpty())
	{
		QStringList argList = qApp->arguments();
		if(argList.size() > 1)
		{
			QString fileArg = argList.at(1);
			if(!fileArg.isEmpty())
			{
				m_fileName = fileArg;
				QFile file(fileArg);
				if (!file.open(QIODevice::ReadOnly)) 
				{
					qDebug() << "EditorWindow: Unable to read group file: "<<fileArg;
				}
				else
				{
					QByteArray array = file.readAll();
					
					GLSceneGroup *group = new GLSceneGroup();
					group->fromByteArray(array);
					
					setGroup(group);
					
					qDebug() << "EditorWindow: [DEBUG]: Loaded File: "<<fileArg<<", GroupID: "<<group->groupId();
				}
			}
		}
	}
}

EditorWindow::~EditorWindow()
{
}


void EditorWindow::closeEvent(QCloseEvent *event)
{
	if(m_graphicsScene)
		m_graphicsScene->setEditingMode(false);
		
	writeSettings();
	event->accept();
	
	if(m_isStandalone && !m_fileName.isEmpty())
	{
		QFile file(m_fileName);
		// Open file
		if (!file.open(QIODevice::WriteOnly))
		{
			QMessageBox::warning(0, QObject::tr("File Error"), QObject::tr("Error saving writing file '%1'").arg(m_fileName));
			//throw 0;
			//return;
		}
		else
		{
			
			//QByteArray array;
			//QDataStream stream(&array, QIODevice::WriteOnly);
			//QVariantMap map;
			
			file.write(group()->toByteArray());
			file.close();
			
			qDebug() << "EditorWindow: Debug: Saved GroupID: "<< m_group->groupId();
		}	
	}
}


void EditorWindow::readSettings()
{
	QSettings settings;
	QPoint pos = settings.value("EditorWindow/pos", QPoint(10, 10)).toPoint();
	QSize size = settings.value("EditorWindow/size", QSize(640,480)).toSize();
	move(pos);
	resize(size);

	m_mainSplitter->restoreState(settings.value("EditorWindow/main_splitter").toByteArray());
	m_centerSplitter->restoreState(settings.value("EditorWindow/center_splitter").toByteArray());
	
	qreal scaleFactor = (qreal)settings.value("EditorWindow/scaleFactor", 1.).toDouble();
	m_graphicsView->setScaleFactor(scaleFactor);

}

void EditorWindow::writeSettings()
{
	QSettings settings;
	settings.setValue("EditorWindow/pos", pos());
	settings.setValue("EditorWindow/size", size());

	settings.setValue("EditorWindow/main_splitter",m_mainSplitter->saveState());
	settings.setValue("EditorWindow/center_splitter",m_centerSplitter->saveState());
	
	settings.setValue("EditorWindow/scaleFactor",m_graphicsView->scaleFactor());
}


void EditorWindow::createUI()
{
	// Create splitters
	m_mainSplitter = new QSplitter();
	m_centerSplitter = new QSplitter();
	
	// Setup splitter orientations
	m_mainSplitter->setOrientation(Qt::Horizontal);
	m_centerSplitter->setOrientation(Qt::Vertical);
	
	// Create two main lsit views
	m_slideList = new QListView();
	m_layoutList = new QListView();
	
	m_slideList->setViewMode(QListView::ListMode);
	//m_layerListView->setViewMode(QListView::IconMode);
//	m_slideList->setMovement(QListView::Free);
//	m_slideList->setWordWrap(true);
//	m_slideList->setSelectionMode(QAbstractItemView::ExtendedSelection);
// 	m_slideList->setDragEnabled(true);
// 	m_slideList->setAcceptDrops(true);
// 	m_slideList->setDropIndicatorShown(true);

	connect(m_slideList, SIGNAL(activated(const QModelIndex &)), this, SLOT(slideSelected(const QModelIndex &)));
	connect(m_slideList, SIGNAL(clicked(const QModelIndex &)),   this, SLOT(slideSelected(const QModelIndex &)));

	// First, create left panel - the slide list, then everything on the right
	m_mainSplitter->addWidget(m_slideList);
	
	// Now create the center panel - on top will be a list view, below it the graphics view
	m_graphicsView = new EditorGraphicsView();
	m_graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	
// 	m_graphicsScene = new GLEditorGraphicsScene();
// 	m_graphicsView->setScene(m_graphicsScene);
// 	m_graphicsScene->setSceneRect(QRectF(0,0,1000.,750.));
// 	connect(m_graphicsScene, SIGNAL(drawableSelected(GLDrawable*)), this, SLOT(drawableSelected(GLDrawable*)));
	
	// Add the layout list (top), and GV (bottom), then add the middle splitter to the right side splitter
	//m_centerSplitter->addWidget(m_layoutList);
	m_centerSplitter->addWidget(m_graphicsView);
	m_mainSplitter->addWidget(m_centerSplitter);
	
	// Now create the control base for the other half of the right side splitter
	m_controlArea = new QScrollArea(m_mainSplitter);
	m_controlArea->setWidgetResizable(true);
	m_controlBase = new QWidget(m_controlArea);
	
	QVBoxLayout *layout = new QVBoxLayout(m_controlBase);
	layout->setContentsMargins(0,0,0,0);
	
	m_controlArea->setWidget(m_controlBase);
	
	m_mainSplitter->addWidget(m_controlArea);
	
	setCentralWidget(m_mainSplitter);
	
	// Now create the toolbar that users can use to add items to the current slide
	QToolBar * toolbar = addToolBar(tr("Editor Actions"));
	
	
	QAction *act;
	
	act = new QAction(QIcon("../data/stock-panel-screenshot.png"), tr("New Video Input Layer"), this);
	connect(act, SIGNAL(triggered()), this, SLOT(addVideoInput()));
	toolbar->addAction(act);
	
	act = new QAction(QIcon("../data/stock-panel-multimedia.png"), tr("New Video Layer"), this);
	connect(act, SIGNAL(triggered()), this, SLOT(addVideoFile()));
	toolbar->addAction(act);
	
	act = new QAction(QIcon("../data/stock-insert-object.png"), tr("New Video Loop Layer"), this);
	connect(act, SIGNAL(triggered()), this, SLOT(addVideoLoop()));
	toolbar->addAction(act);
		
	act = new QAction(QIcon("../data/stock-font.png"), tr("New Text Layer"), this);
	connect(act, SIGNAL(triggered()), this, SLOT(addText()));
	toolbar->addAction(act);
	
	act = new QAction(QIcon("../data/stock-insert-image.png"), tr("New Image Layer"), this);
	connect(act, SIGNAL(triggered()), this, SLOT(addImage()));
	toolbar->addAction(act);
	
	toolbar->addSeparator();
	
	act = new QAction(QIcon("../data/action-add.png"), tr("Add New Slide"), this);
	connect(act, SIGNAL(triggered()), this, SLOT(addScene()));
	toolbar->addAction(act);
	
	act = new QAction(QIcon("../data/action-delete.png"), tr("Delete Current Slide"), this);
	connect(act, SIGNAL(triggered()), this, SLOT(delScene()));
	toolbar->addAction(act);
	
	act = new QAction(QIcon("../data/stock-convert.png"), tr("Duplicate Current Slide"), this);
	connect(act, SIGNAL(triggered()), this, SLOT(dupScene()));
	toolbar->addAction(act);
}


void EditorWindow::setGroup(GLSceneGroup *group,  GLScene *currentScene)
{
	if(!group || group == m_group)
		return;
		
	m_group = group;
	
	//qDebug() << "EditorWindow::setGroup: group:"<<group<<", currentScene:"<<currentScene;
	
	// deleting old selection model per http://doc.trolltech.com/4.5/qabstractitemview.html#setModel
	QItemSelectionModel *m = m_slideList->selectionModel();

	m_slideList->setModel(group);
// 	connect(m_sceneModel, SIGNAL(repaintList()), this, SLOT(repaintLayerList()));
// 	connect(m_sceneModel, SIGNAL(layersDropped(QList<LiveLayer*>)), this, SLOT(layersDropped(QList<LiveLayer*>)));

	if(m)
	{
		disconnect(m, 0, this, 0);
		delete m;
		m=0;
	}

	QItemSelectionModel *currentSelectionModel = m_slideList->selectionModel();
	connect(currentSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(currentSlideChanged(const QModelIndex &, const QModelIndex &)));
	
	if(!currentScene)
		currentScene = group->at(0);
		
	setCurrentScene(currentScene);
}

void EditorWindow::slideSelected(const QModelIndex &idx)
{
	if(idx.isValid())
		setCurrentScene(m_group->at(idx.row()));
}


void EditorWindow::currentSlideChanged(const QModelIndex &idx,const QModelIndex &)
{
	if(idx.isValid())
		slideSelected(idx);
}


void EditorWindow::setCurrentScene(GLScene *scene)
{
	//qDebug() << "EditorWindow::setCurrentScene: [a] "<<scene;
	if(!scene || scene == m_scene)
		return;
	
	if(m_scene)
	{
		disconnect(m_scene, 0, this, 0);
		m_scene = 0;
	}
	
	//qDebug() << "EditorWindow::setCurrentScene: "<<scene;
	m_scene = scene;
	
	if(m_scene)
	{
		if(m_graphicsScene)
			m_graphicsScene->setEditingMode(false);
		
		m_graphicsScene = m_scene->graphicsScene();
		m_graphicsScene->setEditingMode(true);
		m_graphicsView->setScene(m_graphicsScene);
		m_graphicsScene->setSceneRect(QRectF(0,0,1000.,750.));
		connect(m_graphicsScene, SIGNAL(drawableSelected(GLDrawable*)), this, SLOT(drawableSelected(GLDrawable*)));
		
  		GLDrawableList list = m_scene->drawableList();
// 		
// 		m_graphicsScene->clear();
// 		
// 		foreach(GLDrawable *drawable, list)
// 			m_graphicsScene->addItem(drawable);
		
// 		connect(m_scene, SIGNAL(drawableAdded(GLDrawable*)), this, SLOT(drawableAdded(GLDrawable*)));
// 		connect(m_scene, SIGNAL(drawableRemoved(GLDrawable*)), this, SLOT(drawableRemoved(GLDrawable*)));
		
		if(!list.isEmpty())
			list.first()->setSelected(true);
	}
}

void EditorWindow::drawableAdded(GLDrawable */*d*/)
{
	//qDebug() << "EditorWindow::drawableAdded: "<<(QObject*)d;
	//m_graphicsScene->addItem(d);
}

void EditorWindow::drawableRemoved(GLDrawable */*d*/)
{
	//m_graphicsScene->removeItem(d);
}

GLSceneGroup * EditorWindow::group()
{
	if(!m_group)
		setGroup(new GLSceneGroup());
	
	return m_group;
}

GLScene *EditorWindow::scene()
{
	if(!m_scene)
	{
		GLScene *scene = group()->at(0);
		if(scene)
			setCurrentScene(scene);
	}
		
	if(!m_scene)
	{
		GLScene *scene = new GLScene();
		m_group->addScene(scene);
		setCurrentScene(scene);
	}
	
	return m_scene;
}	
	
void EditorWindow::addDrawable(GLDrawable *drawable)
{
	drawable->addShowAnimation(GLDrawable::AnimFade);
	scene()->addDrawable(drawable);
	
	if(m_graphicsScene)
	{
		drawable->setRect(m_graphicsScene->sceneRect());
		m_graphicsScene->setEditingMode(true);
	}
	
	drawable->setSelected(true);
	drawable->show();
}

void EditorWindow::addVideoInput()
{
	GLVideoInputDrawable *drawable = new GLVideoInputDrawable("/dev/video0");
	drawable->setCardInput("S-Video");
	addDrawable(drawable);
}

void EditorWindow::addVideoLoop()
{
	addDrawable(new GLVideoLoopDrawable("../data/Seasons_Loop_3_SD.mpg"));
}

void EditorWindow::addVideoFile()
{
	addDrawable(new GLVideoFileDrawable("../data/Seasons_Loop_3_SD.mpg"));
	//addDrawable(new GLVideoLoopDrawable("../data/Seasons_Loop_3_SD.mpg"));
}

void EditorWindow::addImage()
{
	addDrawable(new GLImageDrawable("Pm5544.jpg"));
}

void EditorWindow::addText(const QString& str)
{
	addDrawable(new GLTextDrawable(str));
}

void EditorWindow::drawableSelected(GLDrawable *d)
{
	if(m_currentLayerPropsEditor)
	{
		m_controlBase->layout()->removeWidget(m_currentLayerPropsEditor);
		m_currentLayerPropsEditor->deleteLater();
		m_currentLayerPropsEditor = 0;
	}

	//m_currentLayer->lockLayerPropertyUpdates(true);
	QWidget *props = createPropertyEditors(d);
	//m_currentLayer->lockLayerPropertyUpdates(false);

	/// TODO What am I doing here??
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

QWidget *EditorWindow::createPropertyEditors(GLDrawable *gld)
{
	m_currentDrawable = gld;
	
	QWidget * base = new QWidget();
	QVBoxLayout *blay = new QVBoxLayout(base);
	blay->setContentsMargins(0,0,0,0);


	PropertyEditorFactory::PropertyEditorOptions opts;

	opts.reset();
	
	{
		ExpandableWidget *group = new ExpandableWidget("Basic Settings",base);
		blay->addWidget(group);
	
		QWidget *cont = new QWidget;
		QFormLayout *lay = new QFormLayout(cont);
		lay->setContentsMargins(3,3,3,3);
		
		group->setWidget(cont);
		
		lay->addRow(tr("&Item Name:"), PropertyEditorFactory::generatePropertyEditor(gld, "itemName", SLOT(setItemName(const QString&)), opts));
		lay->addRow(PropertyEditorFactory::generatePropertyEditor(gld, "userControllable", SLOT(setUserControllable(bool)), opts));
		
		opts.reset();
		opts.suffix = "%";
		opts.min = 0;
		opts.max = 100;
		opts.defaultValue = 100;
		opts.type = QVariant::Int;
		opts.doubleIsPercentage = true;
		lay->addRow(tr("&Opacity:"), PropertyEditorFactory::generatePropertyEditor(gld, "opacity", SLOT(setOpacity(int)), opts));
		
		opts.reset();
		opts.noSlider = true;
		opts.type = QVariant::Int;
		opts.defaultValue = 0;
		lay->addRow(tr("&Z Value:"), PropertyEditorFactory::generatePropertyEditor(gld, "zIndex", SLOT(setZIndex(int)), opts));
		
	}
	
	
	{
		ExpandableWidget *group = new ExpandableWidget("Item Setup",base);
		blay->addWidget(group);
	
		QWidget *cont = new QWidget;
		QFormLayout *lay = new QFormLayout(cont);
		lay->setContentsMargins(3,3,3,3);
		
		group->setWidget(cont);
		
		
		if(GLVideoInputDrawable *item = dynamic_cast<GLVideoInputDrawable*>(gld))
		{
			// show device selection box
			
			opts.type = QVariant::Bool;
			opts.text = "Deinterlace video";
			lay->addRow(PropertyEditorFactory::generatePropertyEditor(item, "deinterlace", SLOT(setDeinterlace(bool)), opts));
			
			opts.text = "Flip horizontal";
			lay->addRow(PropertyEditorFactory::generatePropertyEditor(item, "flipHorizontal", SLOT(setFlipHorizontal(bool)), opts));
			
			opts.text = "Ignore video aspect ratio";
			lay->addRow(PropertyEditorFactory::generatePropertyEditor(item, "ignoreAspectRatio", SLOT(setIgnoreAspectRatio(bool)), opts));
			
		}
		else
		if(GLVideoLoopDrawable *item = dynamic_cast<GLVideoLoopDrawable*>(gld))
		{
		
			PropertyEditorFactory::PropertyEditorOptions opts;
			opts.stringIsFile = true;
			opts.fileTypeFilter = tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif);;Any File (*.*)");
			
			lay->addRow(tr("&File:"), PropertyEditorFactory::generatePropertyEditor(item, "videoFile", SLOT(setVideoFile(const QString&)), opts, SIGNAL(videoFileChanged(const QString&))));
			
			opts.text = "Ignore video aspect ratio";
			lay->addRow(PropertyEditorFactory::generatePropertyEditor(item, "ignoreAspectRatio", SLOT(setIgnoreAspectRatio(bool)), opts));
		}
		else
		if(GLVideoFileDrawable *item = dynamic_cast<GLVideoFileDrawable*>(gld))
		{
			PropertyEditorFactory::PropertyEditorOptions opts;
			opts.stringIsFile = true;
			opts.fileTypeFilter = tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif);;Any File (*.*)");
			
			lay->addRow(tr("&File:"), PropertyEditorFactory::generatePropertyEditor(item, "videoFile", SLOT(setVideoFile(const QString&)), opts, SIGNAL(videoFileChanged(const QString&))));
			
			opts.text = "Ignore video aspect ratio";
			lay->addRow(PropertyEditorFactory::generatePropertyEditor(item, "ignoreAspectRatio", SLOT(setIgnoreAspectRatio(bool)), opts));
		}
		else
		if(GLTextDrawable *item = dynamic_cast<GLTextDrawable*>(gld))
		{
			opts.reset();
			
// 			QHBoxLayout *countdown = new QHBoxLayout();
// 			QWidget *boolEdit = PropertyEditorFactory::generatePropertyEditor(gld, "isCountdown", SLOT(setIsCountdown(bool)), opts);
// 			QWidget *dateEdit  = PropertyEditorFactory::generatePropertyEditor(gld, "targetDateTime", SLOT(setTargetDateTime(QDateTime)), opts);
// 			QCheckBox *box = dynamic_cast<QCheckBox*>(boolEdit);
// 			if(box)
// 			{
// 				connect(boolEdit, SIGNAL(toggled(bool)), dateEdit, SLOT(setEnabled(bool)));
// 				dateEdit->setEnabled(box->isChecked());
// 			}
// 			countdown->addWidget(boolEdit);
// 			countdown->addWidget(dateEdit);
// 			countdown->addStretch(1);
// 			lay->addRow(countdown);
// 			
// 			m_rtfEditor = new RichTextEditorDialog(this);
// 			m_rtfEditor->setText(item->text());
// 			//m_editor->initFontSize(m_model->findFontSize());
// 			
// 			lay->addRow(m_rtfEditor);
// 			m_rtfEditor->adjustSize();
// 			m_rtfEditor->focusEditor();
// 			
// 			QPushButton *btn = new QPushButton("&Save Text");
// 			connect(btn, SIGNAL(clicked()), this, SLOT(rtfEditorSave()));
// 			
// 			QHBoxLayout *hbox = new QHBoxLayout();
// 			hbox->addStretch(1);
// 			hbox->addWidget(btn);
// 			lay->addRow(hbox); 
			
			opts.reset();
		
			QWidget *edit = PropertyEditorFactory::generatePropertyEditor(gld, "plainText", SLOT(setPlainText(const QString&)), opts, SIGNAL(plainTextChanged(const QString&)));
			
// 			QLineEdit *line = dynamic_cast<QLineEdit*>(edit);
// 			if(line)
// 				connect(gld, SIGNAL(plainTextChanged(const QString&)), line, SLOT(setText(const QString&)));
			
			QWidget *base = new QWidget();
			
			RtfEditorWindow *dlg = new RtfEditorWindow(item, base);
			QPushButton *btn = new QPushButton("&Advanced...");
			connect(btn, SIGNAL(clicked()), dlg, SLOT(show()));
			
			QHBoxLayout *hbox = new QHBoxLayout(base);
			hbox->setContentsMargins(0,0,0,0);
			hbox->addWidget(new QLabel("Text:"));
			hbox->addWidget(edit);
			hbox->addWidget(btn);
			
			lay->addRow(base); 
		}
		else
		if(GLImageDrawable *item = dynamic_cast<GLImageDrawable*>(gld))
		{
			PropertyEditorFactory::PropertyEditorOptions opts;
			opts.stringIsFile = true;
			opts.fileTypeFilter = tr("Image Files (*.png *.jpg *.bmp *.svg *.xpm *.gif);;Any File (*.*)");
			lay->addRow(tr("&Image:"), PropertyEditorFactory::generatePropertyEditor(item, "imageFile", SLOT(setImageFile(const QString&)), opts, SIGNAL(imageFileChanged(const QString&))));
			
			opts.text = "Ignore image aspect ratio";
			lay->addRow(PropertyEditorFactory::generatePropertyEditor(item, "ignoreAspectRatio", SLOT(setIgnoreAspectRatio(bool)), opts));
		}
		
		
		
	}
	
	
	{
		
		ExpandableWidget *groupAnim = new ExpandableWidget("Show/Hide Effects",base);
		blay->addWidget(groupAnim);
	
		QWidget *groupAnimContainer = new QWidget;
		QGridLayout *animLayout = new QGridLayout(groupAnimContainer);
		animLayout->setContentsMargins(3,3,3,3);
	
		groupAnim->setWidget(groupAnimContainer);
	
		opts.reset();
		opts.suffix = " ms";
		opts.min = 10;
		opts.max = 8000;
		opts.defaultValue = 300;
	
		int row = 0;
		animLayout->addWidget(PropertyEditorFactory::generatePropertyEditor(gld, "fadeIn", SLOT(setFadeIn(bool)), opts), row, 0);
		animLayout->addWidget(PropertyEditorFactory::generatePropertyEditor(gld, "fadeInLength", SLOT(setFadeInLength(int)), opts), row, 1);
	
		row++;
		animLayout->addWidget(PropertyEditorFactory::generatePropertyEditor(gld, "fadeOut", SLOT(setFadeOut(bool)), opts), row, 0);
		animLayout->addWidget(PropertyEditorFactory::generatePropertyEditor(gld, "fadeOutLength", SLOT(setFadeOutLength(int)), opts), row, 1);
	
		opts.reset();
	
		groupAnim->setExpandedIfNoDefault(false);
	}
	
	return base;
	
}

void EditorWindow::rtfEditorSave()
{
	GLTextDrawable *text = dynamic_cast<GLTextDrawable*>(m_currentDrawable);
	if(!text)
		return;
	text->setText(m_rtfEditor->text(Qt::RichText));
}

void EditorWindow::setCanvasSize(const QSizeF& size)
{
	m_canvasSize = size;
	if(m_graphicsScene)
		m_graphicsScene->setSceneRect(QRectF(0,0,size.width(),size.height()));
}

void EditorWindow::addScene()
{
	GLScene *scene = new GLScene();
	m_group->addScene(scene);
	setCurrentScene(scene);
}

void EditorWindow::delScene()
{
	if(!m_scene)
		return;
	group()->removeScene(m_scene);
	delete m_scene;
	m_scene = 0;
	
	setCurrentScene(scene());
}

void EditorWindow::dupScene()
{
	if(!m_scene)
		return;
	
	QByteArray ba = m_scene->toByteArray();
	GLScene *scene = new GLScene(ba, group());
	
	m_group->addScene(scene);
	setCurrentScene(scene);
}

