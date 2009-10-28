#ifndef SONGSLIDEGROUPFACTORY_H
#define SONGSLIDEGROUPFACTORY_H

#include "model/SlideGroupFactory.h"
#include "itemlistfilters/SlideTextOnlyFilter.h"

class SongFoldbackTextFilter : public SlideTextOnlyFilter
{
	Q_OBJECT
protected:
	SongFoldbackTextFilter();
	static SongFoldbackTextFilter * m_staticInstance;
	
public:
	static SongFoldbackTextFilter * instance() { return m_staticInstance; }
	
	QString filterName() const { return "Song Foldback Text"; }
	QString filterDescription() const { return "Adds in guitar chords and other rear-screen text, as well as the next passage preview on the last line."; }
	
	bool approve(AbstractItem *item);
	AbstractItem * mutate(const AbstractItem*);
	bool isMandatoryFor(OutputInstance * instance = 0);
};


class SongSlideGroupViewControl : public SlideGroupViewControl
{
	Q_OBJECT
public:
	SongSlideGroupViewControl(OutputInstance * view=0, QWidget * parent = 0);
	
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
	
	AbstractItemFilterList customFiltersFor(OutputInstance *instace = 0);
	SlideGroupViewControl * newViewControl();
	AbstractSlideGroupEditor   * newEditor();
	
// 	QPixmap generatePreviewPixmap(SlideGroup*, QSize iconSize, QRect sceneRect);
};


#endif
