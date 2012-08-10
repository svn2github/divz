#include "SlideGroupViewer.h"

#include "RenderOpts.h"

#include <QVBoxLayout>
#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif

#ifdef DVIZ_HAS_QVIDEO
#include "qvideo/QVideoProvider.h"
#endif

#include "MainWindow.h"
#include "AppSettings.h"
#include "MediaBrowser.h"

#include "MyGraphicsScene.h"

#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/TextBoxItem.h"
#include "model/BackgroundItem.h"
#include "model/SlideGroupFactory.h"

#include "itemlistfilters/SlideTextOnlyFilter.h"

#include "SharedMemoryImageWriter.h"

#include "JpegServer.h"

#define PTRS(ptr) QString().sprintf("%p",static_cast<void*>(ptr))

#include <QMessageBox>
#include <QApplication>

#define MAX_BYPRODUCT_SIZE 10
#define NATIVE_CHECK_TIMEOUT 500

/** NativeViewer **/
NativeViewer::NativeViewer()
	: QObject()
	, m_slideGroup(0)
	//, m_contextHint(MyGraphicsScene::Live)
	, m_outputId(0)
{}

NativeViewer::~NativeViewer()
{}

void NativeViewer::dispose()
{
	close();
	deleteLater();
}
	
void NativeViewer::setContainerWidget(QWidget *w)
{
    m_containerWidget = w;
}

void NativeViewer::setSlideGroup(SlideGroup *group)
{
    m_slideGroup = group;
}

void NativeViewer::setSlide(Slide *slide)
{
    int idx = m_slideGroup->indexOf(slide);
    setSlide(idx);
}

void NativeViewer::setFadeSpeed(int value)
{
	m_fadeSpeed = value;
}

void NativeViewer::setFadeQuality(int value)
{
	m_fadeQuality = value;
}

QPixmap NativeViewer::snapshot(Slide *slide)
{
    setSlide(slide);
    return snapshot();
}

QPixmap NativeViewer::snapshot(int x)
{
    setSlide(x);
    return snapshot();
}

/** NativeViewerWin32 **/
NativeViewerWin32::NativeViewerWin32()
	: NativeViewer()
{}

void NativeViewerWin32::setHwnd(HWND hwnd)
{
    m_hwnd = hwnd;
}

void NativeViewerWin32::show()
{
	//qDebug() << "NativeViewerWin32::show(): SingleShot till embedHwnd()";
	qApp->processEvents(); // flush buffer (e.g. show any widgets)
	QTimer::singleShot(0,this,SLOT(embedHwnd()));
	qApp->processEvents(); // flush buffer (clear out any other show calls)
	qApp->processEvents(); // call our singleshot
}

void NativeViewerWin32::close()
{
#ifdef Q_OS_WIN32
    CloseWindow(hwnd());
#endif
}

void NativeViewerWin32::hide()
{
#ifdef Q_OS_WIN32
    SetWindowPos(hwnd(), 0, 0,0,0,0, SWP_HIDEWINDOW);
#endif
}

int NativeViewer::currentSlide() { return -1; }
void NativeViewer::setState(NativeShowState) {}
NativeViewer::NativeShowState NativeViewer::state() { return NativeViewer::Done; }


QPixmap NativeViewerWin32::snapshot()
{
    return QPixmap::grabWindow(hwnd());
}

QPoint WidgetUtil::absoluteWidgetPosition(QWidget *w)
{
    QPoint pos = w->geometry().topLeft();
    if(w->parentWidget())
    {
	QPoint parentPos = WidgetUtil::absoluteWidgetPosition(w->parentWidget());
	pos += parentPos;
    }
    return pos;
}


QWidget * WidgetUtil::getTopLevelWidget(QWidget *w)
{
    if(w->parentWidget())
    	return WidgetUtil::getTopLevelWidget(w->parentWidget());
    return w;
}

void NativeViewerWin32::embedHwnd()
{
	QRect rect = containerWidget()->geometry();
	QPoint abs = WidgetUtil::absoluteWidgetPosition(containerWidget());

#ifdef Q_OS_WIN32
	//MoveWindow(hwnd(), rect.x(), rect.x(), rect.width(), rect.height(), 1);
	//qDebug() << "NativeViewerWin32::embedHwnd(): hwnd:"<<hwnd()<<", rect:"<<rect<<", abs:"<<abs;

	SetWindowPos(hwnd(), 0,
			abs.x(), abs.y(),
			rect.width(), rect.height(),
			SWP_SHOWWINDOW);
	BringWindowToTop(hwnd());
#endif
}




class SlideGroupViewerGraphicsView : public QGraphicsView
{
	public:
		SlideGroupViewerGraphicsView(QWidget * parent)
			: QGraphicsView(parent)
			, m_canZoom(false)
		{
			setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
			setCacheMode(QGraphicsView::CacheBackground);
			setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
			setOptimizationFlags(QGraphicsView::DontSavePainterState);
			setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
			setTransformationAnchor(AnchorUnderMouse);
			setResizeAnchor(AnchorViewCenter);
		}

		bool canZoom() { return m_canZoom; }
		void setCanZoom(bool flag) { m_canZoom = flag; }


	protected:
		void keyPressEvent(QKeyEvent *event)
		{
			if(event->modifiers() & Qt::ControlModifier)
			{

				switch (event->key())
				{
					case Qt::Key_Plus:
						scaleView(qreal(1.2));
						break;
					case Qt::Key_Minus:
                                        case Qt::Key_Equal:
						scaleView(1 / qreal(1.2));
						break;
					default:
						QGraphicsView::keyPressEvent(event);
				}
			}
		}


		void wheelEvent(QWheelEvent *event)
		{
                        scaleView(pow((double)2, event->delta() / 240.0));
		}


		void scaleView(qreal scaleFactor)
		{
			if(!m_canZoom)
				return;

			qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
			if (factor < 0.07 || factor > 100)
				return;

			scale(scaleFactor, scaleFactor);
		}
	private:
		bool m_canZoom;
};




Slide * SlideGroupViewer::m_blackSlide = 0;
int SlideGroupViewer::m_blackSlideRefCount = 0;

SlideGroupViewer::SlideGroupViewer(QWidget *parent)
	    : QWidget(parent)
	    , m_slideGroup(0)
	    , m_slideNum(-1)
	    , m_scene(0)
	    , m_view(0)
	    , m_usingGL(false)
