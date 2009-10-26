#include "SlideGroupViewer.h"

#include "RenderOpts.h"

#include <QVBoxLayout>
#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif

#include <QTextDocument>
#include <QTextBlock>
#include <QTextOption>

#include "qvideo/QVideoProvider.h"

#include "MainWindow.h"
#include "AppSettings.h"
#include "MediaBrowser.h"

#include "model/TextBoxItem.h"
#include "model/BackgroundItem.h"

Slide * SlideGroupViewer::m_blackSlide = 0;

SlideGroupViewer::SlideGroupViewer(QWidget *parent)
	    : QWidget(parent)
	    , m_slideGroup(0)
	    , m_scene(0)
	    , m_view(0) 
	    , m_slideNum(0)
	    , m_usingGL(false)
	    , m_clearSlide(0)
	    , m_clearSlideNum(-1)
	    , m_clearEnabled(false)
	    , m_blackEnabled(false)
	    , m_bgWaitingForNextSlide(false)
	    , m_overlaySlide(0)
	    , m_overlayEnabled(false)
	    , m_textOnlyFilter(false)
	    , m_autoResizeText(false)
	    , m_fadeSpeed(-1)
	    , m_fadeQuality(-1)
{
	QRect sceneRect(0,0,1024,768);
	
	if(MainWindow::mw())
	{
		MainWindow *mw = MainWindow::mw();
		sceneRect = mw->standardSceneRect();
		
		connect(mw, SIGNAL(appSettingsChanged()), this, SLOT(appSettingsChanged()));
 		connect(mw, SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
	}
	
	m_view = new QGraphicsView(this);
	m_view->setFrameStyle(QFrame::NoFrame);
	m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	m_view->setOptimizationFlags(QGraphicsView::DontSavePainterState);
	
	appSettingsChanged();
	//qDebug("SlideGroupViewer: Loaded OpenGL viewport.");
	
	m_scene = new MyGraphicsScene(MyGraphicsScene::Live);
	m_scene->setSceneRect(sceneRect);
	m_view->setScene(m_scene);
	
	// inorder to set the background on the rest of the slides if asked to set background on next live slide
	connect(m_scene, SIGNAL(crossFadeFinished()), this, SLOT(crossFadeFinished()));
	
	m_view->setBackgroundBrush(Qt::gray);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(m_view);
	setLayout(layout);
}


void SlideGroupViewer::setOverlaySlide(Slide * newSlide)
{
	m_overlaySlide = newSlide;
	m_overlayEnabled = true;  // TODO should we default to this?
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
	applySlideFilters();
}

void SlideGroupViewer::setAutoResizeTextEnabled(bool enable)
{
	m_autoResizeText = enable;
	applySlideFilters();	
}

void SlideGroupViewer::setFadeSpeed(int value)
{
	m_fadeSpeed = value;
}

void SlideGroupViewer::setFadeQuality(int value)
{
	m_fadeQuality = value;
}

void SlideGroupViewer::applySlideFilters()
{
	if(!m_slideGroup)
		return;
		
	if(!m_blackEnabled && !m_clearEnabled)
	{
		Slide *currentSlide = m_sortedSlides.at(m_slideNum);
		m_scene->setSlide(applySlideFilters(currentSlide),MyGraphicsScene::CrossFade);
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
		&& !m_textOnlyFilter && !m_autoResizeText)
		return sourceSlide;
	
	Slide * slide = new Slide();
	
	double baseZValue = 0;
	BackgroundItem * originalBg = 0;
	
	QList<AbstractItem *> origList = sourceSlide->itemList();
	foreach(AbstractItem * sourceItem, origList)
	{
		if(m_textOnlyFilter && !sourceItem->inherits("TextBoxItem"))
			continue;
			
		// TODO see if we can do this - adding an item to another slide while its still on the original slide
		slide->addItem(sourceItem);
		
		AbstractVisualItem * visual = dynamic_cast<AbstractVisualItem*>(sourceItem);
		if(visual && visual->zValue() > baseZValue)
			baseZValue = visual->zValue();
			
		BackgroundItem * bgTmp = dynamic_cast<BackgroundItem*>(sourceItem);
		if(bgTmp && bgTmp->fillType() != AbstractVisualItem::None)
			originalBg = bgTmp;
	}
	
	// Apply auto size text filter BEFORE the overlay so we dont resize any overlay text boxes
	if(m_autoResizeText)
	{
		// Use the first textbox in the slide as the slide to resize
		// "first" as defined by ZValue
		QList<AbstractItem *> items = slide->itemList();
		qSort(items.begin(), items.end(), SlideGroupViewer_itemZCompare);


		TextBoxItem *text = 0;
		
		foreach(AbstractItem * item, items)
		{
			AbstractVisualItem * newVisual = dynamic_cast<AbstractVisualItem*>(item);
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
			
			QRectF scene = m_scene->sceneRect();
			int width = scene.width();
			int height = scene.height();
			
			double ptSize = autoText->findFontSize();
			int sizeInc = 4;	// how big of a jump to add to the ptSize each iteration
			int count = 0;		// current loop iteration
			int maxCount = 50; 	// max iterations of the search loop (allows font to get up to 232pt)
			bool done = false;
			
			int lastGoodSize = ptSize;
			QString lastGoodHtml = autoText->text();;
			
			QTextDocument doc;
			
			// for centering
			qreal boxHeight = -1;
			
			int heightTmp;
			
			while(!done && count++ < maxCount)
			{
				
				autoText->setFontSize(ptSize);
				doc.setHtml(autoText->text());
				doc.setTextWidth(width);
				heightTmp = doc.documentLayout()->documentSize().height();
				
				if(heightTmp < height)
				{
					lastGoodSize = ptSize;
					lastGoodHtml = autoText->text();
					boxHeight = heightTmp;
	
					//qDebug()<<"size search: "<<ptSize<<"pt was good, trying higher";
					ptSize += sizeInc;
	
				}
				else
				{
					//qDebug()<<"SongSlideGroup::textToSlides(): size search: last good ptsize:"<<lastGoodSize<<", stopping search";
					done = true;
				}
			}
	
			autoText->setText(lastGoodHtml);
			
			// Center text on screen
			if(boxHeight > -1)
			{
				qreal y = scene.height()/2 - boxHeight/2;
				//qDebug() << "SongSlideGroup::textToSlides(): centering: boxHeight:"<<boxHeight<<", textRect height:"<<textRect.height()<<", centered Y:"<<y;
				autoText->setContentsRect(QRectF(0,y,width,boxHeight));
			}

			// Outline pen for the text
			QPen pen = QPen(Qt::black,1.5);
			pen.setJoinStyle(Qt::MiterJoin);
			
			autoText->setPos(QPointF(0,0));
			autoText->setOutlinePen(pen);
			autoText->setOutlineEnabled(true);
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
		QList<AbstractItem *> items = m_overlaySlide->itemList();
		
		foreach(AbstractItem * overlayItem, items)
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
			
	
			// TODO see if we can do this - adding an item to another slide while its still on the original slide
			AbstractItem * newItem = overlayItem; //->clone();
			AbstractVisualItem * newVisual = dynamic_cast<AbstractVisualItem*>(overlayItem);
	
			// rebase zvalue so everything in this slide is on top of the original slide
			if(newVisual && baseZValue!=0)
			{
				newVisual->setZValue(newVisual->zValue() + baseZValue);
				qDebug()<<"SlideGroupViewer::applySlideFilters: rebased" << newVisual->itemName() << "to new Z" << newVisual->zValue();
			}
			
			if(m_textOnlyFilter ? newItem->inherits("TextBoxItem") : true)
				slide->addItem(newItem);
		}
		
		// If we have both a overlay and a original bg, remove the original bg
		if(secondaryBg && originalBg)
			slide->removeItem(originalBg);
	}
	
	if(m_textOnlyFilter)
	{
		dynamic_cast<AbstractVisualItem*>(slide->background())->setFillBrush(QBrush(Qt::black));
	}
	
	return slide;
}


void SlideGroupViewer::applyBackground(const QFileInfo &info, Slide * onlyThisSlide)
{
	if(!m_slideGroup)
		return;
		
	QString ext = info.suffix();
	
	QString abs = info.absoluteFilePath();
	
	QList<Slide *> slides;
	if(onlyThisSlide)
		slides.append(onlyThisSlide);
	else
		slides = m_slideGroup->slideList();
		
	foreach(Slide * slide, slides)
	{
		AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());
		
		if(MediaBrowser::isVideo(ext))
		{
			bg->setFillType(AbstractVisualItem::Video);
			bg->setFillVideoFile(abs);
			
		}
		else
		if(MediaBrowser::isImage(ext))
		{
			bg->setFillType(AbstractVisualItem::Image);
			bg->setFillImageFile(abs);
		}
	}
}

