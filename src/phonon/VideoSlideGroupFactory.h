#ifndef VideoSlideGroupFactory_H
#define VideoSlideGroupFactory_H

#include "model/SlideGroupFactory.h"

class VideoSlideGroupFactory : public SlideGroupFactory
{
public:
	VideoSlideGroupFactory();
	
	SlideGroup * newSlideGroup();
	
	//AbstractItemFilterList customFiltersFor(OutputInstance *instace = 0);
	//SlideGroupViewControl * newViewControl();
	AbstractSlideGroupEditor   * newEditor();
	NativeViewer * newNativeViewer();
	
// 	QPixmap generatePreviewPixmap(SlideGroup*, QSize iconSize, QRect sceneRect);
};


#endif
