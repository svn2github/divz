#include "MyGraphicsScene.h"
#include "frames/FrameFactory.h"
#include "items/AbstractConfig.h"
#include "items/GenericItemConfig.h"
#include "items/AbstractContent.h"
/*#include "items/PictureContent.h"
#include "items/PictureConfig.h"*/
#include "items/TextContent.h"
#include "items/TextConfig.h"
// #include "items/WebContentSelectorItem.h"
// #include "items/WebcamContent.h"
#include "items/TextContent.h"
#include "items/TextConfig.h"
#include "items/TextBoxConfig.h"
#include "items/TextBoxContent.h"
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

#define qSetEffectOpacity(item,opacity) { QGraphicsOpacityEffect * opac = dynamic_cast<QGraphicsOpacityEffect*>(item->graphicsEffect()); if(opac) opac->setOpacity(opacity); }
//#define qGetEffectOpacity(item,opacity) dynamic_cast<QGraphicsOpacityEffect*>item->graphicsEffect() ? (dynamic_cast<QGraphicsOpacityEffect*>item->graphicsEffect())->opacity() : 0

#if QT_VERSION >= 0x040600
	#include <QGraphicsOpacityEffect>

	#define QT46_OPAC_ENAB 0
#endif

class RootObject : public QGraphicsItem
{
public:
	RootObject(QGraphicsScene*x):QGraphicsItem(0,x){
	
		#if QT46_OPAC_ENAB > 0 
			QGraphicsOpacityEffect * opac = new QGraphicsOpacityEffect();
			opac->setOpacity(1);
			setGraphicsEffect(opac);
		#endif
	}
	QRectF boundingRect() const { return QRectF(); } //MainWindow::mw()->standardSceneRect(); }
	void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {}
};

MyGraphicsScene::MyGraphicsScene(ContextHint hint, QObject * parent)
    : QGraphicsScene(parent)
    , m_slide(0)
    , m_fadeTimer(0)
    , m_contextHint(hint)
{
	m_fadeRoot = new RootObject(this);
	m_fadeRoot->setPos(0,0);
	
	m_liveRoot = new RootObject(this);
	m_liveRoot->setPos(0,0);
}

MyGraphicsScene::~MyGraphicsScene()
{
	m_slide = 0;
	//m_slidePrev = 0;
	
	//qDeleteAll(m_ownedContent);
	delete m_fadeRoot;
	delete m_liveRoot;
	
	m_fadeRoot = 0;
	m_liveRoot = 0;
	
}

void MyGraphicsScene::setContextHint(ContextHint hint)
{
	m_contextHint = hint;
}

void MyGraphicsScene::clear()
{
	foreach(AbstractContent *content, m_content)
	{
		m_content.removeAll(content);
		if(content->parentItem() == m_liveRoot)
			content->setParentItem(0);
		removeItem(content);
		
		disconnect(content, 0, 0, 0);
		//qDebug() << "Disposing of content";
		content->dispose(false);
		//delete content;
		content = 0;
	}
	m_slide = 0;
	
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
		createVisualDelegate(item);
	else
	if(operation == "remove")
		removeVisualDelegate(item);
}

void MyGraphicsScene::removeVisualDelegate(AbstractItem *item)
{
	//qDebug() << "MyGraphicsScene::removeVisualDelegate: Going to remove: "<<item->itemName();
	foreach(AbstractContent *z, m_content)
	{
		if(z->modelItem() == item)
		{
			z->setVisible(false);
			z->setParentItem(0);
			removeItem(z);
			
			m_content.removeAll(z);
			disconnect(z, 0, 0, 0);
			z->dispose(false);
				
			return;
		}
	}
// 	qDebug() << "MyGraphicsScene::removeVisualDelegate: Couldn't find visual delegate for item requested: "<<item->itemName();
// 	fprintf(stderr,"MyGraphicsScene::removeVisualDelegate: Can't find delegate for m_item=%p\n",item);
}