// 	    , m_clearSlide(0)
// 	    , m_clearSlideNum(-1)
// 	    , m_clearEnabled(false)
	    , m_blackEnabled(false)
	    , m_bgWaitingForNextSlide(false)
	    , m_overlaySlide(0)
	    , m_overlayEnabled(false)
	    , m_textOnlyFilter(false)
	    , m_autoResizeText(false)
	    , m_fadeSpeed(-1)
	    , m_fadeQuality(-1)
	    , m_clonedSlide(0)
	    , m_clonedOriginal(0)
	    , m_fadeInProgress(false)
	    , m_isPreviewViewer(false)
	    , m_viewerState(SlideGroupViewer::Running)
	    , m_nativeViewer(0)
	    , m_overrideEndAction(false)
	    , m_groupEndAction(SlideGroup::Stop)
	    , m_forceGLDisabled(false)
	    , m_contextHint(MyGraphicsScene::Live)
	    , m_outputId(0)
	    , m_shmemWriter(0)
	    , m_sharedMemoryImageWriterEnabled(false)
	    , m_jpegServer(0)
	    , m_ignoreAspectRatio(false)
{
	QRect sceneRect(0,0,1024,768);
	m_blackSlideRefCount++;

	if(MainWindow::mw())
	{
		MainWindow *mw = MainWindow::mw();
		sceneRect = mw->standardSceneRect();

		connect(mw, SIGNAL(appSettingsChanged()), this, SLOT(appSettingsChanged()));
 		connect(mw, SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
	}

	//m_view = new QGraphicsView(this);
	m_view = new SlideGroupViewerGraphicsView(this);
	m_view->setFrameStyle(QFrame::NoFrame);
	m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	m_view->setOptimizationFlags(QGraphicsView::DontSavePainterState);

	appSettingsChanged();
	//qDebug("SlideGroupViewer: Loaded OpenGL viewport.");

	m_contextHint = MyGraphicsScene::Live;
	m_scene = new MyGraphicsScene(m_contextHint);
	m_scene->setSceneRect(sceneRect);
	m_view->setScene(m_scene);
	//m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);

	// inorder to set the background on the rest of the slides if asked to set background on next live slide
	connect(m_scene, SIGNAL(crossFadeFinished(Slide*,Slide*)), this, SLOT(crossFadeFinished(Slide*,Slide*)));

	// inorder to delete any slides created during the applyFilteres phase
	connect(m_scene, SIGNAL(slideDiscarded(Slide*)), this, SLOT(slideDiscarded(Slide*)));

	connect(&m_nativeCheckTimer, SIGNAL(timeout()), this, SLOT(checkCurrentNativeSlide()));

// 	m_view->setBackgroundBrush(Qt::green); 

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(m_view);
	setLayout(layout);

// 	QPalette p;
// 	p.setColor(QPalette::Window, Qt::black);
// 	setPalette(p);
}

void SlideGroupViewer::setSharedMemoryImageWriterEnabled(bool /*enable*/, const QString& /*key*/)
{
// 	m_sharedMemoryImageWriterEnabled = enable;
// 	if(!m_shmemWriter)
// 		m_shmemWriter = new SharedMemoryImageWriter(this);
// 	if(enable)
// 		m_shmemWriter->enable(key,m_scene);
// 	else
// 		m_shmemWriter->disable();
}

bool SlideGroupViewer::canZoom() { return m_view->canZoom(); }

void SlideGroupViewer::setCanZoom(bool flag)
{
	m_view->setCanZoom(flag);
}

void SlideGroupViewer::setIsPreviewViewer(bool flag)
{
	m_isPreviewViewer = flag;
}

void SlideGroupViewer::setNativeViewer(NativeViewer *view)
{
	m_nativeViewer = view;
}

void SlideGroupViewer::checkCurrentNativeSlide()
{
	if(!m_nativeViewer)
		return;

	if(m_nativeViewer->currentSlide() != m_slideNum)
	{
		m_slideNum = m_nativeViewer->currentSlide();
		emit slideChanged(m_slideNum);
		qDebug() << "SlideGroupViewer::checkCurrentNativeSlide: Slide Changed, New Slide: "<<m_slideNum;

		// if slide changed, sync state
		m_viewerState = (ViewerState)((int)m_nativeViewer->state());
	}
}

void SlideGroupViewer::setOverlaySlide(Slide * newSlide)
{
	m_overlaySlide = newSlide;
	applySlideFilters();
}

void SlideGroupViewer::setOverlayEnabled(bool enable)
{
	m_overlayEnabled = enable;
	applySlideFilters();
}

void SlideGroupViewer::setTextOnlyFilterEnabled(bool enable)
{
	m_textOnlyFilter = enable;
	if(enable)
		addFilter(SlideTextOnlyFilter::instance());
	else
		removeFilter(SlideTextOnlyFilter::instance());
	//applySlideFilters();
}

void SlideGroupViewer::setAutoResizeTextEnabled(bool enable)
{
	m_autoResizeText = enable;
	applySlideFilters();
}

void SlideGroupViewer::setFadeSpeed(int value)
{
	m_fadeSpeed = value;
	if(m_nativeViewer)
		m_nativeViewer->setFadeSpeed(value);
}

void SlideGroupViewer::setFadeQuality(int value)
{
	m_fadeQuality = value;
	if(m_nativeViewer)
		m_nativeViewer->setFadeQuality(value);
}

void SlideGroupViewer::applySlideFilters()
{
	if(!m_slideGroup)
		return;

	if(!m_blackEnabled /*&& !m_clearEnabled*/)
	{
		if(m_sortedSlides.isEmpty() || 
			m_slideNum < 0 || 
			m_slideNum >= m_sortedSlides.size())
			return;
			
		Slide *currentSlide = m_sortedSlides.at(m_slideNum);
		setSlideInternal(applySlideFilters(currentSlide));
	}
}

bool SlideGroupViewer_itemZCompare(AbstractItem *a, AbstractItem *b)
{
	AbstractVisualItem * va = dynamic_cast<AbstractVisualItem*>(a);
	AbstractVisualItem * vb = dynamic_cast<AbstractVisualItem*>(b);

	// Decending sort (eg. highest to lowest) because we want the highest
	// zvalue first n the list instead of last
	return (va && vb) ? va->zValue() > vb->zValue() : true;
}


Slide * SlideGroupViewer::applySlideFilters(Slide * sourceSlide)
{
	if((!m_overlaySlide || !m_overlayEnabled)
		&& !m_textOnlyFilter
		&& !m_autoResizeText
		&& m_slideFilters.size() <= 0)
		return sourceSlide;

	Slide * slide = new Slide();

	// for deleting this slide after the scene is done with it (in slideDiscarded())
	m_slideFilterByproduct << slide;

	double baseZValue = 0;
	BackgroundItem * originalBg = 0;

	AbstractItemList origList = sourceSlide->itemList();
	foreach(AbstractItem * sourceItem, origList)
	{
		bool approved = true;
		foreach(AbstractItemFilter * filter, m_slideFilters)
			if(!filter->approve(sourceItem))
				approved = false;

		if(approved)
		{
			AbstractItem *mutated = applyMutations(sourceItem);

			// if mutated != originalItem, that means that the filter returned a clone()'ed item with changes,
			// therefore, allow the slide to take ownership and delete the mutated item when slide is discared
			// (in the slideDiscarded() slot)
			// However, if the muted is the SAME as the sourceItem, we DONT want the slide deleting the sourceItem,
			// because the sourceItem comes straight from the sourceSlide, which still has to exist long after
			// this slide goes off-screen.
			if(mutated)
				slide->addItem(mutated,mutated != sourceItem);

			// determine max zvalue for use in rebasing overlay items
			AbstractVisualItem * visual = dynamic_cast<AbstractVisualItem*>(sourceItem);
			if(visual && visual->zValue() > baseZValue)
				baseZValue = visual->zValue();

			// find the background in this list in case their is a background in the overlay
			BackgroundItem * bgTmp = dynamic_cast<BackgroundItem*>(sourceItem);
			if(bgTmp && bgTmp->fillType() != AbstractVisualItem::None)
				originalBg = bgTmp;
		}
	}

	// Apply auto size text filter BEFORE the overlay so we dont resize any overlay text boxes
	if(m_autoResizeText)
	{
		// Use the first textbox in the slide as the slide to resize
		// "first" as defined by ZValue
		AbstractItemList items = slide->itemList();
		qSort(items.begin(), items.end(), SlideGroupViewer_itemZCompare);


		TextBoxItem *text = 0;

		foreach(AbstractItem * item, items)
		{
// 			AbstractVisualItem * newVisual = dynamic_cast<AbstractVisualItem*>(item);
			//qDebug()<<"SongSlideGroup::textToSlides(): slideNbr:"<<slideNbr<<": item list
			if(!text && item->inherits("TextBoxItem"))
			{
				text = dynamic_cast<TextBoxItem*>(item);

			}
		}

		if(text)
		{
			TextBoxItem * autoText = dynamic_cast<TextBoxItem*>(text->clone());
			slide->removeItem(text);

			QSize scene = m_scene->sceneRect().size().toSize();

			int fittedHeight = autoText->fitToSize(scene);

			// Center text on screen
			qreal y = scene.height()/2 - fittedHeight /2;
			//qDebug() << "SongSlideGroup::textToSlides(): centering: boxHeight:"<<boxHeight<<", textRect height:"<<textRect.height()<<", centered Y:"<<y;
			autoText->setContentsRect(QRectF(0,y,scene.width(),fittedHeight));

			// Outline pen for the text
			//QPen pen = QPen(Qt::black,1.5);
			//pen.setJoinStyle(Qt::MiterJoin);

			autoText->setPos(QPointF(0,0));
			autoText->setOutlinePen(Qt::NoPen);
			autoText->setOutlineEnabled(false);
			autoText->setFillBrush(Qt::white);
			autoText->setFillType(AbstractVisualItem::Solid);
			autoText->setShadowEnabled(false);

			slide->addItem(autoText);
		}
	}

	// Apply slide overlay
	if(m_overlaySlide && m_overlayEnabled)
	{
		bool secondaryBg = false;
		AbstractItemList items = m_overlaySlide->itemList();

		foreach(AbstractItem * overlayItem, items)
		{
			// TODO should filters apply to overlays? to be decided, but for now, yes.
			bool approved = true;
			foreach(AbstractItemFilter * filter, m_slideFilters)
				if(!filter->approve(overlayItem))
					approved = false;

			if(approved)
			{

				BackgroundItem * bg = dynamic_cast<BackgroundItem*>(overlayItem);
				if(bg)
				{
					if(bg->fillType() == AbstractVisualItem::None)
					{
						//qDebug() << "Skipping inherited bg from seondary slide because exists and no fill";
						continue;
					}
					else
					{
						secondaryBg = true;
					}
				}


				AbstractVisualItem * visual = dynamic_cast<AbstractVisualItem*>(overlayItem);

				// rebase zvalue so everything in this slide is on top of the original slide
				if(visual && baseZValue!=0)
				{
					visual->setZValue(visual->zValue() + baseZValue);
					//qDebug()<<"SlideGroupViewer::applySlideFilters: rebased" << newVisual->itemName() << "to new Z" << newVisual->zValue();
				}

				AbstractItem *mutated = applyMutations(overlayItem);

				// if mutated != originalItem, that means that the filter returned a clone()'ed item with changes,
				// therefore, allow the slide to take ownership and delete the mutated item when slide is discared
				// (in the slideDiscarded() slot)
				slide->addItem(mutated,mutated != overlayItem);
			}

		}

		// If we have both a overlay and a original bg, remove the original bg
		if(secondaryBg && originalBg)
			slide->removeItem(originalBg);
	}

	// nitch case - not sure if it will ever get hit (why would the slide NOT have a background by now?)
	// but if it doesn't, we still should generate a background and pass it thru the filters
	if(!originalBg)
	{
		AbstractVisualItem * originalItem = dynamic_cast<AbstractVisualItem*>(slide->background());
		AbstractItem *mutated = applyMutations(originalItem);
		if(mutated != originalItem)
		{
			slide->removeItem(originalItem);
			slide->addItem(mutated,true); // allow slide to delete our mutated backgroundd

			// nitch note:
			// since we DIDN'T find an original background (originalBg was false),
			// that means the call to slide->background() called 'new BackgroundItem'
			// and since we removed the newly create background item to replace it with our
			// mutation, that means that it (the originalItem) won't get deleted when
			// the slide is deleted, so we must delete it here.
			delete originalItem;
		}

	}

	return slide;
}


AbstractItem * SlideGroupViewer::applyMutations(AbstractItem *originalItem)
{
	bool mutated = false;
	AbstractItem *mutateTmp = originalItem;
	foreach(AbstractItemFilter * filter, m_slideFilters)
	{
		mutateTmp = filter->mutate(mutateTmp);
		if(mutateTmp)
			mutated = true;
		else
			mutateTmp = originalItem;
	}

	return mutateTmp;
}

void SlideGroupViewer::applyBackground(const QFileInfo &info, Slide * onlyThisSlide)
{
	if(!m_slideGroup)
		return;

	m_slideGroup->changeBackground(info,onlyThisSlide);

}

void SlideGroupViewer::setLiveBackground(const QFileInfo &info, bool waitForNextSlide)
{
	QString ext = info.suffix();
	if(!MediaBrowser::isVideo(ext) &&
	   !MediaBrowser::isImage(ext))
	{
		if(!MainWindow::mw()->isNetworkViewer())
			QMessageBox::warning(this,"Unknown File Type","I'm not sure how to handle that file. Sorry!");
		return;
	}

	if(!waitForNextSlide)
		applyBackground(info);
	else
	{
		m_nextBg = info;
		//qDebug() << "SlideGroupViewer::setLiveBackground: set m_bgWaitingForNextSlide to true"; 
		m_bgWaitingForNextSlide = true;
	}

}

void SlideGroupViewer::setBackground(QColor c)
{
	m_view->setBackgroundBrush(c);
}

void SlideGroupViewer::setSceneContextHint(MyGraphicsScene::ContextHint hint)
{
	m_contextHint = hint;
	scene()->setContextHint(hint);
	if(m_nativeViewer)
		m_nativeViewer->setSceneContextHint(hint);
	//qDebug() << "SlideGroupViewer::setSceneContextHint(): "<<this<<": hint:"<<hint;
}

bool SlideGroupViewer::isTransitionActive()
{
	return scene()->isTransitionActive();
}

SlideGroupViewer::~SlideGroupViewer()
{
	if(m_scene)
	{
		delete m_scene;
		m_scene = 0;
	}

	if(m_view)
	{
		delete m_view;
		m_view = 0;
	}

// 	if(m_clearSlide)
// 	{
// 		delete m_clearSlide;
// 		m_clearSlide = 0;
// 	}

	while(!m_slideFilterByproduct.isEmpty())
		delete m_slideFilterByproduct.takeFirst();

	m_blackSlideRefCount --;
	if(m_blackSlideRefCount <= 0 && m_blackSlide)
	{
		delete m_blackSlide;
		m_blackSlide = 0;
		qDebug() << "SlideGroupViewer: Deleting black slide";
	}
	
	if(m_nativeViewer)
	{
		m_nativeViewer->dispose();
		m_nativeViewer = 0;
	}
	
}
void SlideGroupViewer::forceGLDisabled(bool flag)
{
	m_forceGLDisabled = flag;
	
	appSettingsChanged();
}

void SlideGroupViewer::appSettingsChanged()
{
#ifndef QT_NO_OPENGL
	if(!m_usingGL && !m_forceGLDisabled && AppSettings::useOpenGL())
	{
		m_usingGL = true;
		QGLWidget *glw = new QGLWidget(QGLFormat(QGL::SampleBuffers));
		//if (noScreenSync)
		//	glw->format().setSwapInterval(0);
		glw->setAutoFillBackground(false);
		m_view->setViewport(glw);
		m_view->setCacheMode(QGraphicsView::CacheNone);
		
		//m_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
		//qDebug("SlideGroupViewer::appSettingsChanged(): Loaded OpenGL Viewport");
	}
	else
	if(m_usingGL && (m_forceGLDisabled || !AppSettings::useOpenGL()))
	{
#endif
		m_usingGL = false;
		m_view->setViewport(new QWidget());
		m_view->setCacheMode(QGraphicsView::CacheBackground);
		//qDebug("SlideGroupViewer::appSettingsChanged(): Loaded Non-GL Viewport");
#ifndef QT_NO_OPENGL
	}
#endif
	
// 	QBrush brBrush;
// 	QPalette p(brBrush,brBrush,brBrush,brBrush,brBrush,brBrush,brBrush,brBrush,brBrush);
// // 	QPalette p;
// // 	p.setBrush(QPalette::Window, QBrush());
// 	m_view->viewport()->setPalette(p);
}

void SlideGroupViewer::aspectRatioChanged(double /*x*/)
{
	//qDebug("SlideGroupViewer::aspectRatioChanged(): New aspect ratio: %.02f",x);
	m_scene->setSceneRect(MainWindow::mw()->standardSceneRect());
	adjustViewScaling();
}


void SlideGroupViewer::closeEvent(QCloseEvent *event)
{
	event->accept();
	m_scene->clear();
	close();
	
	if(m_nativeViewer)
	{
		m_nativeViewer->dispose();
		m_nativeViewer = 0;
	}
	
	deleteLater();
}

bool slide_group_viewer_slide_num_compare(Slide *a, Slide *b)
{
	return (a && b) ? a->slideNumber() < b->slideNumber() : true;
}

void SlideGroupViewer::clear()
{
	releaseVideoProvders();
	m_scene->clear();
	m_slideGroup = 0;
	
	if(m_nativeViewer)
	{
		m_nativeViewer->dispose();
		m_nativeViewer = 0;
	}
	
	if(m_jpegServer)
		m_jpegServer->slideChanged();
}

void SlideGroupViewer::slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString /*operation*/, QString /*fieldName*/, QVariant /*value*/)
{
	if(!m_slideGroup)
		return;

	if(slideOperation == "add" || slideOperation == "remove")
	{
		QList<Slide*> slist = m_slideGroup->slideList();
		qSort(slist.begin(), slist.end(), slide_group_viewer_slide_num_compare);
		m_sortedSlides = slist;

		if(m_slideNum >= m_sortedSlides.size())
			m_slideNum = 0;

 		if(AppSettings::liveEditMode() == AppSettings::SmoothEdit)
 		{
 			qDebug() << "SlideGroupViewer::slideChanged() [slot]: SmoothEdit selected, re-showing slide due to add/remove item";
 			setSlideInternal(applySlideFilters(slide));
 		}
	}
	else
	{
		int nbr = m_sortedSlides.indexOf(slide);
		if(nbr == m_slideNum)
			if(!reapplySpecialFrames())
				if(AppSettings::liveEditMode() == AppSettings::SmoothEdit)
				{
					qDebug() << "SlideGroupViewer::slideChanged() [slot]: SmoothEdit selected, re-showing slide due to change on item: "<<item->itemName();
					setSlideInternal(applySlideFilters(slide));
				}
	}

}

