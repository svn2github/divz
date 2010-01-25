#include "OutputViewContent.h"
#include "frames/Frame.h"
// #include "items/BezierCubicItem.h"
#include "items/TextProperties.h"
#include "model/BoxItem.h"
#include "items/CornerItem.h"
// #include "CPixmap.h"
#include "RenderOpts.h"
#include <QDebug>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QMimeData>
#include <QPainter>
#include <QTextDocument>
#include <QTextFrame>
#include <QUrl>
#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QPixmapCache>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QMessageBox>

#include "model/OutputViewItem.h"
#include "OutputInstance.h"
#include "AppSettings.h"
#include "MainWindow.h"
#include "model/Output.h"
#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/BackgroundItem.h"

#include "viewer/NetworkClient.h"

#if QT_VERSION >= 0x040600
	#define QT46_SHADOW_ENAB 0
#endif

#include <QMovie>
#include <QLabel>

#define DEBUG_OUTPUTVIEW 0

#define RECONNECT_WAIT_TIME 1000 * 2
	
OutputViewRootObject::OutputViewRootObject(QGraphicsItem *parent, QGraphicsScene*)
	: QGraphicsItem(parent)
{
	setPos(0,0);
}

Slide * OutputViewContent::m_blackSlide = 0;
int OutputViewContent::m_blackSlideRefCount = 0;

OutputViewContent::OutputViewContent(QGraphicsScene * scene, QGraphicsItem * parent)
	: AbstractContent(scene, parent, false)
	, m_inst(0)
	, m_slide(0)
	, m_group(0)
	, m_fadeTimer(0)
	, m_slidePreBlack(0)
	, m_fadeSpeed(-1)
	, m_fadeQuality(-1)
	, m_fadeBehind(false)
	, m_client(0)
{
	m_dontSyncToModel = true;
	
	m_blackSlideRefCount++;
	
	setFrame(0);
	setFrameTextEnabled(false);
	setToolTip(tr("Output Viewer - right click for options."));
	
// 	for(int i=0;i<m_cornerItems.size();i++)
// 		m_cornerItems.at(i)->setDefaultLeftOp(CornerItem::Scale);

// 	m_viewer = new OutputInstance(Output::widgetInstance());
// 	m_viewer->setCanZoom(false);
// 	m_viewer->forceGLDisabled(true);
// 	
// 	m_widgetProxy = new QGraphicsProxyWidget(this);
// 	m_widgetProxy->setWidget(m_viewer);

	
	m_fakeInst = new OutputViewInst(this);
	m_liveRoot = new OutputViewRootObject(this,0);
	m_fadeRoot = new OutputViewRootObject(this,0);
	if(DEBUG_OUTPUTVIEW)
		qDebug() << "OutputViewContent:: created m_liveRoot:"<<(void*)m_liveRoot<<", m_fadeRoot:"<<(void*)m_fadeRoot<<", this:"<<(void*)this;
	
	// Used to reconnect torn sockets when viewed with a network viewer
	m_reconnectTimer.setSingleShot(true);
	connect(&m_reconnectTimer, SIGNAL(timeout()), this, SLOT(connectNetworkClient()));
	
	m_dontSyncToModel = false;
}

OutputViewContent::~OutputViewContent()
{
}

void OutputViewContent::dispose(bool flag)
{
	// In the QGraphicsProxyWidget, it deletes the widget -
	// since we're using a static global instance of the
	// widget, we dont want it destroyed! So 'remove' it.
// 	m_widgetProxy->setWidget(0);
	if(m_inst)
		m_inst->removeMirror(m_fakeInst);
	
	m_fakeInst->deleteLater();
	m_fakeInst = 0;
	
	m_blackSlideRefCount --;
	if(m_blackSlideRefCount <= 0 && m_blackSlide)
	{
		delete m_blackSlide;
		m_blackSlide = 0;
		qDebug() << "OutputViewContent: Deleting black slide";
	}
	
	AbstractContent::dispose(flag);
}

