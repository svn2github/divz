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
#include <QSplitter>


#include <QDebug>
#include <assert.h>

#include <QUndoView>
#include <QUndoStack>

#include <QGraphicsLineItem>

#include "model/ItemFactory.h"
#include "model/Slide.h"
#include "model/TextItem.h"
#include "MainWindow.h"
#include "AppSettings.h"

#define DEBUG_MODE 0

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
    : AbstractSlideGroupEditor(group,parent), m_usingGL(false),
    m_ignoreUndoPropChanges(false),
    m_slideGroup(0)
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
	QToolBar *toolbar = new QToolBar("main toolbar");
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

	toolbar->addSeparator();
        
        m_undoStack = new QUndoStack();
        setupUndoView();
        
        toolbar->addAction(m_undoStack->createUndoAction(this));
        toolbar->addAction(m_undoStack->createRedoAction(this));

	m_scene = new MyGraphicsScene(MyGraphicsScene::Editor,this);
	m_view = new MyGraphicsView(this);
	
	
	QRect sceneRect(0,0,1024,768);
	
	if(MainWindow::mw())
	{
		MainWindow *mw = MainWindow::mw();
		sceneRect = mw->standardSceneRect();
		
		connect(mw, SIGNAL(appSettingsChanged()),       this, SLOT(appSettingsChanged()));
 		connect(mw, SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
	}
	
	m_view->setMyScene(m_scene);
	m_scene->setSceneRect(sceneRect);
	resize(sceneRect.width(),sceneRect.height());
	
	// Restore state
	QSettings settings;
	QSize sz = settings.value("slideeditor/size").toSize();
	if(sz.isValid())
		resize(sz);
	QPoint p = settings.value("slideeditor/pos").toPoint();
	if(!p.isNull())
		move(p);

	appSettingsChanged();
	
	m_splitter = new QSplitter(this);
	m_splitter->setOrientation(Qt::Horizontal);

	setupSlideList();
	setupViewportLines();

	m_splitter->addWidget(m_view);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(toolbar);
	layout->addWidget(m_splitter);
	setLayout(layout);

	m_splitter->restoreState(settings.value("slideeditor/splitter").toByteArray());

	
	if(DEBUG_MODE)
	{
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
			m_scene->setSlide(slide);
		}
		
		QList<SlideGroup*> glist = m_doc.groupList();
		if(glist.size() >2)
		{
			SlideGroup *g = glist[0];
			assert(g);
			
			setSlideGroup(g);
		}
		else
		{
			qDebug("Error: No groups in test.xml");
		}
	}
	
	if(group)
		setSlideGroup(group);
	//setCentralWidget(m_view);
        
        
}

SlideEditorWindow::~SlideEditorWindow()
{
// 	XmlSave save("test.xml");
// 	save.saveSlide(m_slide);
 	
 	if(DEBUG_MODE)
 		m_doc.save("test.xml");
	
	QSettings settings;
	settings.setValue("slideeditor/size",size());
	settings.setValue("slideeditor/pos",pos());
	settings.setValue("slideeditor/splitter",m_splitter->saveState());
	
	delete m_undoStack;
	
	
// 	delete m_slide;
// 	m_slide = 0;

}

void SlideEditorWindow::closeEvent(QCloseEvent *evt)
{
	evt->accept();
	emit closed();
}


void SlideEditorWindow::setupUndoView()
{
    m_undoView = new QUndoView(m_undoStack);
    m_undoView->setWindowTitle(tr("Undo Stack"));
    m_undoView->show();
    m_undoView->setAttribute(Qt::WA_QuitOnClose, false);
}

void SlideEditorWindow::appSettingsChanged()
{
	if(AppSettings::useOpenGL() && !m_usingGL)
	{
		m_usingGL = true;
		m_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
		qDebug("SlideEditorWindow::appSettingsChanged(): Loaded OpenGL Viewport");
	}
	else
	if(!AppSettings::useOpenGL() && m_usingGL)
	{
		m_usingGL = false;
		m_view->setViewport(new QWidget());
		qDebug("SlideEditorWindow::appSettingsChanged(): Loaded Non-GL Viewport");
	}
}