void SlideGroupViewer::setSlideGroup(SlideGroup *group, Slide *startSlide)
{
	//qDebug() << "SlideGroupViewer::setSlideGroup: (SceneContextHint:"<<m_scene->contextHint()<<"), setting slide group:"<<group->assumedName();
	if(m_slideGroup == group)
	{
		//qDebug() << "SlideGroupViewer::setSlideGroup: Same group, setting start slide.";
		//if(startSlide)
		//	setSlide(startSlide);
		//return;
	}

	bool blackNative = false;
	if(m_nativeViewer)
	{
		//QWidget * topLevel = WidgetUtil::getTopLevelWidget(this);
		//topLevel->show();
		

		if(m_nativeViewer->isEmbeddable())
		{
			m_view->setVisible(true);
			layout()->removeWidget(m_nativeViewer->renderWidget());
		}
		else
		{
			parentWidget()->show();
		}
		
		m_nativeViewer->close();
		delete m_nativeViewer;
		m_nativeViewer = 0;

		blackNative = true;

		if(m_nativeCheckTimer.isActive())
			m_nativeCheckTimer.stop();
	}

	m_slideNum = 0;

// 	m_clearSlideNum = -1;


	if(m_slideGroup && m_slideGroup != group)
	{
		disconnect(m_slideGroup,0,this,0);
		//qDebug() << "SlideGroupViewer::setSlideGroup: Releasing video providers due to slide change";
		releaseVideoProvders();
		
	}

	if(m_slideGroup != group)
		connect(group,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));

	m_slideGroup = group;

	if(!m_isPreviewViewer)// || m_contextHint == MyGraphicsScene::Preview)
	{
		SlideGroupFactory *factory = SlideGroupFactory::factoryForType(m_slideGroup->groupType());
		if(factory)
		{
			NativeViewer *native = factory->newNativeViewer();
			if(native)
			{
				if(m_blackEnabled)
				{
					qDebug() << "SlideGroupViewer::setSlideGroup: Native viewer with black - starting off as black";
					native->setState(NativeViewer::Black);
				}
				
				qDebug() << "SlideGroupViewer::setSlideGroup: Got native viewer, setting up.";
				native->setOutputId(outputId());
				native->setFadeSpeed(m_fadeSpeed);
				native->setFadeQuality(m_fadeQuality);
				native->setSceneContextHint(m_contextHint);
				native->setContainerWidget(this);
				native->setSlideGroup(m_slideGroup);
				
				if(native->isEmbeddable())
				{
					native->renderWidget()->setParent(this);
					layout()->addWidget(native->renderWidget());
					m_view->setVisible(false);
				}
				
				
				native->show();
				
				// copy current 'black' state to viewer
				
				if(!m_blackEnabled)
					native->setState(NativeViewer::Running);
				
				blackNative = false;
	
				//qDebug() << "SlideGroupViewer::setSlideGroup: Setup done, mudging our style and hiding ourself.";
	
				//fadeBlackFrame(true);
				
				//QWidget * topLevel = WidgetUtil::getTopLevelWidget(this);
				//topLevel->setWindowFlags(Qt::WindowStaysOnBottomHint);
				
				//topLevel->hide();
				if(!native->isEmbeddable())
					parentWidget()->hide();
				
				// set native viewer AFTER fadeBlackFrame() called (above) so fadeblack affects the Qt viewer, not native
				m_nativeViewer = native;
				
				m_nativeCheckTimer.start(NATIVE_CHECK_TIMEOUT);
				
				
				//qDebug() << "SlideGroupViewer::setSlideGroup: Setup complete.";
			}
			else
			{
				//qDebug() << "SlideGroupViewer::setSlideGroup: Factory for group, but no viewer.";
			}
		}
		else
		{
			//qDebug() << "SlideGroupViewer::setSlideGroup: No Factory for group";
		}
	}

	QList<Slide*> slist = group->slideList();
	qSort(slist.begin(), slist.end(), slide_group_viewer_slide_num_compare);
	m_sortedSlides = slist;

	// See comments on the function itself for what this is for
	//qDebug() << "SlideGroupViewer::setSlideGroup: Initalizing video providers...";
	initVideoProviders();

	//qDebug() << "SlideGroupViewer::setSlideGroup: Loading slide into scene";
	if(startSlide)
	{
		//qDebug() << "SlideGroupViewer::setSlideGroup(): Setting slide group #"<<group->groupNumber()<<", starting at slide:"<<startSlide;
		setSlide(startSlide);
	}
	else
	{
// 		QList<Slide*> slist = group->slideList();
// 		if(slist.size() > 0)
// 		{
// 			//qDebug() << "SlideGroupViewer::setSlideGroup(): Setting slide group #"<<group->groupNumber()<<", defaulting to slide 0";
// 			setSlide(m_sortedSlides.at(0));
// 			//m_slideListView->setCurrentIndex(m_slideModel->indexForRow(0));
// 		}
// 		else
// 		{
// 			qDebug("SlideGroupViewer::setSlideGroup: Group[0] has 0 slides");
// 		}
	}

