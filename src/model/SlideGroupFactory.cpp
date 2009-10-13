#include "model/SlideGroupFactory.h"

#include "SlideEditorWindow.h"

#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <assert.h>

/** SlideGroupViewMutator:: **/

SlideGroupViewMutator::SlideGroupViewMutator() {}
SlideGroupViewMutator::~SlideGroupViewMutator() {}
QList<AbstractItem *> SlideGroupViewMutator::itemList(Output*, SlideGroup*, Slide*s)
{
	return s->itemList();
}


/** SlideGroupViewControlListView:: **/
/* We reimplement QListView's keyPressEvent to detect
  selection changes on key press events in QListView::ListMode.
  Aparently, in list mode the selection model's currentChanged()
  signal doesn't get fired on keypress, but in IconMode it does.
  We use IconMode by default in the ViewControl below, but the
  SongSlideGroupViewControl uses ListMode - this allows either
  icon or list mode to change slides just by pressing up or down
*/ 
SlideGroupViewControlListView::SlideGroupViewControlListView(SlideGroupViewControl * ctrl) : QListView(ctrl), ctrl(ctrl) {}
void SlideGroupViewControlListView::keyPressEvent(QKeyEvent *event)
{
	QModelIndex oldIdx = currentIndex();
	QListView::keyPressEvent(event);
	QModelIndex newIdx = currentIndex();
	if(oldIdx.row() != newIdx.row())
	{
		ctrl->slideSelected(newIdx);
	}
}

/** SlideGroupViewControl:: **/
#define DEBUG_SLIDEGROUPVIEWCONTROL 0
SlideGroupViewControl::SlideGroupViewControl(SlideGroupViewer *g, QWidget *w )
	: QWidget(w),
	m_slideViewer(0),
	m_slideModel(0),
	m_releasingSlideGroup(false)
{
	QVBoxLayout * layout = new QVBoxLayout();
	
	/** Setup the list view in icon mode */
	//m_listView = new QListView(this);
	m_listView = new SlideGroupViewControlListView(this);
	m_listView->setViewMode(QListView::IconMode);
	m_listView->setMovement(QListView::Static);
	m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
	
	connect(m_listView,SIGNAL(activated(const QModelIndex &)),this,SLOT(slideSelected(const QModelIndex &)));
	connect(m_listView,SIGNAL(clicked(const QModelIndex &)),  this,SLOT(slideSelected(const QModelIndex &)));
	connect(m_listView,SIGNAL(entered(const QModelIndex &)),  this,SLOT(slideSelected(const QModelIndex &)));
	
	// deleting old selection model per http://doc.trolltech.com/4.5/qabstractitemview.html#setModel
	QItemSelectionModel *m = m_listView->selectionModel();
//	if(m)
// 		disconnect(m,0,this,0);
	
	m_slideModel = new SlideGroupListModel();
	m_listView->setModel(m_slideModel);
	
	if(m)
	{
		delete m;
		m=0;
	}
	
	QItemSelectionModel *currentSelectionModel = m_listView->selectionModel();
	connect(currentSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(currentChanged(const QModelIndex &, const QModelIndex &)));
	
	layout->addWidget(m_listView);
	
	/** Setup the button controls at the bottom */
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
	
	
void SlideGroupViewControl::currentChanged(const QModelIndex &idx,const QModelIndex &)
{
	slideSelected(idx);
}

void SlideGroupViewControl::slideSelected(const QModelIndex &idx)
{
	if(m_releasingSlideGroup)
		return;
	Slide *s = m_slideModel->slideFromIndex(idx);
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug() << "SlideGroupViewControl::slideSelected(): selected slide#:"<<s->slideNumber();
	m_slideViewer->setSlide(s);
}

void SlideGroupViewControl::setOutputView(SlideGroupViewer *v) 
{ 
	SlideGroup *g = 0;
	if(m_slideViewer) 
		g = m_slideViewer->slideGroup();
	
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
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
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug()<<"SlideGroupViewControl::setSlideGroup: Loading group#"<<g->groupNumber();
	m_slideModel->setSlideGroup(g);
	// reset seems to be required
	m_listView->reset();
	if(!curSlide)
		curSlide = g->at(0);
	m_listView->setCurrentIndex(m_slideModel->indexForSlide(curSlide));
}

void SlideGroupViewControl::releaseSlideGroup()
{
	m_releasingSlideGroup = true;
	m_slideModel->releaseSlideGroup();
	m_listView->reset();
	m_releasingSlideGroup = false;
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
	
/** AbstractSlideGroupEditor:: **/
AbstractSlideGroupEditor::AbstractSlideGroupEditor(SlideGroup */*g*/, QWidget *parent) : QWidget(parent) {}
AbstractSlideGroupEditor::~AbstractSlideGroupEditor() {}
void AbstractSlideGroupEditor::setSlideGroup(SlideGroup */*g*/,Slide */*curSlide*/) {}


/** SlideGroupFactory:: **/
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
	//return QPixmap();
	
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
	//qDebug() << "SlideGroupFactory::generatePreviewPixmap(): checking scene rect: "<<sceneRect;
	if(m_scene->sceneRect() != sceneRect)
	{
		//qDebug() << "SlideGroupFactory::generatePreviewPixmap(): setting new scene rect: "<<sceneRect;
		m_scene->setSceneRect(sceneRect);
	}
	
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
