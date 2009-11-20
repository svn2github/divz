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

#include "model/OutputViewItem.h"
#include "OutputInstance.h"
#include "AppSettings.h"
#include "MainWindow.h"
#include "model/Output.h"
#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/BackgroundItem.h"

#if QT_VERSION >= 0x040600
	#define QT46_SHADOW_ENAB 0
#endif

#include <QMovie>
#include <QLabel>

OutputViewRootObject::OutputViewRootObject(QGraphicsItem *parent, QGraphicsScene*)
	: QGraphicsItem(parent)
{
	setPos(0,0);
	
	
}

OutputViewContent::OutputViewContent(QGraphicsScene * scene, QGraphicsItem * parent)
    : AbstractContent(scene, parent, false)
    , m_inst(0)
    , m_slide(0)
    , m_group(0)
    
{
	m_dontSyncToModel = true;
	
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
	qDebug() << "OutputViewContent:: created m_liveRoot:"<<(void*)m_liveRoot<<", this:"<<(void*)this;
	
	
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


#define DEBUG_OUTPUTVIEW 0
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



void OutputViewContent::setSlide(Slide *slide)
{
	if(DEBUG_OUTPUTVIEW)
		qDebug() << "OutputViewContent::setSlide: slide:"<<slide;
	if(!slide)
	{
		
		if(DEBUG_OUTPUTVIEW)
			qDebug() << "OutputViewContent::setSlide: * slide is null, NoOp";
		return;
	}
	
	if(m_slide)
	{
// 		qDebug() << "OutputViewContent::setSlide: removing existing content, size:" << m_content.size();
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
	}
	
	
	m_slide = slide;
	
	double baseZValue = 0;
	
	QList<AbstractItem *> items = m_slide->itemList();
	foreach(AbstractItem *item, items)
	{
		if(!item)
			continue;
			
		BackgroundItem * bgTmp = dynamic_cast<BackgroundItem*>(item);
		if(bgTmp && bgTmp->fillType() == AbstractVisualItem::None)
			continue;
				
		AbstractContent * visual = createVisualDelegate(item);
		//determine max zvalue for use in rebasing overlay items
		if(visual && visual->zValue() > baseZValue)
			baseZValue = visual->zValue();

	}
	

	
// 	if(!m_group)
// 	{
// 	
// 	}
// 	else
// 	{
// 		int idx = m_group->indexOf(slide);
// 		if(idx < 0)
// 		{
// 		
// 		}
// 		else
// 		{
// 		
// 		}
// 	
// 	}
// 	
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

void OutputViewContent::setOutputId(int id)
{
	OutputInstance *inst = MainWindow::mw()->outputInst(id);
	if(m_inst)
	{
		//m_inst->removeMirror(m_viewer);
		qDebug() << "OutputViewContent::setOutputId("<<id<<"): Removing m_fakeInst from current m_inst="<<m_inst;
		m_inst->removeMirror(m_fakeInst);
	}
	
	if(!m_fakeInst)
	{
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
			qDebug() << "OutputViewContent::setOutputId("<<id<<"): Initalizing inst for output"<<name;
			m_inst = inst;
			//m_inst->addMirror(m_viewer);
			m_inst->addMirror(m_fakeInst);
		}
	}
	else
	{
		m_inst = 0;
		qDebug() << "OutputViewContent::setOutputId("<<id<<"): Could not find output instance for id:"<<id;
	}
}


/*******************************************************/


OutputViewInst::OutputViewInst(OutputViewContent *impl) // /*Output *output, bool startHidden, QWidget *parent*/)
	: OutputInstance(Output::widgetInstance(),false,0)
	, d(impl)
{
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
void OutputViewInst::clear() {}

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

void OutputViewInst::fadeBlackFrame(bool) {}

void OutputViewInst::setViewerState(SlideGroupViewer::ViewerState) {}

void OutputViewInst::setLiveBackground(const QFileInfo &, bool waitForNextSlide) {}

void OutputViewInst::setOverlaySlide(Slide *) {}
void OutputViewInst::setOverlayEnabled(bool) {}
void OutputViewInst::setTextOnlyFilterEnabled(bool) {}
void OutputViewInst::setAutoResizeTextEnabled(bool) {}

void OutputViewInst::setFadeSpeed(int) {}
void OutputViewInst::setFadeQuality(int) {}

void OutputViewInst::setEndActionOverrideEnabled(bool) {}
void OutputViewInst::setEndGroupAction(SlideGroup::EndOfGroupAction) {}


// protected slots:
void OutputViewInst::applyOutputSettings(bool startHidden) {}

// proxy methods from OutputInstance.cpp - let parent handle it
// void OutputViewInst::slotNextGroup() {}
// void OutputViewInst::slotJumpToGroup(int) {}

// handled in parent just fine
// void OutputViewInst::slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant) {}

void OutputViewInst::slotGrabPixmap() {}
