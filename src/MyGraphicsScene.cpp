
#ifndef UINT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include "MyGraphicsScene.h"
#include "frames/FrameFactory.h"
#include "items/AbstractConfig.h"
#include "items/GenericItemConfig.h"
#include "items/AbstractContent.h"
/*#include "items/PictureContent.h"
#include "items/PictureConfig.h"*/
#include "items/TextContent.h"
// #include "items/WebContentSelectorItem.h"
// #include "items/WebcamContent.h"
#include "items/TextContent.h"
#include "items/TextBoxConfig.h"
#include "items/TextBoxContent.h"
#include "items/OutputViewConfig.h"
#include "items/OutputViewContent.h"
#include "items/VideoFileContent.h"
#include "items/VideoFileConfig.h"
#include "items/BackgroundContent.h"
#include "items/BackgroundConfig.h"
#include "items/BoxConfig.h"
#include "items/BoxContent.h"
#include "items/ImageConfig.h"
#include "items/ImageContent.h"

#include "model/ItemFactory.h"
#include "model/Slide.h"
#include "model/TextItem.h"
#include "model/TextBoxItem.h"
#include "model/BoxItem.h"
#include "model/VideoFileItem.h"
#include "model/BackgroundItem.h"
#include "model/ImageItem.h"
#include "model/OutputViewItem.h"



#include "AppSettings.h"

#include "RenderOpts.h"

#include <QPushButton>

#include <QAbstractTextDocumentLayout>
#include <QFile>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsView>
#include <QImageReader>
#include <QLabel>
#include <QList>
#include <QMessageBox>
#include <QMimeData>
// #include <QNetworkAccessManager>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QTimer>
#include <QUrl>

#include <assert.h>

#define COLORPICKER_W 200
#define COLORPICKER_H 150

#include "MainWindow.h"

#define DEBUG_MYGRAPHICSSCENE 0

#define DEBUG_KEYHANDLER 0

#define DEBUG_MYGRAPHICSSCENE_ITEM_MGMT 0

#define qSetEffectOpacity(item,opacity) { QGraphicsOpacityEffect * opac = dynamic_cast<QGraphicsOpacityEffect*>(item->graphicsEffect()); if(opac) opac->setOpacity(opacity); }
//#define qGetEffectOpacity(item,opacity) dynamic_cast<QGraphicsOpacityEffect*>item->graphicsEffect() ? (dynamic_cast<QGraphicsOpacityEffect*>item->graphicsEffect())->opacity() : 0

// #if QT_VERSION >= 0x040600
// 	#include <QGraphicsOpacityEffect>
// 
// 	#define QT46_OPAC_ENAB 0
// #endif

#define PTRS(ptr) QString().sprintf("%p",static_cast<void*>(ptr))

class RootObject : public QGraphicsItem
{
public:
	RootObject(QGraphicsScene*x):QGraphicsItem(0,x){
	
		#if QT46_OPAC_ENAB > 0 
			QGraphicsOpacityEffect * opac = new QGraphicsOpacityEffect();
			opac->setOpacity(1);
			setGraphicsEffect(opac);
		#endif
		
		//setCacheMode(QGraphicsItem::DeviceCoordinateCache);
	}
	QRectF boundingRect() const { return QRectF(); } //MainWindow::mw()->standardSceneRect(); }
	void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {}
};

MyGraphicsScene::MyGraphicsScene(ContextHint hint, QObject * parent)
    : QGraphicsScene(parent)
    , m_slide(0)
    , m_fadeTimer(0)
    , m_contextHint(hint)
    , m_masterSlide(0)
    , m_bg(0)
    , m_fadeClockStarted(false)
    , m_lastElapsed(-1)
{
	m_staticRoot = new RootObject(this);
	m_staticRoot->setPos(0,0);
	
	m_fadeRoot = new RootObject(this);
	m_fadeRoot->setPos(0,0);
	
	m_liveRoot = new RootObject(this);
	m_liveRoot->setPos(0,0);
	
	//installEventFilter(this);
}

MyGraphicsScene::~MyGraphicsScene()
{
	m_slide = 0;
	//m_slidePrev = 0;
	
	//qDeleteAll(m_ownedContent);
	delete m_staticRoot;
	delete m_fadeRoot;
	delete m_liveRoot;
	
	m_staticRoot = 0;
	m_fadeRoot = 0;
	m_liveRoot = 0;
	
	// Since the items are cloned when added to the copy buffer,
	// AND cloned when pasted, its safe to delete them from memory
	// in destructor
	if(m_copyBuffer.size())
		qDeleteAll(m_copyBuffer);
	
}

void MyGraphicsScene::setMasterSlide(Slide *slide)
{
	//qDebug() << "MyGraphicsScene::setMasterSlide(): EXISTING master ptr:"<<PTRS(m_masterSlide);
	if(m_masterSlide)
		disconnect(m_masterSlide,0,this,0);
	
	m_masterSlide = slide;
	//qDebug() << "MyGraphicsScene::setMasterSlide(): master ptr:"<<PTRS(m_masterSlide);
	
	if(m_masterSlide)
		connect(m_masterSlide,SIGNAL(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)),this,SLOT(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)));
	
	
}

void MyGraphicsScene::setContextHint(ContextHint hint)
{
	m_contextHint = hint;
}

