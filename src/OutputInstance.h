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

#include "model/SlideGroup.h"
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
	Slide * slide() { return m_slide; }
	
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
	
	bool isClearEnabled() { return m_clearEnabled; }
	bool isBlackEnabled() { return m_blackEnabled; }
	
	bool isEndActionOverrideEnabled() { return m_overrideEndAction; }
	SlideGroup::EndOfGroupAction groupEndAction() { return m_groupEndAction; }
	
	// Must be able to disable OpenGL inorder to embed using QGraphicsProxyWidget
	void forceGLDisabled(bool);
	bool isGLDisabled() { return m_forceGLDisabled; }
	
signals:
	void nextGroup();
	void jumpToGroup(int);
	void endOfGroup();
	
	void slideChanged(int);
	void slideChanged(Slide*);
	void slideGroupChanged(SlideGroup*,Slide*);
	
	void imageReady(QImage*);

public slots:
	virtual void addMirror(OutputInstance *);
	virtual void removeMirror(OutputInstance *);

	virtual void addFilter(AbstractItemFilter *);
	virtual void removeFilter(AbstractItemFilter *);
	virtual void removeAllFilters();
	
	virtual void setSlideGroup(SlideGroup*, Slide *slide = 0);
	virtual void setSlideGroup(SlideGroup*, int startSlide);
	virtual void clear();
	
	void setBackground(QColor);
	void setCanZoom(bool);
	
	virtual Slide * setSlide(Slide *, bool takeOwnership=false);
	virtual Slide * setSlide(int);
	virtual Slide * nextSlide();
	virtual Slide * prevSlide();
	
	virtual void fadeBlackFrame(bool);
// 	void fadeClearFrame(bool);

	virtual void setViewerState(SlideGroupViewer::ViewerState);
	
	virtual void setLiveBackground(const QFileInfo &, bool waitForNextSlide);
	
	virtual void setOverlaySlide(Slide *);
	virtual void setOverlayEnabled(bool);
	virtual void setTextOnlyFilterEnabled(bool);
	virtual void setAutoResizeTextEnabled(bool);
	
	virtual void setFadeSpeed(int);
	virtual void setFadeQuality(int);
	
	virtual void setEndActionOverrideEnabled(bool);
	virtual void setEndGroupAction(SlideGroup::EndOfGroupAction);

protected slots:
	//void appSettingsChanged();
	//void aspectRatioChanged(double);
	virtual void applyOutputSettings(bool startHidden=false);
	
	void slotNextGroup();
	void slotJumpToGroup(int);
	
	void slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant);
	
	void slotGrabPixmap();
	
protected:
	//void closeEvent(QCloseEvent *);
	//void resizeEvent(QResizeEvent*);

protected:
// 	void updateControlWidget();

	Output *m_output;
	SlideGroupViewer *m_viewer;
	
	QTimer * m_grabTimer;

	SlideGroup * m_slideGroup;
	QList<Slide*> m_sortedSlides;
	QList<QPointer<Slide> > m_ownedSlides;
//	QHash<int,QPixmap> m_pixmaps;

	int m_slideNum;
	Slide *m_slide;
	
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
	
	bool m_clearEnabled;
	bool m_blackEnabled;
	
	bool m_overrideEndAction;
	SlideGroup::EndOfGroupAction m_groupEndAction;
	
	bool m_forceGLDisabled;
};

#endif // SLIDEGROUPVIEWER_H