// 	if(m_blackEnabled && blackNative)
// 		fadeBlackFrame(false);
}

void SlideGroupViewer::setViewerState(ViewerState state)
{
	m_viewerState = state;
	switch(state)
	{
		// Black methods automatically set the native viewer state
		// and emits the viewerStateChanged() signal (even if not native)
		case Black:
			fadeBlackFrame(true);
			break;
		case Clear:
// 			fadeClearFrame(true);
// 			break;
		case Done:
		case Running:
		case Paused:
			emit viewerStateChanged(m_viewerState);
			// nothing needed to be done unless native
			if(m_nativeViewer)
				m_nativeViewer->setState((NativeViewer::NativeShowState)((int)state));
			break;
		default:
			break;
	};
}

Slide * SlideGroupViewer::setSlide(int x)
{
	m_slideNum = x;
	qDebug() << "SlideGroupViewer::setSlide(): Setting slide to idx"<<x;
	return setSlide(m_sortedSlides.at(x));
}

Slide * SlideGroupViewer::setSlide(Slide *slide, bool takeOwnership)
{
	m_slide = slide;
	
	//qDebug() << "SlideGroupViewer::setSlide(): Setting slide to ptr "<<slide;
	if(m_bgWaitingForNextSlide)
	{
		//qDebug() << "SlideGroupViewer::setSlide: Applying Background because m_bgWaitingForNextSlide is true";
		applyBackground(m_nextBg, slide);
	}
		
	if(takeOwnership)
		m_slideFilterByproduct << slide;

	int slideNum = m_sortedSlides.indexOf(slide);
	if(slideNum > -1)
		m_slideNum = slideNum;

// 	if(AppSettings::liveEditMode() == AppSettings::Smooth && m_clonedSlide)
// 		slide = m_clonedSlide;

	if(m_nativeViewer)
	{
		m_nativeViewer->setSlide(m_slideNum);
	}
	else
	{
		if(!reapplySpecialFrames())
		{
			//qDebug() << "SlideGroupViewer::setSlide(): Special frames returned false, setting slide directly";
			setSlideInternal(applySlideFilters(slide));
		}
		else
		{
			//qDebug() << "SlideGroupViewer::setSlide(): Special frames did it itself";
		}
	}

	return slide;
}