void MyGraphicsScene::clear(bool emitTxFinished)
{
	//qDebug() << "MyGraphicsScene::clear() "<<this;
	foreach(AbstractContent *content, m_content)
	{
		m_content.removeAll(content);
		if(content->parentItem() == m_liveRoot)
			content->setParentItem(0);
		removeItem(content);
		
		disconnect(content, 0, 0, 0);
		//qDebug() << "MyGraphicsScene::clear: Disposing of content:"<<content;
		if(DEBUG_MYGRAPHICSSCENE_ITEM_MGMT)
			qDebug() << "MyGraphicsScene::clear(): Disposing of item: "<<content->modelItem()->itemName();
		content->dispose(false);
		//delete content;
		content = 0;
	}
	
	emit slideDiscarded(m_slide);
	m_slide = 0;
	
	if(emitTxFinished)
	{
		//qDebug() << "MyGraphicsScene::clear(): "<<this<<" emitting transitionFinished()";
		emit transitionFinished(0);
	}
	
	// dont remove our fade/live root
	//QGraphicsScene::clear();
}
	
void MyGraphicsScene::slideItemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value, QVariant /*oldValue*/)
{
	Slide * slide = dynamic_cast<Slide *>(sender());
	
	if(operation == "change")
		return;
	else
	if(operation == "add")
	{
		AbstractContent * content = createVisualDelegate(item);
		if(content)
		{
			if(slide == m_masterSlide)
			{
				applyMasterSlideItemFlags(content);
			}
			else
			{
				clearSelection();
				content->setSelected(true);
			}
		}
	}
	else
	if(operation == "remove")
		removeVisualDelegate(item);
}

AbstractContent * MyGraphicsScene::findVisualDelegate(AbstractItem *item)
{
	foreach(AbstractContent *z, m_content)
		if(z->modelItem() == item)
			return z;
	return 0;
}

void MyGraphicsScene::removeVisualDelegate(AbstractItem *item)
{
	//qDebug() << "MyGraphicsScene::removeVisualDelegate: Going to remove: "<<item->itemName();
	AbstractContent *z = findVisualDelegate(item);
	if(!z)
		return;
		
	z->setVisible(false);
	z->setParentItem(0);
	removeItem(z);
	
	m_content.removeAll(z);
	disconnect(z, 0, 0, 0);
	if(DEBUG_MYGRAPHICSSCENE_ITEM_MGMT)
		qDebug() << "MyGraphicsScene::removeVisualDelegate(): Disposing of item: "<<z->modelItem()->itemName();
	z->dispose(false);
		
// 	qDebug() << "MyGraphicsScene::removeVisualDelegate: Couldn't find visual delegate for item requested: "<<item->itemName();
// 	fprintf(stderr,"MyGraphicsScene::removeVisualDelegate: Can't find delegate for m_item=%p\n",item);
}


void MyGraphicsScene::setSlide(Slide *slide, SlideTransition trans, int speed, int quality)
{
	if(m_slide && !slide)
	{
		clear();
		m_slide = 0;
		//emit transitionFinished(0);
		return;
	}
	
	if(m_slide == slide)
	{
		//qDebug("MyGraphicsScene::setSlide: Not changing slide - same slide!");
		return;
	}
	
	if(m_slide)
		disconnect(m_slide,0,this,0);
	
	if(slide)
		connect(slide,SIGNAL(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)),this,SLOT(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)));
	
	if(contextHint() == Preview)
		trans = None; 
	
	m_currentTransition = trans;
	
	if(DEBUG_MYGRAPHICSSCENE)
		qDebug() << "MyGraphicsScene::setSlide(): "<<this<<" Setting slide # "<<slide->slideNumber();
	
// 	//QMap<QString,AbstractItem*> consumedDuplicates;
// 	QList<quint32> consumedDuplicates;
// 	//QList<AbstractItem*> consumedItems;
// 	//QStringList consumedItemNames;

