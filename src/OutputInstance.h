#ifndef OUTPUTINSTANCE_H
#define OUTPUTINSTANCE_H

#include <QGraphicsView>

#include <QList>
#include <QGraphicsView>
#include <QTimer>
#include <QCloseEvent>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>


#include "model/AbstractItemFilter.h"

#include "SlideGroupViewer.h"

class SlideGroup;
class Slide;

//extern typedef SlideGroupViewer::ViewerState enum;

#include <QWidget>
class Output;
#include "MyGraphicsScene.h"
class JpegServer;
class OutputServer;
class QImage;
class QResizeEvent; 

#include <QPointer>

class OutputInstance : public QWidget
{
	Q_OBJECT
public:
	OutputInstance(Output *output, bool startHidden = false, QWidget *parent=0);
	~OutputInstance();
	
	Output * output() { return m_output; }
	
	SlideGroup * slideGroup();
	
	int numSlides();
	
	void setSceneContextHint(MyGraphicsScene::ContextHint);
	
	Slide *overlaySlide() { return m_overlaySlide; }
	bool isOverlayEnabled() { return m_overlayEnabled; } 
	
	bool isTextOnlyFilterEnabled() { return m_textOnlyFilter; }
	bool isAutoResizeTextEnabled() { return m_autoResizeText; }
	
	bool hasFilter(AbstractItemFilter *filter) { return m_slideFilters.contains(filter); }
	
	int fadeSpeed() { return m_fadeSpeed; }
	int fadeQuality() { return m_fadeQuality; } 

	SlideGroupViewer::ViewerState viewerState() { return m_viewerState; }
	
signals:
	void nextGroup();
	
	void slideChanged(int);
	void slideChanged(Slide*);
	void slideGroupChanged(SlideGroup*,Slide*);
	
	void imageReady(QImage*);

public slots:
	void addMirror(OutputInstance *);
	void removeMirror(OutputInstance *);

	void addFilter(AbstractItemFilter *);
	void removeFilter(AbstractItemFilter *);
	void removeAllFilters();
	
	void setSlideGroup(SlideGroup*, Slide *slide = 0);
	void setSlideGroup(SlideGroup*, int startSlide);
	void clear();
	
	void setBackground(QColor);
	void setCanZoom(bool);
	
	Slide * setSlide(Slide *, bool takeOwnership=false);
	Slide * setSlide(int);
	Slide * nextSlide();
	Slide * prevSlide();
	
	void fadeBlackFrame(bool);
	void fadeClearFrame(bool);

	void setViewerState(SlideGroupViewer::ViewerState);
	
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
	//void resizeEvent(QResizeEvent*);

private:
	void updateControlWidget();

	Output *m_output;
	SlideGroupViewer *m_viewer;
	
	QTimer * m_grabTimer;

	SlideGroup * m_slideGroup;
	QList<Slide*> m_sortedSlides;
	QList<QPointer<Slide> > m_ownedSlides;
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
	
	QWidget * m_ctrlWidget;
	
	QVBoxLayout *m_vbox;
	QHBoxLayout *m_hbox;

	SlideGroupViewer::ViewerState m_viewerState;
	
	bool m_lockResizeEvent;
	
	QList<OutputInstance*> m_mirrors;
};

#endif // SLIDEGROUPVIEWER_H