void OutputViewContent::setSlideGroup(SlideGroup *group, Slide *slide)
{
	m_group = group;
	setSlide(slide);
}


void OutputViewContent::addContent(AbstractContent * content, bool takeOwnership) // const QPoint & pos)
{
	content->applySceneContextHint(MyGraphicsScene::Live);
	if(m_content.contains(content))
	{
		content->show();
		return;
	}

	content->show();
	
	m_content.append(content);
	//addItem(content);
	
	if(takeOwnership)
		m_ownedContent.append(content);
}


AbstractContent * OutputViewContent::createVisualDelegate(AbstractItem *item, QGraphicsItem * parent)
{
// 	if(!parent)
// 		parent = m_liveRoot;
	if (AbstractVisualItem * visualItem = dynamic_cast<AbstractVisualItem *>(item))
	{
		if(DEBUG_OUTPUTVIEW)
			qDebug() << "OutputViewContent::createVisualDelegate(): Creating new content item from:"<<visualItem->itemName()<<", root:"<<(void*)m_liveRoot;
		AbstractContent * visual = visualItem->createDelegate(0,m_liveRoot);
		addContent(visual, true);
		
		// We store the background item purely for the sake of keeping it on the bottom when 
		// another item requests a re-ordering of the content stack
// 		if(visualItem->itemClass() == BackgroundItem::ItemClass)
// 			m_bg = dynamic_cast<BackgroundItem*>(visualItem);
			
		
		return visual;
	}
	
	return 0;
}


void OutputViewContent::clearSlide()
{

//		qDebug() << "OutputViewContent::setSlide: removing existing content, size:" << m_content.size();
	while(!m_content.isEmpty())
	{
		AbstractContent * content = m_content.takeFirst();
		
		m_content.removeAll(content);
		if(content->parentItem() == m_liveRoot)
			content->setParentItem(0);
		//removeItem(content);
		
		disconnect(content, 0, 0, 0);
		//qDebug() << "Disposing of content";
		content->dispose(false);
		//delete content;
		content = 0;
	}
// 		qDebug() << "OutputViewContent::setSlide: content removed";
	m_slide = 0;
}


void OutputViewContent::slideItemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value, QVariant /*oldValue*/)
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
// 			if(slide == m_masterSlide)
// 			{
// 				applyMasterSlideItemFlags(content);
// 			}
// 			else
// 			{
// 				clearSelection();
// 				content->setSelected(true);
// 			}
		}
	}
	else
	if(operation == "remove")
		removeVisualDelegate(item);
}

AbstractContent * OutputViewContent::findVisualDelegate(AbstractItem *item)
{
	foreach(AbstractContent *z, m_content)
		if(z->modelItem() == item)
			return z;
	return 0;
}

void OutputViewContent::removeVisualDelegate(AbstractItem *item)
{
	//qDebug() << "MyGraphicsScene::removeVisualDelegate: Going to remove: "<<item->itemName();
	AbstractContent *z = findVisualDelegate(item);
	if(!z)
		return;
		
	z->setVisible(false);
	z->setParentItem(0);
	//removeItem(z); // will be removed by dispose
	
	m_content.removeAll(z);
	disconnect(z, 0, 0, 0);
	z->dispose(false);
		
// 	qDebug() << "MyGraphicsScene::removeVisualDelegate: Couldn't find visual delegate for item requested: "<<item->itemName();
// 	fprintf(stderr,"MyGraphicsScene::removeVisualDelegate: Can't find delegate for m_item=%p\n",item);
}


void OutputViewContent::setSlide(Slide *slide, MyGraphicsScene::SlideTransition trans)
{
	
	
// 	clearSlide();
	
	if(m_blackEnabled)//;// && slide != m_blackSlide);
	{
		if(DEBUG_OUTPUTVIEW)
			qDebug() << "OutputViewContent::setSlide(): Not changing to new slide yet because black is enabled, slide="<<slide<<", m_blackEnabled:"<<m_blackEnabled;
		
		m_slidePreBlack = slide;
		return;
	}
	
	setSlideInternal(slide,trans);
}