// 	QString ptr;
// 	ptr = QString().sprintf("%p",this);
// 	
	if(DEBUG_MYGRAPHICSSCENE)
		qDebug() << "MyGraphicsScene::setSlide(): "<<this<<" trans:"<<trans<<", speed:"<<speed<<", quality:"<<quality;
	//trans = None;
	
	bool crossFading = false;
	if(trans == None || (speed <= 1 && quality <= 1))
	{
		clear(false); // dont emit transitionFinished(0) - we do that later
	}
	else
	{
		crossFading = true;
// 		//QStringList newSlideKeys;
// 		QMap<quint32,AbstractItem*> newSlideKeys;
// 		// have existing content?
// 		if(m_content.size() > 0)
// 		{
// 			qDebug() << "MyGraphicsScene::setSlide(): Existing content, creating key hash";
// 			// create key list 
// 			QList<AbstractItem *> items = m_slide->itemList();
// 			foreach(AbstractItem *item, items)
// 			{
// 				assert(item != NULL);
// 				
// 				//newSlideKeys << item->valueKey();
// 				quint32 key = item->valueKey();
// 				newSlideKeys[key] = item;
// 				qDebug() << "MyGraphicsScene::setSlide(): Key Hash for New Slide: item:"<<item->itemName()<<", key:"<<key;
// 			}
// 		}
				
		if(speed <= 0)
		{
			speed = AppSettings::crossFadeSpeed();
			//qDebug() << "MyGraphicsScene::setSlide(): Using app settings for fade speed";
		}
		
		if(quality < 0)
		{
			quality = AppSettings::crossFadeQuality();
			//qDebug() << "MyGraphicsScene::setSlide(): Using app settings for fade quality";
		}
			
		if(DEBUG_MYGRAPHICSSCENE)
			qDebug() << "MyGraphicsScene::setSlide(): "<<this<<" [final] speed:"<<speed<<", quality:"<<quality;
		
		if(!m_fadeTimer)
		{
			m_fadeTimer = new QTimer(this);
			connect(m_fadeTimer, SIGNAL(timeout()), this, SLOT(slotTransitionStep()));
		}
		
		
		if(m_fadeTimer->isActive())
			endTransition(); 
			
 		//if(DEBUG_MYGRAPHICSSCENE)
 			//qDebug() << "MyGraphicsScene::setSlide(): Reparenting"<<m_content.size()<<"items";
		m_staticRoot->setZValue(100);
			
		foreach(AbstractContent *x, m_content)
		{
			
// 			AbstractItem * model = x->modelItem();
// 			quint32 key = model->valueKey();
// 			
// 			// item with exact same attributes on the new slide
// 			if(newSlideKeys.contains(key))
// 			{
// 				qDebug() << "MyGraphicsScene::setSlide(): Reparenting: Found duplicate:"<<model->itemName();
// 				// Since this item in m_content (x) has EXACTLY the same 
// 				// attributes on the new slide as on the old slide,
// 				// we DONT cross fade it. Instead, we just create the new 
// 				// slide's item on the static root and remove the old
// 				// item completly from the slide. Since the static root
// 				// doesnt get faded, it will just "show thru." And since
// 				// we flag this item as a consumed duplicate, it doesn't 
// 				// get added to the live root till endTransition() is called
// 				AbstractItem * newItem = newSlideKeys[key];
// 				createVisualDelegate(newItem,m_staticRoot);
// 				//consumedItems << newItem;
// 				//consumedItemNames << newItem->itemName();
// 				
// 				//qDebug() << "MyGraphicsScene::setSlide(): Reparenting: Removing old";
// 				x->setVisible(false);
// 				x->setParentItem(0);
// 				removeItem(x);
// 				
// 				m_content.removeAll(x);
// 				disconnect(x, 0, 0, 0);
// 				x->dispose(false);
// 				
// 				qDebug() << "MyGraphicsScene::setSlide(): Reparenting: Consuming key:"<<key;
// 				consumedDuplicates << key;
// 				//duplicateKeys[key] = true;
// 			}
// 			else
// 			{
// 				qDebug() << "MyGraphicsScene::setSlide(): Reparenting: Not duplicate, moving to fade root";
				x->setParentItem(m_fadeRoot);
//			}
		}
			
// 		if(DEBUG_MYGRAPHICSSCENE)
// 			qDebug() << "MyGraphicsScene::setSlide(): Done reparenting.";

		m_fadeRoot->setZValue(200);
		
		
		// start with faderoot fully visible, and live root invisible, then cross fade between the two
		// UPDATE: flip faderoot behind now.
		
		// With live root now "in front of" the fade root (stuff that should fade out)
		// it is actually the live root that is fading in. This has the advantage of being able to 
		// fade in a slide with nothing else behind it - which is a nice effect.

		m_fadeStepCounter = 0;
		m_fadeSteps = quality; //15;
		//int ms = 250  / m_fadeSteps;
		int ms = speed / m_fadeSteps;
		m_fadeTimer->start(ms); //ms);
		
		m_fadeFrameMs = ms;
		m_fadeLength = speed;
		m_currentFadeTime = 0;
		//m_fadeTime.start();
		m_fadeClockStarted = false;
		m_lastElapsed = -1;

 		double inc = (double)1 / m_fadeSteps;

		
				
		#if QT46_OPAC_ENAB > 0
			qSetEffectOpacity(m_fadeRoot,1);
			qSetEffectOpacity(m_liveRoot,0);
		#else
			m_fadeRoot->setOpacity(1);
			m_liveRoot->setOpacity(0);
		#endif

		emit crossFadeStarted(m_slide,slide);
		if(DEBUG_MYGRAPHICSSCENE)
			qDebug() << "MyGraphicsScene::setSlide(): "<<this<<" Starting fade timer for"<<ms<<"ms"<<"/frame, inc:"<<inc<<", steps:"<<m_fadeSteps<<" ( speed:"<<speed<<", quality:"<<quality<<") at "<<QTime::currentTime().msec();
		
	}

	m_slidePrev = m_slide;
	m_slide = slide;
	
	// force creation of bg if doesnt exist
	BackgroundItem * bgTmp = dynamic_cast<BackgroundItem*>(m_slide->background());
	BackgroundItem * originalBg = 0;
	if(bgTmp && bgTmp->fillType() != AbstractVisualItem::None)
		originalBg = bgTmp;
	
 	double baseZValue = 0;
	QList<AbstractItem *> items = m_slide->itemList();
	foreach(AbstractItem *item, items)
	{
		assert(item != NULL);
		
		AbstractContent * visual = createVisualDelegate(item);
		//determine max zvalue for use in rebasing overlay items
		if(visual && visual->zValue() > baseZValue)
			baseZValue = visual->zValue();
	}
	

	if(m_masterSlide)
	{
		//qDebug() << "MyGraphicsScene::setSlide(): using master ptr:"<<PTRS(m_masterSlide);
		bool secondaryBg = false;
		
		QList<AbstractItem *> items = m_masterSlide->itemList();
		foreach(AbstractItem *item, items)
		{
			assert(item != NULL);
			
			BackgroundItem * bg = dynamic_cast<BackgroundItem*>(item);
			if(bg)
			{
				continue;
			}
			else
			{
				AbstractContent * content = createVisualDelegate(item);
				applyMasterSlideItemFlags(content);

				// rebase zvalue so everything in this slide is on top of the original slide
				if(content && baseZValue!=0)
					content->setZValue(content->zValue() + baseZValue);
			}
				
			//qDebug() << "MyGraphicsScene::setSlide(): Added Master Item: "<<item->itemName()<<", Z: "<<content->zValue();
		}
		
		if(items.isEmpty())
		{
			//qDebug() << "MyGraphicsScene::setSlide(): Master Item List EMPTY";
		}
	}
	else
	{
		//qDebug() << "MyGraphicsScene::setSlide(): NO MASTER SLIDE";
	}

	

