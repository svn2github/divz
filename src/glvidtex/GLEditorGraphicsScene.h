#ifndef GLEditorGraphicsScene_H
#define GLEditorGraphicsScene_H

#include <QGraphicsScene>
#include "GLDrawable.h"

class RectItem;

class GLEditorGraphicsScene : public QGraphicsScene
{
	Q_OBJECT
public:
	GLEditorGraphicsScene();
	
	QList<GLDrawable*> selectedItems() { return m_selection; }
	
signals:
	void selectionChanged();
	void drawableSelected(GLDrawable*);
	
public slots:
	void clearSelection(QList<GLDrawable*> ignoreList = QList<GLDrawable*>());
	void setSceneRect(const QRectF&);
	
protected:
	friend class GLDrawable;
	void itemSelected(GLDrawable*);
	
	virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	virtual void mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	virtual void keyPressEvent(QKeyEvent * event);

private:
	QList<GLDrawable*> m_selection;
	RectItem * m_bgRect;
	RectItem * m_dragRect;
	bool m_lockClearSelection;	
	
};

#endif
