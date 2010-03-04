#ifndef SLIDEGROUPVIEWER_H
#define SLIDEGROUPVIEWER_H

#include <QGraphicsView>

#include <QList>
#include <QGraphicsView>
#include <QTimer>
#include <QCloseEvent>
#include <QFileInfo>
#include <QCache>
#include <QGLWidget>

#include "MyGraphicsScene.h"

#include "model/SlideGroup.h"

#include "model/AbstractItemFilter.h"

class BackgroundContent;

class SlideGroupViewerGraphicsView;

class NativeViewer : public QObject
{
	Q_OBJECT
public:
	NativeViewer();
	virtual ~NativeViewer();

	virtual void setContainerWidget(QWidget*);
	QWidget * containerWidget() { return m_containerWidget; }
	
	virtual bool isEmbeddable() { return false; } // If you return true, 
	virtual QWidget *renderWidget() { return 0; } // SlideGroupViewer will embed your renderWidget() inside itself
	
	virtual void setSceneContextHint(MyGraphicsScene::ContextHint) {}
	
	virtual void setOutputId(int id) { m_outputId=id; }
	int outputId() { return m_outputId; }

	virtual void setSlideGroup(SlideGroup*);
	SlideGroup * slideGroup() { return m_slideGroup; }

	virtual void setSlide(Slide*);
	virtual void setSlide(int) = 0;
	virtual int currentSlide();

	typedef enum NativeShowState {
		Running=1, // Offset from 1 for compat with OLE enums / PPT
		Paused=2,
		Black=3,
		White=4,
		Done=5,
	};

	virtual void setState(NativeShowState);
	virtual NativeShowState state();


	virtual QPixmap snapshot(Slide*);
	virtual QPixmap snapshot(int);
	virtual QPixmap snapshot() = 0;

	virtual void show()  = 0;
	virtual void close() = 0;
	virtual void hide()  = 0;
	
	virtual void setFadeSpeed(int);
	virtual void setFadeQuality(int);
	
	int fadeSpeed() { return m_fadeSpeed; }
	int fadeQuality() { return m_fadeQuality; }

protected:

	QWidget * m_containerWidget;
	SlideGroup * m_slideGroup;
	int m_fadeQuality;
	int m_fadeSpeed;
	int m_outputId;
};

#ifdef Q_OS_WIN32
    #include <windows.h>
#else
    #define HWND int
#endif

class NativeViewerWin32 : public NativeViewer
{
	Q_OBJECT
public:
	NativeViewerWin32();

	void setHwnd(HWND);
	HWND hwnd() { return m_hwnd; }

	virtual void show();
	virtual void close();
	virtual void hide();

	virtual QPixmap snapshot();

protected slots:
	virtual void embedHwnd();


protected:
	HWND m_hwnd;

};

class WidgetUtil
{
public:
	static QWidget * getTopLevelWidget(QWidget*);
	static QPoint absoluteWidgetPosition(QWidget*);
};

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
	
	void addFilter(AbstractItemFilter *);
	void removeFilter(AbstractItemFilter *);
	bool hasFilter(AbstractItemFilter *filter) { return m_slideFilters.contains(filter); }
	void removeAllFilters();
	
	int fadeSpeed()   { return m_fadeSpeed;   }
	int fadeQuality() { return m_fadeQuality; } 
	
	void setCanZoom(bool);
	bool canZoom();

	void setIsPreviewViewer(bool);
	
	static Slide * blackSlide();

	NativeViewer * nativeViewer() { return m_nativeViewer; }
	void setNativeViewer(NativeViewer*);

	typedef enum ViewerState
	{
		Black,
		Done,
		Paused,
		Running,
		Clear
	};

	ViewerState viewerState() { return m_viewerState; }
	
	bool isEndActionOverrideEnabled() { return m_overrideEndAction; }
	SlideGroup::EndOfGroupAction groupEndAction() { return m_groupEndAction; }
	
	// Only relevant AFTER a slide has been set on the instance.
	QList<QWidget*> controlWidgets(); 
	
	bool isTransitionActive();
	
	bool startBackgroundVideoPausedInPreview();
	void setStartBackgroundVideoPausedInPreview(bool flag);
	
	
	int outputId() { return m_outputId; }

signals:
	void nextGroup();
	void jumpToGroup(int);
	void endOfGroup();
	void viewerStateChanged(ViewerState);
	void slideChanged(int);

public slots:
	void setOutputId(int id) { m_outputId=id; }
	Slide * setSlide(Slide *, bool takeOwnership=false);
	Slide * setSlide(int);
	Slide * nextSlide();
	Slide * prevSlide();
	
	void fadeBlackFrame(bool);
// 	void fadeClearFrame(bool);
	
	void setViewerState(ViewerState);

	void setLiveBackground(const QFileInfo &, bool waitForNextSlide);
	
	void setOverlaySlide(Slide *);
	void setOverlayEnabled(bool);
	void setTextOnlyFilterEnabled(bool);
	void setAutoResizeTextEnabled(bool);
	
	void setFadeSpeed(int);
	void setFadeQuality(int);
	
	void setEndActionOverrideEnabled(bool);
	void setEndGroupAction(SlideGroup::EndOfGroupAction);
	
	// Must be able to disable OpenGL inorder to embed using QGraphicsProxyWidget
	void forceGLDisabled(bool);
	bool isGLDisabled() { return m_forceGLDisabled; }

private slots:
	void appSettingsChanged();
	void aspectRatioChanged(double);
	
	void videoStreamStarted();
	
	void crossFadeFinished(Slide*,Slide*);
	void slideDiscarded(Slide*);
	
	void slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);

	void checkCurrentNativeSlide();
		
protected:
	void resizeEvent(QResizeEvent *);
	void closeEvent(QCloseEvent *);
// 	void paintEvent(QPaintEvent *);
 
	void adjustViewScaling();

private:
	MyGraphicsScene * scene() { return m_scene; }
	
	Slide * applySlideFilters(Slide*);
	AbstractItem * applyMutations(AbstractItem*);
	
	// internal routine to find the cross fade quality/speed and set the slideon the scene
	void setSlideInternal(Slide*);
	// just calls setSlideInternal(applySlideFilters(currentSlide)) and finds the current slide
	void applySlideFilters();
	// internal routine to apply the background to the slide group
	void applyBackground(const QFileInfo&, Slide *slide=0);
	
// 	void generateClearFrame();
	static void generateBlackFrame();
	
	bool reapplySpecialFrames();
	
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
	SlideGroupViewerGraphicsView * m_view;
	bool m_usingGL;

	static Slide * m_blackSlide;
	static int m_blackSlideRefCount;
	
// 	Slide * m_clearSlide;
// 	int m_clearSlideNum;
	
// 	bool m_clearEnabled;
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
	
	AbstractItemFilterList m_slideFilters;
	
	Slide * m_clonedSlide;
	Slide * m_clonedOriginal;
	
	bool m_fadeInProgress;

	bool m_isPreviewViewer;

	ViewerState m_viewerState;

	QTimer m_nativeCheckTimer;
	
	NativeViewer * m_nativeViewer;
	
	bool m_overrideEndAction;
	SlideGroup::EndOfGroupAction m_groupEndAction;

	bool m_forceGLDisabled;
	
	MyGraphicsScene::ContextHint m_contextHint;
	
	int m_outputId;
	
	BackgroundContent * m_bgContent;
};

#endif // SLIDEGROUPVIEWER_H