// 	m_fadeRoot->setOpacity(1);
// 	m_liveRoot->setOpacity(0);
	m_liveRoot->setZValue(300);
	
	if(!crossFading)
	{
		//qDebug() << "MyGraphicsScene::setSlide(): "<<this<<" emitting transitionFinished()"; 
		emit transitionFinished(slide);
	}
	
	if(DEBUG_MYGRAPHICSSCENE)
		qDebug() << "MyGraphicsScene::setSlide(): "<<this<<" Setting slide # "<<slide->slideNumber()<<" - DONE.";
}

QList<AbstractContent *> MyGraphicsScene::abstractContent(bool onlyMasterItems)
{
	QList<AbstractContent *> newList;
	foreach(AbstractContent * item, m_content)
	{
		QVariant masterFlag = item->property("flag_fromMaster");
		if(onlyMasterItems)
		{
			if(!masterFlag.isNull() && masterFlag.toBool())
				newList << item;
		}
		else
		{
			if(masterFlag.isNull() || !masterFlag.toBool())
				newList << item;
		}
	}
	
	return newList;
}

void MyGraphicsScene::applyMasterSlideItemFlags(AbstractContent *content)
{
	if(!content)
		return;
		
	// customize item's behavior
	content->setFlag(QGraphicsItem::ItemIsMovable, false);
	content->setFlag(QGraphicsItem::ItemIsFocusable, false);
	content->setFlag(QGraphicsItem::ItemIsSelectable, false);
	content->setAcceptHoverEvents(false);
	content->setProperty("flag_fromMaster",true);
	if(m_contextHint == Editor)
		content->setToolTip("Master Slide Item - Double-Click to Edit Master Slide");
	
	connect(content, SIGNAL(doubleClicked(AbstractContent*)), this, SLOT(slotItemDoubleClicked(AbstractContent*)));
}

void MyGraphicsScene::slotItemDoubleClicked(AbstractContent *item)
{
	emit itemDoubleClicked(item);
}

bool MyGraphicsScene::isDataLoadComplete()
{
	foreach(AbstractContent *item, m_content)
		if(!item->isDataLoadComplete())
			return false;
	return true;
}

AbstractContent * MyGraphicsScene::createVisualDelegate(AbstractItem *item, QGraphicsItem * parent)
{
	if(!parent)
		parent = m_liveRoot;
	if (AbstractVisualItem * visualItem = dynamic_cast<AbstractVisualItem *>(item))
	{
		if(DEBUG_MYGRAPHICSSCENE)
			qDebug() << "MyGraphicsScene::createVisualDelegate(): "<<this<<" Creating new content item from:"<<visualItem->itemName();
		AbstractContent * visual = visualItem->createDelegate(this,m_liveRoot);
		addContent(visual, true);
		
		// We store the background item purely for the sake of keeping it on the bottom when 
		// another item requests a re-ordering of the content stack
		if(visualItem->itemClass() == BackgroundItem::ItemClass)
			m_bg = dynamic_cast<BackgroundItem*>(visualItem);
			
		//addItem(visual);
		//visual->setParentItem(m_liveRoot);
		//visual->setAnimationState(AbstractContent::AnimStop);
		
		return visual;
	}
	
	return 0;
}

void MyGraphicsScene::startTransition()
{
}

bool MyGraphicsScene::isTransitionActive()
{
	return m_fadeTimer ? m_fadeTimer->isActive() : false;
}

void MyGraphicsScene::endTransition()
{
	m_fadeTimer->stop();
	
	if(DEBUG_MYGRAPHICSSCENE)
		qDebug() << "MyGraphicsScene::endTransition(): "<<this<<" mark";

	#if QT46_OPAC_ENAB > 0
		qSetEffectOpacity(m_fadeRoot,0);
		qSetEffectOpacity(m_liveRoot,1);
	#else
		m_fadeRoot->setOpacity(0);
		m_liveRoot->setOpacity(1);
	#endif
	
	QList<QGraphicsItem*> kids = m_staticRoot->childItems();
	foreach(QGraphicsItem *k, kids)
		k->setParentItem(m_liveRoot);
	
	kids = m_fadeRoot->childItems();
	foreach(QGraphicsItem *k, kids)
	{
		k->setVisible(false);
		k->setParentItem(0);
		removeItem(k);
		
		AbstractContent *z = dynamic_cast<AbstractContent*>(k);
		if(z)
		{
			m_content.removeAll(z);
			disconnect(z, 0, 0, 0);
			if(DEBUG_MYGRAPHICSSCENE_ITEM_MGMT)
				qDebug() << "MyGraphicsScene::endTransition(): Disposing of item: "<<z->modelItem()->itemName();
			z->dispose(false);
		}
	}
	
	emit crossFadeFinished(m_slidePrev,m_slide);
	emit slideDiscarded(m_slidePrev);
	//qDebug() << "MyGraphicsScene::endTransition(): "<<this<<" emitting transitionFinished()";
	emit transitionFinished(m_slide);
	update();
	
}