void MyGraphicsScene::setSlide(Slide *slide, SlideTransition trans)
{
	//TODO implement slide transitions
	if(m_slide == slide)
	{
		//qDebug("MyGraphicsScene::setSlide: Not changing slide - same slide!");
		return;
	}
	
	if(m_slide)// && m_slideGroup != g)
	{
		disconnect(m_slide,0,this,0);
	}
	
	if(slide)
	{
		
		//qDebug() << "MyGraphicsScene::setSlide(): going to dump slide QObject info:";
		//slide->dumpObjectInfo();
		//disconnect(slide,0,this,0);
		connect(slide,SIGNAL(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)),this,SLOT(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)));
	}
	
	if(contextHint() == Preview)
		trans = None; 
	
	m_currentTransition = trans;
	
	if(DEBUG_MYGRAPHICSSCENE)
		qDebug() << "MyGraphicsScene::setSlide(): Setting slide # "<<slide->slideNumber();
	
	//trans = None;
	if(trans == None)
	{
		clear();
	}
	else
	{
		
		if(!m_fadeTimer)
		{
			m_fadeTimer = new QTimer(this);
			connect(m_fadeTimer, SIGNAL(timeout()), this, SLOT(slotTransitionStep()));
		}
		
		
		if(m_fadeTimer->isActive())
			endTransition(); 
			
// 		if(DEBUG_MYGRAPHICSSCENE)
// 			qDebug() << "MyGraphicsScene::setSlide(): Reparenting"<<m_content.size()<<"items";
		
		foreach(AbstractContent *x, m_content)
			x->setParentItem(m_fadeRoot);
			
// 		if(DEBUG_MYGRAPHICSSCENE)
// 			qDebug() << "MyGraphicsScene::setSlide(): Done reparenting.";

		m_fadeRoot->setZValue(250);
		
		
		// start with faderoot fully visible, and live root invisible, then cross fade between the two
		// UPDATE: flip faderoot behind now

		m_fadeStepCounter = 0;
		m_fadeSteps = 30;
		int ms = 250  / m_fadeSteps;
		m_fadeTimer->start(ms); //ms);

// 		double inc = (double)1 / m_fadeSteps;

		
				
		#if QT46_OPAC_ENAB > 0
			qSetEffectOpacity(m_fadeRoot,1);
			qSetEffectOpacity(m_liveRoot,0);
		#else
			m_fadeRoot->setOpacity(1);
			m_liveRoot->setOpacity(0);
		#endif

// 		if(DEBUG_MYGRAPHICSSCENE)
// 			qDebug() << "MyGraphicsScene::setSlide(): Starting fade timer for "<<ms<<"ms";
		
	}

	m_slide = slide;
	
	// force creation of bg if doesnt exist
	m_slide->background();
	
	QList<AbstractItem *> items = m_slide->itemList();
	foreach(AbstractItem *item, items)
	{
		assert(item != NULL);
		
		createVisualDelegate(item);
	}
	
	m_liveRoot->setZValue(500);
}

AbstractContent * MyGraphicsScene::createVisualDelegate(AbstractItem *item)
{
	if (AbstractVisualItem * visualItem = dynamic_cast<AbstractVisualItem *>(item))
	{
		if(DEBUG_MYGRAPHICSSCENE)
			qDebug() << "MyGraphicsScene::setSlide(): Creating new content item from:"<<visualItem->itemName();
		AbstractContent * visual = visualItem->createDelegate(this,m_liveRoot);
		addContent(visual, true);
		
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

void MyGraphicsScene::endTransition()
{
	m_fadeTimer->stop();
	#if QT46_OPAC_ENAB > 0
		qSetEffectOpacity(m_fadeRoot,0);
		qSetEffectOpacity(m_liveRoot,1);
	#else
		m_fadeRoot->setOpacity(0);
		m_liveRoot->setOpacity(1);
	#endif
	
	QList<QGraphicsItem*> kids = m_fadeRoot->childItems();
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
			z->dispose(false);
		}
	}
	
	update();
	
}

void MyGraphicsScene::slotTransitionStep()
{
	if( ++ m_fadeStepCounter < m_fadeSteps)
	{
		double inc = (double)1 / m_fadeSteps;
		//m_fadeRoot->setOpacity(m_fadeRoot->opacity() - inc);
		#if QT46_OPAC_ENAB > 0
			QGraphicsOpacityEffect * opac = dynamic_cast<QGraphicsOpacityEffect*>(m_liveRoot->graphicsEffect()); 
			if(opac) 
				opac->setOpacity(opac->opacity() + inc);
		#else
			m_liveRoot->setOpacity(m_liveRoot->opacity() + inc);
		#endif
		if(DEBUG_MYGRAPHICSSCENE)
			qDebug()<<"MyGraphicsScene::slotTransitionStep(): step"<<m_fadeStepCounter<<"/"<<m_fadeSteps<<", inc:"<<inc<<", fade:"<<m_fadeRoot->opacity()<<", live:"<<m_liveRoot->opacity();
	}
	else
	{
		endTransition();
	}
	
}

