#ifndef SONGSLIDEGROUPFACTORY_H
#define SONGSLIDEGROUPFACTORY_H

#include "model/SlideGroupFactory.h"

class SongSlideGroupViewMutator : public SlideGroupViewMutator
{
public:
	SongSlideGroupViewMutator();
	~SongSlideGroupViewMutator();
	virtual QList<AbstractItem *> itemList(Output*, SlideGroup*, Slide*);
};

class SongSlideGroupViewControl : public SlideGroupViewControl
{
	Q_OBJECT
public:
	SongSlideGroupViewControl(SlideGroupViewer * view=0, QWidget * parent = 0);
	
	//SlideGroupViewer * view() { return m_slideViewer; }
	//virtual void setOutputView(SlideGroupViewer *);
	
// public slots:
// 	virtual void setSlideGroup(SlideGroup *g, Slide *curSlide=0);
// 	virtual void nextSlide();
// 	virtual void prevSlide();
// 	virtual void setCurrentSlide(int);
// 	virtual void setCurrentSlide(Slide*);
// 	
};

// class SongSlideGroupEditor : public AbstractSlideGroupEditor 
// {
// 	Q_OBJECT
// public:
// 	SongSlideGroupEditor(SlideGroup *g=0, QWidget * parent = 0);
// public slots:
// 	virtual void setSlideGroup(SlideGroup*g,Slide *curSlide=0);
// };


class SongSlideGroupFactory : public SlideGroupFactory 
{
public:
	SongSlideGroupFactory();
	
	SlideGroup * newSlideGroup();
	
	SlideGroupViewMutator * newViewMutator();
	SlideGroupViewControl * newViewControl();
	AbstractSlideGroupEditor   * newEditor();
	
// 	QPixmap generatePreviewPixmap(SlideGroup*, QSize iconSize, QRect sceneRect);
};


#endif
