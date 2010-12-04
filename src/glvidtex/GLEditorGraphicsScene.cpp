#include "GLEditorGraphicsScene.h"

#include "GLDrawable.h"

GLEditorGraphicsScene::GLEditorGraphicsScene()
	: QGraphicsScene()
{
	m_bgRect = new RectItem();
	m_bgRect->brush = Qt::black;
	m_bgRect->pen = QPen(Qt::black, 1.);
	addItem(m_bgRect);
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
}

void GLEditorGraphicsScene::clearSelection(QList<GLDrawable*> ignoreList)
{
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
// 	QGraphicsItem *item = itemAt(mouseEvent->pos());
// 	if(!item)
// 	{
// 		qDebug() << "GLEditorGraphicsScene::mousePressEvent: No item at:"<<mouseEvent->pos();
// 		clearSelection();
// 	}
// 	else
	//setFocus();
	QGraphicsScene::mousePressEvent(mouseEvent);
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
					//slotDeleteContent();
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

