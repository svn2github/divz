#include "SlideEditorWindow.h"
#include "MyGraphicsScene.h"
#include "RenderOpts.h"

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QDesktopWidget>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFile>
#include <QImageReader>
#include <QInputDialog>
#include <QMenu>
/*#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>*/
#include <QPushButton>
#include <QSettings>
#include <QTimer>
#include <QVBoxLayout>
#include <QGraphicsSimpleTextItem>
#include <QLabel>
#include <QStyleOption>
#include <QCheckBox>
#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif
#include <QDockWidget>
#include <QListView>

#include <QWheelEvent>
#include <QToolBar>

#include <QDebug>
#include <assert.h>

#include "model/ItemFactory.h"
#include "model/Slide.h"
#include "model/TextItem.h"


#include <QCommonStyle>
class RubberBandStyle : public QCommonStyle 
{
	public:
		void drawControl(ControlElement element, const QStyleOption * option, QPainter * painter, const QWidget * widget = 0) const
		{
			if (element == CE_RubberBand) 
			{
				painter->save();
				QColor color = option->palette.color(QPalette::Highlight);
				painter->setPen(color);
				color.setAlpha(80); painter->setBrush(color);
				painter->drawRect(option->rect.adjusted(0,0,-1,-1));
				painter->restore();
				return;
			}
			return QCommonStyle::drawControl(element, option, painter, widget);
		}
		
		int styleHint(StyleHint hint, const QStyleOption * option, const QWidget * widget, QStyleHintReturn * returnData) const
		{
			if (hint == SH_RubberBand_Mask)
				return false;
			return QCommonStyle::styleHint(hint, option, widget, returnData);
		}
};

#include <QGraphicsView>
class MyGraphicsView : public QGraphicsView 
{
	public:
		MyGraphicsView(QWidget * parent)
			: QGraphicsView(parent)
			, m_desk(0)
		{
			// customize widget
// 			setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
// 			setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			setInteractive(true);
			setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
			setDragMode(QGraphicsView::RubberBandDrag);
			//setAcceptDrops(true);
			//setFrameStyle(QFrame::NoFrame);
			
			setRenderHint( QPainter::TextAntialiasing, true);
			setRenderHint( QPainter::Antialiasing, true );
			
			setCacheMode(QGraphicsView::CacheBackground);
			setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
			setOptimizationFlags(QGraphicsView::DontSavePainterState);
			setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
			
			
			setTransformationAnchor(AnchorUnderMouse);
			setResizeAnchor(AnchorViewCenter);
		
		//             // don't autofill the view with the Base brush
		//             QPalette pal;
		//             pal.setBrush(QPalette::Base, Qt::NoBrush);
		//             setPalette(pal);
		
			// use own style for drawing the RubberBand (opened on the viewport)
			viewport()->setStyle(new RubberBandStyle);
		
			// can't activate the cache mode by default, since it inhibits dynamical background picture changing
			//setCacheMode(CacheBackground);
		}
	
		void setMyScene(MyGraphicsScene * desk)
		{
			setScene(desk);
			m_desk = desk;
		}
		
	
	protected:
		void resizeEvent(QResizeEvent * event)
		{
// 			if (m_desk)
// 				m_desk->resize(contentsRect().size());
			QGraphicsView::resizeEvent(event);
		}
		
		
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
		}
		
		
		void wheelEvent(QWheelEvent *event)
		{
                        scaleView(pow((double)2, event->delta() / 240.0));
		}
		
		
		void scaleView(qreal scaleFactor)
		{
			qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
			if (factor < 0.07 || factor > 100)
				return;
			
			scale(scaleFactor, scaleFactor);
		}

	
	private:
		MyGraphicsScene * m_desk;
};




