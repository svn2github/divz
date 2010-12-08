#ifndef EditorGraphicsView_H
#define EditorGraphicsView_H

#include <QtGui>

class EditorGraphicsView : public QGraphicsView
{
	public:
		EditorGraphicsView(QWidget * parent=0);
		bool canZoom() { return m_canZoom; }
		void setCanZoom(bool flag) { m_canZoom = flag; }

		qreal scaleFactor() { return m_scaleFactor; }
		void setScaleFactor(qreal);

	protected:
		void keyPressEvent(QKeyEvent *event);
		void wheelEvent(QWheelEvent *event);
		void scaleView(qreal scaleFactor);
	private:
		bool m_canZoom;
		qreal m_scaleFactor;
};



#endif
