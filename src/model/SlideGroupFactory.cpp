#include "model/SlideGroupFactory.h"

#include "SlideEditorWindow.h"

/** SlideGroupViewMutator **/

SlideGroupViewMutator::SlideGroupViewMutator() {}
SlideGroupViewMutator::~SlideGroupViewMutator() {}
QList<AbstractItem *> SlideGroupViewMutator::itemList(Output*, SlideGroup*, Slide*s)
{
	return s->itemList();
}

/** SlideGroupViewControl **/
SlideGroupViewControl::SlideGroupViewControl(OutputView *, QWidget * )
{

}
	
void SlideGroupViewControl::setOutputView(OutputView *v) { m_view = v; }
	
void SlideGroupViewControl::setSlideGroup(SlideGroup */*g*/, Slide */*curSlide*/)
{

}
	
void SlideGroupViewControl::nextSlide()
{

}
void SlideGroupViewControl::prevSlide()
{

}

void SlideGroupViewControl::setCurrentSlide(int)
{

}

void SlideGroupViewControl::setCurrentSlide(Slide*)
{

}
	
/** SlideGroupEditor **/
SlideGroupEditor::SlideGroupEditor(SlideGroup */*g*/, QWidget */*parent*/) {}
SlideGroupEditor::~SlideGroupEditor() {}
void SlideGroupEditor::setSlideGroup(SlideGroup */*g*/,Slide */*curSlide*/) {}


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

SlideGroupEditor * SlideGroupFactory::newEditor()
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