void OutputViewContent::setSlideInternal(Slide *slide, MyGraphicsScene::SlideTransition trans)
{
	
	if(DEBUG_OUTPUTVIEW)
		qDebug() << "OutputViewContent::setSlide: slide:"<<slide;
		
	if(!slide)
	{
		//clearSlide();
		if(DEBUG_OUTPUTVIEW)
			qDebug() << "OutputViewContent::setSlide: * slide is null, NoOp";
		return;
	}
	
	if(m_slide == slide)
	{
		if(DEBUG_OUTPUTVIEW)
			qDebug("OutputViewContent::setSlide: Not changing slide - same slide!");
		return;
	}
	
	if(m_slide)
		disconnect(m_slide,0,this,0);
	
	if(slide)
		connect(slide,SIGNAL(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)),this,SLOT(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)));
	
	
	if(sceneContextHint() == MyGraphicsScene::StaticPreview)
		trans = MyGraphicsScene::None; 
	
	m_currentTransition = trans;
	
	if(DEBUG_OUTPUTVIEW)
		qDebug() << "OutputViewContent::setSlide(): Setting slide # "<<slide->slideNumber();
		
	m_slide = slide;
	
	
	// start cross fade settings with program settings
	int speed   = AppSettings::crossFadeSpeed();
	int quality = AppSettings::crossFadeQuality();
	//qDebug() << "SlideGroupViewer::setSlideInternal(): [app] speed:"<<speed<<", quality:"<<quality;

	// allow this viewer's cross fade settings to override app settings
	if(m_fadeQuality > -1 || m_fadeSpeed > -1)
	{
		if(m_fadeQuality > -1)
			quality = m_fadeQuality;
		if(m_fadeSpeed > -1)
			speed = m_fadeSpeed;
		//qDebug() << "SlideGroupViewer::setSlideInternal(): [viewer] speed:"<<speed<<", quality:"<<quality;
	}

	// change to slide group if set
	if(m_group && !m_group->inheritFadeSettings())
	{
		speed = m_group->crossFadeSpeed();
		quality = m_group->crossFadeQuality();
		//qDebug() << "SlideGroupViewer::setSlideInternal(): [group] speed:"<<speed<<", quality:"<<quality;
	}

	// and use slide settings if set instead of the other two sets of settings
	if(slide && !slide->inheritFadeSettings())
	{
		// the 0.01 test is due to the fact that I forgot to initalize cross fade speed/quality in the Slide object constructor in
		// older builds (fixed now.) So, if user loads file from older builds, these variables may have corrupted settings, so dont allow overide.
		if(slide->crossFadeSpeed() > 0.01)
			speed = slide->crossFadeSpeed();
		if(slide->crossFadeQuality() > 0.01)
			quality = slide->crossFadeQuality();
		//qDebug() << "SlideGroupViewer::setSlideInternal(): [slide] speed:"<<speed<<", quality:"<<quality;
	}
	
	
	if(trans == MyGraphicsScene::None || (speed == 0 && quality == 0))
	{
		clearSlide();
	}
	else
	{	
// 		if(DEBUG_OUTPUTVIEW)
// 			qDebug() << "OutputViewContent::setSlide(): [final] speed:"<<speed<<", quality:"<<quality;
		
		if(!m_fadeTimer)
		{
			m_fadeTimer = new QTimer(this);
			connect(m_fadeTimer, SIGNAL(timeout()), this, SLOT(slotTransitionStep()));
		}
		
		
		if(m_fadeTimer->isActive())
		{
// 			qDebug() << "OutputViewContent::setSlide(): Timer active, ending existing timer.";
			endTransition(); 
		}
			
 		//if(DEBUG_MYGRAPHICSSCENE)
//  			qDebug() << "OutputViewContent::setSlide(): Reparenting"<<m_content.size()<<"items";
// 		m_staticRoot->setZValue(100);
			
		foreach(AbstractContent *x, m_content)
		{
// 			if(DEBUG_OUTPUTVIEW)
//  				qDebug() << "OutputViewContent::setSlide(): Reparenting item:"<<x;
			x->setParentItem(m_fadeRoot);
			
		}
			
// 		if(DEBUG_MYGRAPHICSSCENE)
 			//qDebug() << "OutputViewContent::setSlide(): Done reparenting.";

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

 		double inc = (double)1 / m_fadeSteps;

		
			
		m_fadeRoot->setOpacity(1);
		m_liveRoot->setOpacity(0);

// 		emit crossFadeStarted(m_slide,slide);
  		if(DEBUG_OUTPUTVIEW)
 			qDebug() << "OutputViewContent::setSlide(): Starting fade timer for"<<ms<<"ms"<<"/frame, inc:"<<inc<<", steps:"<<m_fadeSteps<<" ( speed:"<<speed<<", quality:"<<quality<<")";
		
	
	}
	
	double baseZValue = 0;
	
	QList<AbstractItem *> items = m_slide->itemList();
	foreach(AbstractItem *item, items)
	{
		if(!item)
			continue;
			
		BackgroundItem * bgTmp = dynamic_cast<BackgroundItem*>(item);
		if(bgTmp && bgTmp->fillType() == AbstractVisualItem::None)
		{
			m_fadeBehind = true;
			continue;
		}
				
		AbstractContent * visual = createVisualDelegate(item);
		//determine max zvalue for use in rebasing overlay items
		if(visual && visual->zValue() > baseZValue)
			baseZValue = visual->zValue();

	}
	
	m_liveRoot->setZValue(300);
}


