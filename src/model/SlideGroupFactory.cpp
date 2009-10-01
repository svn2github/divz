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

SlideGroupFactory::SlideGroupFactory() {}
	
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