SlideEditorWindow::SlideEditorWindow(SlideGroup *group, QWidget * parent)
    : QMainWindow(parent)
{
	// setup widget
	QRect geom = QApplication::desktop()->availableGeometry();
	//resize(2 * geom.width() / 3, 2 * geom.height() / 3);
	//resize(1400,600);
	//move(3500,100);
	
	QPixmap newpix("new.png");
// 	QPixmap openpix("open.png");
// 	QPixmap quitpix("quit.png");
// 	
	QToolBar *toolbar = addToolBar("main toolbar");
	toolbar->setObjectName("maintoolbar");
	QAction  *newAction = toolbar->addAction(QIcon(), "New Text Item");
// 	toolbar->addAction(QIcon(openpix), "Open File");
// 	toolbar->addSeparator();
// 	QAction *quit = toolbar->addAction(QIcon(quitpix), 
// 	"Quit Application");
	
// 	connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(newAction, SIGNAL(triggered()), this, SLOT(newTextItem()));

	QAction  *newBox = toolbar->addAction(QIcon(), "New Box Item");
	connect(newBox, SIGNAL(triggered()), this, SLOT(newBoxItem()));

	QAction  *newVideo = toolbar->addAction(QIcon(), "New Video Item");
	connect(newVideo, SIGNAL(triggered()), this, SLOT(newVideoItem()));
	
	QAction  *newSlide = toolbar->addAction(QIcon(), "New Slide");
	connect(newSlide, SIGNAL(triggered()), this, SLOT(newSlide()));
	
	QAction  *delSlide = toolbar->addAction(QIcon(), "Delete Slide");
	connect(delSlide, SIGNAL(triggered()), this, SLOT(delSlide()));


	m_scene = new MyGraphicsScene(this);
	MyGraphicsView *graphicsView = new MyGraphicsView(this);
	
	graphicsView->setMyScene(m_scene);
	m_scene->setSceneRect(0,0,1024,768);
	resize(1024,768);
	
	// Restore state
	QSettings settings;
	QSize sz = settings.value("slideeditor/size").toSize();
	if(sz.isValid())
		resize(sz);
	QPoint p = settings.value("slideeditor/pos").toPoint();
	if(!p.isNull())
		move(p);
	restoreState(settings.value("slideeditor/state").toByteArray());

	
	//qDebug("Checking for OpenGL...");
	#ifndef QT_NO_OPENGL
	if(!RenderOpts::DisableOpenGL)
	{
		graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
		qDebug("SlideEditorWindow: Loaded OpenGL viewport.");
	}
	#endif
	
// 	SimpleTextItem * textItem = new SimpleTextItem("Hello, World");
// 	QPen textPen;
// 	textPen.setWidthF(3);
// 	textPen.setColor(QColor(0,0,0,240));
// 	textItem->setPen(textPen);
// 	textItem->setBrush(QColor(255,255,255,255));
// 	textItem->setFont(QFont("Tahoma",88,QFont::Bold));
// 	textItem->setZValue(2);
// 	textItem->setPos(10,230);
// 	m_scene->addItem(textItem);
// 	textItem->setFlags(QGraphicsItem::ItemIsMovable);
// 	
	
	//TextContent * text = m_scene->addTextContent();
	
	setupSlideGroupDockWidget();
	

	
/*
	if(QFile("test.xml").exists())
	{
		m_doc.load("test.xml");
		//r.readSlide(m_slide);
		
		qDebug("Loaded test.xml");
		
	}
	else
	{
		Slide * slide = new Slide();
		SlideGroup *g = new SlideGroup();
		g->addSlide(slide);
		m_doc.addGroup(g);
		//m_scene->setSlide(slide);
	}
	
	QList<SlideGroup*> glist = m_doc.groupList();
	if(glist.size() >0)
	{
		SlideGroup *g = glist[0];
		assert(g);
		
		setSlideGroup(g);
	}
	else
	{
		qDebug("Error: No groups in test.xml");
	}*/
	
	if(group != 0)
		setSlideGroup(group);
        setCentralWidget(graphicsView);
}

SlideEditorWindow::~SlideEditorWindow()
{
// 	XmlSave save("test.xml");
// 	save.saveSlide(m_slide);
// 	m_doc.save("test.xml");
	
	QSettings settings;
	settings.setValue("slideeditor/size",size());
	settings.setValue("slideeditor/pos",pos());
	settings.setValue("slideeditor/state",saveState());
	
	
// 	delete m_slide;
// 	m_slide = 0;

}

