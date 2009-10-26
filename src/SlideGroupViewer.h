#ifndef SLIDEGROUPVIEWER_H
#define SLIDEGROUPVIEWER_H

#include <QGraphicsView>

#include <QList>
#include <QGraphicsView>
#include <QTimer>
#include <QCloseEvent>
#include <QFileInfo>

#include "MyGraphicsScene.h"
#include "model/SlideGroup.h"

#include "model/AbstractItemListFilter.h"

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
	
	void setBackground(QColor);
	void setSceneContextHint(MyGraphicsScene::ContextHint);
	
	Slide *overlaySlide() { return m_overlaySlide; }
	bool isOverlayEnabled() { return m_overlayEnabled; } 
	
	bool isTextOnlyFilterEnabled() { return m_textOnlyFilter; }
	bool isAutoResizeTextEnabled() { return m_autoResizeText; }
	
	void addFilter(AbstractItemListFilter *);
	void removeFilter(AbstractItemListFilter *);
	bool hasFilter(AbstractItemListFilter *filter) { return m_slideFilters.contains(filter); }
	
	int fadeSpeed() { return m_fadeSpeed; }
	int fadeQuality() { return m_fadeQuality; } 
	
signals:
	void nextGroup();

public slots:
	Slide * setSlide(Slide *);
	Slide * setSlide(int);
	Slide * nextSlide();
	Slide * prevSlide();
	
	void fadeBlackFrame(bool);
	void fadeClearFrame(bool);
	
	void setLiveBackground(const QFileInfo &, bool waitForNextSlide);
	
	void setOverlaySlide(Slide *);
	void setOverlayEnabled(bool);
	void setTextOnlyFilterEnabled(bool);
	void setAutoResizeTextEnabled(bool);
	
	void setFadeSpeed(int);
	void setFadeQuality(int);
	

private slots:
	void appSettingsChanged();
	void aspectRatioChanged(double);
	
	void videoStreamStarted();
	
	void crossFadeFinished(Slide*,Slide*);
	void slideDiscarded(Slide*);
		
protected:
	void resizeEvent(QResizeEvent *);
	void closeEvent(QCloseEvent *);
 
	void adjustViewScaling();

private:
	MyGraphicsScene * scene() { return m_scene; }
	QGraphicsView * view() { return m_view; }
	
	Slide * applySlideFilters(Slide*);
	AbstractItem * applyMutations(AbstractItem*);
	
	void applySlideFilters();
	
	void applyBackground(const QFileInfo&, Slide *slide=0);

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
	bool m_blackEnabled;
	
	QFileInfo m_nextBg;
	bool m_bgWaitingForNextSlide;
	
	Slide * m_overlaySlide;
	bool m_overlayEnabled;
	
	bool m_textOnlyFilter;
	bool m_autoResizeText;
	
	int m_fadeSpeed;
	int m_fadeQuality;
	
	QList<Slide*>	m_slideFilterByproduct;
	
	QList<AbstractItemListFilter*> m_slideFilters;
};

#endif // SLIDEGROUPVIEWER_H