void MyGraphicsScene::addContent(AbstractContent * content, bool takeOwnership) // const QPoint & pos)
{
	connect(content, SIGNAL(configureMe(const QPoint &)), this, SLOT(slotConfigureContent(const QPoint &)));
	//connect(content, SIGNAL(backgroundMe()), this, SLOT(slotBackgroundContent()));
	connect(content, SIGNAL(changeStack(int)), this, SLOT(slotStackContent(int)));
	connect(content, SIGNAL(deleteItem()), this, SLOT(slotDeleteContent()));
	/*
	if (!pos.isNull())
		content->setPos(pos);
	//content->setCacheMode(QGraphicsItem::DeviceCoordinateCache);*/
	if(content->zValue() == 0)
	{
		QList<QGraphicsItem*> kids = items();
		double z = kids.isEmpty() ? 1 : (kids.last()->zValue() + 1);
		content->modelItem()->setZValue(z);
	}
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
	assert(m_slide);
	
	// text should automatically enlarge the rect
	t->setContentsRect(QRect(0,0,400,10));
	t->setText(text);
	t->setPos(nearCenter(sceneRect()));
	t->setItemId(ItemFactory::nextId());
	t->setItemName(QString("TextBoxItem%1").arg(t->itemId()));
	t->setFontSize(38.0);
	
	m_slide->addItem(t); //m_slide->createText();
	
	AbstractContent * item = t->createDelegate(this,m_liveRoot);
	addContent(item); //, QPoint((int)t->pos().x(),(int)t->pos().y()));
	
	return t;
}


AbstractVisualItem * MyGraphicsScene::newBoxItem()
{
	BoxItem *t = new BoxItem();
	assert(m_slide);
	
	//t->setText(text);
	t->setPos(nearCenter(sceneRect()));
	t->setItemId(ItemFactory::nextId());
	t->setItemName(QString("BoxItem%1").arg(t->itemId()));
	
	m_slide->addItem(t); //m_slide->createText();
	
	AbstractContent * item = t->createDelegate(this,m_liveRoot);
	addContent(item); //, QPoint((int)t->pos().x(),(int)t->pos().y()));
	
	return t;
}

AbstractVisualItem * MyGraphicsScene::newVideoItem()
{
	VideoFileItem *t = new VideoFileItem();
	assert(m_slide);
	t->setFilename("data/Seasons_Loop_3_SD.mpg");
	
	//t->setText(text);
	t->setPos(nearCenter(sceneRect()));
	t->setItemId(ItemFactory::nextId());
	t->setItemName(QString("VideoItem%1").arg(t->itemId()));
	
	m_slide->addItem(t); //m_slide->createText();
	
	AbstractContent * item = t->createDelegate(this,m_liveRoot);
	addContent(item); //, QPoint((int)t->pos().x(),(int)t->pos().y()));
	
	return t;
}

AbstractVisualItem * MyGraphicsScene::newImageItem()
{
	ImageItem *t = new ImageItem();
	assert(m_slide);
	
	t->setPos(nearCenter(sceneRect()));
	t->setItemId(ItemFactory::nextId());
	t->setItemName(QString("ImageItem%1").arg(t->itemId()));
	
	m_slide->addItem(t);
	
	AbstractContent * item = t->createDelegate(this,m_liveRoot);
	addContent(item);
	
	return t;
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
	foreach (GenericItemConfig * config, m_configs) 
	{
// 		if (config->content() == content)
// 			return;
		// force only 1 property instance
		slotDeleteConfig(config);
	}
	//AbstractConfig * p = 0;
	GenericItemConfig * p = 0;
	
// 	// picture config (dialog and connections)
// 	if (PictureContent * picture = dynamic_cast<PictureContent *>(content)) 
// 	{
// 		p = new PictureConfig(picture);
// 		connect(p, SIGNAL(applyEffect(const PictureEffect &, bool)), this, SLOT(slotApplyEffect(const PictureEffect &, bool)));
// 	}
	
	// text config (dialog and connections)
// 	if (TextContent * text = dynamic_cast<TextContent *>(content))
// 		p = new TextConfig(text);
// 	else
	if (TextBoxContent * text = dynamic_cast<TextBoxContent *>(content))
		p = new TextBoxConfig(text);
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
	m_configs.append(p);
	//addItem(p);
	connect(p, SIGNAL(applyLook(quint32,bool,bool)), this, SLOT(slotApplyLook(quint32,bool,bool)));
	p->setScene(this);
	p->show();
	
// 	p->setPos(scenePoint - QPoint(10, 10));
// 	p->keepInBoundaries(sceneRect().toRect());
// 	p->setFocus();
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
	
		// remove related property if deleting its content
		foreach (GenericItemConfig * config, m_configs) {
			if (config->content() == content) 
			{
				slotDeleteConfig(config);
				break;
			}
		}
	
		// unlink content from lists, myself(the Scene) and memory
		m_content.removeAll(content);
		m_slide->removeItem(content->modelItem());
		content->dispose();
		//delete content;
	}
}

void MyGraphicsScene::slotDeleteConfig(GenericItemConfig * config)
{
	m_configs.removeAll(config);
	config->dispose();
	update();
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
