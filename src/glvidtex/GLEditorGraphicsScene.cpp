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
		QGraphicsScene::mousePressEvent(mouseEvent);
}
