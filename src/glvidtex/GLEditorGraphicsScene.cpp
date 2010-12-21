#include "GLEditorGraphicsScene.h"

#include "GLDrawable.h"

class RectItem : public QGraphicsItem
{
public:
	RectItem (QGraphicsItem * parent = 0 ) :
		QGraphicsItem (parent), diff(false), filled(true) {}

	QRectF boundingRect() const { return rect; }
	void setBoundingRect(const QRectF& r) { rect = r; update(); }
	
	void paint(QPainter*p, const QStyleOptionGraphicsItem*, QWidget*)
	{
		p->save();
		p->setPen(pen);
		if(filled)
			p->fillRect(boundingRect(), brush);
		else
		{
			QRectF r = boundingRect();
			p->setPen(Qt::black);
			p->setBrush(QBrush());
			p->drawRect(r);
			
			p->setPen(Qt::white);
			p->drawRect(r.adjusted(-0.5, -0.5, +0.5, +0.5));
		}
		p->restore();
	}
	
	bool diff;
	QPen pen;
	QBrush brush;
	QRectF rect;
	bool filled;
	
};


GLEditorGraphicsScene::GLEditorGraphicsScene()
	: QGraphicsScene()
	, m_bgRect(0)
	, m_dragRect(0)
	, m_lockClearSelection(false)
{
	m_bgRect = new RectItem();
	m_bgRect->brush = Qt::black;
	m_bgRect->pen = QPen(Qt::black, 1.);
	m_bgRect->setZValue(-999999999);
	//qDebug() << "m_bgRect:"<<m_bgRect;
	
	m_dragRect = new RectItem();
	m_dragRect->diff = true;
	m_dragRect->filled = false;
	m_dragRect->setVisible(false);
	m_dragRect->setZValue(999999999);
	m_dragRect->rect = QRectF(0,0,0,0);
	
	addItem(m_bgRect);
	addItem(m_dragRect);
	//qDebug() << "m_dragRect:"<<m_dragRect;
}

void GLEditorGraphicsScene::clear()
{
	// Remove before clear, because ::clear() deletes all items
	removeItem(m_bgRect);
	removeItem(m_dragRect);
	
	// Drawables are owned by the GLScene they are a member of,
	// remove so that ::clear() doesnt delete the items
	removeDrawables();
	
	// Clear any remaining items
	QGraphicsScene::clear();
	
	// Pokeyoke
	QList<QGraphicsItem*> list = items();
	if(list.size() > 0)
	{
		qDebug() << "GLEditorGraphicsScene::clear: Error clearing, still "<<list.size()<<" items in scene.";
	}
	
	// Add back in our internal items
	addItem(m_bgRect);
	addItem(m_dragRect);
}

void GLEditorGraphicsScene::setSceneRect(const QRectF& rect)
{
	QGraphicsScene::setSceneRect(rect);
	m_bgRect->setBoundingRect(rect);
}

void GLEditorGraphicsScene::itemSelected(GLDrawable *item)
{
	//qDebug() << "GLEditorGraphicsScene::itemSelected: item:"<<(QObject*)item;
	clearSelection(QList<GLDrawable*>() << item);
			
	m_selection.clear();
	m_selection.append(item);
	emit selectionChanged();
	emit drawableSelected(item);
}

void GLEditorGraphicsScene::clearSelection(QList<GLDrawable*> ignoreList)
{
	if(m_lockClearSelection)
		return;
		
	if(!m_selection.isEmpty())
		foreach(GLDrawable *tmp, m_selection)
			if(ignoreList.isEmpty() || !ignoreList.contains(tmp))
			{
				//qDebug() << "GLEditorGraphicsScene::clearSelection: clearing selection on item:"<<(QObject*)tmp;
				tmp->setSelected(false);
			}
}

void GLEditorGraphicsScene::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
//  	QGraphicsItem *item = itemAt(mouseEvent->scenePos());
//  	if(!item || item == m_bgRect || item == m_dragRect)
//  	{
//  		//qDebug() << "GLEditorGraphicsScene::mousePressEvent: No item at:"<<mouseEvent->scenePos();
// // 		clearSelection();
// 		m_dragRect->rect = QRectF(0,0,0,0);
// 		m_dragRect->setPos(mouseEvent->scenePos());
// 		m_dragRect->setVisible(true);
// 		m_dragRect->update();
//  	}
//  	else
//  	{
 		QGraphicsScene::mousePressEvent(mouseEvent);
//	}
}

void GLEditorGraphicsScene::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
// 	QGraphicsItem *item = itemAt(mouseEvent->pos());
// 	if(!item)
// 	{
// 		qDebug() << "GLEditorGraphicsScene::mousePressEvent: No item at:"<<mouseEvent->pos();
// 		clearSelection();
// 	}
// 	else
	//setFocus();
	if(m_dragRect->isVisible())
	{
		QPointF mousePos = mouseEvent->scenePos();
		QPointF curPos = m_dragRect->pos();
		
		QRectF newRect = QRectF(0,0,mousePos.x() - curPos.x(),mousePos.y() - curPos.y()).normalized();
		
		m_dragRect->rect = newRect;
		m_dragRect->update();
	}
	else
	{
		QGraphicsScene::mousePressEvent(mouseEvent);
	}
}

void GLEditorGraphicsScene::removeDrawables()
{
	QList<QGraphicsItem*> list = items();
	foreach(QGraphicsItem *item, list)
	{
		GLDrawable *gld = dynamic_cast<GLDrawable*>(item);	
		if(gld)
			removeItem(gld);
	}
}

