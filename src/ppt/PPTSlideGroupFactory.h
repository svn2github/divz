#ifndef PPTSLIDEGROUPFACTORY_H
#define PPTSLIDEGROUPFACTORY_H

#include "model/SlideGroupFactory.h"

class PPTSlideGroupFactory : public SlideGroupFactory
{
public:
	PPTSlideGroupFactory();
	
	SlideGroup * newSlideGroup();
	
	//AbstractItemFilterList customFiltersFor(OutputInstance *instace = 0);
	//SlideGroupViewControl * newViewControl();
	AbstractSlideGroupEditor   * newEditor();
	NativeViewer * newNativeViewer(OutputInstance *instance = 0);
	
// 	QPixmap generatePreviewPixmap(SlideGroup*, QSize iconSize, QRect sceneRect);
};


#endif
