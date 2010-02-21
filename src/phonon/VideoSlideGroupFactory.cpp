#include "VideoSlideGroupFactory.h"

#include "VideoSlideGroup.h"

//#include "OutputInstance.h"
//#include "model/Output.h"
//#include "model/TextBoxItem.h"

#include "NativeViewerPhonon.h"

/*
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>*/

#include <assert.h>

/** VideoSlideGroupFactory:: **/

VideoSlideGroupFactory::VideoSlideGroupFactory() : SlideGroupFactory() {}
/*VideoSlideGroupFactory::~VideoSlideGroupFactory()
{
}*/
	
SlideGroup * VideoSlideGroupFactory::newSlideGroup()
{
	return dynamic_cast<SlideGroup*>(new VideoSlideGroup());
}

AbstractSlideGroupEditor * VideoSlideGroupFactory::newEditor()
{
	// In futre: Call out to the system to launch native video editor with the file as argument?
	return 0;
}

NativeViewer * VideoSlideGroupFactory::newNativeViewer()
{
	return new NativeViewerPhonon();
}