void SlideEditorWindow::setupSlideGroupDockWidget()
{
	QDockWidget *dock = new QDockWidget(tr("Slides"), this);
	dock->setObjectName("SlidesDockWidget");
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	m_slideListView = new QListView(dock);
	
	connect(m_slideListView,SIGNAL(activated(const QModelIndex &)),this,SLOT(slideSelected(const QModelIndex &)));
	connect(m_slideListView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(slideSelected(const QModelIndex &)));
	
	// deleting old selection model per http://doc.trolltech.com/4.5/qabstractitemview.html#setModel
	QItemSelectionModel *m = m_slideListView->selectionModel();
	
	m_slideModel = new SlideGroupListModel();
	m_slideListView->setModel(m_slideModel);
	connect(m_slideModel, SIGNAL(modelChanged()), this, SLOT(slideListModelChanged()));
	
	if(m)
	{
		delete m;
		m=0;
	}
 	dock->setWidget(m_slideListView);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
	//viewMenu->addAction(dock->toggleViewAction());
}

void SlideEditorWindow::slideListModelChanged()
{
	//m_slideModel->setSlideGroup(m_slideGroup);
	m_slideListView->reset();
	
	Slide *s = m_scene->slide();
	m_slideListView->setCurrentIndex(m_slideModel->indexForSlide(s));
}

void SlideEditorWindow::setSlideGroup(SlideGroup *g,Slide *curSlide)
{
	m_slideGroup = g;
	m_slideModel->setSlideGroup(g);
	m_slideListView->reset();

	setWindowTitle(QString("%1 - Slide Editor").arg(g->groupTitle().isEmpty() ? QString("Group %1").arg(g->groupNumber()) : g->groupTitle()));
	//m_slideListView->setModel(m_slideModel);
	
	if(curSlide)
	{
		m_scene->setSlide(curSlide);
		m_slideListView->setCurrentIndex(m_slideModel->indexForSlide(curSlide));
	}
	else
	{
		QList<Slide*> slist = g->slideList();
		if(slist.size() > 0)
		{
			Slide *s = m_slideModel->slideAt(0);
			m_scene->setSlide(s);
			m_slideListView->setCurrentIndex(m_slideModel->indexForRow(0));
		}
		else
		{
			qDebug("SlideEditorWindow::setSlideGroup: Group[0] has 0 slides");
		}
	}
		

}

void SlideEditorWindow::slideSelected(const QModelIndex &idx)
{
	Slide *s = m_slideModel->slideFromIndex(idx);
	qDebug() << "SlideEditorWindow::slideSelected(): selected slide#:"<<s->slideNumber();
	m_scene->setSlide(s);
}

void SlideEditorWindow::newSlide()
{
	Slide * slide = new Slide();
	slide->setSlideNumber(m_slideGroup->numSlides());
	slide->setSlideId(m_slideGroup->numSlides());
	m_slideGroup->addSlide(slide);
	
	//m_scene->setSlide(slide);
	
	setSlideGroup(m_slideGroup,slide);
}

void SlideEditorWindow::delSlide()
{
	Slide * slide = m_scene->slide();
	qDebug() << "delSlide: Removing slide#"<<slide->slideNumber();
	
	m_slideGroup->removeSlide(slide);
	
	QModelIndex idx = m_slideModel->indexForSlide(slide);
	QModelIndex prev = m_slideModel->indexForRow(idx.row() - 1 > 0 ? idx.row() - 1 : 0);
	
	Slide *newSlide = 0;
	if(prev.isValid())
	{
		m_slideListView->setCurrentIndex(prev);
		newSlide = m_slideModel->slideAt(prev.row());
	}
	
	if(newSlide)
	{
		m_scene->setSlide(newSlide);
	}
	
	QList<Slide*> slides = m_slideGroup->slideList();
	int counter = 0;
	foreach(Slide *s, slides)
	{
		s->setSlideNumber(counter++);
	}
	
	setSlideGroup(m_slideGroup,newSlide);
	
	delete slide;
	slide = 0;
}


void SlideEditorWindow::newTextItem()
{
	m_scene->newTextItem();
}

void SlideEditorWindow::newBoxItem()
{
        m_scene->newBoxItem();
}

void SlideEditorWindow::newVideoItem()
{
        m_scene->newVideoItem();
}