void OutputViewContent::slotTransitionStep()
{
	if( ++ m_fadeStepCounter < m_fadeSteps)
	{
		double inc = (double)1 / m_fadeSteps;
		if(m_fadeBehind)
			m_fadeRoot->setOpacity(m_fadeRoot->opacity() - inc);
		
		m_liveRoot->setOpacity(m_liveRoot->opacity() + inc);
		
		if(DEBUG_OUTPUTVIEW)
			qDebug()<<"OutputViewContent::slotTransitionStep(): step"<<m_fadeStepCounter<<"/"<<m_fadeSteps<<", inc:"<<inc<<", fade:"<<m_fadeRoot->opacity()<<", live:"<<m_liveRoot->opacity();
			
		update();
	}
	else
	{
		endTransition();
	}
	
}



void OutputViewContent::endTransition()
{
	m_fadeTimer->stop();
	m_fadeRoot->setOpacity(0);
	m_liveRoot->setOpacity(1);
	
	QList<QGraphicsItem*> kids = m_fadeRoot->childItems();
	foreach(QGraphicsItem *k, kids)
	{
		k->setVisible(false);
		k->setParentItem(0);
// 		removeItem(k); // will be done in dispose
		
		AbstractContent *z = dynamic_cast<AbstractContent*>(k);
		if(z)
		{
			m_content.removeAll(z);
			disconnect(z, 0, 0, 0);
			z->dispose(false);
		}
		else
		{
// 			k->deleteLater();
			delete k;
		}
	}
	
// 	emit crossFadeFinished(m_slidePrev,m_slide);
// 	emit slideDiscarded(m_slidePrev);
	update();
	
}


