#ifndef SLIDEGROUPCONTENT_H
#define SLIDEGROUPCONTENT_H

#include "AbstractContent.h"
//#include "qvideo/QVideoBuffer.h"
/*#include "qvideo/QVideo.h"
#include "qvideo/QVideoProvider.h"*/
#include "ButtonItem.h"
#include "OutputInstance.h"


/// \brief SlideGroupContent displayes a slide group from either an output or a specific slide group

class SlideGroupContent;

/// \brief SlideGroupOutputInstanceMirror implements the OutputInstance API 
// so that it can set itself a mirror to an output instance using the addMirror() method.
// It then proxies the commands to the SlideGroupContentViewerImpl (which implements a 
// SlideGroupViewer), which proxies the commands to SlideGroupContent.
class SlideGroupOutputInstanceMirror : public OutputInstance
{
	Q_OBJECT
public:
	SlideGroupOutputInstanceMirror();
	~SlideGroupOutputInstanceMirror();
	
	SlideGroup * slideGroup();
	
	void setSceneContextHint(MyGraphicsScene::ContextHint);

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
	
	void fadeBlackFrame(bool);
	void fadeClearFrame(bool);

	void setViewerState(SlideGroupViewer::ViewerState);
	
	void setOverlaySlide(Slide *);
	void setOverlayEnabled(bool);
	
	void setFadeSpeed(int);
	void setFadeQuality(int);

};

// Implements a SlideGroupViewer which can proxy commands from
// a real OutputInstance as well as SlideGroupOutputInstanceMirror 
// to potentially multiple SlideGroupContent instances.
class SlideGroupContentViewerImpl : public SlideGroupViewer
{
	Q_OBJECT
public:
	SlideGroupContentViewerImpl();

};

class SlideGroupContent : public AbstractContent
{
	Q_OBJECT
	Q_PROPERTY(QString filename READ filename WRITE setFilename)
	
	public:
		SlideGroupContent(QGraphicsScene * scene, QGraphicsItem * parent = 0);
		~SlideGroupContent();
		
		
// 		OutputInstance(Output *output, bool startHidden = false, QWidget *parent=0);
// 		~OutputInstance();
		
		SlideGroup * slideGroup();
		
		int numSlides();
		
		void setSceneContextHint(MyGraphicsScene::ContextHint);
		
		
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
			
		
	public Q_SLOTS:
		QString filename() const { return m_filename; }
		void setFilename(const QString & text);

	private slots:
		void setPixmap(const QPixmap & pixmap);
		void slotTogglePlay();

	public:
		// ::AbstractContent
		QString contentName() const { return tr("VideoFile"); }
		QWidget * createPropertyWidget();
		
		QPixmap renderContent(const QSize & size, Qt::AspectRatioMode ratio) const;
		int contentHeightForWidth(int width) const;
		
		void syncFromModelItem(AbstractVisualItem*);
		AbstractVisualItem * syncToModelItem(AbstractVisualItem*);
		
		void applySceneContextHint(MyGraphicsScene::ContextHint);
		
		// ::QGraphicsItem
		void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
		
	private:
	
		QString m_filename;
		
		QPixmap m_pixmap;
		QSize m_imageSize;
		
		QVideoProvider * m_videoProvider;
		ButtonItem * m_bSwap;
		bool m_still;

};

#endif