void GLEditorGraphicsScene::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
// 	QGraphicsItem *item = itemAt(mouseEvent->pos());
// 	if(!item)
// 	{
// 		qDebug() << "GLEditorGraphicsScene::mousePressEvent: No item at:"<<mouseEvent->pos();
// 		clearSelection();
// 	}
// 	else
	//setFocus();
	if(m_dragRect->isVisible())
	{
		m_dragRect->setVisible(false);
		
		clearSelection();
		
		m_lockClearSelection = true;
		QRectF selRect(m_dragRect->pos(), m_dragRect->rect.normalized().size());
		//qDebug() << "selRect:"<<selRect;
		QList<QGraphicsItem*> list = items(selRect);
		foreach(QGraphicsItem *item, list)
		{
			GLDrawable *gld = dynamic_cast<GLDrawable*>(item);
			if(gld)
			{
				gld->setSelected(true);
			}
		}
		m_lockClearSelection = false;
	}
	else
	{
		QGraphicsScene::mousePressEvent(mouseEvent);
	}
}


void GLEditorGraphicsScene::keyPressEvent(QKeyEvent * event)
{
	#define DEBUG_KEYHANDLER 0
	if(DEBUG_KEYHANDLER)
		qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key();
	if(event->modifiers() & Qt::ControlModifier)
	{
		if(DEBUG_KEYHANDLER)
			qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 1";
		switch(event->key())
		{
			case Qt::Key_C:
				//copyCurrentSelection();
				event->accept();
				break;
				
			case Qt::Key_X:
				//copyCurrentSelection(true);
				event->accept();
				break;
				
			case Qt::Key_V:
				//pasteCopyBuffer();
				event->accept();
				break;
				
			case Qt::Key_A:
				//selectAll();
				event->accept();
				break;
				
			default:
				if(DEBUG_KEYHANDLER)
					qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 1: fall thru, no key";
				break;
		}
	}
	else
	{
		
		if(DEBUG_KEYHANDLER)
			qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 1, end of path, accepted?"<<event->isAccepted();
		
		if(DEBUG_KEYHANDLER)
			qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2";	
		QSizeF grid(10.,10.0);// = AppSettings::gridSize();
		// snap to half a grid point - the content->flagKeyboardMotivatedMovement() call tells AppSettings::snapToGrid()
		// in AbstractContent to allow it to be half a grid point
		qreal x = grid.width();///2;
		qreal y = grid.height();///2;
		
		// arbitrary magic numbers - no significance, just random preference
		if(x<=0)
			x = 5;
		if(y<=5)
			y = 5;
		
		//QList<GLDrawable *> selection = selectedItems();
		if(DEBUG_KEYHANDLER)
			qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2, selection size:"<<m_selection.size();
		if(m_selection.size() > 0)
		{
			
			if(DEBUG_KEYHANDLER)
				qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2"; //, first content model item name:"<<content->modelItem()->itemName();
			switch(event->key())
			{
				case Qt::Key_Delete:
					deleteSelectedItems();
					event->accept();
					break;
				case Qt::Key_Up:
					if(DEBUG_KEYHANDLER)
						qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2: move up:"<<y;
					foreach(GLDrawable *item, m_selection)
					{
						item->moveBy(0,-y);
					}
					event->accept();
					break;
				case Qt::Key_Down:
					if(DEBUG_KEYHANDLER)
						qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2: move down:"<<y;
					foreach(GLDrawable *item, m_selection)
					{
						item->moveBy(0,+y);
					}
					event->accept();
					break;
				case Qt::Key_Left:
					if(DEBUG_KEYHANDLER)
						qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2: move left:"<<x;
					foreach(GLDrawable *item, m_selection)
					{
						item->moveBy(-x,0);
					}
					event->accept();
					break;
				case Qt::Key_Right:
					if(DEBUG_KEYHANDLER)
						qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2: move right:"<<x;
					foreach(GLDrawable *item, m_selection)
					{
						item->moveBy(+x,0);
					}
					event->accept();
					break;
				case Qt::Key_F4:
				case Qt::Key_Space:
				//case Qt::Key_Enter:
					if(DEBUG_KEYHANDLER)
						qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2: config content key";
// 					foreach(GLDrawable *item, m_selection)
// 					{
// 						//configureContent(content);
// 					}
					event->accept();
					break;
				default:
					if(DEBUG_KEYHANDLER)
						qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2: fall thru, no key";
					break;
			}
		}
		
		if(DEBUG_KEYHANDLER)
			qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", path 2, end of path, accepted?"<<event->isAccepted();
		
	}
	
	if(!event->isAccepted())
	{
		if(DEBUG_KEYHANDLER)
			qDebug() << "GLEditorGraphicsScene::keyPressEvent(): key:"<<event->key()<<", default - event not accepted, sending to parent";
		QGraphicsScene::keyPressEvent(event);
	}
}



void GLEditorGraphicsScene::deleteSelectedItems()
{
	
	if (m_selection.size() > 1)
		if (QMessageBox::question(0, tr("Delete content"), tr("All the %1 selected content will be deleted, do you want to continue ?").arg(m_selection.size()), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
		return;
	
	foreach(GLDrawable *item, m_selection)
	{
		// unlink content from lists, myself(the Scene) and memory
		removeItem(item);
		//if(DEBUG_MYGRAPHICSSCENE_ITEM_MGMT)
		//	qDebug() << "MyGraphicsScene::slotDeleteContent(): Disposing of item: "<<content->modelItem()->itemName();
		//content->dispose();
		item->deleteLater();
		//delete content;
	}
}

void GLEditorGraphicsScene::setEditingMode(bool flag)
{
	m_editingMode = flag;
	
	QList<QGraphicsItem*> list = items();
	foreach(QGraphicsItem *item, list)
	{
		GLDrawable *gld = dynamic_cast<GLDrawable*>(item);	
		if(gld)
			gld->editingModeChanged(flag);
	}
	
	update();
}
