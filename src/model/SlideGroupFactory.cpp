#include "model/SlideGroupFactory.h"

#include "SlideEditorWindow.h"

#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <assert.h>

/** SlideGroupViewMutator **/

SlideGroupViewMutator::SlideGroupViewMutator() {}
SlideGroupViewMutator::~SlideGroupViewMutator() {}
QList<AbstractItem *> SlideGroupViewMutator::itemList(Output*, SlideGroup*, Slide*s)
{
	return s->itemList();
}


/** SlideGroupViewControl **/
SlideGroupViewControl::SlideGroupViewControl(SlideGroupViewer *g, QWidget *w )
	: QWidget(w),
	m_slideViewer(0)
{
		
	QVBoxLayout * layout = new QVBoxLayout();
	
	m_listView = new QListView(this);
	m_listView->setMovement(QListView::Static);
	m_listView->setViewMode(QListView::IconMode);
	m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	
	connect(m_listView,SIGNAL(activated(const QModelIndex &)),this,SLOT(slideSelected(const QModelIndex &)));
	connect(m_listView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(slideSelected(const QModelIndex &)));
	
	// deleting old selection model per http://doc.trolltech.com/4.5/qabstractitemview.html#setModel
	QItemSelectionModel *m = m_listView->selectionModel();
	
	m_slideModel = new SlideGroupListModel();
	m_listView->setModel(m_slideModel);
	
	if(m)
	{
		delete m;
		m=0;
	}
	
	layout->addWidget(m_listView);
	
	QHBoxLayout *hbox = new QHBoxLayout();
	
	QPushButton *btn;
	
	btn = new QPushButton("<< Prev");
	connect(btn, SIGNAL(clicked()), this, SLOT(prevSlide()));
	hbox->addWidget(btn);
	
	btn = new QPushButton(">> Next");
	connect(btn, SIGNAL(clicked()), this, SLOT(nextSlide()));
	hbox->addWidget(btn);
	
	layout->addLayout(hbox);
	setLayout(layout);
	
	if(g)
		setOutputView(g);
	
}
	
void SlideGroupViewControl::slideSelected(const QModelIndex &idx)
{
	Slide *s = m_slideModel->slideFromIndex(idx);
	qDebug() << "SlideGroupViewControl::slideSelected(): selected slide#:"<<s->slideNumber();
	//m_scene->setSlide(s);
	//setupViewportLines();
	m_slideViewer->setSlide(s);
}

void SlideGroupViewControl::setOutputView(SlideGroupViewer *v) 
{ 
	SlideGroup *g = 0;
	if(m_slideViewer) 
		g = m_slideViewer->slideGroup();
	
	qDebug() << "SlideGroupViewControl::setOutputView()";
	m_slideViewer = v;
	
	if(g)
	{
		//m_slideModel->setSlideGroup(g);
		m_slideViewer->setSlideGroup(g);
	}
}
	
void SlideGroupViewControl::setSlideGroup(SlideGroup *g, Slide *curSlide)
{
	assert(g);
	qDebug()<<"SlideGroupViewControl::setSlideGroup: mark1";
	m_slideModel->setSlideGroup(g);
	m_listView->setModel(m_slideModel);
	qDebug()<<"SlideGroupViewControl::setSlideGroup: mark2";
	if(!m_slideViewer)
	{
		qWarning("SlideGroupViewControl::setSlideGroup: No slide viewer given to the control!");
	}
	else
	{
		//m_slideViewer->setSlideGroup(g); //,curSlide);
	}
	qDebug()<<"SlideGroupViewControl::setSlideGroup: mark3";
	//m_slideViewer->setCurrentIndex(m_slideModel->indexForSlide(curSlide));
}
	
void SlideGroupViewControl::nextSlide()
{
	Slide *s = m_slideViewer->nextSlide();
	m_listView->setCurrentIndex(m_slideModel->indexForSlide(s));
}

void SlideGroupViewControl::prevSlide()
{
	Slide *s = m_slideViewer->prevSlide();
	m_listView->setCurrentIndex(m_slideModel->indexForSlide(s));
}

void SlideGroupViewControl::setCurrentSlide(int x)
{
	Slide *s = m_slideViewer->setSlide(x);
	m_listView->setCurrentIndex(m_slideModel->indexForSlide(s));
}

void SlideGroupViewControl::setCurrentSlide(Slide *s)
{
	m_slideViewer->setSlide(s);
	m_listView->setCurrentIndex(m_slideModel->indexForSlide(s));
}
	
/** AbstractSlideGroupEditor **/
AbstractSlideGroupEditor::AbstractSlideGroupEditor(SlideGroup */*g*/, QWidget *parent) : QWidget(parent) {}
AbstractSlideGroupEditor::~AbstractSlideGroupEditor() {}
void AbstractSlideGroupEditor::setSlideGroup(SlideGroup */*g*/,Slide */*curSlide*/) {}


/** SlideGroupFactory **/
/** Static Members **/
QMap<SlideGroup::GroupType, SlideGroupFactory*> SlideGroupFactory::m_factoryMap;

void SlideGroupFactory::registerFactoryForType(SlideGroup::GroupType type, SlideGroupFactory *f)
{
	m_factoryMap[type] = f;
}
	
void SlideGroupFactory::removeFactoryForType(SlideGroup::GroupType type)
{
	m_factoryMap.remove(type);
}

SlideGroupFactory * SlideGroupFactory::factoryForType(SlideGroup::GroupType type)
{
	return m_factoryMap[type];
}

/** Class Members **/

SlideGroupFactory::SlideGroupFactory() : m_scene(0) {}
SlideGroupFactory::~SlideGroupFactory()
{
	if(m_scene)
	{
		delete m_scene;
		m_scene = 0;
	}
}
	
SlideGroup * SlideGroupFactory::newSlideGroup()
{
	return new SlideGroup();
}
	
SlideGroupViewMutator * SlideGroupFactory::newViewMutator()
{
	return new SlideGroupViewMutator();
}

SlideGroupViewControl * SlideGroupFactory::newViewControl()
{
	return new SlideGroupViewControl();
}

AbstractSlideGroupEditor * SlideGroupFactory::newEditor()
{
	return new SlideEditorWindow();
}

QPixmap	SlideGroupFactory::generatePreviewPixmap(SlideGroup *g, QSize iconSize, QRect sceneRect)
{
		
	Slide * slide = g->at(0);
	if(!slide)
	{
		qDebug("SlideGroupFactory::generatePreviewPixmap: No slide at 0");
		return QPixmap();
	}

	int icon_w = iconSize.width();
	int icon_h = iconSize.height();
	
	if(!m_scene)
		m_scene = new MyGraphicsScene(MyGraphicsScene::Preview);
	if(m_scene->sceneRect() != sceneRect)
		m_scene->setSceneRect(sceneRect);
	
	m_scene->setSlide(slide);
	
	QPixmap icon(icon_w,icon_h);
	QPainter painter(&icon);
	painter.fillRect(0,0,icon_w,icon_h,Qt::white);
	
	m_scene->render(&painter,QRectF(0,0,icon_w,icon_h),sceneRect);
	painter.setPen(Qt::black);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(0,0,icon_w-1,icon_h-1);
	
	// clear() so we can free memory, stop videos, etc
	m_scene->clear();
	
	return icon;
}
