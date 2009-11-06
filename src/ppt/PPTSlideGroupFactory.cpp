#include "PPTSlideGroupFactory.h"

#include "PPTSlideGroup.h"

//#include "OutputInstance.h"
//#include "model/Output.h"
//#include "model/TextBoxItem.h"

#include "NativeViewerWin32PPT.h"


#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

#include <assert.h>

/** PPTSlideGroupFactory:: **/

PPTSlideGroupFactory::PPTSlideGroupFactory() : SlideGroupFactory() {}
/*PPTSlideGroupFactory::~PPTSlideGroupFactory()
{
}*/
	
SlideGroup * PPTSlideGroupFactory::newSlideGroup()
{
	return dynamic_cast<SlideGroup*>(new PPTSlideGroup());
}

AbstractSlideGroupEditor * PPTSlideGroupFactory::newEditor()
{
	// In futre: Call out to the system to launch powerpnt with the file as argument?
	return 0;
}

NativeViewer * PPTSlideGroupFactory::newNativeViewer()
{
	return new NativeViewerWin32PPT();
}