void MyGraphicsScene::slotTransitionStep()
{
	// We dont start the fade clock until the first slot is hit so that any first-frame initalization (like loading images)
	// doesnt eat up the fade clock and cause us to make visually unappealing jumps in opacity (like from 0 to 0.5 or whatever)
	if(!m_fadeClockStarted)
	{
		m_fadeClockStarted = true;
		m_fadeTime.start();
	}
	
	int elapsed = m_fadeTime.elapsed();
	if(m_lastElapsed > 0)
	{
		// Try to adjust for something that slowed down the fade process by stopping and resuming the timer
		
		int elapsedDelta = elapsed - m_lastElapsed;
		int interval = m_fadeTimer->interval();
		if(elapsedDelta > interval * 4) // we skipped something, reset and adjust to last elapsed
		{
			int orig = m_fadeLength;
			int moreTime = elapsedDelta * .9; // magic number
			m_fadeLength += moreTime;
			//m_fadeTime.start();
			qDebug() << "ERROR: Skipped by "<<elapsedDelta<<"ms (interval*2:"<<(interval*2)<<"), adding "<<moreTime<<"ms to "<<orig<<"ms, new fadeLength: "<<m_fadeLength<<"ms"; 
		}
	}
	
	
	m_lastElapsed = elapsed;
	
	double fadeVal = ((double)elapsed) / ((double)m_fadeLength); 
		
	if( /*++ m_fadeStepCounter < m_fadeSteps && */elapsed < m_fadeLength)
	{
		//if(DEBUG_MYGRAPHICSSCENE)
		//	qDebug()<<"MyGraphicsScene::slotTransitionStep(): [STEP BEGIN] step"<<m_fadeStepCounter<<"/"<<m_fadeSteps;
		
// 		m_fadeFrameMs = ms;
// 		m_fadeLength = speed;
// 		m_currentFadeTime = 0;
// 		m_fadeTime.start();
		
		//double inc = (double)1 / m_fadeSteps;
		if(!m_bg || m_bg->fillType() == AbstractVisualItem::None)
			m_fadeRoot->setOpacity(1.0 - fadeVal); //m_fadeRoot->opacity() - inc);
		#if QT46_OPAC_ENAB > 0
			QGraphicsOpacityEffect * opac = dynamic_cast<QGraphicsOpacityEffect*>(m_liveRoot->graphicsEffect()); 
			if(opac) 
				opac->setOpacity(fadeVal); //opac->opacity() + inc);
		#else
			m_liveRoot->setOpacity(fadeVal); //m_liveRoot->opacity() + inc);
		#endif
		if(DEBUG_MYGRAPHICSSCENE)
		//step"<<m_fadeStepCounter<<"/"<<m_fadeSteps<<", 
			qDebug()<<"MyGraphicsScene::slotTransitionStep(): "<<this<<" [STEP DONE] fadeVal:"<<fadeVal<<", fade:"<<m_fadeRoot->opacity()<<", live:"<<m_liveRoot->opacity()<<" [m_fadeFrameMs:"<<m_fadeFrameMs<<", elapsed:"<<elapsed<<"] at "<<QTime::currentTime().msec();
			//qDebug()<<"MyGraphicsScene::slotTransitionStep(): [STEP DONE] step"<<m_fadeStepCounter<<"/"<<m_fadeSteps<<", inc:"<<inc<<", fade:"<<m_fadeRoot->opacity()<<", live:"<<m_liveRoot->opacity();
		update();
	}
	else
	{
		endTransition();
	}

	//if(DEBUG_MYGRAPHICSSCENE)
	//	qDebug()<<"MyGraphicsScene::slotTransitionStep(): [FUNCTION DONE]";
	
}

void MyGraphicsScene::addContent(AbstractContent * content, bool takeOwnership) // const QPoint & pos)
{
	if(m_content.contains(content))
	{
		if(DEBUG_MYGRAPHICSSCENE_ITEM_MGMT)
			qDebug() << "MyGraphicsScene::addContent(): [2] Showing item: "<<content->modelItem()->itemName();
		content->show();
		return;
	}
		
	connect(content, SIGNAL(configureMe(const QPoint &)), this, SLOT(slotConfigureContent(const QPoint &)));
	//connect(content, SIGNAL(backgroundMe()), this, SLOT(slotBackgroundContent()));
	connect(content, SIGNAL(changeStack(int)), this, SLOT(slotStackContent(int)));
	connect(content, SIGNAL(deleteItem()), this, SLOT(slotDeleteContent()));
	/*
	if (!pos.isNull())
		content->setPos(pos);
	//content->setCacheMode(QGraphicsItem::DeviceCoordinateCache);*/
// 	if(content->zValue() == 0)
// 	{
// 		//QList<QGraphicsItem*> kids = m_liveRoot->childItems();
// 		
// 		double zMax = 1;
// 		qDebug() << "Add content: found "<<m_content.size()<<" kids";
// 		foreach(AbstractContent *item, m_content)
// 		{
// 			qDebug() << "item->zValue():"<<item->modelItem()->zValue();
// 			if(item->modelItem()->zValue() > zMax)
// 				zMax = item->modelItem()->zValue();
// 		}
// 		qDebug() << "Add content: zMax is now: "<<zMax;
// 		content->modelItem()->setZValue(zMax);
// 		content->setZValue(zMax);
// 	}
	if(DEBUG_MYGRAPHICSSCENE_ITEM_MGMT)
		qDebug() << "MyGraphicsScene::addContent(): [1] Showing item: "<<content->modelItem()->itemName();
	content->show();
	
	m_content.append(content);
	//addItem(content);
	
	if(takeOwnership)
		m_ownedContent.append(content);
}

void MyGraphicsScene::setSceneRect(const QRectF& rect)
{
	QGraphicsScene::setSceneRect(rect);
	emit sceneRectChanged(rect);
}

static QPoint nearCenter(const QRectF & rect)
{
    return rect.center().toPoint() + QPoint(2 - (qrand() % 5), 2 - (qrand() % 5));
}
/*
TextContent * MyGraphicsScene::addTextContent()
{
	return createText(nearCenter(sceneRect()));
}*/


AbstractVisualItem * MyGraphicsScene::newTextItem(QString text)
{
	TextBoxItem *t = new TextBoxItem();
	if(!m_slide)
		return 0;
	
	// text should automatically enlarge the rect
	t->setContentsRect(QRect(0,0,400,10));
	t->setText(text);
	t->setPos(nearCenter(sceneRect()));
	t->setItemId(ItemFactory::nextId());
	t->setItemName(QString("TextBoxItem%1").arg(t->itemId()));
	t->changeFontSize(52.0);
	t->setShadowBlurRadius(7.0);
	t->setShadowEnabled(true);
	t->setShadowOffsetX(1.5);
	t->setShadowOffsetX(3.0);
	t->setOutlinePen(QPen(Qt::black,1.5));
	t->setOutlineEnabled(true);
	t->setZValue(maxZValue());
	
	m_slide->addItem(t); //m_slide->createText();
	

	return t;
}


