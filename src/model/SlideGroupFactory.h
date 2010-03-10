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
#include <QPushButton>
#include <QPointer>

class QListView;
class SlideGroupViewControl;

class SlideGroupViewControlListView : public QListView
{ 
public:
	SlideGroupViewControlListView(SlideGroupViewControl * ctrl);
protected:
	void keyPressEvent(QKeyEvent *event);	
	QPointer<SlideGroupViewControl> ctrl;
};

#include <QTime>
class QLabel;
class OutputInstance;
class TextBoxItem;
class QSpinBox;
class SlideGroupViewControl : public QWidget
{
	Q_OBJECT
public:
	SlideGroupViewControl(OutputInstance * view=0, QWidget * parent = 0, bool initUI=true);
	~SlideGroupViewControl();
	
	virtual SlideGroup * slideGroup() { return m_group; }
	SlideGroupListModel * slideGroupListModel();
	
	OutputInstance * view() { return m_slideViewer; }
	virtual void setOutputView(OutputInstance *);
	
	Slide * selectedSlide();
	
	bool isQuickSlideToggled() { return m_showQuickSlideBtn->isChecked(); }
	
	typedef enum TimerState { Undefined, Running, Stopped, Paused };
	
	TimerState timerState() { return m_timerState; }
	
	int iconSize() { return m_iconSize; }
	
	// reimplement in sub-classes for those that don't use the icon size
	virtual bool canSetIconSize() { return true; }
	
signals:
	void slideDoubleClicked(Slide *);
	void slideSelected(Slide *);
	void iconSizeChanged(int);
	
public slots:
	virtual void setSlideGroup(SlideGroup *g, Slide *curSlide=0, bool allowProgressDialog=true);
	virtual void releaseSlideGroup();
	virtual void nextSlide();
	virtual void prevSlide();
	virtual void setCurrentSlide(int);
	virtual void setCurrentSlide(Slide*);
	virtual void toggleTimerState(TimerState state = Undefined, bool resetTimer = false);
	
// 	virtual void fadeBlackFrame(bool);
// 	virtual void fadeClearFrame(bool);
	
	virtual void setIsPreviewControl(bool);
	virtual void setEnabled(bool);
	
	virtual void setQuickSlideEnabled(bool);
	virtual void addQuickSlide();
	virtual void showQuickSlide(bool flag=true);
	virtual void setQuickSlideText(const QString& text = "");
	
	virtual void setIconSize(int);

protected slots:
	virtual void slideSelected(const QModelIndex &);
	virtual void currentChanged(const QModelIndex &, const QModelIndex &);
	virtual void slideDoubleClicked(const QModelIndex &);
	
	virtual void updateTimeLabel();
	virtual void enableAnimation(double time = 0, bool startPaused=false);
	
private slots:
	void repaintList();
	void slideChanged(Slide*);
	
protected:
	virtual void makeQuickSlide();
	void fitQuickSlideText();
	
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
// 	bool m_clearActive;
// 	bool m_blackActive;
	
	bool m_isPreviewControl;
	
	QWidget  * m_itemControlBase;
	QList<QPointer<QWidget> > m_controlWidgets;
	
	int m_iconSize;
	QSlider * m_iconSizeSlider;
	bool m_lockIconSizeSetter;
	QSpinBox * m_spinBox;
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
	static void registerFactoryForType(int type, SlideGroupFactory *);
	static void removeFactoryForType(int type/*, SlideGroupFactory **/);
	static SlideGroupFactory * factoryForType(int type);

private /*static*/:
	static QHash<int, SlideGroupFactory*> m_factoryMap;

public:
	SlideGroupFactory();
	virtual ~SlideGroupFactory();
	
	virtual SlideGroup * newSlideGroup();
	
	virtual AbstractItemFilterList customFiltersFor(OutputInstance *instace = 0, SlideGroup *group = 0);
	virtual SlideGroupViewControl * newViewControl();
	virtual AbstractSlideGroupEditor   * newEditor();
	virtual NativeViewer * newNativeViewer();
	
	virtual QPixmap generatePreviewPixmap(SlideGroup*, QSize iconSize, QRect sceneRect);

protected:
	// for use in generating preview pixmaps
	MyGraphicsScene * m_scene;
};


#endif