void SlideEditorWindow::aspectRatioChanged(double x)
{
	qDebug("SlideEditorWindow::aspectRatioChanged(): New aspect ratio: %.02f",x);
	if(MainWindow::mw())
	{
		m_scene->setSceneRect(MainWindow::mw()->standardSceneRect());
		setupViewportLines();
	}
}


void SlideEditorWindow::setupSlideList()
{
	m_slideListView = new QListView(this);
	m_slideListView->setViewMode(QListView::ListMode);
	//m_slideListView->setViewMode(QListView::IconMode);
	m_slideListView->setMovement(QListView::Free);
	m_slideListView->setWordWrap(true);
	m_slideListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_slideListView->setDragEnabled(true);
	m_slideListView->setAcceptDrops(true);
	m_slideListView->setDropIndicatorShown(true);

	m_splitter->addWidget(m_slideListView);

	connect(m_slideListView,SIGNAL(activated(const QModelIndex &)),this,SLOT(slideSelected(const QModelIndex &)));
	connect(m_slideListView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(slideSelected(const QModelIndex &)));

	// deleting old selection model per http://doc.trolltech.com/4.5/qabstractitemview.html#setModel
	QItemSelectionModel *m = m_slideListView->selectionModel();

	m_slideModel = new SlideGroupListModel();
	m_slideListView->setModel(m_slideModel);
	connect(m_slideModel, SIGNAL(slidesDropped(QList<Slide*>)), this, SLOT(slidesDropped(QList<Slide*>)));

	if(m)
	{
		delete m;
		m=0;
	}
	//dock->setWidget(m_slideListView);
	//addDockWidget(Qt::LeftDockWidgetArea, dock);
	//viewMenu->addAction(dock->toggleViewAction());
}

class MyLine : public QGraphicsLineItem
{
public:
	MyLine ( qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem * parent = 0 ) : 
		QGraphicsLineItem (x1,y1,x2,y2,parent) {}
	
	void paint(QPainter*p, const QStyleOptionGraphicsItem*, QWidget*)
	{
		p->save();
		//p->setCompositionMode(QPainter::CompositionMode_Difference);
		p->setPen(pen());
		p->drawLine(line());
		p->restore();
	}	
};

void SlideEditorWindow::addVpLineX(qreal x, qreal y1, qreal y2, bool /*in*/)
{
	static QPen pw(Qt::gray, 0,Qt::DotLine);
	static QPen pb(Qt::black, 0,Qt::DotLine);

	qreal z1 = 0; //in ? 2:0;
//	qreal z2 = in ? 0:2;

	QGraphicsLineItem * g;
	g = new MyLine(x,y1-z1,x,y2-z1);
	g->setPen(pw);
	g->setZValue(9999);
	m_scene->addItem(g);
	m_viewportLines << g;
/*
	g = new QGraphicsLineItem(x,y1-z2,x,y2-z2);
	g->setPen(pw);
	g->setZValue(9999);
	m_scene->addItem(g);
	m_viewportLines << g;
	*/
}

void SlideEditorWindow::addVpLineY(qreal y, qreal x1, qreal x2, bool /*in*/)
{
	static QPen pw(Qt::gray, 0,Qt::DotLine);
	static QPen pb(Qt::black, 0,Qt::DotLine);

	qreal z1 = 0; //in ? 2:0;
	//qreal z2 = in ? 0:2;

	QGraphicsLineItem * g;
	g = new MyLine(x1-z1,y,x2-z1,y);
	g->setPen(pw);
	g->setZValue(9999);
	m_scene->addItem(g);
	m_viewportLines << g;
/*
	g = new QGraphicsLineItem(x1-z2,y,x2-z2,y);
	g->setPen(pw);
	g->setZValue(9999);
	m_scene->addItem(g);
	m_viewportLines << g;
	*/
}


