#include "SongSlideGroupFactory.h"

#include "SongEditorWindow.h"
#include "SongSlideGroup.h"
#include "SongSlideGroupListModel.h"

#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <assert.h>

/** SlideGroupViewMutator:: **/

SongSlideGroupViewMutator::SongSlideGroupViewMutator() {}
SongSlideGroupViewMutator::~SongSlideGroupViewMutator() {}
QList<AbstractItem *> SongSlideGroupViewMutator::itemList(Output*, SlideGroup*, Slide*s)
{
	return s->itemList();
}


/** SlideGroupViewControl:: **/
// class SlideGroupViewControlListView : public QListView
// { 
// public:
// 	SlideGroupViewControlListView(SlideGroupViewControl * ctrl, QWidget *parent) : QListView(parent), ctrl(ctrl) {}
// protected:
// 	void keyPressEvent(QKeyEvent *event)
// 	{
// 		QModelIndex oldIdx = currentIndex();
// 		QListView::keyPressEvent(event);
// 		QModelIndex newIdx = currentIndex();
// 		if(oldIdx.row() != newIdx.row())
// 		{
// 			ctrl->slideSelected(newIdx);
// 		}
// 	}
// 	
// 	SlideGroupViewControl *ctrl;
// };

#define DEBUG_SLIDEGROUPVIEWCONTROL 0
SongSlideGroupViewControl::SongSlideGroupViewControl(SlideGroupViewer *g, QWidget *w )
	: SlideGroupViewControl(g,w)
{
	if(m_slideModel)
		delete m_slideModel;
		
	//qDebug()<<"SongSlideGroupViewControl(): creating new SongSlideGroupListModel()";
	m_slideModel = new SongSlideGroupListModel();
	m_listView->setModel(m_slideModel);
	m_listView->setViewMode(QListView::ListMode);
	m_listView->reset();	
	
}

/*
	
void SlideGroupViewControl::currentChanged(const QModelIndex &idx,const QModelIndex &)
{
	slideSelected(idx);
}

void SlideGroupViewControl::slideSelected(const QModelIndex &idx)
{
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
	*/
/** AbstractSlideGroupEditor:: **/
// SongSlideGroupEditor::SongSlideGroupEditor(SlideGroup *g, QWidget *parent) : AbstractSlideGroupEditor(g,parent) {}
// SongSlideGroupEditor::~SongSlideGroupEditor() {}
// void SongSlideGroupEditor::setSlideGroup(SlideGroup */*g*/,Slide */*curSlide*/) {}


/** SlideGroupFactory:: **/

/** Class Members **/

SongSlideGroupFactory::SongSlideGroupFactory() : SlideGroupFactory() {}
/*SongSlideGroupFactory::~SongSlideGroupFactory()
{
}*/
	
SlideGroup * SongSlideGroupFactory::newSlideGroup()
{
	return dynamic_cast<SlideGroup*>(new SongSlideGroup());
}
	
SlideGroupViewMutator * SongSlideGroupFactory::newViewMutator()
{
	return new SongSlideGroupViewMutator();
}

SlideGroupViewControl * SongSlideGroupFactory::newViewControl()
{
	return new SongSlideGroupViewControl();
}

AbstractSlideGroupEditor * SongSlideGroupFactory::newEditor()
{
	return new SongEditorWindow();
}
/*
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
}*/

