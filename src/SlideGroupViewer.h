#ifndef SLIDEGROUPVIEWER_H
#define SLIDEGROUPVIEWER_H

#include <QGraphicsView>

#include <QList>
#include <QGraphicsView>
#include <QTimer>
#include <QCloseEvent>

#include "MyGraphicsScene.h"
#include "model/SlideGroup.h"

class SlideGroupViewer : public QWidget
{
	Q_OBJECT
public:
	SlideGroupViewer(QWidget *parent=0);
	~SlideGroupViewer();

	void setSlideGroup(SlideGroup*, int startSlide = 0);
	int numSlides() { return m_sortedSlides.size(); }
	
	void clear();
	
	MyGraphicsScene * scene() { return m_scene; }
	QGraphicsView * view() { return m_view; }

public slots:
	void setSlide(int);
	void setSlide(Slide*);
	void nextSlide();
	void prevSlide();

private slots:
	void appSettingsChanged();
	void aspectRatioChanged(double);

protected:
	void resizeEvent(QResizeEvent *);
	void closeEvent(QCloseEvent *);
 
	void adjustViewScaling();

private:
	SlideGroup * m_slideGroup;
	QList<Slide*> m_sortedSlides;
//	QHash<int,QPixmap> m_pixmaps;

	int m_slideNum;

	MyGraphicsScene * m_scene;
	QGraphicsView * m_view;
	bool m_usingGL;

};

#endif // SLIDEGROUPVIEWER_H