void SlideEditorWindow::setupViewportLines()
{
	bool showThirds = true;

	QRectF r = m_scene->sceneRect();

	foreach(QGraphicsLineItem *l, m_viewportLines)
	{
		m_scene->removeItem(l);
		delete l;
		l=0;
	}
	
	m_viewportLines.clear();

	// top line
	addVpLineY(r.y(),r.x(),r.width(),false);

	// bottom line
	addVpLineY(r.height(),r.x(),r.width());

	// left line
	addVpLineX(r.x(), r.y(), r.height(),false);

	// right line
	addVpLineX(r.width(), r.y(), r.height());

	if(showThirds)
	{
		// third lines
		qreal x3 = r.width() / 3;
		qreal y3 = r.height() / 3;

		qreal x1 = x3;
		qreal x2 = x3+x3;

		qreal y1 = y3;
		qreal y2 = y3+y3;

		// draw top horizontal third
		addVpLineY(y1,r.x(),r.width());

		// bottom horiz. third
		addVpLineY(y2,r.x(),r.width());

		// left vert third
		addVpLineX(x1,r.y(),r.height());

		// right vert third
		addVpLineX(x2,r.y(), r.height());
	}
}

void SlideEditorWindow::slidesDropped(QList<Slide*> list)
{
	QModelIndex idx = m_slideModel->indexForSlide(list.first());
	m_slideListView->setCurrentIndex(idx);
}

void SlideEditorWindow::setSlideGroup(SlideGroup *g,Slide *curSlide)
{
	if(m_slideGroup)
		disconnect(m_slideGroup,0,this,0);
	
// 	if(g != m_slideGroup)
// 	{
// 		m_slideModel->releaseSlideGroup();
// 		
		//bool newFlag = m_slideModel ? false :true;
		
		m_slideGroup = g;
		m_slideModel->setSlideGroup(g);
		
		//if(newFlag)
		//	m_slideListView->reset();
		
		
	//}
	
	// Trigger slideItemChange slot connections
	m_ignoreUndoPropChanges = true;
	QList<Slide*> slist = g->slideList();
	foreach(Slide *slide, slist)
		slideChanged(slide, "add", 0, "", "", QVariant());
	m_ignoreUndoPropChanges = false;
	
	connect(g,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));
	connect(g,SIGNAL(destroyed(QObject*)), this, SLOT(releaseSlideGroup()));
	
	setWindowTitle(QString("%1 - Slide Editor").arg(g->groupTitle().isEmpty() ? QString("Group %1").arg(g->groupNumber()) : g->groupTitle()));
	//m_slideListView->setModel(m_slideModel);
	
	if(curSlide)
	{
		m_scene->setSlide(curSlide);
		m_slideListView->setCurrentIndex(m_slideModel->indexForSlide(curSlide));
		setupViewportLines();
	}
	else
	{
		QList<Slide*> slist = g->slideList();
		if(slist.size() > 0)
		{
			Slide *s = m_slideModel->slideAt(0);
			m_scene->setSlide(s);
			m_slideListView->setCurrentIndex(m_slideModel->indexForRow(0));
			setupViewportLines();
		}
		else
		{
			qDebug("SlideEditorWindow::setSlideGroup: Group[0] has 0 slides");
		}
	}
		

}