bool SlideGroupViewer::reapplySpecialFrames()
{
	// If clear or black is enabled, then we dont want to take it off clear or black just becase we
	// changed slides. Therefore, regenerate the clear frame with the new slide's background and fade
	// to the clear slide (or black slide, if that's enabled)
	if(/*m_clearEnabled || */m_blackEnabled)
	{
		// setting it to -1 forces generateClearFrame() to re-generate the frame, even if the slide
		// index stayed the same
// 		m_clearSlideNum = -1;
		//generateClearFrame();

		if(m_blackEnabled)
		{
			generateBlackFrame();
			setSlideInternal(m_blackSlide);
		}
// 		else
// 		{
// 			setSlideInternal(m_clearSlide);
// 		}

		return true;
	}

	return false;
}

void SlideGroupViewer::crossFadeFinished(Slide */*oldSlide*/,Slide*/*newSlide*/)
{
//  	qDebug() << "SlideGroupViewer::crossFadeFinished: mark"; 
// 	if(m_bgWaitingForNextSlide)
// 	{
// 		qDebug() << "SlideGroupViewer::crossFadeFinished: set m_bgWaitingForNextSlide to false";
// 		m_bgWaitingForNextSlide = false;
// 		applyBackground(m_nextBg);
// 	}
}

void SlideGroupViewer::slideDiscarded(Slide */*oldSlide*/)
{
// 	qDebug() << "SlideGroupViewer::slideDiscarded: mark";
	if(m_bgWaitingForNextSlide)
	{
 		qDebug() << "SlideGroupViewer::slideDiscarded: set m_bgWaitingForNextSlide to false";
		m_bgWaitingForNextSlide = false;
		applyBackground(m_nextBg);
	}
	
	m_fadeInProgress = false;
// // 	if(oldSlide && m_slideFilterByproduct.contains(oldSlide))
// // 	{
// // 		m_slideFilterByproduct.removeAll(oldSlide);
// // 		delete oldSlide;
// // 	}

	while(m_slideFilterByproduct.size() > MAX_BYPRODUCT_SIZE)
		delete m_slideFilterByproduct.takeFirst();
}

