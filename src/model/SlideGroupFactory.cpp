#include "model/SlideGroupFactory.h"

#include "SlideEditorWindow.h"

#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <QLabel>
#include <assert.h>

/** SlideGroupViewMutator:: **/

SlideGroupViewMutator::SlideGroupViewMutator() {}
SlideGroupViewMutator::~SlideGroupViewMutator() {}
QList<AbstractItem *> SlideGroupViewMutator::itemList(Output*, SlideGroup*, Slide*s)
{
	return s->itemList();
}


/** SlideGroupViewControlListView:: **/
/* We reimplement QListView's keyPressEvent to detect
  selection changes on key press events in QListView::ListMode.
  Aparently, in list mode the selection model's currentChanged()
  signal doesn't get fired on keypress, but in IconMode it does.
  We use IconMode by default in the ViewControl below, but the
  SongSlideGroupViewControl uses ListMode - this allows either
  icon or list mode to change slides just by pressing up or down
*/ 
SlideGroupViewControlListView::SlideGroupViewControlListView(SlideGroupViewControl * ctrl) : QListView(ctrl), ctrl(ctrl) {}
void SlideGroupViewControlListView::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Space)
	{
		ctrl->nextSlide();
	}
	else
	{
		QModelIndex oldIdx = currentIndex();
		QListView::keyPressEvent(event);
		QModelIndex newIdx = currentIndex();
		if(oldIdx.row() != newIdx.row())
		{
			ctrl->slideSelected(newIdx);
		}
	}
}

/** SlideGroupViewControl:: **/
#define DEBUG_SLIDEGROUPVIEWCONTROL 0
SlideGroupViewControl::SlideGroupViewControl(SlideGroupViewer *g, QWidget *w )
	: QWidget(w),
	m_slideViewer(0),
	m_slideModel(0),
	m_releasingSlideGroup(false),
	m_changeTimer(0),
	m_countTimer(0),
	m_timeLabel(0),
	m_timerState(Undefined),
	m_currentTimeLength(0),
	m_elapsedAtPause(0),
	m_selectedSlide(0),
	m_timerWasActiveBeforeFade(0)
{
	QVBoxLayout * layout = new QVBoxLayout();
	
	/** Setup top buttons */
	QHBoxLayout * hbox1 = new QHBoxLayout();
	hbox1->addStretch(1);
	
	m_blackButton = new QPushButton(QIcon(":/data/stock-media-stop.png"),"&Black");
	m_blackButton->setCheckable(true);
	m_blackButton->setEnabled(false); // enable on first slide thats set on us
	connect(m_blackButton, SIGNAL(toggled(bool)), this, SLOT(fadeBlackFrame(bool)));
	hbox1->addWidget(m_blackButton);
	
	m_clearButton = new QPushButton(QIcon(":/data/stock-media-eject.png"),"&Clear");
	m_clearButton->setCheckable(true);
	m_clearButton->setEnabled(false); // enable on first slide thats set on us
	connect(m_clearButton, SIGNAL(toggled(bool)), this, SLOT(fadeClearFrame(bool)));
	hbox1->addWidget(m_clearButton);
	
	layout->addLayout(hbox1);
	
	/** Setup the list view in icon mode */
	//m_listView = new QListView(this);
	m_listView = new SlideGroupViewControlListView(this);
	m_listView->setViewMode(QListView::IconMode);
	m_listView->setMovement(QListView::Static);
	m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
	setFocusProxy(m_listView);
	setFocusPolicy(Qt::StrongFocus);
	
	connect(m_listView,SIGNAL(activated(const QModelIndex &)),this,SLOT(slideSelected(const QModelIndex &)));
	connect(m_listView,SIGNAL(clicked(const QModelIndex &)),  this,SLOT(slideSelected(const QModelIndex &)));
	connect(m_listView,SIGNAL(entered(const QModelIndex &)),  this,SLOT(slideSelected(const QModelIndex &)));
	
	// deleting old selection model per http://doc.trolltech.com/4.5/qabstractitemview.html#setModel
	QItemSelectionModel *m = m_listView->selectionModel();
//	if(m)
// 		disconnect(m,0,this,0);
	
	m_slideModel = new SlideGroupListModel();
	m_listView->setModel(m_slideModel);
	
	if(m)
	{
		delete m;
		m=0;
	}
	
	QItemSelectionModel *currentSelectionModel = m_listView->selectionModel();
	connect(currentSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(currentChanged(const QModelIndex &, const QModelIndex &)));
	
	layout->addWidget(m_listView);
	
	/** Setup the button controls at the bottom */
	QHBoxLayout *hbox = new QHBoxLayout();
	QPushButton *btn;
	
	// "Prev" button
	btn = new QPushButton(QIcon(":/data/control_start_blue.png"),"P&rev");
	connect(btn, SIGNAL(clicked()), this, SLOT(prevSlide()));
	hbox->addWidget(btn);
	
	hbox->addStretch(1);
	
	// animation controls
	m_timeLabel = new QLabel(this);
	m_timeLabel->setEnabled(false);
	m_timeLabel->setText("00:00");
	m_timeLabel->setFont(QFont("Monospace",10,QFont::Bold));
	hbox->addWidget(m_timeLabel);
	
	m_timeButton = new QPushButton(QIcon(":/data/action-play.png"),"&Play");
	connect(m_timeButton, SIGNAL(clicked()), this, SLOT(toggleTimerState()));
	m_timeButton->setEnabled(false);
	hbox->addWidget(m_timeButton);
	
	hbox->addStretch(1);

	// "Next" button
	btn = new QPushButton(QIcon(":/data/control_end_blue.png"),"&Next");
	connect(btn, SIGNAL(clicked()), this, SLOT(nextSlide()));
	hbox->addWidget(btn);


	/** Initalize animation timers **/
	m_elapsedTime.start();
	
	m_changeTimer = new QTimer(this);
	m_changeTimer->setSingleShot(true);
	connect(m_changeTimer, SIGNAL(timeout()), this, SLOT(nextSlide()));
	
	m_countTimer = new QTimer(this);
	connect(m_countTimer, SIGNAL(timeout()), this, SLOT(updateTimeLabel()));
	m_countTimer->setInterval(100);
	
	layout->addLayout(hbox);
	setLayout(layout);
	
	if(g)
		setOutputView(g);
	
}

