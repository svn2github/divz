#ifndef OUTPUTINSTANCE_H
#define OUTPUTINSTANCE_H

#include <QGraphicsView>

#include <QList>
#include <QGraphicsView>
#include <QTimer>
#include <QCloseEvent>
#include <QFileInfo>

#include "model/AbstractItemFilter.h"

class SlideGroupViewer;
class SlideGroup;
class Slide;

#include <QWidget>
class Output;
#include "MyGraphicsScene.h"
class JpegServer;
class OutputServer;
class QImage;

class OutputInstance : public QWidget
{
	Q_OBJECT
public:
	OutputInstance(Output *output, bool startHidden = false, QWidget *parent=0);
	~OutputInstance();
	
	Output * output() { return m_output; }
	
	void setSlideGroup(SlideGroup*, int startSlide);
	void setSlideGroup(SlideGroup*, Slide *slide = 0);
	SlideGroup * slideGroup();
	
	int numSlides();
	void clear();
	
	void setBackground(QColor);
	void setSceneContextHint(MyGraphicsScene::ContextHint);
	void setCanZoom(bool);
	
	Slide *overlaySlide() { return m_overlaySlide; }
	bool isOverlayEnabled() { return m_overlayEnabled; } 
	
	bool isTextOnlyFilterEnabled() { return m_textOnlyFilter; }
	bool isAutoResizeTextEnabled() { return m_autoResizeText; }
	
	void addFilter(AbstractItemFilter *);
	void removeFilter(AbstractItemFilter *);
	bool hasFilter(AbstractItemFilter *filter) { return m_slideFilters.contains(filter); }
	void removeAllFilters();
	
	int fadeSpeed() { return m_fadeSpeed; }
	int fadeQuality() { return m_fadeQuality; } 
	
signals:
	void nextGroup();
	
	void slideChanged(int);
	
	void imageReady(QImage*);

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
	//void appSettingsChanged();
	//void aspectRatioChanged(double);
	void applyOutputSettings(bool startHidden=false);
	
	void slotNextGroup();
	
	void slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant);
	
	void slotGrabPixmap();
	
protected:
	//void closeEvent(QCloseEvent *);

private:
	Output *m_output;
	SlideGroupViewer *m_viewer;
	
	QTimer * m_grabTimer;

	SlideGroup * m_slideGroup;
	QList<Slide*> m_sortedSlides;
//	QHash<int,QPixmap> m_pixmaps;

	int m_slideNum;
	
	Slide * m_overlaySlide;
	bool m_overlayEnabled;
	
	bool m_textOnlyFilter;
	bool m_autoResizeText;
	
	int m_fadeSpeed;
	int m_fadeQuality;

	AbstractItemFilterList m_slideFilters;
	
	bool m_isFoldback;
	
	JpegServer *m_jpegServer;
	QList<QImage*> m_imgBuffer;
	
	OutputServer *m_outputServer;
};

#endif // SLIDEGROUPVIEWER_H
