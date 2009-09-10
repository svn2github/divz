#include "MyGraphicsScene.h"
#include "frames/FrameFactory.h"
#include "items/AbstractConfig.h"
#include "items/AbstractContent.h"
/*#include "items/PictureContent.h"
#include "items/PictureConfig.h"*/
#include "items/TextContent.h"
#include "items/TextConfig.h"
// #include "items/WebContentSelectorItem.h"
// #include "items/WebcamContent.h"
#include "items/TextContent.h"
#include "items/TextConfig.h"

#include "model/ItemFactory.h"
#include "model/Slide.h"
#include "model/TextItem.h"


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

MyGraphicsScene::MyGraphicsScene(QObject * parent)
    : QGraphicsScene(parent)
    , m_slide(0)
{
}

MyGraphicsScene::~MyGraphicsScene()
{
	m_slide = 0;
	m_slidePrev = 0;
	
}

void MyGraphicsScene::setSlide(Slide *slide, SlideTransition /*trans*/)
{
	m_slide = slide;
	
	QList<AbstractItem *> items = m_slide->itemList();
	for(int i=0;i<items.size();i++)
	{
		AbstractItem * item = items.at(i);
		assert(item != NULL);
		
		if (AbstractVisualItem * visualItem = dynamic_cast<AbstractVisualItem *>(item))
		{
			AbstractContent * visual = visualItem->createDelegate(this);
			addContent(visual); // QPoint((int)visualItem->pos().x(),(int)visualItem->pos().y()));
		}
	}
}

void MyGraphicsScene::slotTransitionStep()
{
	// TBD
}

void MyGraphicsScene::addContent(AbstractContent * content) // const QPoint & pos)
{
	connect(content, SIGNAL(configureMe(const QPoint &)), this, SLOT(slotConfigureContent(const QPoint &)));
	//connect(content, SIGNAL(backgroundMe()), this, SLOT(slotBackgroundContent()));
	connect(content, SIGNAL(changeStack(int)), this, SLOT(slotStackContent(int)));
	connect(content, SIGNAL(deleteItem()), this, SLOT(slotDeleteContent()));
	/*
	if (!pos.isNull())
		content->setPos(pos);
	content->setZValue(m_content.isEmpty() ? 1 : (m_content.last()->zValue() + 1));
	//content->setCacheMode(QGraphicsItem::DeviceCoordinateCache);*/
	content->show();
	
	m_content.append(content);
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


TextItem * MyGraphicsScene::newTextItem(QString text)
{
	TextItem *t = new TextItem();
	assert(m_slide);
	m_slide->addItem(t); //m_slide->createText();
	
	t->setText(text);
	t->setPos(nearCenter(sceneRect()));
	t->setItemName("TextItem");
	t->setItemId(ItemFactory::nextId());
	
	AbstractContent * item = t->createDelegate(this);
	addContent(item); //, QPoint((int)t->pos().x(),(int)t->pos().y()));
	
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
	// get the content and ensure it hasn't already a property window
	AbstractContent * content = dynamic_cast<AbstractContent *>(sender());
	foreach (AbstractConfig * config, m_configs) 
	{
		if (config->content() == content)
			return;
		// force only 1 property instance
		slotDeleteConfig(config);
	}
	AbstractConfig * p = 0;
	
// 	// picture config (dialog and connections)
// 	if (PictureContent * picture = dynamic_cast<PictureContent *>(content)) 
// 	{
// 		p = new PictureConfig(picture);
// 		connect(p, SIGNAL(applyEffect(const PictureEffect &, bool)), this, SLOT(slotApplyEffect(const PictureEffect &, bool)));
// 	}
	
	// text config (dialog and connections)
	if (TextContent * text = dynamic_cast<TextContent *>(content))
		p = new TextConfig(text);
	
	// generic config
	if (!p)
		p = new AbstractConfig(content);
	
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
		content->setZValue(z++);
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
		foreach (AbstractConfig * config, m_configs) {
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

void MyGraphicsScene::slotDeleteConfig(AbstractConfig * config)
{
	m_configs.removeAll(config);
	config->dispose();
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
