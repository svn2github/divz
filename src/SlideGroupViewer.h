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

	//void setSlideGroup(SlideGroup*, int startSlide = 0);
	void setSlideGroup(SlideGroup*, Slide *slide = 0);
	SlideGroup * slideGroup() { return m_slideGroup; }
	
	int numSlides() { return m_sortedSlides.size(); }
	
	void clear();
	
	MyGraphicsScene * scene() { return m_scene; }
	QGraphicsView * view() { return m_view; }

signals:
	void nextGroup();

public slots:
	Slide * setSlide(Slide *);
	Slide * setSlide(int);
	Slide * nextSlide();
	Slide * prevSlide();
	
	void fadeBlackFrame(bool);
	void fadeClearFrame(bool);

private slots:
	void appSettingsChanged();
	void aspectRatioChanged(double);
	
	void videoStreamStarted();
		
protected:
	void resizeEvent(QResizeEvent *);
	void closeEvent(QCloseEvent *);
 
	void adjustViewScaling();

private:
	void initVideoProviders();
	void releaseVideoProvders();
	QList<QVideoProvider*> m_videoProviders;
	QMap<QString,bool> m_videoProvidersConsumed;
	QMap<QString,bool> m_videoProvidersOpened;
	
	SlideGroup * m_slideGroup;
	QList<Slide*> m_sortedSlides;
//	QHash<int,QPixmap> m_pixmaps;

	int m_slideNum;

	MyGraphicsScene * m_scene;
	QGraphicsView * m_view;
	bool m_usingGL;

	static Slide * m_blackSlide;
	Slide * m_clearSlide;
	int m_clearSlideNum;
	bool m_clearEnabled;
};

#endif // SLIDEGROUPVIEWER_H
