#include "model/SlideGroupFactory.h"
#include "model/AbstractItem.h"
#include "model/SlideGroup.h"
#include "model/Output.h"
#include "model/Slide.h"
#include "model/TextBoxItem.h"
#include "model/ItemFactory.h"
#include "MainWindow.h"

#include "SlideEditorWindow.h"
#include "OutputInstance.h"
#include "SlideGroupViewer.h"

#include "DeepProgressIndicator.h"

#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <assert.h>

/** SlideGroupViewControlListView:: **/
/* We reimplement QListView's keyPressEvent to detect
  selection changes on key press events in QListView::ListMode.
  Aparently, in list mode the selection model's currentChanged()
  signal doesn't get fired on keypress, but in IconMode it does.
  We use IconMode by default in the ViewControl below, but the
  SongSlideGroupViewControl uses ListMode - this allows either
  icon or list mode to change slides just by pressing up or down
*/ 
SlideGroupViewControlListView::SlideGroupViewControlListView(SlideGroupViewControl * ctrl)
	: QListView(ctrl)
	, ctrl(ctrl)
{

	QString stylesheet =
		" QListView {"
		"     show-decoration-selected: 1;" /* make the selection span the entire width of the view */
		" }"
		""
		" QListView::item:alternate {"
		"     background: #EEEEEE;"
		" }"
		""
		" QListView::item:selected {"
		"     border: 1px solid #6a6ea9;"
		" }"
		""
		" QListView::item:selected:!active {"
		"     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
		"                                 stop: 0 #ABAFE5, stop: 1 #8588B2);"
		" }"
		""
		" QListView::item:selected:active {"
		"     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
		"                                 stop: 0 #6a6ea9, stop: 1 #888dd9);"
		" }"
		""
		" QListView::item:hover {"
		"     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
		"                                 stop: 0 #FAFBFE, stop: 1 #DCDEF1);"
		" }";

	setStyleSheet(stylesheet);
}
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
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
/** SlideGroupViewControl:: **/
#define DEBUG_SLIDEGROUPVIEWCONTROL 0
SlideGroupViewControl::SlideGroupViewControl(OutputInstance *g, QWidget *w )
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
	m_timerWasActiveBeforeFade(0),
	m_clearActive(false),
	m_blackActive(false),
	m_group(0),
	m_quickSlide(0),
	m_originalQuickSlide(true)
	
{
	QVBoxLayout * layout = new QVBoxLayout();
	layout->setMargin(0);
	
	QPushButton *btn;
	
	
	/** Setup quickslide widget */
	m_quickSlideBase = new QWidget();
 	QHBoxLayout * hbox1 = new QHBoxLayout(m_quickSlideBase);
 	hbox1->setMargin(0);
 	
 	QLabel *label = new QLabel("Quick Slide:");
 	hbox1->addWidget(label);
 	
 	m_quickSlideText = new QLineEdit();
 	hbox1->addWidget(m_quickSlideText);
 	connect(m_quickSlideText, SIGNAL(returnPressed()), this, SLOT(showQuickSlide()));
 	
 	btn = new QPushButton("Show");
 	btn->setCheckable(true);
 	hbox1->addWidget(btn);
 	connect(btn, SIGNAL(toggled(bool)), this, SLOT(showQuickSlide(bool)));
 	m_showQuickSlideBtn = btn;
 	
 	btn = new QPushButton("Add");
 	hbox1->addWidget(btn);
 	connect(btn, SIGNAL(clicked()), this, SLOT(addQuickSlide()));
 	
 	
 	layout->addWidget(m_quickSlideBase);


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
	connect(m_listView,SIGNAL(doubleClicked(const QModelIndex &)),this,SLOT(slideDoubleClicked(const QModelIndex &)));
	//connect(m_listView,SIGNAL(entered(const QModelIndex &)),  this,SLOT(slideSelected(const QModelIndex &)));
	
	// deleting old selection model per http://doc.trolltech.com/4.5/qabstractitemview.html#setModel
	QItemSelectionModel *m = m_listView->selectionModel();
//	if(m)
// 		disconnect(m,0,this,0);
	
	m_slideModel = new SlideGroupListModel();
	m_listView->setModel(m_slideModel);
	connect(m_slideModel, SIGNAL(repaintList()), this, SLOT(repaintList()));
	
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
	
	//hbox->addStretch(1);
	
	// "Prev" button
	m_prevBtn = new QPushButton(QIcon(":/data/control_start_blue.png"),"P&rev");
	connect(m_prevBtn, SIGNAL(clicked()), this, SLOT(prevSlide()));
	hbox->addWidget(m_prevBtn);
	
	// "Next" button
	m_nextBtn = new QPushButton(QIcon(":/data/control_end_blue.png"),"Nex&t");
	connect(m_nextBtn, SIGNAL(clicked()), this, SLOT(nextSlide()));
	hbox->addWidget(m_nextBtn);
	
	
	hbox->addStretch(1);

	// animation controls
	m_timeLabel = new QLabel(this);
	m_timeLabel->setEnabled(false);
	m_timeLabel->setText("00:00.00");
	m_timeLabel->setFont(QFont("Monospace",10,QFont::Bold));
	hbox->addWidget(m_timeLabel);
	
	m_timeButton = new QPushButton(QIcon(":/data/action-play.png"),"&Start");
	connect(m_timeButton, SIGNAL(clicked()), this, SLOT(toggleTimerState()));
	m_timeButton->setEnabled(false);
	hbox->addWidget(m_timeButton);


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


void SlideGroupViewControl::setQuickSlideEnabled(bool flag)
{
	m_quickSlideBase->setVisible(flag);
}


void SlideGroupViewControl::addQuickSlide()
{
	if(!m_group)
		return;
		
	if(!m_quickSlide)
		makeQuickSlide();
	
	m_quickSlideTextBox->setText(m_quickSlideText->text());
	if(m_originalQuickSlide)
		fitQuickSlideText();
	
	Slide *slide = m_quickSlide->clone();
	slide->setSlideNumber(m_group->numSlides()+1);
	m_group->addSlide(slide);
}

void SlideGroupViewControl::makeQuickSlide()
{

	TextBoxItem * tmpText = new TextBoxItem();
	tmpText->setItemId(ItemFactory::nextId());
	tmpText->setItemName(QString("TextBoxItem%1").arg(tmpText->itemId()));
	
	Slide * slide = new Slide();
	AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());
	
	bg->setFillType(AbstractVisualItem::Solid);
	bg->setFillBrush(Qt::blue);
	
	
	// Outline pen for the text
	QPen pen = QPen(Qt::black,1.5);
	pen.setJoinStyle(Qt::MiterJoin);
	
	tmpText->setPos(QPointF(0,0));
	tmpText->setOutlinePen(pen);
	tmpText->setOutlineEnabled(true);
	tmpText->setFillBrush(Qt::white);
	tmpText->setFillType(AbstractVisualItem::Solid);
	tmpText->setShadowEnabled(true);
	tmpText->setShadowBlurRadius(6);
	
	slide->addItem(tmpText);
	
	m_quickSlideTextBox = tmpText;
	m_quickSlide = slide;
	m_originalQuickSlide = true;
}

