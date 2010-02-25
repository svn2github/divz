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
#include <QScrollArea>
#include <QSettings>
#include <assert.h>

#include <QTextDocument>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QAbstractTextDocumentLayout>

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
SlideGroupViewControl::SlideGroupViewControl(OutputInstance *group, QWidget *w,bool initUI )
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
// 	m_clearActive(false),
// 	m_blackActive(false),
	m_group(0),
	m_quickSlide(0),
	m_originalQuickSlide(true),
	m_listView(0),
	m_isPreviewControl(false),
	m_quickSlideBase(0),
	m_timeButton(0),
	m_showQuickSlideBtn(0),
	m_iconSize(192),
	m_iconSizeSlider(0),
	m_lockIconSizeSetter(false),
	m_itemControlBase(0)

	
{
	if(initUI)
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
		//m_listView->setViewMode(QListView::ListMode);
		m_listView->setMovement(QListView::Static);
		m_listView->setWrapping(true);
		m_listView->setWordWrap(true);
		m_listView->setLayoutMode(QListView::Batched);
		m_listView->setFlow(QListView::LeftToRight);
		m_listView->setResizeMode(QListView::Adjust);
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
		//connect(currentSelectionModel, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(currentChanged(const QModelIndex &, const QModelIndex &)));
		
		layout->addWidget(m_listView);
		
		/** Setup the button controls at the bottom */
		QHBoxLayout *hbox = new QHBoxLayout();
		
		//hbox->addStretch(1);
		
		// "Prev" button
		m_prevBtn = new QPushButton(QIcon(":/data/control_start_blue.png"),"");
		connect(m_prevBtn, SIGNAL(clicked()), this, SLOT(prevSlide()));
		hbox->addWidget(m_prevBtn);
		
		// "Next" button
		m_nextBtn = new QPushButton(QIcon(":/data/control_end_blue.png"),"");
		connect(m_nextBtn, SIGNAL(clicked()), this, SLOT(nextSlide()));
		hbox->addWidget(m_nextBtn);
		
		label = new QLabel("Zoom:");
		hbox->addWidget(label);
		
		m_iconSizeSlider= new QSlider(Qt::Horizontal);
		m_iconSizeSlider->setMinimum(16);
		m_iconSizeSlider->setMaximum(480);
		m_iconSizeSlider->setTickInterval(16);
		m_iconSizeSlider->setSingleStep(16);
		m_iconSizeSlider->setPageStep(32);
		m_iconSizeSlider->setTickPosition(QSlider::TicksBelow);
		hbox->addWidget(m_iconSizeSlider,1); 
		
		connect(m_iconSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(setIconSize(int)));
		
// 		m_spinBox = new QSpinBox(this);
// 		m_spinBox->setMinimum(16);
// 		m_spinBox->setMaximum(480);
// 		m_spinBox->setSingleStep(16);
// 		//spin->setPageStep(32);
// 		hbox->addWidget(m_spinBox);
// 		
// 		connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(setIconSize(int)));
		
		
		QSettings s;
		setIconSize(s.value(QString("slideviewcontrol/%1").arg(m_isPreviewControl ? "preview-icon-size" : "icon-size"),m_iconSize).toInt());
		
		
		//hbox->addStretch(1);
	
		// animation controls
		m_timeLabel = new QLabel(this);
		m_timeLabel->setEnabled(false);
		m_timeLabel->setText("00:00.00");
		m_timeLabel->setFont(QFont("Monospace",10,QFont::Bold));
		hbox->addWidget(m_timeLabel);
		
		m_timeButton = new QPushButton(QIcon(":/data/action-play.png"),"");
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
		
// 		QScrollArea * scrollArea = new QScrollArea(this);
// 		scrollArea->setBackgroundRole(QPalette::Dark);
// 		layout->addWidget(scrollArea);
		
		//m_itemControlBase = new QWidget(scrollArea);
		m_itemControlBase = new QWidget(this);
// 		scrollArea->setWidget(m_itemControlBase);
		
		QVBoxLayout *vbox = new QVBoxLayout(m_itemControlBase);
		
		layout->addWidget(m_itemControlBase);
		m_itemControlBase->setVisible(false);
		setLayout(layout);
		
		if(group)
			setOutputView(group);
	}
	
}

SlideGroupViewControl::~SlideGroupViewControl()
{
	if(m_listView && m_listView->model())
		delete m_listView->model();
	if(m_quickSlide)
		delete m_quickSlide;
}

