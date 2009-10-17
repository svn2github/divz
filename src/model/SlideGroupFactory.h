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
#include <QMainWindow>

class SlideGroupViewMutator
{
public:
	SlideGroupViewMutator();
	virtual ~SlideGroupViewMutator();
	virtual QList<AbstractItem *> itemList(Output*, SlideGroup*, Slide*);
};

class QListView;
class SlideGroupViewControl;

class SlideGroupViewControlListView : public QListView
{ 
public:
	SlideGroupViewControlListView(SlideGroupViewControl * ctrl);
protected:
	void keyPressEvent(QKeyEvent *event);	
	SlideGroupViewControl *ctrl;
};

#include <QTime>
class QLabel;
class QPushButton;
class SlideGroupViewControl : public QWidget
{
	Q_OBJECT
public:
	SlideGroupViewControl(SlideGroupViewer * view=0, QWidget * parent = 0);
	
	SlideGroupViewer * view() { return m_slideViewer; }
	virtual void setOutputView(SlideGroupViewer *);
	
	typedef enum TimerState { Undefined, Running, Stopped };
public slots:
	virtual void setSlideGroup(SlideGroup *g, Slide *curSlide=0);
	virtual void releaseSlideGroup();
	virtual void nextSlide();
	virtual void prevSlide();
	virtual void setCurrentSlide(int);
	virtual void setCurrentSlide(Slide*);
	virtual void toggleTimerState(TimerState state = Undefined, bool resetTimer = false);
	
protected slots:
	virtual void slideSelected(const QModelIndex &);
	virtual void currentChanged(const QModelIndex &, const QModelIndex &);
	
	virtual void updateTimeLabel();
	virtual void enableAnimation(double time = 0);
	
	
protected:
	QString formatTime(double);

	SlideGroupViewer *m_slideViewer;
	SlideGroupListModel *m_slideModel;	
	SlideGroupViewControlListView *m_listView;
	friend class SlideGroupViewControlListView;
	bool m_releasingSlideGroup;
	QTimer * m_changeTimer;
	QTimer * m_countTimer;
	QTime m_elapsedTime;
	QLabel * m_timeLabel;
	QPushButton * m_timeButton;
	TimerState m_timerState;
	double m_currentTimeLength;
	double m_elapsedAtPause;

};

class AbstractSlideGroupEditor : public QMainWindow
{
	Q_OBJECT
public:
	AbstractSlideGroupEditor(SlideGroup *g=0, QWidget * parent = 0);
	virtual ~AbstractSlideGroupEditor();
	
	virtual void setSlideGroup(SlideGroup*g,Slide *curSlide=0);

signals:
	void closed();
	
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
	virtual AbstractSlideGroupEditor   * newEditor();
	
	virtual QPixmap generatePreviewPixmap(SlideGroup*, QSize iconSize, QRect sceneRect);

protected:
	// for use in generating preview pixmaps
	MyGraphicsScene * m_scene;
};


#endif
