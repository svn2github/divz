#ifndef SLIDEGROUPFACTORY_H
#define SLIDEGROUPFACTORY_H

class Output;
#include "model/SlideGroup.h"
class AbstractItem;

#include <QList>
#include <QMap>
#include <QWidget>

//class OutputView;
#include "SlideGroupViewer.h"
#include "SlideGroupListModel.h"
#include <QListView>
#include <QMainWindow>

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
class OutputInstance;
class TextBoxItem;
class SlideGroupViewControl : public QWidget
{
	Q_OBJECT
public:
	SlideGroupViewControl(OutputInstance * view=0, QWidget * parent = 0);
	~SlideGroupViewControl();
	
	SlideGroupListModel * slideGroupListModel();
	
	OutputInstance * view() { return m_slideViewer; }
	virtual void setOutputView(OutputInstance *);
	
	Slide * selectedSlide();
	
	typedef enum TimerState { Undefined, Running, Stopped };
	
signals:
	void slideDoubleClicked(Slide *);
	void slideSelected(Slide *);
	
public slots:
	virtual void setSlideGroup(SlideGroup *g, Slide *curSlide=0, bool allowProgressDialog=true);
	virtual void releaseSlideGroup();
	virtual void nextSlide();
	virtual void prevSlide();
	virtual void setCurrentSlide(int);
	virtual void setCurrentSlide(Slide*);
	virtual void toggleTimerState(TimerState state = Undefined, bool resetTimer = false);
	
	virtual void fadeBlackFrame(bool);
	virtual void fadeClearFrame(bool);
	
	virtual void setIsPreviewControl(bool);
	virtual void setEnabled(bool);
	
	virtual void setQuickSlideEnabled(bool);
	virtual void addQuickSlide();
	virtual void showQuickSlide(bool flag=true);
	
protected slots:
	virtual void slideSelected(const QModelIndex &);
	virtual void currentChanged(const QModelIndex &, const QModelIndex &);
	virtual void slideDoubleClicked(const QModelIndex &);
	
	virtual void updateTimeLabel();
	virtual void enableAnimation(double time = 0);
	
private slots:
	void repaintList();
	
protected:
	virtual void makeQuickSlide();
	void fitQuickSlideText();
	void setQuickSlideText();
	
	QString formatTime(double);

	SlideGroup * m_group;
	OutputInstance *m_slideViewer;
	SlideGroupListModel *m_slideModel;	
	SlideGroupViewControlListView *m_listView;
	friend class SlideGroupViewControlListView;
	bool m_releasingSlideGroup;
	QTimer * m_changeTimer;
	QTimer * m_countTimer;
	QTime m_elapsedTime;
	QLabel * m_timeLabel;
	QPushButton * m_timeButton;
	QPushButton * m_prevBtn;
	QPushButton * m_nextBtn;
	TimerState m_timerState;
	double m_currentTimeLength;
	double m_elapsedAtPause;
	
	QWidget * m_quickSlideBase;
	QLineEdit * m_quickSlideText;
	Slide * m_quickSlide;
	TextBoxItem * m_quickSlideTextBox;
	QPushButton * m_showQuickSlideBtn;
	bool m_originalQuickSlide;
	
// 	QPushButton * m_blackButton;
// 	QPushButton * m_clearButton;
	
	Slide * m_selectedSlide;
	
	bool m_timerWasActiveBeforeFade;
	bool m_clearActive;
	bool m_blackActive;
	
	bool m_isPreviewControl;
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
class NativeViewer;
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
	
	virtual AbstractItemFilterList customFiltersFor(OutputInstance *instace = 0);
	virtual SlideGroupViewControl * newViewControl();
	virtual AbstractSlideGroupEditor   * newEditor();
	virtual NativeViewer * newNativeViewer();
	
	virtual QPixmap generatePreviewPixmap(SlideGroup*, QSize iconSize, QRect sceneRect);

protected:
	// for use in generating preview pixmaps
	MyGraphicsScene * m_scene;
};


#endif
