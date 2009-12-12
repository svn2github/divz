#ifndef MainGraphicsView_H
#define MainGraphicsView_H

#include <QGraphicsView>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QWidget>

class MainGraphicsView : public QGraphicsView
{
	Q_OBJECT
	
public:
	MainGraphicsView(QWidget * parent);

	bool canZoom() { return m_canZoom; }
	void setCanZoom(bool flag) { m_canZoom = flag; }
	
	void adjustViewScaling();
	
signals:
	void nextImage();
	void prevImage();

protected slots:
	void unlockZoomChanging();

protected:
	void resizeEvent(QResizeEvent *);
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);
	void scaleView(qreal scaleFactor);
	
private:
	bool m_canZoom;
	bool m_zoomChanging;
	qreal m_wheelCounter;
};


#endif //MainGraphicsView_H