void SlideGroupViewControl::fitQuickSlideText()
{
	QSize fitSize = MainWindow::mw()->standardSceneRect().size();
	int realHeight = m_quickSlideTextBox->fitToSize(fitSize,48);

	// Center text on screen
	qreal y = fitSize.height()/2 - realHeight/2;
	//qDebug() << "SongSlideGroup::textToSlides(): centering: boxHeight:"<<boxHeight<<", textRect height:"<<textRect.height()<<", centered Y:"<<y;
	m_quickSlideTextBox->setContentsRect(QRectF(0,y,fitSize.width(),realHeight));
}

void SlideGroupViewControl::showQuickSlide(bool flag)
{
	if(!m_quickSlide)
	{
		makeQuickSlide();
	}
	
	if(flag)
	{
		m_quickSlideTextBox->setText(m_quickSlideText->text());
		if(m_originalQuickSlide)
			fitQuickSlideText();
		m_selectedSlide = m_slideModel->slideFromIndex(m_listView->currentIndex());
		m_slideViewer->setSlide(m_quickSlide);
	}
	else
	{
		if(m_selectedSlide)
			m_slideViewer->setSlide(m_selectedSlide);
		else
			m_slideViewer->setSlide(SlideGroupViewer::blackSlide());
	}
	
	if(m_showQuickSlideBtn->isChecked() != flag)
		m_showQuickSlideBtn->setChecked(flag);
}
	
void SlideGroupViewControl::setIsPreviewControl(bool flag)
{
	m_isPreviewControl= flag;
	m_timeButton->setText(!flag ? "&Start" : "Start");
	m_prevBtn->setText(!flag ? "P&rev" : "Prev");
	m_nextBtn->setText(!flag ? "Nex&t" : "Next");
}

void SlideGroupViewControl::repaintList()
{
	//qDebug() << "SlideGroupViewControl::repaintList(): mark";
 	m_listView->clearFocus();
 	m_listView->setFocus();
	m_listView->repaint();
	
	//qDebug() << "SlideGroupViewControl::repaintList(): mark done";
}