void SlideGroupViewer::addFilter(AbstractItemFilter * filter)
{
	if(!m_slideFilters.contains(filter))
		m_slideFilters.append(filter);
// 	qDebug() << "SlideGroupViewer::addFilter: adding filter"<<filter->filterId();
	applySlideFilters();
}

void SlideGroupViewer::removeFilter(AbstractItemFilter *filter)
{
// 	qDebug() << "SlideGroupViewer::addFilter: removing filter"<<filter->filterId();
	m_slideFilters.removeAll(filter);
	applySlideFilters();
}

void SlideGroupViewer::removeAllFilters()
{
	m_slideFilters.clear();
	applySlideFilters();
}

void SlideGroupViewer::setEndActionOverrideEnabled(bool flag) 
{
	m_overrideEndAction = flag;
}

void SlideGroupViewer::setEndGroupAction(SlideGroup::EndOfGroupAction act)
{
	m_groupEndAction = act;
}
	
Slide * SlideGroupViewer::nextSlide()
{
	m_slideNum ++;
	if(m_slideNum >= m_sortedSlides.size())
	{
		SlideGroup::EndOfGroupAction action = m_slideGroup->endOfGroupAction();
		if(isEndActionOverrideEnabled())
			action = groupEndAction();
			
		if(action == SlideGroup::GotoNextGroup)
		{
			//m_slideNum = m_sortedSlides.size() - 1;
			emit nextGroup();
			return 0;
		}
		else
		if(action == SlideGroup::GotoGroupIndex)
		{
			//m_slideNum = m_sortedSlides.size() - 1;
			emit jumpToGroup(m_slideGroup->jumpToGroupIndex());
			return 0;
		}
		else
		if(action == SlideGroup::LoopToStart)
		{
			m_slideNum = 0;
		}
		else
		{
			emit endOfGroup();
			return 0;
		}
	}
	
	return setSlide(m_slideNum);
}

Slide * SlideGroupViewer::prevSlide()
{
	m_slideNum --;
	if(m_slideNum < 0)
		m_slideNum = 0;
	return setSlide(m_slideNum);
}


