#ifndef SLIDEGROUPFACTORY_H
#define SLIDEGROUPFACTORY_H

#include "AbstractItem.h"
#include "SlideGroup.h"
#include "Output.h"
#include <QList>
#include <QMap>
#include <QWidget>

class OutputView;

class SlideGroupViewMutator
{
public:
	SlideGroupViewMutator();
	virtual ~SlideGroupViewMutator();
	virtual QList<AbstractItem *> itemList(Output*, SlideGroup*, Slide*);
};

class SlideGroupViewControl : public QWidget
{
	Q_OBJECT
public:
	SlideGroupViewControl(OutputView * view=0, QWidget * parent = 0);
	
	OutputView * view() { return m_view; }
	virtual void setOutputView(OutputView *);
	
public slots:
	virtual void setSlideGroup(SlideGroup*g,Slide *curSlide=0);
	virtual void nextSlide();
	virtual void prevSlide();
	virtual void setCurrentSlide(int);
	virtual void setCurrentSlide(Slide*);
	
private:
	OutputView *m_view;	
};

class SlideGroupEditor
{
public:
	SlideGroupEditor(SlideGroup *g=0, QWidget * parent = 0);
	virtual ~SlideGroupEditor();
	
	virtual void setSlideGroup(SlideGroup*g,Slide *curSlide=0);
};

class MyGraphicsScene;
class SlideGroupFactory 
{

public /*static*/:
	static void registerFactoryForType(SlideGroup::GroupType type, SlideGroupFactory *);
	static void removeFactoryForType(SlideGroup::GroupType type/*, SlideGroupFactory **/);
	static SlideGroupFactory * factoryForType(SlideGroup::GroupType type);

private /*static*/:
	static QMap<SlideGroup::GroupType, SlideGroupFactory*> m_factoryMap;

public:
	SlideGroupFactory();
	virtual ~SlideGroupFactory();
	
	virtual SlideGroup * newSlideGroup();
	
	virtual SlideGroupViewMutator * newViewMutator();
	virtual SlideGroupViewControl * newViewControl();
	virtual SlideGroupEditor      * newEditor();
	
	virtual QPixmap			generatePreviewPixmap(SlideGroup*, QSize iconSize, QRect sceneRect);

protected:
	// for use in generating preview pixmaps
	MyGraphicsScene * m_scene;
};


#endif