void SlideGroupViewer::setLiveBackground(const QFileInfo &info, bool waitForNextSlide)
{
	QString ext = info.suffix();
	if(!MediaBrowser::isVideo(ext) &&
	   !MediaBrowser::isImage(ext))
	{
		QMessageBox::warning(this,"Unknown File Type","I'm not sure how to handle that file. Sorry!");
		return;
	}
	
	if(!waitForNextSlide)
		applyBackground(info);
	else
	{
		m_nextBg = info;
		m_bgWaitingForNextSlide = true;
	}
	
}

void SlideGroupViewer::setBackground(QColor c)
{
	view()->setBackgroundBrush(c);
}

void SlideGroupViewer::setSceneContextHint(MyGraphicsScene::ContextHint hint)
{
	scene()->setContextHint(hint);
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
	
	if(m_clearSlide)
	{
		delete m_clearSlide;
		m_clearSlide = 0;
	}
	
}

void SlideGroupViewer::appSettingsChanged()
{
	if(AppSettings::useOpenGL() && !m_usingGL)
	{
		m_usingGL = true;
		m_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
		qDebug("SlideGroupViewer::appSettingsChanged(): Loaded OpenGL Viewport");
	}
	else
	if(!AppSettings::useOpenGL() && m_usingGL)
	{
		m_usingGL = false;
		m_view->setViewport(new QWidget());
		qDebug("SlideGroupViewer::appSettingsChanged(): Loaded Non-GL Viewport");
	}
}

