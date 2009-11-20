#ifndef OutputViewContent_H
#define OutputViewContent_H

#include "AbstractContent.h"

class OutputInstance;
class QGraphicsProxyWidget;

#include "OutputInstance.h"

class OutputViewContent;
class OutputViewInst : public OutputInstance 
{
	Q_OBJECT
public:
	OutputViewInst(OutputViewContent *impl);//Output *output, bool startHidden = false, QWidget *parent=0);
	~OutputViewInst();
	
	SlideGroup * slideGroup();
	
	int numSlides();
	
	void setSceneContextHint(MyGraphicsScene::ContextHint);
	
	void forceGLDisabled(bool);
	
signals:
	void nextGroup();
	void jumpToGroup(int);
	void endOfGroup();
	
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
// 	void setSlideGroup(SlideGroup*, int startSlide);
	void clear();
	
	void setBackground(QColor);
	void setCanZoom(bool);
	
	Slide * setSlide(Slide *, bool takeOwnership=false);
// 	Slide * setSlide(int);
// 	Slide * nextSlide();
// 	Slide * prevSlide();
	
	void fadeBlackFrame(bool);
// 	void fadeClearFrame(bool);

	void setViewerState(SlideGroupViewer::ViewerState);
	
	void setLiveBackground(const QFileInfo &, bool waitForNextSlide);
	
	void setOverlaySlide(Slide *);
	void setOverlayEnabled(bool);
	void setTextOnlyFilterEnabled(bool);
	void setAutoResizeTextEnabled(bool);
	
	void setFadeSpeed(int);
	void setFadeQuality(int);
	
	void setEndActionOverrideEnabled(bool);
	void setEndGroupAction(SlideGroup::EndOfGroupAction);
	
protected slots:
	//void appSettingsChanged();
	//void aspectRatioChanged(double);
	void applyOutputSettings(bool startHidden=false);
	
	//void slotNextGroup();
	//void slotJumpToGroup(int);
	//void slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant);
	
	void slotGrabPixmap();
	
private:
	OutputViewContent * d;

};


class OutputViewRootObject : public QGraphicsItem
{
public:
	OutputViewRootObject(QGraphicsItem *parent, QGraphicsScene*x); 
	QRectF boundingRect() const { return QRectF(); } //MainWindow::mw()->standardSceneRect(); }
	void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) {}
};

/// \brief The OutputViewContent displays a viewer for dviz output inside a slide.
class OutputViewContent : public AbstractContent
{
	Q_OBJECT
public:
	OutputViewContent(QGraphicsScene * scene, QGraphicsItem * parent = 0);
	~OutputViewContent();

public Q_SLOTS:

public:
	// ::AbstractContent
	QString contentName() const { return tr("OutputViewContent"); }
	QWidget * createPropertyWidget();
	QPixmap renderContent(const QSize & size, Qt::AspectRatioMode ratio) const;
	int contentHeightForWidth(int width) const;

	void syncFromModelItem(AbstractVisualItem*);
	AbstractVisualItem * syncToModelItem(AbstractVisualItem*);
	
	void setOutputId(int);
	
	// AbstractContent::
	void dispose(bool);
	
	
	// ::QGraphicsItem
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

	void setSlideGroup(SlideGroup*, Slide *startSlide);
	void setSlide(Slide*);

private:
	AbstractContent * createVisualDelegate(AbstractItem *item, QGraphicsItem * parent=0);
	void addContent(AbstractContent * content, bool takeOwnership);
	
	OutputInstance * m_viewer;
	OutputInstance * m_inst;
	QGraphicsProxyWidget * m_widgetProxy;
	
	SlideGroup *m_group;
	Slide *m_slide;
	OutputViewInst * m_fakeInst;
	OutputViewRootObject * m_liveRoot;
	QList<AbstractContent *> m_content;
	QList<AbstractContent *> m_ownedContent;
	

};

#endif