AbstractVisualItem * MyGraphicsScene::newBoxItem()
{
	BoxItem *t = new BoxItem();
	if(!m_slide)
		return 0;
	
	//t->setText(text);
	t->setPos(nearCenter(sceneRect()));
	t->setItemId(ItemFactory::nextId());
	t->setItemName(QString("BoxItem%1").arg(t->itemId()));
	t->setZValue(maxZValue());
	
	m_slide->addItem(t); //m_slide->createText();
	

	return t;
}

AbstractVisualItem * MyGraphicsScene::newVideoItem()
{
	VideoFileItem *t = new VideoFileItem();
	if(!m_slide)
		return 0;
	t->setFilename("data/Seasons_Loop_3_SD.mpg");
	
	//t->setText(text);
	t->setPos(nearCenter(sceneRect()));
	t->setItemId(ItemFactory::nextId());
	t->setItemName(QString("VideoItem%1").arg(t->itemId()));
	t->setZValue(maxZValue());
	
	m_slide->addItem(t); //m_slide->createText();
	

	return t;
}

AbstractVisualItem * MyGraphicsScene::newImageItem()
{
	ImageItem *t = new ImageItem();
	if(!m_slide)
		return 0;
	
	t->setPos(nearCenter(sceneRect()));
	t->setItemId(ItemFactory::nextId());
	t->setItemName(QString("ImageItem%1").arg(t->itemId()));
	t->setZValue(maxZValue());
	
	m_slide->addItem(t);
	
	
	return t;
}

AbstractVisualItem * MyGraphicsScene::newOutputView()
{
	OutputViewItem *t = new OutputViewItem();
	if(!m_slide)
		return 0;
	
	t->setPos(nearCenter(sceneRect()));
	t->setItemId(ItemFactory::nextId());
	t->setItemName(QString("OutputViewItem%1").arg(t->itemId()));
	t->setZValue(maxZValue());
	
	m_slide->addItem(t);
	
	
	return t;
}

int MyGraphicsScene::maxZValue()
{
	//qDebug()<< "newImageItem: m_content.size():"<<m_content.size();
	int max = 0;
	foreach(AbstractContent *c, m_content)
		if(c->zValue() > max)
			max = c->zValue();
	//qDebug()<<"newImageItem: max:"<<max;
	return max+1;
}

void MyGraphicsScene::copyCurrentSelection(bool removeSelection)
{
	QList<QGraphicsItem *> selection = selectedItems();
		
	// Since the items are cloned when added to the copy buffer,
	// AND cloned when pasted, its safe to delete them from memory
	// when replacing the buffer
	if(m_copyBuffer.size())
		qDeleteAll(m_copyBuffer);
	m_copyBuffer.clear();
		
		
	foreach(QGraphicsItem *item, selection)
	{
		AbstractContent * content = dynamic_cast<AbstractContent *>(item);
		if(content)
		{
			AbstractItem * model = content->modelItem();
			if(!model)
				continue;
				
			m_copyBuffer << model->clone();
			
			// this will add it to the undo stack and remove it from the scene
			if(removeSelection && m_slide)
				m_slide->removeItem(model);
		}
	}
}

void MyGraphicsScene::pasteCopyBuffer()
{
	if(!m_copyBuffer.size())
		return;
	
	if(!m_slide)
		return;
	
	clearSelection();
	
	foreach(AbstractItem *item, m_copyBuffer)
	{
		if(!item)
			continue;
		AbstractItem * clone = item->clone();
		if(!clone)
			continue;
		m_slide->addItem(clone);
		
		// last visual item, not last AbstractItem
		m_content.last()->setSelected(true);
	}
}

void MyGraphicsScene::selectAll()
{
	// TODO is clearSelection() neeeded?
	clearSelection();
	QList<QGraphicsItem*> kids = items();
	foreach(QGraphicsItem *item, kids)
		item->setSelected(true);
}

bool MyGraphicsScene::eventFilter(QObject *obj, QEvent *event)
{
// 	if (event->type() == QEvent::KeyPress) {
// 		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
// 		qDebug("Ate key press %d", keyEvent->key());
// 		return true;
// 	} else {
// 		// standard event processing
// 		return QObject::eventFilter(obj, event);
// 	}
	return QObject::eventFilter(obj, event);
}
     