void SlideGroupViewer::aspectRatioChanged(double x)
{
	qDebug("SlideGroupViewer::aspectRatioChanged(): New aspect ratio: %.02f",x);
	m_scene->setSceneRect(MainWindow::mw()->standardSceneRect());
	adjustViewScaling();
}


void SlideGroupViewer::closeEvent(QCloseEvent *event)
{
	event->accept();
	m_scene->clear();
	close();
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
}

void SlideGroupViewer::setSlideGroup(SlideGroup *g, Slide *startSlide)
{
	//qDebug() << "SlideGroupViewer::setSlideGroup: (SceneContextHint:"<<m_scene->contextHint()<<"), setting slide group:"<<g->groupNumber();
	if(m_slideGroup == g)
		return;
		
	m_slideNum = 0;
	
	m_clearSlideNum = -1;


	if(m_slideGroup && m_slideGroup != g)
	{
		//disconnect(m_slideGroup,0,this,0);
		//qDebug() << "SlideGroupViewer::setSlideGroup: Releasing video providers due to slide change";
		releaseVideoProvders();
	}

	//if(m_slideGroup != g)
	//	connect(g,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));

	m_slideGroup = g;

	QList<Slide*> slist = g->slideList();
	qSort(slist.begin(), slist.end(), slide_group_viewer_slide_num_compare);
	m_sortedSlides = slist;
	
	// See comments on the function itself for what this is for
	//qDebug() << "SlideGroupViewer::setSlideGroup: Initalizing video providers...";
	initVideoProviders();

	//qDebug() << "SlideGroupViewer::setSlideGroup: Loading slide into scene";
	if(startSlide)
	{
		//qDebug() << "SlideGroupViewer::setSlideGroup(): Setting slide group #"<<g->groupNumber()<<", starting at slide:"<<startSlide;
		setSlide(startSlide);
	}
	else
	{
		QList<Slide*> slist = g->slideList();
		if(slist.size() > 0)
		{
			//qDebug() << "SlideGroupViewer::setSlideGroup(): Setting slide group #"<<g->groupNumber()<<", defaulting to slide 0";
			setSlide(m_sortedSlides.at(0));
			//m_slideListView->setCurrentIndex(m_slideModel->indexForRow(0));
		}
		else
		{
			qDebug("SlideGroupViewer::setSlideGroup: Group[0] has 0 slides");
		}
	}
}