QString guessTitle(QString field)
{
	static QRegExp rUpperCase = QRegExp("([a-z])([A-Z])");
	static QRegExp rFirstLetter = QRegExp("([a-z])");
	static QRegExp rLetterNumber = QRegExp("([a-z])([0-9])");
	//static QRegExp rUnderScore 
	//$name =~ s/([a-z])_([a-z])/$1.' '.uc($2)/segi;
	
	QString tmp = field;
	tmp.replace(rUpperCase,"\\1 \\2");
	if(tmp.indexOf(rFirstLetter) == 0)
	{
		QChar x = tmp.at(0);
		tmp.remove(0,1);
		tmp.prepend(QString(x).toUpper());
	}
	
	tmp.replace(rLetterNumber,"\\1 #\\2");
	//$name =~ s/^([a-z])/uc($1)/seg;
	
// 	$name =~ s/\/([a-z])/'\/'.uc($1)/seg;
// 	$name =~ s/\s([a-z])/' '.uc($1)/seg;
// 	$name =~ s/\s(of|the|and|a)\s/' '.lc($1).' '/segi;
// 	$name .= '?' if $name =~ /^is/i;
// 	$name =~ s/id$//gi;
// 	my $chr = '#';
// 	$name =~ s/num$/$chr/gi; 
// 	$name =~ s/datetime$/Date\/Time/gi;
// 	$name =~ s/\best\b/Est./gi;

	return tmp;
	

}

 class UndoSlideItemChanged : public QUndoCommand
 {
 public:
	UndoSlideItemChanged(SlideEditorWindow *window, AbstractItem *item, QString field, QVariant value, QVariant oldValue)
		: m_window(window), m_item(item), m_field(field), m_value(value), m_oldValue(oldValue), redoCount(0) 
		{ 
			setText(QString("Change %2 of %1").arg(guessTitle(item->itemName())).arg(guessTitle(field)));
		}
	
	virtual int id() const { return 0x001; }
	virtual void undo() 
	{ 
		m_window->ignoreUndoChanged(true);
		m_item->setProperty(m_field.toLocal8Bit().constData(),m_oldValue); 
		m_window->ignoreUndoChanged(false);
	}
	virtual void redo() 
	{ 
		if(redoCount++ > 0)
		{
			//qDebug() << "UndoSlideItemChanged::redo: REDO cmd for "<<m_item->itemName()<<", field:"<<m_field<<", oldValue:"<<m_oldValue<<", newValue:"<<m_value;
			m_window->ignoreUndoChanged(true);
			m_item->setProperty(m_field.toLocal8Bit().constData(),m_value); 
			m_window->ignoreUndoChanged(false);
		}
	}
	virtual bool mergeWidth(const QUndoCommand * other)
	{
		if(other->id() != id())
			return false;
		const UndoSlideItemChanged * cmd = static_cast<const UndoSlideItemChanged*>(other);
		if(cmd->m_field    == m_field &&
		   cmd->m_value    == m_value &&
		   cmd->m_oldValue == m_oldValue)
			return true;
			
		return false;
	}
private:
	SlideEditorWindow *m_window;
	AbstractItem *m_item;
	QString m_field;
	QVariant m_value;
	QVariant m_oldValue;
	int redoCount;
};
 
class UndoSlideItemAdded : public QUndoCommand
{
public:
	UndoSlideItemAdded(SlideEditorWindow *window, Slide *slide, AbstractItem *item)
		: m_window(window), m_slide(slide), m_item(item), redoCount(0) 
		{ 
			setText(QString("Added %1").arg(guessTitle(item->itemName())));
		}
	
	virtual void undo() 
	{ 
		m_window->ignoreUndoChanged(true);
		m_slide->removeItem(m_item);
		m_window->ignoreUndoChanged(false);
	}
	virtual void redo() 
	{ 
		if(redoCount++ > 0)
		{
			//qDebug() << "UndoSlideItemChanged::redo: REDO cmd for "<<m_item->itemName()<<", field:"<<m_field<<", oldValue:"<<m_oldValue<<", newValue:"<<m_value;
			m_window->ignoreUndoChanged(true);
			m_slide->addItem(m_item);
			m_window->ignoreUndoChanged(false);
		}
	}
private:
	SlideEditorWindow *m_window;
	Slide *m_slide;
	AbstractItem *m_item;
	int redoCount;
};


 
 class UndoSlideItemRemoved : public QUndoCommand
 {
 public:
	UndoSlideItemRemoved(SlideEditorWindow *window, Slide *slide, AbstractItem *item)
		: m_window(window), m_slide(slide), m_item(item), redoCount(0) 
		{ 
			setText(QString("Removed %1").arg(guessTitle(item->itemName())));
		}
	
	virtual void undo() 
	{ 
		m_window->ignoreUndoChanged(true);
		m_slide->removeItem(m_item);
		m_window->ignoreUndoChanged(false);
	}
	virtual void redo() 
	{ 
		if(redoCount++ > 0)
		{
			//qDebug() << "UndoSlideItemChanged::redo: REDO cmd for "<<m_item->itemName()<<", field:"<<m_field<<", oldValue:"<<m_oldValue<<", newValue:"<<m_value;
			m_window->ignoreUndoChanged(true);
			m_slide->addItem(m_item);
			m_window->ignoreUndoChanged(false);
		}
	}
private:
	SlideEditorWindow *m_window;
	Slide *m_slide;
	AbstractItem *m_item;
	int redoCount;
};