void MyGraphicsScene::keyPressEvent(QKeyEvent * event)
{
	if(DEBUG_KEYHANDLER)
		qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key();
	if(event->modifiers() & Qt::ControlModifier)
	{
		if(DEBUG_KEYHANDLER)
			qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 1";
		switch(event->key())
		{
			case Qt::Key_C:
				copyCurrentSelection();
				event->accept();
				break;
				
			case Qt::Key_X:
				copyCurrentSelection(true);
				event->accept();
				break;
				
			case Qt::Key_V:
				pasteCopyBuffer();
				event->accept();
				break;
				
			case Qt::Key_A:
				selectAll();
				event->accept();
				break;
				
			default:
				if(DEBUG_KEYHANDLER)
					qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 1: fall thru, no key";
				break;
		}
	}
	else
	{
		
		if(DEBUG_KEYHANDLER)
			qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 1, end of path, accepted?"<<event->isAccepted();
		
		if(DEBUG_KEYHANDLER)
			qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2";	
		QSizeF grid = AppSettings::gridSize();
		// snap to half a grid point - the content->flagKeyboardMotivatedMovement() call tells AppSettings::snapToGrid()
		// in AbstractContent to allow it to be half a grid point
		qreal x = grid.width();///2;
		qreal y = grid.height();///2;
		
		// arbitrary magic numbers - no significance, just random preference
		if(x<=0)
			x = 5;
		if(y<=5)
			y = 5;
		
		QList<QGraphicsItem *> selection = selectedItems();
		if(DEBUG_KEYHANDLER)
			qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2, selection size:"<<selection.size();
		if(selection.size() > 0)
		{
			
			if(DEBUG_KEYHANDLER)
				qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2"; //, first content model item name:"<<content->modelItem()->itemName();
			switch(event->key())
			{
				case Qt::Key_Delete:
					slotDeleteContent();
					event->accept();
					break;
				case Qt::Key_Up:
					if(DEBUG_KEYHANDLER)
						qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2: move up:"<<y;
					foreach(QGraphicsItem *item, selection)
					{
						AbstractContent * content = dynamic_cast<AbstractContent *>(item);
						if(content->modelItem()->itemClass() != BackgroundItem::ItemClass)
						{
							content->flagKeyboardMotivatedMovement();
							content->moveBy(0,-y);
							content->syncToModelItem(0);
						}
					}
					event->accept();
					break;
				case Qt::Key_Down:
					if(DEBUG_KEYHANDLER)
						qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2: move down:"<<y;
					foreach(QGraphicsItem *item, selection)
					{
						AbstractContent * content = dynamic_cast<AbstractContent *>(item);
						if(content->modelItem()->itemClass() != BackgroundItem::ItemClass)
						{
							content->flagKeyboardMotivatedMovement();
							content->moveBy(0,+y);
							content->syncToModelItem(0);
						}
					}
					event->accept();
					break;
				case Qt::Key_Left:
					if(DEBUG_KEYHANDLER)
						qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2: move left:"<<x;
					foreach(QGraphicsItem *item, selection)
					{
						AbstractContent * content = dynamic_cast<AbstractContent *>(item);
						if(content->modelItem()->itemClass() != BackgroundItem::ItemClass)
						{
							content->flagKeyboardMotivatedMovement();
							content->moveBy(-x,0);
							content->syncToModelItem(0);
						}
					}
					event->accept();
					break;
				case Qt::Key_Right:
					if(DEBUG_KEYHANDLER)
						qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2: move right:"<<x;
					foreach(QGraphicsItem *item, selection)
					{
						AbstractContent * content = dynamic_cast<AbstractContent *>(item);
						if(content->modelItem()->itemClass() != BackgroundItem::ItemClass)
						{
							content->flagKeyboardMotivatedMovement();
							content->moveBy(+x,0);
							content->syncToModelItem(0);
						}
					}
					event->accept();
					break;
				case Qt::Key_F4:
				case Qt::Key_Space:
				//case Qt::Key_Enter:
					if(DEBUG_KEYHANDLER)
						qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2: config content key";
					foreach(QGraphicsItem *item, selection)
					{
						AbstractContent * content = dynamic_cast<AbstractContent *>(item);
						configureContent(content);
					}
					event->accept();
					break;
				default:
					if(DEBUG_KEYHANDLER)
						qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2: fall thru, no key";
					break;
			}
		}
		
		if(DEBUG_KEYHANDLER)
			qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2, end of path, accepted?"<<event->isAccepted();
		
	}
	
	if(!event->isAccepted())
	{
		if(DEBUG_KEYHANDLER)
			qDebug() << "MyGraphicsScene::keyPressEvent(): key:"<<event->key()<<", default - event not accepted, sending to parent";
		QGraphicsScene::keyPressEvent(event);
	}
}

/// Slots
void MyGraphicsScene::slotSelectionChanged()
{
	// show the config widget if 1 AbstractContent is selected
	QList<QGraphicsItem *> selection = selectedItems();
	if (selection.size() == 1) 
	{
		AbstractContent * content = dynamic_cast<AbstractContent *>(selection.first());
		if (content) 
		{
			QWidget * pWidget = content->createPropertyWidget();
			if (pWidget)
				pWidget->setWindowTitle(tr("%1").arg(content->contentName().toUpper()));
			emit showPropertiesWidget(pWidget);
			return;
		}
	}
	
	// show a 'selection' properties widget
	if (selection.size() > 1) 
	{
		QLabel * label = new QLabel(tr("%1 objects selected").arg(selection.size()));
		label->setWindowTitle(tr("SELECTION"));
		emit showPropertiesWidget(label);
		return;
	}
	
	// or don't show anything
	emit showPropertiesWidget(0);
}

void MyGraphicsScene::slotConfigureContent(const QPoint & /*scenePoint*/)
{
	clearSelection();
	
	// get the content and ensure it hasn't already a property window
	AbstractContent * content = dynamic_cast<AbstractContent *>(sender());
// 	foreach (GenericItemConfig * config, m_configs) 
// 	{
// // 		if (config->content() == content)
// // 			return;
// 		// force only 1 property instance
// 		//slotDeleteConfig(config);
// 	}
	
	configureContent(content);
}

void MyGraphicsScene::configureContent(AbstractContent *content)
{

	GenericItemConfig * p = 0;
	
	if (TextBoxContent * text = dynamic_cast<TextBoxContent *>(content))
		p = new TextBoxConfig(text);
	else
	if (OutputViewContent * text = dynamic_cast<OutputViewContent *>(content))
		p = new OutputViewConfig(text);
	else
	if (VideoFileContent * vid = dynamic_cast<VideoFileContent *>(content))
		p = new VideoFileConfig(vid);
	else
	if (BackgroundContent * bg = dynamic_cast<BackgroundContent *>(content))
		p = new BackgroundConfig(bg);
	else
	if (BoxContent * box = dynamic_cast<BoxContent *>(content))
		p = new BoxConfig(box);
	else
	if (ImageContent * box = dynamic_cast<ImageContent *>(content))
		p = new ImageConfig(box);
	
	// generic config
	if (!p)
		p = new GenericItemConfig(content);
	
	// common links
	//m_configs.append(p);
	
	//connect(p, SIGNAL(applyLook(quint32,bool,bool)), this, SLOT(slotApplyLook(quint32,bool,bool)));
	//p->setScene(this);
	p->show();
	
	//QDialog * d = p->toDialog();
	//d->show();

}