void SlideGroupViewControl::setIconSize(int size)
{
	if(m_lockIconSizeSetter)
		return;
	m_lockIconSizeSetter = true;
	int old = size;
	size = (int)(size / 16) * 16;
	//qDebug() << "setIconSize: old:"<<old<<", size:"<<size;
	if(m_iconSizeSlider->value() != size)
		m_iconSizeSlider->setValue(size);
// 	if(m_spinBox->value() != size)
// 		m_spinBox->setValue(size);
		
	QSettings s;
	s.setValue(QString("slideviewcontrol/%1").arg(m_isPreviewControl ? "preview-icon-size" : "icon-size"),size);
	m_iconSize = size;
		
	// the model will automatically correct aspect ratio as needed
	m_slideModel->setIconSize(QSize(size,size));
	m_listView->reset();
	m_lockIconSizeSetter = false;
}

SlideGroupListModel * SlideGroupViewControl::slideGroupListModel()
{
	//SlideGroupListModel * model = dynamic_cast<SlideGroupListModel*>(m_listView->model());
	if(!m_slideModel || !m_listView)
		return 0;
	return m_slideModel;
}

void SlideGroupViewControl::setQuickSlideEnabled(bool flag)
{
	if(m_quickSlideBase)
	    m_quickSlideBase->setVisible(flag);
}


void SlideGroupViewControl::addQuickSlide()
{
	if(!m_quickSlideBase)
	    return;

	if(!m_group)
		return;
		
	if(!m_quickSlide)
		makeQuickSlide();
	
	setQuickSlideText();
	
	Slide *slide = m_quickSlide->clone();
	slide->setSlideNumber(m_group->numSlides()+1);
	m_group->addSlide(slide);
}


bool SlideGroupViewControl_itemZCompare(AbstractItem *a, AbstractItem *b)
{
	AbstractVisualItem * va = dynamic_cast<AbstractVisualItem*>(a);
	AbstractVisualItem * vb = dynamic_cast<AbstractVisualItem*>(b);

	// Decending sort (eg. highest to lowest) because we want the highest
	// zvalue first n the list instead of last
	return (va && vb) ? va->zValue() > vb->zValue() : true;
}

