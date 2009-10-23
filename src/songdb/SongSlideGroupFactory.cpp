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
SongSlideGroupViewControl::SongSlideGroupViewControl(OutputInstance *g, QWidget *w )
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



/** SongSlideGroupFactory:: **/

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