void OutputViewContent::fadeBlackFrame(bool enable)
{
	qDebug() << "OutputViewContent::fadeBlackFrame: "<<enable; 
	m_blackEnabled = enable;
	if(enable)
	{
		m_slidePreBlack = m_slide;
		generateBlackFrame();
		
 		qDebug() << "OutputViewContent::fadeBlackFrame: preSlide:"<<m_slide<<", blackSlide:"<<m_blackSlide<<",m_blackEnabled:"<<m_blackEnabled;
		setSlideInternal(m_blackSlide);
 		qDebug() << "OutputViewContent::fadeBlackFrame: done with fade init";
	}
	else
	{
 		qDebug() << "OutputViewContent::fadeBlackFrame: RESTORIUNG preSlide:"<<m_slide<<", blackSlide:"<<m_blackSlide<<",m_blackEnabled:"<<m_blackEnabled;
		setSlideInternal(m_slidePreBlack);
	}
}

void OutputViewContent::generateBlackFrame()
{
	if(!m_blackSlide)
	{
		m_blackSlide = new Slide();
		dynamic_cast<AbstractVisualItem*>(m_blackSlide->background())->setFillType(AbstractVisualItem::None);
		//dynamic_cast<AbstractVisualItem*>(m_blackSlide->background())->setFillBrush(QBrush(Qt::black));
	}
}

Slide * OutputViewContent::blackSlide()
{
	generateBlackFrame();
	return m_blackSlide;
}

void OutputViewContent::setFadeSpeed(int value)
{
	m_fadeSpeed = value;
}

void OutputViewContent::setFadeQuality(int value)
{
	m_fadeQuality = value;
}



QWidget * OutputViewContent::createPropertyWidget()
{
	return 0;
}

void OutputViewContent::syncFromModelItem(AbstractVisualItem *model)
{
	if(!modelItem())
		setModelItem(model);

	AbstractContent::syncFromModelItem(model);
	
	setOutputId(dynamic_cast<OutputViewItem*>(model)->outputId());
	
	//m_viewer->setGeometry(contentsRect());
	
	QRect cr = contentsRect();
	QSize scene = QSize(1024,768);
	if(MainWindow::mw())
		scene = MainWindow::mw()->standardSceneRect().size();
	
	float sx = ((float)cr.width()) / scene.width();
	float sy = ((float)cr.height()) / scene.height();

	//float scale = qMin(sx,sy);
	m_liveRoot->setTransform(QTransform().scale(sx,sy));
	m_liveRoot->setPos(cr.left(),cr.top());
	
	m_fadeRoot->setTransform(QTransform().scale(sx,sy));
	m_fadeRoot->setPos(cr.left(),cr.top());
        //qDebug("Scaling: %.02f x %.02f",sx,sy);
        //qDebug() 
	update();
	//m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
	//m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}



AbstractVisualItem * OutputViewContent::syncToModelItem(AbstractVisualItem *model)
{	
	return AbstractContent::syncToModelItem(model);
}

QPixmap OutputViewContent::renderContent(const QSize & size, Qt::AspectRatioMode /*ratio*/) const
{
	// get the base empty pixmap
	QSize textSize = boundingRect().size().toSize();
	const float w = size.width(),
			h = size.height(),
			tw = textSize.width(),
			th = textSize.height();
	if (w < 2 || h < 2 || tw < 2 || th < 2)
		return QPixmap();
	
	// draw text (centered, maximized keeping aspect ratio)
	float scale = qMin(w / (tw + 16), h / (th + 16));
	QPixmap pix(size);
	pix.fill(Qt::transparent);
	QPainter pixPainter(&pix);
	pixPainter.translate((w - (int)((float)tw * scale)) / 2, (h - (int)((float)th * scale)) / 2);
	pixPainter.scale(scale, scale);
	//m_text->drawContents(&pixPainter);
// 	pixPainter.drawText(0,0,m_text);
	pixPainter.end();
	return pix;
}

int OutputViewContent::contentHeightForWidth(int width) const
{
	return AbstractContent::contentHeightForWidth(width);
}


void OutputViewContent::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	// paint parent
	AbstractContent::paint(painter, option, widget);
	
	
	if(sceneContextHint() == MyGraphicsScene::Editor)
	{
		painter->save();
		QPen p = modelItem() ? modelItem()->outlinePen() : QPen(Qt::black,1.5);
		painter->setPen(p);
		painter->setBrush(Qt::NoBrush);

		painter->drawRect(contentsRect());
		painter->restore();
	}
}


