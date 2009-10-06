#ifndef SLIDEGROUPFACTORY_H
#define SLIDEGROUPFACTORY_H

#include "AbstractItem.h"
#include "SlideGroup.h"
#include "Output.h"
#include <QList>
#include <QMap>
#include <QWidget>

//class OutputView;
#include "SlideGroupViewer.h"
#include "SlideGroupListModel.h"
#include <QListView>

class SlideGroupViewMutator
{
public:
	SlideGroupViewMutator();
	virtual ~SlideGroupViewMutator();
	virtual QList<AbstractItem *> itemList(Output*, SlideGroup*, Slide*);
};

class QListView;
class SlideGroupViewControl : public QWidget
{
	Q_OBJECT
public:
	SlideGroupViewControl(SlideGroupViewer * view=0, QWidget * parent = 0);
	
	SlideGroupViewer * view() { return m_slideViewer; }
	virtual void setOutputView(SlideGroupViewer *);
	
public slots:
	virtual void setSlideGroup(SlideGroup *g, Slide *curSlide=0);
	virtual void nextSlide();
	virtual void prevSlide();
	virtual void setCurrentSlide(int);
	virtual void setCurrentSlide(Slide*);
	
private slots:
	virtual void slideSelected(const QModelIndex &);
	
private:
	SlideGroupViewer *m_slideViewer;
	SlideGroupListModel *m_slideModel;	
	QListView *m_listView;
};

class AbstractSlideGroupEditor : public QWidget
{
public:
	AbstractSlideGroupEditor(SlideGroup *g=0, QWidget * parent = 0);
	virtual ~AbstractSlideGroupEditor();
	
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
	virtual AbstractSlideGroupEditor * newEditor();
	
	virtual QPixmap			generatePreviewPixmap(SlideGroup*, QSize iconSize, QRect sceneRect);

protected:
	// for use in generating preview pixmaps
	MyGraphicsScene * m_scene;
};


#endif