Slide * SlideGroupViewer::setSlide(int x)
{
	m_slideNum = x;
	//qDebug() << "SlideGroupViewer::setSlide(): Setting slide to idx"<<x;
	return setSlide(m_sortedSlides.at(x));	
}

Slide * SlideGroupViewer::setSlide(Slide *slide)
{
	// start cross fade settings with program settings
	int speed = AppSettings::crossFadeSpeed();
	int quality = AppSettings::crossFadeQuality();
	//qDebug() << "SlideGroupViewer::setSlide(): [app] speed:"<<speed<<", quality:"<<quality;
	
	// change to slide group if sett
	if(m_slideGroup && !m_slideGroup->inheritFadeSettings())
	{
		speed = m_slideGroup->crossFadeSpeed();
		quality = m_slideGroup->crossFadeQuality();
		//qDebug() << "SlideGroupViewer::setSlide():     [group] speed:"<<speed<<", quality:"<<quality;
	}
	
	// and use slide settings if set instead of the other two sets of settings
	if(slide && !slide->inheritFadeSettings())
	{
		if(slide->crossFadeSpeed() > 0)
			speed = slide->crossFadeSpeed();
		if(slide->crossFadeQuality() > 0)
			quality = slide->crossFadeQuality();
		//qDebug() << "SlideGroupViewer::setSlide():         [slide] speed:"<<speed<<", quality:"<<quality;
	}
	
	// allow this viewer's cross fade settings to override everything else
	if(m_fadeQuality > 0)
		quality = m_fadeQuality;
	if(m_fadeSpeed > 0)
		speed = m_fadeSpeed;
	
	if(m_bgWaitingForNextSlide)
		applyBackground(m_nextBg, slide);
	
	m_scene->setSlide(applySlideFilters(slide),MyGraphicsScene::CrossFade,speed,quality);
	m_slideNum = m_sortedSlides.indexOf(slide);
	return slide;
}

void SlideGroupViewer::crossFadeFinished()
{
	if(m_bgWaitingForNextSlide)
	{
		m_bgWaitingForNextSlide = false;
		applyBackground(m_nextBg);
	}
}