void OutputViewContent::socketError(QAbstractSocket::SocketError socketError)
{
	switch (socketError)
	{
		case QAbstractSocket::RemoteHostClosedError:
			break;
		case QAbstractSocket::HostNotFoundError:
			QMessageBox::critical(0,"Host Not Found",tr("The host was not found. Please check the host name and port settings."));
			break;
		case QAbstractSocket::ConnectionRefusedError:
			qDebug() << "OutputViewContent::connectNetworkClient: [INFO] Connection Refused:"<<m_client->errorString();
// 			if(m_reconnect)
				m_reconnectTimer.start(RECONNECT_WAIT_TIME);
// 			else
// 				QMessageBox::critical(0,"Connection Refused",
// 					tr("The connection was refused by the peer. "
// 						"Make sure the DViz server is running, "
// 						"and check that the host name and port "
// 						"settings are correct."));
			break;
		default:
			QMessageBox::critical(0,"Connection Problem",tr("The following error occurred: %1.").arg(m_client->errorString()));
	}
}


void OutputViewContent::connectNetworkClient()
{
	if(m_client)
	{
		//qDebug() << "MainWindow::slotDisconnect(): Exiting client";
		m_client->exit();
// 		disconnect(m_client,0,this,0);
// 		delete m_client;
		m_client->deleteLater();
		m_client = 0;
		qDebug() << "OutputViewContent::connectNetworkClient: [INFO] Disconnected";
	}
	
	m_client = new NetworkClient(this);
	//m_client->setLogger(this);
	connect(m_client, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	//connect(m_client, SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectChanged(double)));
	connect(m_client, SIGNAL(socketConnected()), this, SLOT(networkClientConnected()));
	connect(m_client, SIGNAL(socketDisconnected()), this, SLOT(connectNetworkClient()));
	m_client->setInstance(m_fakeInst);

	
	QString host = MainWindow::mw()->networkHost();
	int port = dynamic_cast<OutputViewItem*>(modelItem())->outputPort();

	qDebug() << "OutputViewContent::connectNetworkClient [INFO] Connecting to "<<host<<":"<<port;
	
	m_client->connectTo(host,port);
}

void OutputViewContent::networkClientConnected()
{
	qDebug() << "OutputViewContent::connectNetworkClient [INFO] Connected!";
}


void OutputViewContent::setOutputId(int id)
{
	if(MainWindow::mw()->isNetworkViewer())
	{
		connectNetworkClient();
	}
	else
	{
		OutputInstance *inst = MainWindow::mw()->outputInst(id);
		if(m_inst)
		{
			//m_inst->removeMirror(m_viewer);
			if(DEBUG_OUTPUTVIEW)
				qDebug() << "OutputViewContent::setOutputId("<<id<<"): Removing m_fakeInst from current m_inst="<<m_inst;
				
			m_inst->removeMirror(m_fakeInst);
		}
		
		if(!m_fakeInst)
		{
			if(DEBUG_OUTPUTVIEW)
				qDebug() << "OutputViewContent::setOutputId("<<id<<"): Not initalizing new outputId, m_fakeInst is already dead.";
				
			return;
		}
			
		if(inst)
		{
			QString name = inst->output()->name();
			if(name == "Live")
			{
				qDebug() << "OutputViewContent::setOutputId("<<id<<"): CANNOT EMBED PRIMARY OUTPUT IN VIEWER";
				m_inst = 0;
			}
			else
			{
				if(DEBUG_OUTPUTVIEW)
					qDebug() << "OutputViewContent::setOutputId("<<id<<"): Initalizing inst for output"<<name;
					
				m_inst = inst;
				//m_inst->addMirror(m_viewer);
				m_inst->addMirror(m_fakeInst);
			}
		}
		else
		{
			m_inst = 0;
			
			if(DEBUG_OUTPUTVIEW)
				qDebug() << "OutputViewContent::setOutputId("<<id<<"): Could not find output instance for id:"<<id;
		}
	}
}


/*******************************************************/


OutputViewInst::OutputViewInst(OutputViewContent *impl) // /*Output *output, bool startHidden, QWidget *parent*/)
	: OutputInstance(Output::widgetInstance(),false,0)
	, d(impl)
{
	if(DEBUG_OUTPUTVIEW)
		qDebug() << "OutputViewInst::OutputViewInst: Created instance, ptr: "<<(void*)this<<" for OutputViewContent: "<<(void*)impl;
}

OutputViewInst::~OutputViewInst() {}
	
// SlideGroup * OutputViewInst::slideGroup() {}
// int OutputViewInst::numSlides() {}
void OutputViewInst::setSceneContextHint(MyGraphicsScene::ContextHint) {}
void OutputViewInst::forceGLDisabled(bool) {}
	
// signals:
// 	void nextGroup();
// 	void jumpToGroup(int);
// 	void endOfGroup();
// 	
// 	void slideChanged(int);
// 	void slideChanged(Slide*);
// 	void slideGroupChanged(SlideGroup*,Slide*);
// 	
// 	void imageReady(QImage*);

// public slots:
void OutputViewInst::addMirror(OutputInstance *) {}
void OutputViewInst::removeMirror(OutputInstance *) {}

void OutputViewInst::addFilter(AbstractItemFilter *) {}
void OutputViewInst::removeFilter(AbstractItemFilter *) {}
void OutputViewInst::removeAllFilters() {}

void OutputViewInst::setSlideGroup(SlideGroup *group, Slide *slide) 
{
	slide = setSlideGroupInternal(group,slide);
	d->setSlideGroup(group,slide);
}
// void OutputViewInst::setSlideGroup(SlideGroup*, int startSlide) {}
void OutputViewInst::clear() 
{
	d->clearSlide();
}

void OutputViewInst::setBackground(QColor) {}
void OutputViewInst::setCanZoom(bool) {}

Slide * OutputViewInst::setSlide(Slide *slide, bool /*takeOwnership*/) 
{
	setSlideInternal(slide);
	d->setSlide(slide);
}
// Slide * OutputViewInst::setSlide(int) {}
// Slide * OutputViewInst::nextSlide() {}
// Slide * OutputViewInst::prevSlide() {}

void OutputViewInst::fadeBlackFrame(bool flag) 
{
	d->fadeBlackFrame(flag);
}

void OutputViewInst::setViewerState(SlideGroupViewer::ViewerState) {}

void OutputViewInst::setLiveBackground(const QFileInfo &, bool waitForNextSlide) {}

void OutputViewInst::setOverlaySlide(Slide *) {}
void OutputViewInst::setOverlayEnabled(bool) {}
void OutputViewInst::setTextOnlyFilterEnabled(bool) {}
void OutputViewInst::setAutoResizeTextEnabled(bool) {}

void OutputViewInst::setFadeSpeed(int x)
{
	d->setFadeSpeed(x);
}
void OutputViewInst::setFadeQuality(int x) 
{
	d->setFadeQuality(x);
}

// void OutputViewInst::setEndActionOverrideEnabled(bool) {}
// void OutputViewInst::setEndGroupAction(SlideGroup::EndOfGroupAction) {}


// protected slots:
void OutputViewInst::applyOutputSettings(bool startHidden) {}

// proxy methods from OutputInstance.cpp - let parent handle it
// void OutputViewInst::slotNextGroup() {}
// void OutputViewInst::slotJumpToGroup(int) {}

// handled in parent just fine
// void OutputViewInst::slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant) {}

void OutputViewInst::slotGrabPixmap() {}