void SlideGroupViewControl::enableAnimation(double time)
{
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug() << "SlideGroupViewControl::enableAnimation(): time:"<<time;
		
	if(time == 0)
	{
		if(DEBUG_SLIDEGROUPVIEWCONTROL)
			qDebug() << "SlideGroupViewControl::enableAnimation(): stopping all timers";
		
		toggleTimerState(Stopped,true);
		m_timeButton->setEnabled(false);
		return;
	}
	
	
	m_timeButton->setEnabled(true);
	m_timeLabel->setText(formatTime(time));
	
	m_currentTimeLength = time;
	
	toggleTimerState(Running,true);
}

void SlideGroupViewControl::toggleTimerState(TimerState state, bool resetTimer)
{
	if(state == Undefined)
		state = m_timerState == Running ? Stopped : Running;
	m_timerState = state;
		
	bool flag = state == Running;
	
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug() << "SlideGroupViewControl::toggleTimerState: state:"<<state<<", resetTimer:"<<resetTimer<<", flag:"<<flag;
	
	m_timeButton->setIcon(flag ? QIcon(":/data/action-pause.png") : QIcon(":/data/action-play.png"));
	m_timeButton->setText(flag ? "&Pause" : "&Play");
	m_timeLabel->setEnabled(flag);
	
	if(flag)
	{
		if(!resetTimer)
			m_currentTimeLength -= m_elapsedAtPause/1000;
			
		if(DEBUG_SLIDEGROUPVIEWCONTROL)
			qDebug() << "SlideGroupViewControl::toggleTimerState(): starting timer at:"<<m_currentTimeLength;
		
		if(m_currentTimeLength <= 0)
		{
			nextSlide();
		}
		else
		{
			m_changeTimer->start(m_currentTimeLength * 1000);
			m_countTimer->start();
			m_elapsedTime.start();
		}
	}
	else
	{
		m_changeTimer->stop();
		m_countTimer->stop();
		m_elapsedAtPause = m_elapsedTime.elapsed();
		
		if(DEBUG_SLIDEGROUPVIEWCONTROL)
			qDebug() << "SlideGroupViewControl::toggleTimerState(): stopping timer at:"<<(m_elapsedAtPause/1000);
		
		if(resetTimer)
			m_timeLabel->setText(formatTime(0));
	}
		
}