/*
void MyGraphicsScene::slotBackgroundContent()
{
    setBackContent(dynamic_cast<AbstractContent *>(sender()));
}*/

void MyGraphicsScene::slotStackContent(int op)
{
	AbstractContent * content = dynamic_cast<AbstractContent *>(sender());
	if (!content || m_content.size() < 2)
		return;
	int size = m_content.size();
	int index = m_content.indexOf(content);
	
	// find out insertion indexes over the stacked items
	QList<QGraphicsItem *> stackedItems = items(content->sceneBoundingRect(), Qt::IntersectsItemShape);
	int prevIndex = 0;
	int nextIndex = size - 1;
	foreach (QGraphicsItem * item, stackedItems) 
	{
		// operate only on different Content
		AbstractContent * c = dynamic_cast<AbstractContent *>(item);
		if (!c || c == content)
			continue;
	
		// refine previous/next indexes (close to 'index')
		int cIdx = m_content.indexOf(c);
		if (cIdx < nextIndex && cIdx > index)
			nextIndex = cIdx;
		else if (cIdx > prevIndex && cIdx < index)
			prevIndex = cIdx;
	}
	
	// move items
	switch (op) 
	{
		case 1: // front
			m_content.append(m_content.takeAt(index));
			break;
		case 2: // raise
			if (index >= size - 1)
				return;
			m_content.insert(nextIndex, m_content.takeAt(index));
			break;
		case 3: // lower
			if (index <= 0)
				return;
			m_content.insert(prevIndex, m_content.takeAt(index));
			break;
		case 4: // back
			m_content.prepend(m_content.takeAt(index));
			break;
	}
	
	// reassign z-levels
	int z = 1;
	foreach (AbstractContent * content, m_content)
		content->modelItem()->setZValue(z++);
	
	m_bg->setZValue(-999);
}

static QList<AbstractContent *> content(const QList<QGraphicsItem *> & items) 
{
	QList<AbstractContent *> contentList;
	foreach (QGraphicsItem * item, items) 
	{
		AbstractContent * c = dynamic_cast<AbstractContent *>(item);
		if (c)
			contentList.append(c);
	}
	return contentList;
}


void MyGraphicsScene::slotDeleteContent()
{
	QList<AbstractContent *> selectedContent = content(selectedItems());
	AbstractContent * senderContent = dynamic_cast<AbstractContent *>(sender());
	if (senderContent && !selectedContent.contains(senderContent)) 
	{
		selectedContent.clear();
		selectedContent.append(senderContent);
	}
	if (selectedContent.size() > 1)
		if (QMessageBox::question(0, tr("Delete content"), tr("All the %1 selected content will be deleted, do you want to continue ?").arg(selectedContent.size()), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
		return;
	
	foreach (AbstractContent * content, selectedContent) 
	{
	
		// unset background if deleting its content
// 		if (m_backContent == content)
// 			setBackContent(0);
	
// 		// remove related property if deleting its content
// 		foreach (GenericItemConfig * config, m_configs) {
// 			if (config->content() == content) 
// 			{
// 				slotDeleteConfig(config);
// 				break;
// 			}
// 		}
	
		// unlink content from lists, myself(the Scene) and memory
		m_content.removeAll(content);
		m_slide->removeItem(content->modelItem());
		if(DEBUG_MYGRAPHICSSCENE_ITEM_MGMT)
			qDebug() << "MyGraphicsScene::slotDeleteContent(): Disposing of item: "<<content->modelItem()->itemName();
		content->dispose();
		//delete content;
	}
}

void MyGraphicsScene::slotDeleteConfig(GenericItemConfig * config)
{
// 	m_configs.removeAll(config);
// 	config->dispose();
// 	update();
}


void MyGraphicsScene::slotApplyLook(quint32 frameClass, bool mirrored, bool all)
{
	QList<AbstractContent *> selectedContent = content(selectedItems());
	foreach (AbstractContent * content, m_content) 
	{
		if (all || selectedContent.contains(content)) 
		{
			if (content->frameClass() != frameClass)
				content->setFrame(FrameFactory::createFrame(frameClass));
			content->setMirrorEnabled(mirrored);
		}
	}
}


	
	
// Only relevant AFTER a slide has been set on the instance.
QList<QWidget*> MyGraphicsScene::controlWidgets()
{
	QList<QWidget*> widgets;
	foreach(AbstractContent *item, m_content)
	{
		if(item->parentItem() == m_liveRoot)
		{
			QWidget * widget = item->controlWidget();
			
			if(widget)
				widgets << widget;
		}
	}
	
	return widgets;
}


bool MyGraphicsScene::startBackgroundVideoPausedInPreview()
{
	if(!m_bg)
		return false;
	AbstractContent * content = findVisualDelegate(m_bg);
	if(!content)
		return false;
	BackgroundContent * background = dynamic_cast<BackgroundContent*>(content);
	if(!background)
		return false;
	return background->startVideoPausedInPreview();
}

void MyGraphicsScene::setStartBackgroundVideoPausedInPreview(bool flag)
{
	if(!m_bg)
		return;
	AbstractContent * content = findVisualDelegate(m_bg);
	if(!content)
		return;
	BackgroundContent * background = dynamic_cast<BackgroundContent*>(content);
	if(!background)
		return;
	background->setStartVideoPausedInPreview(flag);
}