void SlideGroupViewer::setSlideInternal(Slide *slide)
{
	//qDebug() << "SlideGroupViewer::setSlideInternal(): Setting slide# "<<slide->slideNumber()<<", group ptr: "<<PTRS(m_slideGroup);
	if(AppSettings::liveEditMode() == AppSettings::SmoothEdit ||
	   AppSettings::liveEditMode() == AppSettings::PublishEdit)
	{
		if(m_clonedOriginal == slide && m_fadeInProgress)
		{
			qDebug() << "SlideGroupViewer::setSlideInternal(): Smooth/Publish selected, and slide givin is same as the one thats fading in, so rejecting set request until fade completed";
			return;
		}

		// for deleting this slide after the scene is done with it (in slideDiscarded())
		if(m_clonedSlide)
			m_slideFilterByproduct << m_clonedSlide;

		m_clonedSlide = slide->clone();

		qDebug() << "SlideGroupViewer::setSlideInternal(): Smooth/Publish selected, cloning slide for internal use";

		slide = m_clonedSlide;
	}

	m_fadeInProgress = true;


	// start cross fade settings with program settings
	int speed   = AppSettings::crossFadeSpeed();
	int quality = AppSettings::crossFadeQuality();
	//qDebug() << "SlideGroupViewer::setSlideInternal(): [app] speed:"<<speed<<", quality:"<<quality;

	// allow this viewer's cross fade settings to override app settings
	if(m_fadeQuality > -1 || m_fadeSpeed > -1)
	{
		if(m_fadeQuality > -1)
			quality = m_fadeQuality;
		if(m_fadeSpeed > -1)
			speed = m_fadeSpeed;
		//qDebug() << "SlideGroupViewer::setSlideInternal(): [viewer] speed:"<<speed<<", quality:"<<quality;
	}

	// change to slide group if set
	if(m_slideGroup && !m_slideGroup->inheritFadeSettings())
	{
		speed   = (int)m_slideGroup->crossFadeSpeed();
		quality = (int)m_slideGroup->crossFadeQuality();
		//qDebug() << "SlideGroupViewer::setSlideInternal(): [group] speed:"<<speed<<", quality:"<<quality;
	}

	// and use slide settings if set instead of the other two sets of settings
	if(slide && !slide->inheritFadeSettings())
	{
		// the 0.01 test is due to the fact that I forgot to initalize cross fade speed/quality in the Slide object constructor in
		// older builds (fixed now.) So, if user loads file from older builds, these variables may have corrupted settings, so dont allow overide.
		if(slide->crossFadeSpeed() > 0.01)
			speed = (int)slide->crossFadeSpeed();
		if(slide->crossFadeQuality() > 0.01)
			quality = (int)slide->crossFadeQuality();
		//qDebug() << "SlideGroupViewer::setSlideInternal(): [slide] speed:"<<speed<<", quality:"<<quality;
	}

	//qDebug() << "SlideGroupViewer::setSlideInternal(): [final] speed:"<<speed<<", quality:"<<quality;

	if(m_slideGroup &&
		slide != m_blackSlide/* &&
		slide != m_clearSlide*/)
	{
		//qDebug() << "SlideGroupViewer::setSlideInternal(): Slide# "<<slide->slideNumber()<<": Have slide group, using master slide, group ptr: "<<PTRS(m_slideGroup)<<", master ptr:"<<PTRS(m_slideGroup->masterSlide());
		
		// false = dont auto-create master slide
		Slide * master = m_slideGroup->masterSlide(false);
		if(master)
			m_scene->setMasterSlide(applySlideFilters(master)); 
		else
			m_scene->setMasterSlide(0);
	}
	else
	{
		//qDebug() << "SlideGroupViewer::setSlideInternal(): Slide# "<<slide->slideNumber()<<": No slide group, clearing master, group ptr: "<<PTRS(m_slideGroup);
		m_scene->setMasterSlide(0);
	}

	//qDebug() << "SlideGroupViewer::setSlideInternal(): Slide# "<<slide->slideNumber()<<": Have slide group, setting slide ptr:"<<slide;
	m_scene->setSlide(slide,m_isPreviewViewer ? MyGraphicsScene::None : MyGraphicsScene::CrossFade,speed,quality);
	//qDebug() << "SlideGroupViewer::setSlideInternal(): Slide# "<<slide->slideNumber()<<": setSlide() completed";
	
	AbstractContent * absBg = m_scene->findVisualDelegate(m_scene->backgroundItem());
	if(absBg)
	{
// 		m_bgContent = dynamic_cast<BackgroundContent *>(absBg);
// 		m_bgContent->setViewerWidget(this);
	}
	
	if(m_jpegServer)
		m_jpegServer->slideChanged();

	//if(m_jpegServer)
	//	QTimer::singleShot(5000,m_jpegServer,SLOT(slideChanged()));
}

// void SlideGroupViewer::paintEvent(QPaintEvent*)
// {
// 	QPainter p(this);
// 	p.fillRect(rect(),Qt::yellow);
// }

void SlideGroupViewer::fadeBlackFrame(bool enable)
{
	m_blackEnabled = enable;
	if(enable)
	{
		m_viewerState = SlideGroupViewer::Black;
		emit viewerStateChanged(m_viewerState);
		if(m_nativeViewer)
			m_nativeViewer->setState(NativeViewer::Black);

		generateBlackFrame();
		//qDebug() << "SlideGroupViewer::fadeBlackFrame: "<<enable;

		// *dont* use our setSlide() method because we dont want to
		// change m_slideNum - we just want the "fadeToBlack" to be temporary
		setSlideInternal(m_blackSlide);
	}
	else
	{
// 		if(m_clearEnabled && m_clearSlide)
// 		{
// 			m_viewerState = SlideGroupViewer::Clear;
// 			emit viewerStateChanged(m_viewerState);
// 			if(m_nativeViewer)
// 			    m_nativeViewer->setState(NativeViewer::White);
// 
// 			setSlideInternal(m_clearSlide);
// 		}
// 		else
// 		{
			m_viewerState = SlideGroupViewer::Running;
			emit viewerStateChanged(m_viewerState);
			if(m_nativeViewer)
			    m_nativeViewer->setState(NativeViewer::Running);

			if(m_sortedSlides.size() <= 0)
				return;

			if(m_slideNum >= 0 && m_slideNum < m_sortedSlides.size())
			{
				Slide *currentSlide = m_sortedSlides.at(m_slideNum);
				setSlideInternal(applySlideFilters(currentSlide));
			}
// 		}
	}
}

// void SlideGroupViewer::fadeClearFrame(bool enable)
// {
// 	if(enable)
// 	{
// 		m_viewerState = SlideGroupViewer::Clear;
// 		emit viewerStateChanged(m_viewerState);
// 		if(m_nativeViewer)
// 			m_nativeViewer->setState(NativeViewer::White);
// 
// 		generateClearFrame();
// 		if(m_clearSlide)
// 			setSlideInternal(m_clearSlide);
// 	}
// 	else
// 	{
// 		m_viewerState = SlideGroupViewer::Running;
// 		emit viewerStateChanged(m_viewerState);
// 		if(m_nativeViewer)
// 			m_nativeViewer->setState(NativeViewer::Running);
// 		
// 		if(m_sortedSlides.isEmpty() ||
// 			m_slideNum < 0 || 
// 			m_slideNum >= m_sortedSlides.size())
// 			return;
// 
// 		Slide *currentSlide = m_sortedSlides.at(m_slideNum);
// 		if(currentSlide)
// 			setSlideInternal(applySlideFilters(currentSlide));
// 		
// 	}
// 
// 	m_clearEnabled = enable;
// }
/*
void SlideGroupViewer::generateClearFrame()
{
	if(m_clearSlide &&
	   m_clearSlideNum != m_slideNum)
	{
		// can't delete now because if m_clearSlide is live on the scene,
		// then when we call m_scene->setSlide(), the scene will try to
		// disconnect slots from m_clearSlide. Also, we cant delete it
		// using deleteLater() because the scene will want to cross fade
		// away from this slide and deleting it would delete the items
		// during the cross fade. However, when the scene emits
		// slideDiscarded(), it is safe to delete this slide. Our
		// slideDiscarded() handler uses the list m_slideFilterByproduct
		// to delete any slides needed when the scene is done with them.
		m_slideFilterByproduct << m_clearSlide;
		m_clearSlide = 0;
	}

	if(!m_clearSlide)
	{
		m_clearSlideNum = m_slideNum;
		
		if(m_sortedSlides.isEmpty() || 
			m_slideNum < 0 ||
			m_slideNum >= m_sortedSlides.size())
			{
				m_clearSlide = 0;
				return;
			}
		

		Slide *currentSlide = m_sortedSlides.at(m_slideNum);

		m_clearSlide = new Slide();

		m_clearSlide->addItem(currentSlide->background()->clone());
	}
}*/