QString SlideGroupViewControl::formatTime(double time)
{
	double min = time/60;
	double sec = (min - (int)(min)) * 60;
	double ms  = (sec - (int)(sec)) * 60;
	return  (min<10? "0":"") + QString::number((int)min) + ":" +
		(sec<10? "0":"") + QString::number((int)sec) + "." +
		(ms <10? "0":"") + QString::number((int)ms );

}

void SlideGroupViewControl::updateTimeLabel()
{
	double time = ((double)m_currentTimeLength) - ((double)m_elapsedTime.elapsed())/1000;
	m_timeLabel->setText(QString("<font color='%1'>%2</font>").arg(time <= 3 ? "red" : "black").arg(formatTime(time)));
}
	
	
void SlideGroupViewControl::currentChanged(const QModelIndex &idx,const QModelIndex &)
{
	slideSelected(idx);
}

void SlideGroupViewControl::slideSelected(const QModelIndex &idx)
{
	if(m_releasingSlideGroup)
		return;
	Slide *slide = m_slideModel->slideFromIndex(idx);
	if(!slide)
		return;
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug() << "SlideGroupViewControl::slideSelected(): selected slide#:"<<slide->slideNumber();
	m_slideViewer->setSlide(slide);
	enableAnimation(slide->autoChangeTime());
	
	m_selectedSlide = slide;
}

void SlideGroupViewControl::setOutputView(SlideGroupViewer *v) 
{ 
	SlideGroup *g = 0;
	if(m_slideViewer) 
		g = m_slideViewer->slideGroup();
	
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug() << "SlideGroupViewControl::setOutputView()";
	m_slideViewer = v;
	
	if(g)
	{
		//m_slideModel->setSlideGroup(g);
		m_slideViewer->setSlideGroup(g);
	}
}
	
void SlideGroupViewControl::setSlideGroup(SlideGroup *g, Slide *curSlide)
{
	assert(g);
	
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug()<<"SlideGroupViewControl::setSlideGroup: Loading group#"<<g->groupNumber();
	
	m_clearButton->setEnabled(true);
	m_blackButton->setEnabled(true); 
	
	enableAnimation(0);
	
	m_slideModel->setSlideGroup(g);
	
	// reset seems to be required
	m_listView->reset();
	
	if(!curSlide)
		curSlide = g->at(0);
	m_listView->setCurrentIndex(m_slideModel->indexForSlide(curSlide));
	
	//if(DEBUG_SLIDEGROUPVIEWCONTROL)
	//	qDebug()<<"SlideGroupViewControl::setSlideGroup: DONE Loading group#"<<g->groupNumber();
}

void SlideGroupViewControl::releaseSlideGroup()
{
	m_releasingSlideGroup = true;
	m_slideModel->releaseSlideGroup();
	m_listView->reset();
	m_releasingSlideGroup = false;
}

void SlideGroupViewControl::nextSlide()
{
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug() << "SlideGroupViewControl::nextSlide(): mark";
	Slide *nextSlide = m_slideViewer->nextSlide();
	if(nextSlide)
		m_listView->setCurrentIndex(m_slideModel->indexForSlide(nextSlide));
	else
		toggleTimerState(Stopped,true);
}

void SlideGroupViewControl::prevSlide()
{
	Slide *s = m_slideViewer->prevSlide();
	m_listView->setCurrentIndex(m_slideModel->indexForSlide(s));
}

void SlideGroupViewControl::setCurrentSlide(int x)
{
	Slide *s = m_slideViewer->setSlide(x);
	m_listView->setCurrentIndex(m_slideModel->indexForSlide(s));
}

void SlideGroupViewControl::setCurrentSlide(Slide *s)
{
	m_slideViewer->setSlide(s);
	m_listView->setCurrentIndex(m_slideModel->indexForSlide(s));
}