class UndoSlideAdded : public QUndoCommand
{
public:
	UndoSlideAdded(SlideEditorWindow *window, Slide *slide)
		: m_window(window), m_slide(slide), redoCount(0) 
		{ 
			setText(QString("Added Slide# %1").arg(slide->slideNumber()));
		}
	
	virtual void undo() 
	{ 
		m_window->ignoreUndoChanged(true);
		m_window->slideGroup()->removeSlide(m_slide);
		m_window->ignoreUndoChanged(false);
	}
	virtual void redo() 
	{ 
		if(redoCount++ > 0)
		{
			//qDebug() << "UndoSlideItemChanged::redo: REDO cmd for "<<m_item->itemName()<<", field:"<<m_field<<", oldValue:"<<m_oldValue<<", newValue:"<<m_value;
			m_window->ignoreUndoChanged(true);
			m_window->slideGroup()->addSlide(m_slide);
			m_window->ignoreUndoChanged(false);
		}
	}
private:
	SlideEditorWindow *m_window;
	Slide *m_slide;
	int redoCount;
};

class UndoSlideRemoved : public QUndoCommand
{
public:
	UndoSlideRemoved(SlideEditorWindow *window, Slide *slide)
		: m_window(window), m_slide(slide), redoCount(0) 
		{ 
			setText(QString("Removed Slide# %1").arg(slide->slideNumber()));
		}
	
	virtual void undo() 
	{ 
		m_window->ignoreUndoChanged(true);
		m_window->slideGroup()->addSlide(m_slide);
		m_window->ignoreUndoChanged(false);
	}
	virtual void redo() 
	{ 
		if(redoCount++ > 0)
		{
			//qDebug() << "UndoSlideItemChanged::redo: REDO cmd for "<<m_item->itemName()<<", field:"<<m_field<<", oldValue:"<<m_oldValue<<", newValue:"<<m_value;
			m_window->ignoreUndoChanged(true);
			m_window->slideGroup()->removeSlide(m_slide);
			m_window->ignoreUndoChanged(false);
		}
	}
private:
	SlideEditorWindow *m_window;
	Slide *m_slide;
	int redoCount;
};


void SlideEditorWindow::ignoreUndoChanged(bool flag)
{
	m_ignoreUndoPropChanges = flag;
}


void SlideEditorWindow::slideChanged(Slide *slide, QString slideOperation, AbstractItem */*item*/, QString /*itemOperation*/, QString /*fieldName*/, QVariant /*value*/)
{
	if(slideOperation == "remove")
	{
		//qDebug()<<"SlideEditorWindow::slideChanged: (remove), disconnecting from slide#"<<slide->slideNumber();
		disconnect(slide,0,this,0);
		if(!m_ignoreUndoPropChanges)
		{
			m_undoStack->push(new UndoSlideRemoved(this,m_scene->slide()));
		}
	}
	else
	if(slideOperation == "add")
	{
		//qDebug()<<"SlideEditorWindow::slideChanged: (add), connecting to slide#"<<slide->slideNumber();
		// so we dont duplicate events in case we vet this signal twice
		disconnect(slide,0,this,0);
		connect(slide,SIGNAL(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)),this,SLOT(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)));
		
		if(!m_ignoreUndoPropChanges)
		{
			m_undoStack->push(new UndoSlideAdded(this,m_scene->slide()));
		}
	}
	else
	if(slideOperation == "change")
	{
		// "change" would be an add/remove/change of an AbstractItem to the slide itself.
		// This will be procssed in slideItemChanged().
	}
}