void SlideGroupViewer::generateBlackFrame()
{

	if(!m_blackSlide)
	{
		m_blackSlide = new Slide();
		dynamic_cast<AbstractVisualItem*>(m_blackSlide->background())->setFillBrush(QBrush(Qt::black));
	}

}

Slide * SlideGroupViewer::blackSlide()
{
	generateBlackFrame();
	return m_blackSlide;
}


void SlideGroupViewer::resizeEvent(QResizeEvent *)
{
	adjustViewScaling();
}

void SlideGroupViewer::setIgnoreAspectRatio(bool flag)
{
	m_ignoreAspectRatio = flag;
	adjustViewScaling();
}

void SlideGroupViewer::adjustViewScaling()
{
	float sx = ((float)m_view->width()) / m_scene->width();
	float sy = ((float)m_view->height()) / m_scene->height();

	if(m_ignoreAspectRatio)
	{
		m_view->setTransform(QTransform().scale(sx,sy));
	}
	else
	{
		float scale = qMin(sx,sy);
		m_view->setTransform(QTransform().scale(scale,scale));
	}
	
        //qDebug("Scaling: %.02f x %.02f = %.02f",sx,sy,scale);
	m_view->update();
	//m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
	//m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

// Method: void initVideoProviders()
// This whole business of integrating with the QVideoProvider framework in the SlideGroupViewer itself
// is desiged to basically initalize the video streams before it "goes live" to create a seamless
// transition. The vote is still out on whether or not this actually helps - it seems to. We'll see.
void SlideGroupViewer::initVideoProviders()
{
	if(!m_slideGroup)
		return;

	#ifdef DVIZ_HAS_QVIDEO
	
	if(scene()->contextHint() != MyGraphicsScene::Live && 
	   scene()->contextHint() != MyGraphicsScene::Preview)
	   return;
		
	QList<Slide*> slist = m_slideGroup->slideList();
	foreach(Slide *slide, slist)
	{
		AbstractItemList items = slide->itemList();
		foreach(AbstractItem *item, items)
		{
			if (AbstractVisualItem * visualItem = dynamic_cast<AbstractVisualItem *>(item))
			{
				QString videoFile = visualItem->fillVideoFile();
				if(visualItem->fillType() == AbstractVisualItem::Video &&
				  !videoFile.isEmpty())
				{
					QVideoProvider * p = QVideoProvider::providerForFile(videoFile);
					if(p)
					{
						if(m_videoProvidersOpened[p->canonicalFilePath()] ||
						   p->isStreamStarted())
						{
							QVideoProvider::releaseProvider(p);
						}
						else
						{
							m_videoProvidersOpened[p->canonicalFilePath()]   = true;
							m_videoProvidersConsumed[p->canonicalFilePath()] = false;
	
							connect(p, SIGNAL(streamStarted()), this, SLOT(videoStreamStarted()));
							//qDebug() << "SlideGroupViewer::initVideoProviders: Playing video stream";
							p->play();
	
							m_videoProviders << p;
	
						}
					}
				}
			}
		}
	}
	#endif
}

void SlideGroupViewer::releaseVideoProvders()
{
	if(scene()->contextHint() != MyGraphicsScene::Live && 
	   scene()->contextHint() != MyGraphicsScene::Preview)
	   return;
	
	#ifdef DVIZ_HAS_QVIDEO
	foreach(QVideoProvider *p, m_videoProviders)
	{
// 		p->pause();
		p->disconnectReceiver(this);
		QVideoProvider::releaseProvider(p);
	}
	#endif

	m_videoProvidersConsumed.clear();
	m_videoProvidersOpened.clear();
	m_videoProviders.clear();

}

void SlideGroupViewer::videoStreamStarted()
{
	#ifdef DVIZ_HAS_QVIDEO
	QVideoProvider *p = dynamic_cast<QVideoProvider *>(sender());
	if(!p)
		return;

	if(!m_videoProvidersConsumed[p->canonicalFilePath()])
	{
		m_videoProvidersConsumed[p->canonicalFilePath()] = true;
// 		qDebug() << "SlideGroupViewer::videoStreamStarted: "<<this<<" Consuming video stream "<<p->canonicalFilePath()<<" and requesting pause";
		p->pause();
	}
	#endif
}

// Only relevant AFTER a slide has been set on the instance.
QList<QWidget*> SlideGroupViewer::controlWidgets()
{
	return m_scene->controlWidgets();
}

bool SlideGroupViewer::startBackgroundVideoPausedInPreview()
{
	return m_scene->startBackgroundVideoPausedInPreview();
}

void SlideGroupViewer::setStartBackgroundVideoPausedInPreview(bool flag)
{
	m_scene->setStartBackgroundVideoPausedInPreview(flag);
}

void SlideGroupViewer::setMjpegServerEnabled(bool enable, int port, int fps)
{
	if(enable)
	{
		if(!m_jpegServer ||
		    m_jpegServer->listenPort() != port)
		{
			if(m_jpegServer)
				delete m_jpegServer;
			
			m_jpegServer = new JpegServer();
			m_jpegServer->setScene(m_scene);
			m_jpegServer->setFps(fps);
			
			//if (!m_jpegServer->listen(QHostAddress::Any,port))
			//qDebug() << "SlideGroupViewer::setMjpegServerEnabled: port:"<<port<<", fps:"<<fps; 
			if(!m_jpegServer->start(port, fps < 1)) 
			{
				qDebug() << "JpegServer could not start: "<<m_jpegServer->errorString();
			}
			else
			{
				//qDebug() << "OutputInstance"<<output()->name()<<": JpegServer listening on "<<m_jpegServer->myAddress();
			}
		}
	}
	else
	{
		delete m_jpegServer;
		m_jpegServer = 0;
	}

}
