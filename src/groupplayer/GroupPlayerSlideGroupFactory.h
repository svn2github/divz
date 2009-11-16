#ifndef GroupPlayerSlideGroupFactory_H
#define GroupPlayerSlideGroupFactory_H

#include "model/SlideGroupFactory.h"

class GroupPlayerSlideGroupFactory : public SlideGroupFactory 
{
public:
	GroupPlayerSlideGroupFactory();
	
	SlideGroup * newSlideGroup();
	AbstractSlideGroupEditor   * newEditor();
};


#endif
