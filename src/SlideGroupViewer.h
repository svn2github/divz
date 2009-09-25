#ifndef SLIDEGROUPVIEWER_H
#define SLIDEGROUPVIEWER_H

#include <QGraphicsView>

#include <QList>
#include <QGraphicsView>
#include <QTimer>

#include "MyGraphicsScene.h"
#include "model/SlideGroup.h"

class SlideGroupViewer : public QWidget
{
public:
	SlideGroupViewer(QWidget *parent=0);
	~SlideGroupViewer();

	void setSlideGroup(SlideGroup*, int startSlide = 0);
	int numSlides() { return m_sortedSlides.size(); }
	
	void clear();
	
	MyGraphicsScene * scene() { return m_scene; }

public slots:
	void setSlide(int);
	void nextSlide();
	void prevSlide();

protected:
	void resizeEvent(QResizeEvent *);
	void adjustViewScaling();

private:
	SlideGroup * m_slideGroup;
	QList<Slide*> m_sortedSlides;
//	QHash<int,QPixmap> m_pixmaps;

	MyGraphicsScene * m_scene;
	QGraphicsView * m_view;

};

#endif // SLIDEGROUPVIEWER_H
