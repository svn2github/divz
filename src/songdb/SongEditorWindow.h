#ifndef SONGEDITORWINDOW_H
#define SONGEDITORWINDOW_H

#include "SongSlideGroupFactory.h"

class SongEditorWindow : public AbstractSlideGroupEditor 
{
	Q_OBJECT
public:
	SongEditorWindow(SlideGroup *g=0, QWidget * parent = 0);
	
public slots:
	void setSlideGroup(SlideGroup*g,Slide *curSlide=0);
};


#endif // SONGEDITORWINDOW_H
