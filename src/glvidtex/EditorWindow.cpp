#include "EditorWindow.h"

#include "GLSceneGroup.h"
#include "GLEditorGraphicsScene.h"

#include "GLImageDrawable.h"
#include "GLVideoLoopDrawable.h"
#include "GLVideoInputDrawable.h"
#include "GLVideoFileDrawable.h"
#include "GLVideoReceiverDrawable.h"
#include "GLTextDrawable.h"
#include "GLVideoMjpegDrawable.h"

#include "../livemix/ExpandableWidget.h"
#include "../livemix/EditorUtilityWidgets.h"

#include "../3rdparty/richtextedit/richtexteditor_p.h"

class EditorGraphicsView : public QGraphicsView
{
	public:
		EditorGraphicsView(QWidget * parent=0)
			: QGraphicsView(parent)
			, m_canZoom(true)
		{
			setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
			setCacheMode(QGraphicsView::CacheBackground);
			setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
			setOptimizationFlags(QGraphicsView::DontSavePainterState);
			setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
			setTransformationAnchor(AnchorUnderMouse);
			setResizeAnchor(AnchorViewCenter);
			
			//setFrameStyle(QFrame::NoFrame);
			//setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			//setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			
			setBackgroundBrush(Qt::gray);
			
		}

		bool canZoom() { return m_canZoom; }
		void setCanZoom(bool flag) { m_canZoom = flag; }


	protected:
		void keyPressEvent(QKeyEvent *event)
		{
			if(event->modifiers() & Qt::ControlModifier)
			{

				switch (event->key())
				{
					case Qt::Key_Plus:
						scaleView(qreal(1.2));
						break;
					case Qt::Key_Minus:
                                        case Qt::Key_Equal:
						scaleView(1 / qreal(1.2));
						break;
					default:
						QGraphicsView::keyPressEvent(event);
				}
			}
			else
				QGraphicsView::keyPressEvent(event);
		}


		void wheelEvent(QWheelEvent *event)
		{
                        scaleView(pow((double)2, event->delta() / 240.0));
		}


		void scaleView(qreal scaleFactor)
		{
			if(!m_canZoom)
				return;

			qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
			if (factor < 0.07 || factor > 100)
				return;

			scale(scaleFactor, scaleFactor);
		}
	private:
		bool m_canZoom;
};



EditorWindow::EditorWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_canvasSize(1000.,750.)
	, m_group(0)
	, m_scene(0)
	, m_currentLayerPropsEditor(0)
	, m_rtfEditor(0)
	, m_currentDrawable(0)
	
{
	createUI();
	setCanvasSize(m_canvasSize);
}

EditorWindow::~EditorWindow()
{
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
	m_slideList->setMovement(QListView::Free);
	m_slideList->setWordWrap(true);
	m_slideList->setSelectionMode(QAbstractItemView::ExtendedSelection);
// 	m_slideList->setDragEnabled(true);
// 	m_slideList->setAcceptDrops(true);
// 	m_slideList->setDropIndicatorShown(true);

	connect(m_slideList, SIGNAL(activated(const QModelIndex &)), this, SLOT(slideSelected(const QModelIndex &)));
	connect(m_slideList, SIGNAL(clicked(const QModelIndex &)),   this, SLOT(slideSelected(const QModelIndex &)));

	// First, create left panel - the slide list, then everything on the right
	m_mainSplitter->addWidget(m_slideList);
	
	// Now create the center panel - on top will be a list view, below it the graphics view
	EditorGraphicsView *graphicsView = new EditorGraphicsView();
	graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	
	m_graphicsScene = new GLEditorGraphicsScene();
	graphicsView->setScene(m_graphicsScene);
	m_graphicsScene->setSceneRect(QRectF(0,0,1000.,750.));
	connect(m_graphicsScene, SIGNAL(drawableSelected(GLDrawable*)), this, SLOT(drawableSelected(GLDrawable*)));
	
	// Add the layout list (top), and GV (bottom), then add the middle splitter to the right side splitter
	m_centerSplitter->addWidget(m_layoutList);
	m_centerSplitter->addWidget(graphicsView);
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
	if(!scene) // || scene == m_scene)
		return;
	
	if(m_scene)
	{
		disconnect(m_scene, 0, this, 0);
		m_scene = 0;
	}
	
	qDebug() << "EditorWindow::setCurrentScene: "<<scene;
	m_scene = scene;
	
	m_layoutList->setModel(m_scene->layoutListModel());
	
	GLDrawableList list = m_scene->drawableList();
	foreach(GLDrawable *drawable, list)
	{
		m_graphicsScene->addItem(drawable);
	}
	
	connect(m_scene, SIGNAL(drawableAdded(GLDrawable*)), this, SLOT(drawableAdded(GLDrawable*)));
	connect(m_scene, SIGNAL(drawableRemoved(GLDrawable*)), this, SLOT(drawableRemoved(GLDrawable*)));
}

