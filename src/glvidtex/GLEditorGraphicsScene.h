#ifndef GLEditorGraphicsScene_H
#define GLEditorGraphicsScene_H

#include <QGraphicsScene>
#include "GLDrawable.h"

class RectItem : public QGraphicsItem
{
public:
	RectItem (QGraphicsItem * parent = 0 ) :
		QGraphicsItem (parent) {}

	QRectF boundingRect() const { return m_rect; }
	void setBoundingRect(const QRectF& rect) { m_rect = rect; update(); }
	
	void paint(QPainter*p, const QStyleOptionGraphicsItem*, QWidget*)
	{
		p->save();
		//p->setCompositionMode(QPainter::CompositionMode_Difference);
		p->setPen(pen);
		p->fillRect(boundingRect(), brush);
		p->restore();
	}
	
	QPen pen;
	QBrush brush;
private:
	QRectF m_rect;
	
};


class GLEditorGraphicsScene : public QGraphicsScene
{
	Q_OBJECT
public:
	GLEditorGraphicsScene();
	
	QList<GLDrawable*> selectedItems() { return m_selection; }
	
signals:
	void selectionChanged();
	
public slots:
	void clearSelection(QList<GLDrawable*> ignoreList = QList<GLDrawable*>());
	void setSceneRect(const QRectF&);
	
protected:
	friend class GLDrawable;
	void itemSelected(GLDrawable*);
	
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	virtual void keyPressEvent(QKeyEvent * event);

private:
	QList<GLDrawable*> m_selection;
	RectItem * m_bgRect;	
	
};

#endif
