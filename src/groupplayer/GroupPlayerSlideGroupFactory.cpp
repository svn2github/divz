#include "GroupPlayerSlideGroupFactory.h"

#include "GroupPlayerSlideGroup.h"
#include "GroupPlayerEditorWindow.h"

GroupPlayerSlideGroupFactory::GroupPlayerSlideGroupFactory() {}
	
SlideGroup * GroupPlayerSlideGroupFactory::newSlideGroup() 
{
	return new GroupPlayerSlideGroup(); 
}
AbstractSlideGroupEditor * GroupPlayerSlideGroupFactory::newEditor()
{
	return new GroupPlayerEditorWindow(0);
}