Slide * SlideGroupViewer::nextSlide()
{
	m_slideNum ++;
	if(m_slideNum >= m_sortedSlides.size())
	{
		if(m_slideGroup->autoChangeGroup())
		{
			//m_slideNum = m_sortedSlides.size() - 1;
			emit nextGroup();
			return 0;
		}
		else
		{
			m_slideNum = 0;
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

void SlideGroupViewer::fadeBlackFrame(bool enable)
{
	if(m_sortedSlides.size() <= 0)
		return;
		
	m_blackEnabled = enable;
	
	if(!m_blackSlide)
	{
		m_blackSlide = new Slide();
		dynamic_cast<AbstractVisualItem*>(m_blackSlide->background())->setFillBrush(QBrush(Qt::black));
	}
	
	if(enable)
	{
		// *dont* use our setSlide() method because we dont want to 
		// change m_slideNum - we just want the "fadeToBlack" to be temporary
		m_scene->setSlide(m_blackSlide,MyGraphicsScene::CrossFade);
	}
	else
	{
		if(m_clearEnabled && m_clearSlide)
		{
			m_scene->setSlide(m_clearSlide,MyGraphicsScene::CrossFade);
		}
		else
		{
			if(m_slideNum < m_sortedSlides.size())
			{
				Slide *currentSlide = m_sortedSlides.at(m_slideNum);
				m_scene->setSlide(applySlideFilters(currentSlide),MyGraphicsScene::CrossFade);
			}
		}
	}
}

void SlideGroupViewer::fadeClearFrame(bool enable)
{
	if(enable)
	{
		if(m_clearSlide && m_clearSlideNum != m_slideNum)
		{
			delete m_clearSlide;
			m_clearSlide = 0;
		}
		
		if(!m_clearSlide)
		{
			m_clearSlideNum = m_slideNum;
			
			Slide *currentSlide = m_sortedSlides.at(m_slideNum);
			
			m_clearSlide = new Slide();
			m_clearSlide->addItem(currentSlide->background()->clone());
		}
		
		m_scene->setSlide(m_clearSlide,MyGraphicsScene::CrossFade);
	}
	else
	{
		if(m_slideNum < m_sortedSlides.size())
		{
			Slide *currentSlide = m_sortedSlides.at(m_slideNum);
			m_scene->setSlide(applySlideFilters(currentSlide),MyGraphicsScene::CrossFade);
		}
	}
	
	m_clearEnabled = enable;
}

void SlideGroupViewer::resizeEvent(QResizeEvent *)
{
	adjustViewScaling();
}

void SlideGroupViewer::adjustViewScaling()
{
	float sx = ((float)m_view->width()) / m_scene->width();
	float sy = ((float)m_view->height()) / m_scene->height();

	float scale = qMin(sx,sy);
	m_view->setTransform(QTransform().scale(scale,scale));
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
		
	QList<Slide*> slist = m_slideGroup->slideList();	
	foreach(Slide *slide, slist)
	{
		QList<AbstractItem *> items = slide->itemList();
		foreach(AbstractItem *item, items)
		{
			if (AbstractVisualItem * visualItem = dynamic_cast<AbstractVisualItem *>(item))
			{
				QString videoFile = visualItem->fillVideoFile();
				if(!videoFile.isEmpty())
				{
					QVideoProvider * p = QVideoProvider::providerForFile(videoFile);
					if(m_videoProvidersOpened[p->canonicalFilePath()])
					{
						QVideoProvider::releaseProvider(p);
					}
					else
					{
						m_videoProvidersOpened[p->canonicalFilePath()]   = true;
						m_videoProvidersConsumed[p->canonicalFilePath()] = false;
						
						connect(p, SIGNAL(streamStarted()), this, SLOT(videoStreamStarted()));
						
						m_videoProviders << p;
						
						p->play();
					}
				}
			}
		}
	}
}

void SlideGroupViewer::releaseVideoProvders()
{
	foreach(QVideoProvider *p, m_videoProviders)
	{
		p->disconnectReceiver(this);
		QVideoProvider::releaseProvider(p);
	}
	
	m_videoProvidersConsumed.clear();
	m_videoProvidersOpened.clear();
	m_videoProviders.clear();
	
}

void SlideGroupViewer::videoStreamStarted()
{
	QVideoProvider *p = dynamic_cast<QVideoProvider *>(sender());
	if(!p)
		return;
		
	if(!m_videoProvidersConsumed[p->canonicalFilePath()])
	{
		m_videoProvidersConsumed[p->canonicalFilePath()] = true;
		//qDebug() << "SlideGroupViewer::videoStreamStarted: Consuming video stream and requesting pause";
		p->pause();
	}
}