void SlideGroupViewControl::fadeBlackFrame(bool toggled)
{
	m_slideViewer->fadeBlackFrame(toggled);
	m_clearButton->setEnabled(!toggled);
	
	if(!m_clearButton->isChecked())
	{
		if(toggled)
		{
			m_timerWasActiveBeforeFade = m_timerState == Running;
			if(m_timerWasActiveBeforeFade)
				toggleTimerState(Stopped);
		}
		else
		{
			if(m_timerWasActiveBeforeFade)
				toggleTimerState(Running);
		}
	}
}
	
void SlideGroupViewControl::fadeClearFrame(bool toggled)
{
	m_slideViewer->fadeClearFrame(toggled);
	
	if(toggled)
	{
		m_timerWasActiveBeforeFade = m_timerState == Running;
		if(m_timerWasActiveBeforeFade)
			toggleTimerState(Stopped);
	}
	else
	{
		if(m_timerWasActiveBeforeFade)
			toggleTimerState(Running);
	}
}

/** AbstractSlideGroupEditor:: **/
AbstractSlideGroupEditor::AbstractSlideGroupEditor(SlideGroup */*g*/, QWidget *parent) : QMainWindow(parent) {}
AbstractSlideGroupEditor::~AbstractSlideGroupEditor() {}
void AbstractSlideGroupEditor::setSlideGroup(SlideGroup */*g*/,Slide */*curSlide*/) {}


/** SlideGroupFactory:: **/
/** Static Members **/
QMap<SlideGroup::GroupType, SlideGroupFactory*> SlideGroupFactory::m_factoryMap;

void SlideGroupFactory::registerFactoryForType(SlideGroup::GroupType type, SlideGroupFactory *f)
{
	m_factoryMap[type] = f;
}
	
void SlideGroupFactory::removeFactoryForType(SlideGroup::GroupType type)
{
	m_factoryMap.remove(type);
}

SlideGroupFactory * SlideGroupFactory::factoryForType(SlideGroup::GroupType type)
{
	return m_factoryMap[type];
}

/** Class Members **/

SlideGroupFactory::SlideGroupFactory() : m_scene(0) {}
SlideGroupFactory::~SlideGroupFactory()
{
	if(m_scene)
	{
		delete m_scene;
		m_scene = 0;
	}
}
	
SlideGroup * SlideGroupFactory::newSlideGroup()
{
	return new SlideGroup();
}
	
SlideGroupViewMutator * SlideGroupFactory::newViewMutator()
{
	return new SlideGroupViewMutator();
}

SlideGroupViewControl * SlideGroupFactory::newViewControl()
{
	return new SlideGroupViewControl();
}

AbstractSlideGroupEditor * SlideGroupFactory::newEditor()
{
	return new SlideEditorWindow();
}

QPixmap	SlideGroupFactory::generatePreviewPixmap(SlideGroup *g, QSize iconSize, QRect sceneRect)
{
	//return QPixmap();
	
	Slide * slide = g->at(0);
	if(!slide)
	{
		qDebug("SlideGroupFactory::generatePreviewPixmap: No slide at 0");
		return QPixmap();
	}

	int icon_w = iconSize.width();
	int icon_h = iconSize.height();
	
	if(!m_scene)
		m_scene = new MyGraphicsScene(MyGraphicsScene::Preview);
	//qDebug() << "SlideGroupFactory::generatePreviewPixmap(): checking scene rect: "<<sceneRect;
	if(m_scene->sceneRect() != sceneRect)
	{
		//qDebug() << "SlideGroupFactory::generatePreviewPixmap(): setting new scene rect: "<<sceneRect;
		m_scene->setSceneRect(sceneRect);
	}
	
	//qDebug() << "SlideGroupFactory::generatePixmap: Loading slide";
	m_scene->setSlide(slide);
	
	QPixmap icon(icon_w,icon_h);
	QPainter painter(&icon);
	painter.fillRect(0,0,icon_w,icon_h,Qt::white);
	
	m_scene->render(&painter,QRectF(0,0,icon_w,icon_h),sceneRect);
	painter.setPen(Qt::black);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(0,0,icon_w-1,icon_h-1);
	
	// clear() so we can free memory, stop videos, etc
	m_scene->clear();
	//qDebug() << "SlideGroupFactory::generatePixmap: Releasing slide\n";
	
	return icon;
}