void EditorWindow::drawableAdded(GLDrawable *d)
{
	qDebug() << "EditorWindow::drawableAdded: "<<(QObject*)d;
	m_graphicsScene->addItem(d);
}

void EditorWindow::drawableRemoved(GLDrawable *d)
{
	m_graphicsScene->removeItem(d);
}

GLSceneGroup * EditorWindow::group()
{
	if(!m_group)
	{
		m_group = new GLSceneGroup();
		setGroup(m_group);
	}
	return m_group;
}

GLScene *EditorWindow::scene()
{
	if(!m_scene)
	{
		m_scene = group()->at(0);
		if(m_scene)
		{
			setCurrentScene(m_scene);
		}
	}
		
	if(!m_scene)
	{
		m_scene = new GLScene();
		m_group->addScene(m_scene);
		setCurrentScene(m_scene);
	}
	
	return m_scene;
}	
	
void EditorWindow::addDrawable(GLDrawable *drawable)
{
	drawable->addShowAnimation(GLDrawable::AnimFade);
	drawable->setRect(m_graphicsScene->sceneRect());
	scene()->addDrawable(drawable);
	
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
			// show deinterlace checkbox
			
		}
		else
		if(GLVideoLoopDrawable *item = dynamic_cast<GLVideoLoopDrawable*>(gld))
		{
		
			PropertyEditorFactory::PropertyEditorOptions opts;
			opts.stringIsFile = true;
			opts.fileTypeFilter = tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif);;Any File (*.*)");
			
			lay->addRow(tr("&File:"), PropertyEditorFactory::generatePropertyEditor(item, "videoFile", SLOT(setVideoFile(const QString&)), opts));
		}
		else
		if(GLVideoFileDrawable *item = dynamic_cast<GLVideoFileDrawable*>(gld))
		{
			PropertyEditorFactory::PropertyEditorOptions opts;
			opts.stringIsFile = true;
			opts.fileTypeFilter = tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif);;Any File (*.*)");
			
			lay->addRow(tr("&File:"), PropertyEditorFactory::generatePropertyEditor(item, "videoFile", SLOT(setVideoFile(const QString&)), opts));
		}
		else
		if(GLTextDrawable *item = dynamic_cast<GLTextDrawable*>(gld))
		{
			m_rtfEditor = new RichTextEditorDialog(this);
			m_rtfEditor->setText(item->text());
			//m_editor->initFontSize(m_model->findFontSize());
			
			lay->addRow(m_rtfEditor);
			m_rtfEditor->adjustSize();
			m_rtfEditor->focusEditor();
			
			QPushButton *btn = new QPushButton("&Save Text");
			connect(btn, SIGNAL(clicked()), this, SLOT(rtfEditorSave()));
			
			QHBoxLayout *hbox = new QHBoxLayout();
			hbox->addStretch(1);
			hbox->addWidget(btn);
			lay->addRow(hbox); 
		}
		else
		if(GLImageDrawable *item = dynamic_cast<GLImageDrawable*>(gld))
		{
			PropertyEditorFactory::PropertyEditorOptions opts;
			opts.stringIsFile = true;
			opts.fileTypeFilter = tr("Image Files (*.png *.jpg *.bmp *.svg *.xpm *.gif);;Any File (*.*)");
			lay->addRow(tr("&Image:"), PropertyEditorFactory::generatePropertyEditor(item, "imageFile", SLOT(setImageFile(const QString&)), opts));
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
	m_graphicsScene->setSceneRect(QRectF(0,0,size.width(),size.height()));
}

void EditorWindow::addScene()
{
	m_scene = new GLScene();
	m_group->addScene(m_scene);
	setCurrentScene(m_scene);
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
	m_scene = new GLScene(ba, group());
	
	m_group->addScene(m_scene);
	setCurrentScene(m_scene);
}