void SlideEditorWindow::slideItemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value, QVariant oldValue)
{
// 	Slide * slide = dynamic_cast<Slide *>(sender());
	
	if(operation == "add")
	{
		if(!m_ignoreUndoPropChanges)
		{
			m_undoStack->push(new UndoSlideItemAdded(this,m_scene->slide(),item));
		}
	}
	else
	if(operation == "remove")
	{
		if(!m_ignoreUndoPropChanges)
		{
			m_undoStack->push(new UndoSlideItemRemoved(this,m_scene->slide(),item));
		}
	}
	else
	if(operation == "change")
	{
		if(item)
		{
			if(!m_ignoreUndoPropChanges)
			{
				if(value != oldValue)
				{
					QUndoCommand * changeCmd = new UndoSlideItemChanged(this,item,fieldName,value,oldValue);
					m_undoStack->push(changeCmd);
					
					//qDebug() << "SlideEditorWindow::slideItemChanged: New Cmd for "<<item->itemName()<<", field:"<<fieldName<<", oldValue:"<<oldValue<<", newValue:"<<value;
				}
				else
				{
					//qDebug() << "SlideEditorWindow::slideItemChanged: IGNORING Cmd for "<<item->itemName()<<", field:"<<fieldName<<", oldValue:"<<oldValue<<", newValue:"<<value;
				}
			}
			else
			{
				//qDebug() << "SlideEditorWindow::slideItemChanged: Ignoring prop change on "<<item->itemName();
			}
		}
		else
		{
			//qDebug() << "SlideEditorWindow::slideItemChanged: Item was null, no undo data recorded.";
		}
		
	}
	

}



void SlideEditorWindow::releaseSlideGroup()
{
	if(!m_slideGroup)
		return;
		
	disconnect(m_slideGroup,0,this,0);
	m_slideGroup = 0;
	
}


void SlideEditorWindow::slideSelected(const QModelIndex &idx)
{
	Slide *s = m_slideModel->slideFromIndex(idx);
	qDebug() << "SlideEditorWindow::slideSelected(): selected slide#:"<<s->slideNumber();
	m_scene->setSlide(s);
	setupViewportLines();
}

void SlideEditorWindow::newSlide()
{
	Slide * slide = new Slide();
	slide->setSlideNumber(m_slideGroup->numSlides());
	slide->setSlideId(m_slideGroup->numSlides());
	
	//qDebug() << "newSlide: ADDING "<<slide->slideNumber();
	m_slideGroup->addSlide(slide);
	qDebug() << "newSlide: Added slide#"<<slide->slideNumber();
	
	//m_scene->setSlide(slide);
	
	setSlideGroup(m_slideGroup,slide);
}

void SlideEditorWindow::delSlide()
{
	Slide * slide = m_scene->slide();
	qDebug() << "delSlide: Removing slide#"<<slide->slideNumber();
	
	QModelIndex idx = m_slideModel->indexForSlide(slide);
	QModelIndex prev = m_slideModel->indexForRow(idx.row() - 1 > 0 ? idx.row() - 1 : 0);
	
	m_slideGroup->removeSlide(slide);
	
	
	Slide *newSlide = 0;
	if(prev.isValid())
	{
		m_slideListView->setCurrentIndex(prev);
		newSlide = m_slideModel->slideAt(prev.row());
	}
	
	if(newSlide)
	{
		m_scene->setSlide(newSlide);
		setupViewportLines();
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