void SlideGroupViewControl::enableAnimation(double time)
{
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug() << "SlideGroupViewControl::enableAnimation(): time:"<<time;
		
	if(time == 0 || !isEnabled())
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

void SlideGroupViewControl::setEnabled(bool flag)
{
	if(!flag && m_timerState == Running)
		toggleTimerState(Stopped);
	QWidget::setEnabled(flag);
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
	m_timeButton->setText(m_isPreviewControl ? (flag ? "Pause" : "Start") : (flag ? "&Pause" : "&Start"));
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
	if(m_slideViewer->slideGroup() != m_group)
		m_slideViewer->setSlideGroup(m_group,slide);
	else
		m_slideViewer->setSlide(slide);
	enableAnimation(slide->autoChangeTime());
	
	m_selectedSlide = slide;
	
	emit slideSelected(slide);
}


void SlideGroupViewControl::slideDoubleClicked(const QModelIndex &idx)
{
	if(m_releasingSlideGroup)
		return;
	Slide *slide = m_slideModel->slideFromIndex(idx);
	if(!slide)
		return;
	emit slideDoubleClicked(slide);
}

void SlideGroupViewControl::setOutputView(OutputInstance *v) 
{ 
	SlideGroup *g = 0;
	if(m_slideViewer) 
		g = m_slideViewer->slideGroup();
	
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug() << "SlideGroupViewControl::setOutputView()";
	m_slideViewer = v;
	
	if(g)
		m_slideViewer->setSlideGroup(g);
}
	
void SlideGroupViewControl::setSlideGroup(SlideGroup *g, Slide *curSlide, bool allowProgressDialog)
{
	assert(g);
	
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug()<<"SlideGroupViewControl::setSlideGroup: Loading group#"<<g->groupNumber();
	
// 	m_clearButton->setEnabled(true);
// 	m_blackButton->setEnabled(true); 
	
	enableAnimation(0);
	
	m_group = g;
	
	DeepProgressIndicator *d = 0;
	if(allowProgressDialog)
	{
		d = new DeepProgressIndicator(m_slideModel,this);
		d->setText(QString("Loading Group #%1...").arg(g->groupNumber()));
		d->setTitle(QString("Loading Group #%1").arg(g->groupNumber()));
		d->setSize(g->numSlides());
	}
	
	m_slideModel->setSlideGroup(g);
	
	// reset seems to be required
	m_listView->reset();
	
	if(d)
		d->close();
	
	
	if(!curSlide)
		curSlide = g->at(0);
	if(curSlide)
		m_listView->setCurrentIndex(m_slideModel->indexForSlide(curSlide));
	
	//if(DEBUG_SLIDEGROUPVIEWCONTROL)
	//	qDebug()<<"SlideGroupViewControl::setSlideGroup: DONE Loading group#"<<g->groupNumber();
}

void SlideGroupViewControl::releaseSlideGroup()
{
	m_releasingSlideGroup = true;
	m_group = 0;
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
	if(m_timerState == Running)
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
	//m_clearButton->setEnabled(!toggled);
	m_blackActive = toggled;
	
		
	SlideGroup *g = 0;
	if(m_slideViewer) 
		g = m_slideViewer->slideGroup();
	
		if(!toggled && m_clearActive)
			m_slideViewer->fadeClearFrame(true);
		else
			m_slideViewer->fadeBlackFrame(toggled);
	if(g)
	{
		if(!m_clearActive)
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
}
	
void SlideGroupViewControl::fadeClearFrame(bool toggled)
{
	m_clearActive = toggled;
		
	SlideGroup *g = 0;
	if(m_slideViewer) 
		g = m_slideViewer->slideGroup();
		
	if(!m_blackActive)
	{
		m_slideViewer->fadeClearFrame(toggled);
		if(g)
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

AbstractItemFilterList SlideGroupFactory::customFiltersFor(OutputInstance *instace)
{
	return AbstractItemFilterList();
}
	
SlideGroup * SlideGroupFactory::newSlideGroup()
{
	return new SlideGroup();
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
	int icon_w = iconSize.width();
	int icon_h = iconSize.height();


	if(g->numSlides() <= 0 || g->groupTitle().startsWith("--"))
	{
		QPixmap icon(icon_w,icon_h);
		icon.fill(Qt::white);
		QPainter painter(&icon);
		painter.setPen(QPen(Qt::black,1.0,Qt::DotLine));
		painter.setBrush(Qt::NoBrush);
		painter.drawRect(0,0,icon_w-1,icon_h-1);
		return icon;
	}


	Slide * slide = g->at(0);
	if(!slide)
	{
		qDebug("SlideGroupFactory::generatePreviewPixmap: No slide at 0");
		return QPixmap();
	}

	
	if(!m_scene)
		m_scene = new MyGraphicsScene(MyGraphicsScene::Preview);
	if(m_scene->sceneRect() != sceneRect)
		m_scene->setSceneRect(sceneRect);
	
	//qDebug() << "SlideGroupFactory::generatePixmap: Loading slide";
	m_scene->setMasterSlide(g->masterSlide());
	m_scene->setSlide(slide);
	
	QPixmap icon(icon_w,icon_h);
	QPainter painter(&icon);
	painter.fillRect(0,0,icon_w,icon_h,Qt::white);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);

	m_scene->render(&painter,QRectF(0,0,icon_w,icon_h),sceneRect);
	painter.setPen(Qt::black);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(0,0,icon_w-1,icon_h-1);
	
	// clear() so we can free memory, stop videos, etc
	m_scene->clear();
	// clear the master slide because it may be deleted after generating the pixmap (e.g. in OutputControl::setOverlayDocument)
	m_scene->setMasterSlide(0);
	//qDebug() << "SlideGroupFactory::generatePixmap: Releasing slide\n";
	
	return icon;
}