void SlideGroupViewControl::makeQuickSlide()
{
	int lastRow = m_slideModel->rowCount()-1;
	Slide * slide = 0;
	if(lastRow > -1)
	{
		slide = m_slideModel->slideAt(lastRow)->clone();
		m_originalQuickSlide = false;
	}
	else
	{
		slide = new Slide();
		m_originalQuickSlide = true;
	}
		
		
	// Use the first textbox in the slide as the lyrics slide
	// "first" as defined by ZValue
	QList<AbstractItem *> items = slide->itemList();
	qSort(items.begin(), items.end(), SlideGroupViewControl_itemZCompare);

	TextBoxItem *text = 0;
	bool textboxFromTemplate = false;
		
	foreach(AbstractItem * item, items)
	{
		AbstractVisualItem * newVisual = dynamic_cast<AbstractVisualItem*>(item);
// 		if(DEBUG_TEXTOSLIDES)
// 			qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": item list: "<<newVisual->itemName();
		if(!text && item->itemClass() == TextBoxItem::ItemClass)
		{
			text = dynamic_cast<TextBoxItem*>(item);
			textboxFromTemplate = true;
// 			if(DEBUG_TEXTOSLIDES)
// 				qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": Found textbox from template, name:"<<text->itemName();
		}
	}

	// If no textbox, we've got to create one!
	if(!text)
	{
		// Create the textbox to hold the slide lyrics
		text = new TextBoxItem();
		text->setItemId(ItemFactory::nextId());
		text->setItemName(QString("TextBox%1").arg(text->itemId()));

// 		if(DEBUG_TEXTOSLIDES)
// 			qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": No textbox in template, adding new box.";
			
		// Outline pen for the text
		QPen pen = QPen(Qt::black,1.5);
		pen.setJoinStyle(Qt::MiterJoin);
	
		text->setPos(QPointF(0,0));
		text->setOutlinePen(pen);
		text->setOutlineEnabled(true);
		text->setFillBrush(Qt::white);
		text->setFillType(AbstractVisualItem::Solid);
		text->setShadowEnabled(true);
		text->setShadowBlurRadius(6);
	}



	/*
	
	AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());
	
	bg->setFillType(AbstractVisualItem::Solid);
	bg->setFillBrush(Qt::blue);*/
	
	
	if(!textboxFromTemplate)
		slide->addItem(text);
	
	m_quickSlideTextBox = text;
	m_quickSlide = slide;
	
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

Slide * SlideGroupViewControl::selectedSlide() 
{
	if(!m_slideModel || !m_listView)
		return 0;
	return m_slideModel->slideFromIndex(m_listView->currentIndex());
}

void SlideGroupViewControl::showQuickSlide(bool flag)
{
	if(m_showQuickSlideBtn->isChecked() != flag)
	{
		m_showQuickSlideBtn->setChecked(flag);
		// since setChecked() will trigger a call back to this function,
		// return here so we dont show the slide twice (triggering no cross fade)
		return;
	}
		
	if(!m_quickSlide)
	{
		makeQuickSlide();
	}
	
	if(flag)
	{
		setQuickSlideText();
		m_selectedSlide = m_slideModel->slideFromIndex(m_listView->currentIndex());
		
		// clone the slide so that it cross fades to it instead of just cutting to the slide
		// AND so that the setQuickSlideText() doesnt set text on the live slide
		m_slideViewer->setSlide(m_quickSlide->clone(),true); // true = take ownership, delete when done
		
		m_quickSlide = 0;
		delete m_quickSlide;
	}
	else
	{
		if(m_selectedSlide)
			m_slideViewer->setSlide(m_selectedSlide);
		else
			m_slideViewer->setSlide(SlideGroupViewer::blackSlide());
	}
	
	if(flag)
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
	
	m_quickSlideText->selectAll();
}

void SlideGroupViewControl::setQuickSlideText(const QString& tmp)
{
	if(!m_quickSlide)
		makeQuickSlide();

	QString text = tmp;
	if(text.isEmpty())
		text = m_quickSlideText->text();
	else
		m_quickSlideText->setText(text);
		
	if(m_originalQuickSlide)
	{	
		m_quickSlideTextBox->setText(text);
		fitQuickSlideText();
	}
	else
	{
		QTextDocument doc;
		QString origText = m_quickSlideTextBox->text();
		if (Qt::mightBeRichText(origText))
			doc.setHtml(origText);
		else
			doc.setPlainText(origText);
	
		QTextCursor cursor(&doc);
		cursor.select(QTextCursor::Document);
		
		QTextCharFormat format = cursor.charFormat();
 		if(!format.isValid())
 			format = cursor.blockCharFormat();
		//qDebug() << "Format at cursor: "<<format.fontPointSize()<<", "<<format.font()<<", "<<format.fontItalic()<<", "<<format.font().rawName();
		
		if(format.fontPointSize() > 0)
		{
			cursor.insertText(text);
			
			// doesnt seem to be needed
			//cursor.mergeCharFormat(format);
		
			m_quickSlideTextBox->setText(doc.toHtml());
			
		}
		else
		{
			m_quickSlideTextBox->setText(text);
			fitQuickSlideText();
		}
	}
}
	
void SlideGroupViewControl::setIsPreviewControl(bool flag)
{
	m_isPreviewControl = flag;
	if(!m_timeButton)
	    return;
	
	QSettings s;
	setIconSize(s.value(QString("slideviewcontrol/%1").arg(m_isPreviewControl ? "preview-icon-size" : "icon-size"),m_iconSize).toInt());

// 	m_timeButton->setText(!flag ? "&Start" : "Start");
// 	m_prevBtn->setText(!flag ? "P&rev" : "Prev");
// 	m_nextBtn->setText(!flag ? "Nex&t" : "Next");
	setQuickSlideEnabled(!flag);
}

void SlideGroupViewControl::repaintList()
{
// 	qDebug() << "SlideGroupViewControl::repaintList(): mark";
	if(!m_listView)
	    return;
 	m_listView->clearFocus();
 	m_listView->setFocus();
	m_listView->reset();
	m_listView->repaint();
	
	
	//qDebug() << "SlideGroupViewControl::repaintList(): mark done";
}

void SlideGroupViewControl::enableAnimation(double time)
{
	if(!m_timeButton)
		return;

	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug() << "SlideGroupViewControl::enableAnimation(): time:"<<time;
		
	if(time == 0 || !isEnabled())
	{
		if(DEBUG_SLIDEGROUPVIEWCONTROL)
			qDebug() << "SlideGroupViewControl::enableAnimation(): stopping all timers, isEnabled:"<<isEnabled();
		
		toggleTimerState(Stopped,true);
		m_timeButton->setEnabled(false);
		return;
	}
	
	
	m_timeButton->setEnabled(true);
	m_timeLabel->setText(formatTime(time));
	
	m_currentTimeLength = time;
	
	if(!m_isPreviewControl)
		toggleTimerState(Running,true);
	else
		if(DEBUG_SLIDEGROUPVIEWCONTROL)
			qDebug() << "SlideGroupViewControl::enableAnimation(): Not calling toggleTimerState() because m_isPreviewControl";
}

void SlideGroupViewControl::setEnabled(bool flag)
{
	if(!flag && m_timerState == Running)
		toggleTimerState(Stopped);
	QWidget::setEnabled(flag);
}

void SlideGroupViewControl::toggleTimerState(TimerState state, bool resetTimer)
{
	if(!m_timeButton)
		return;

	if(state == Undefined)
		state = m_timerState == Running ? Stopped : Running;
	m_timerState = state;
		
	bool flag = state == Running;
	
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug() << "SlideGroupViewControl::toggleTimerState(): state:"<<state<<", resetTimer:"<<resetTimer<<", flag:"<<flag<<" (s:Running:"<<Running<<",s:Stopped:"<<Stopped<<")";
	
	m_timeButton->setIcon(flag ? QIcon(":/data/action-pause.png") : QIcon(":/data/action-play.png"));
	m_timeButton->setText(m_isPreviewControl ? (flag ? "Pause" : "Start") : (flag ? "&Pause" : "&Start"));
	m_timeLabel->setEnabled(flag);
	
	if(flag)
	{
		if(!resetTimer)
		{
			m_currentTimeLength -= m_elapsedAtPause/1000;
			if(m_currentTimeLength < 0)
				m_currentTimeLength = 0;
		}
			
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
	if(!m_timeLabel)
		return;

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
	
	if(m_showQuickSlideBtn && m_showQuickSlideBtn->isChecked())
		m_showQuickSlideBtn->setChecked(false);
		
	
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

void SlideGroupViewControl::setOutputView(OutputInstance *inst) 
{ 
	SlideGroup *currentGroup = 0;
	if(m_slideViewer) 
	{
		currentGroup = m_slideViewer->slideGroup();
		disconnect(m_slideViewer, 0, this, 0);
	}
	
 	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug() << "SlideGroupViewControl::setOutputView()";
		
	m_slideViewer = inst;
	
	connect(m_slideViewer, SIGNAL(slideChanged(Slide*)), this, SLOT(slideChanged(Slide*))); 
	
	if(currentGroup)
	{
		qDebug() << "SlideGroupViewControl::setOutputView(): Sending currentGroup"<<currentGroup->assumedName()<<" to output view";
		m_slideViewer->setSlideGroup(currentGroup);
	}
}

void SlideGroupViewControl::slideChanged(Slide* newSlide)
{
	if(!m_itemControlBase)
		return;
		
	//qDebug() << "SlideGroupViewControl::slideChanged(): newSlide:"<<newSlide;
	while(!m_controlWidgets.isEmpty())
	{
		QWidget * widget = m_controlWidgets.takeFirst();
//  		qDebug() << "SlideGroupViewControl::slideChanged(): removing widget:"<<widget;
		m_itemControlBase->layout()->removeWidget(widget);
		widget->deleteLater();
		widget = 0;
	}
	
	QList<QWidget*> widgets = m_slideViewer->controlWidgets();
	if(!widgets.isEmpty())
	{
		if(m_slideViewer->isLocal())
		{
		    
// 			qDebug() << "SlideGroupViewControl::slideChanged(): got local viewer, looking for controls.";
// 			qDebug() << "SlideGroupViewControl::slideChanged(): got local viewer, got:"<<widgets.size()<<"controls";
			if(m_itemControlBase)
			    m_itemControlBase->setVisible(true);
			foreach(QWidget *widget, widgets)
			{
//  				qDebug() << "SlideGroupViewControl::slideChanged(): adding widget:"<<widget;
				widget->setParent(m_itemControlBase);
				m_itemControlBase->layout()->addWidget(widget);
				m_controlWidgets << widget;
			}
			
// 			qDebug() << "SlideGroupViewControl::slideChanged(): done setting up";
		}
		else
		{
			QLabel * label = new QLabel("Control widgets for this slide will appear on the network viewer client.");
			m_controlWidgets << label;
			m_itemControlBase->layout()->addWidget(label);
			
			while(!widgets.isEmpty())
			{
				QWidget * widget = widgets.takeFirst();
				m_itemControlBase->layout()->removeWidget(widget);
				widget->deleteLater();
				widget = 0;
			}
		}
	}
	else
	{
	    if(m_itemControlBase)
		m_itemControlBase->setVisible(false);
	}
}
	
void SlideGroupViewControl::setSlideGroup(SlideGroup *group, Slide *curSlide, bool allowProgressDialog)
{
	assert(group);
	
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug()<<"SlideGroupViewControl::setSlideGroup: Loading "<<group->assumedName();
	
// 	m_clearButton->setEnabled(true);
// 	m_blackButton->setEnabled(true); 
	
	enableAnimation(0);
	m_quickSlide = 0;
	m_group = group;
	
	DeepProgressIndicator *d = 0;
	if(allowProgressDialog)
	{
		d = new DeepProgressIndicator(m_slideModel,this);
		d->setText(QString(tr("Loading %1...")).arg(group->assumedName()));
		d->setTitle(QString(tr("Loading %1")).arg(group->assumedName()));
		d->setSize(group->numSlides());
	}
	
	if(m_listView)
	{
	    m_slideModel->setSlideGroup(group);

	    // reset seems to be required
	    m_listView->reset();
	}
	
	if(d)
		d->close();
	
	
	if(!curSlide)
		curSlide = group->at(0);
		
	if(curSlide)
		setCurrentSlide(curSlide);
	
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug()<<"SlideGroupViewControl::setSlideGroup: DONE Loading group#"<<group->groupNumber();
}

void SlideGroupViewControl::releaseSlideGroup()
{
	m_releasingSlideGroup = true;
	m_group = 0;
	if(m_listView)
	{
	    m_slideModel->releaseSlideGroup();
	    m_listView->reset();
	}
	m_releasingSlideGroup = false;
}

void SlideGroupViewControl::nextSlide()
{
	if(DEBUG_SLIDEGROUPVIEWCONTROL)
		qDebug() << "SlideGroupViewControl::nextSlide(): mark";
		
	SlideGroup * tmpGroup = m_group;
	Slide *nextSlide = m_slideViewer->nextSlide();
	if(nextSlide)
		//m_listView->setCurrentIndex(m_slideModel->indexForSlide(nextSlide));
		setCurrentSlide(nextSlide);
	else
	if(m_timerState == Running && 
	   tmpGroup == m_group) // make sure the group hasn't changed
	{
		//qDebug() << "SlideGroupViewControl::nextSlide(): stopping timer";
		toggleTimerState(Stopped,true);
		//qDebug() << "SlideGroupViewControl::nextSlide(): timer stopped";
	}
}

void SlideGroupViewControl::prevSlide()
{
	Slide *s = m_slideViewer->prevSlide();
	setCurrentSlide(s);
}

void SlideGroupViewControl::setCurrentSlide(int x)
{
	Slide *s = m_slideViewer->setSlide(x);
	setCurrentSlide(s);
}

void SlideGroupViewControl::setCurrentSlide(Slide *s)
{
	if(m_listView)
	{
		//m_slideViewer->setSlide(s);
		QModelIndex idx = m_slideModel->indexForSlide(s);
		m_listView->setCurrentIndex(idx);
		slideSelected(idx);
	}
}

// void SlideGroupViewControl::fadeBlackFrame(bool toggled)
// {
// 	//m_clearButton->setEnabled(!toggled);
// 	m_blackActive = toggled;
// 	
// 		
// 	SlideGroup *group = 0;
// 	if(m_slideViewer) 
// 		group = m_slideViewer->slideGroup();
// 	
// // 	if(!toggled && m_clearActive)
// // 		m_slideViewer->fadeClearFrame(true);
// // 	else
// 		m_slideViewer->fadeBlackFrame(toggled);
// 	
// 	if(group)
// 	{
// 		if(!m_clearActive)
// 		{
// 			if(toggled)
// 			{
// 				m_timerWasActiveBeforeFade = m_timerState == Running;
// 				if(m_timerWasActiveBeforeFade)
// 					toggleTimerState(Stopped);
// 			}
// 			else
// 			{
// 				if(m_timerWasActiveBeforeFade)
// 					toggleTimerState(Running);
// 			}
// 		}
// 	}
// }
	
// void SlideGroupViewControl::fadeClearFrame(bool toggled)
// {
// 	m_clearActive = toggled;
// 		
// 	SlideGroup *group = 0;
// 	if(m_slideViewer) 
// 		group = m_slideViewer->slideGroup();
// 		
// 	if(!m_blackActive)
// 	{
// 		m_slideViewer->fadeClearFrame(toggled);
// 		if(group)
// 		{
// 			if(toggled)
// 			{
// 				m_timerWasActiveBeforeFade = m_timerState == Running;
// 				if(m_timerWasActiveBeforeFade)
// 					toggleTimerState(Stopped);
// 			}
// 			else
// 			{
// 				if(m_timerWasActiveBeforeFade)
// 					toggleTimerState(Running);
// 			}
// 		}
// 	}
// }

/** AbstractSlideGroupEditor:: **/
AbstractSlideGroupEditor::AbstractSlideGroupEditor(SlideGroup */*group*/, QWidget *parent) : QMainWindow(parent) {}
AbstractSlideGroupEditor::~AbstractSlideGroupEditor() {}
void AbstractSlideGroupEditor::setSlideGroup(SlideGroup */*group*/,Slide */*curSlide*/) {}


/** SlideGroupFactory:: **/
/** Static Members **/
QHash<int, SlideGroupFactory*> SlideGroupFactory::m_factoryMap;

void SlideGroupFactory::registerFactoryForType(int type, SlideGroupFactory *f)
{
	m_factoryMap[type] = f;
}
	
void SlideGroupFactory::removeFactoryForType(int type)
{
	m_factoryMap.remove(type);
}

SlideGroupFactory * SlideGroupFactory::factoryForType(int type)
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

AbstractItemFilterList SlideGroupFactory::customFiltersFor(OutputInstance *,SlideGroup *)
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

NativeViewer * SlideGroupFactory::newNativeViewer()
{
	return 0;
}

QPixmap	SlideGroupFactory::generatePreviewPixmap(SlideGroup *group, QSize iconSize, QRect sceneRect)
{
// 	qDebug() << "SlideGroupFactory::generatePreviewPixmap: Generating pixmap for group "<<group;
	//return QPixmap();
	int icon_w = iconSize.width();
	int icon_h = iconSize.height();


	if(group->numSlides() <= 0 || group->groupTitle().startsWith("--"))
	{
		QPixmap icon(icon_w,icon_h);
		icon.fill(Qt::white);
		QPainter painter(&icon);
		painter.setPen(QPen(Qt::black,1.0,Qt::DotLine));
		painter.setBrush(Qt::NoBrush);
		painter.drawRect(0,0,icon_w-1,icon_h-1);
		return icon;
	}


	Slide * slide = group->at(0);
	if(!slide)
	{
		qDebug("SlideGroupFactory::generatePreviewPixmap: No slide at 0");
		return QPixmap();
	}

	
	if(!m_scene)
		m_scene = new MyGraphicsScene(MyGraphicsScene::StaticPreview);
	if(m_scene->sceneRect() != sceneRect)
		m_scene->setSceneRect(sceneRect);
	
	//qDebug() << "SlideGroupFactory::generatePixmap: Loading slide";
	m_scene->setMasterSlide(group->masterSlide());
	
// 	qDebug() << "SlideGroupFactory::generatePreviewPixmap: Loading slide";
	m_scene->setSlide(slide);
// 	qDebug() << "SlideGroupFactory::generatePreviewPixmap: Slide loaded";
	
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
// 	qDebug() << "SlideGroupFactory::generatePreviewPixmap: Clearing scene";
	m_scene->clear();
// 	qDebug() << "SlideGroupFactory::generatePreviewPixmap: Scene cleared";
	// clear the master slide because it may be deleted after generating the pixmap (e.g. in OutputControl::setOverlayDocument)
	m_scene->setMasterSlide(0);
	//qDebug() << "SlideGroupFactory::generatePixmap: Releasing slide\n";
	
// 	qDebug() << "SlideGroupFactory::generatePreviewPixmap: Done with group "<<group;
	
	return icon;
}
