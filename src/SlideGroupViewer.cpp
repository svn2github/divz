#include "SlideGroupViewer.h"

#include "RenderOpts.h"

#include <QVBoxLayout>
#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif

#include "qvideo/QVideoProvider.h"

#include "MainWindow.h"
#include "AppSettings.h"

SlideGroupViewer::SlideGroupViewer(QWidget *parent)
	    : QWidget(parent), m_slideGroup(0), m_scene(0), m_view(0), m_usingGL(false), m_slideNum(0)
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
	
	m_view->setBackgroundBrush(Qt::gray);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(m_view);
	setLayout(layout);


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

void SlideGroupViewer::setSlideGroup(SlideGroup *g, int startSlide)
{
	//qDebug() << "SlideGroupViewer::setSlideGroup: (SceneContextHint:"<<m_scene->contextHint()<<"), setting slide group:"<<g->groupNumber();
	if(m_slideGroup == g)
		return;

	if(m_slideGroup && m_slideGroup != g)
	{
		//disconnect(m_slideGroup,0,this,0);
		releaseVideoProvders();
	}

	//if(m_slideGroup != g)
	//	connect(g,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));

	m_slideGroup = g;

	QList<Slide*> slist = g->slideList();
	qSort(slist.begin(), slist.end(), slide_group_viewer_slide_num_compare);
	m_sortedSlides = slist;
	
	// See comments on the function itself for what this is for
	initVideoProviders();

	if(startSlide)
	{
		Slide *s = m_sortedSlides.at(startSlide);
		//qDebug() << "SlideGroupViewer::setSlideGroup(): Setting slide group #"<<g->groupNumber()<<", starting at slide:"<<startSlide;
		m_scene->setSlide(s,MyGraphicsScene::CrossFade);
		//m_slideListView->setCurrentIndex(m_slideModel->indexForSlide(curSlide));
	}
	else
	{
		QList<Slide*> slist = g->slideList();
		if(slist.size() > 0)
		{
			//qDebug() << "SlideGroupViewer::setSlideGroup(): Setting slide group #"<<g->groupNumber()<<", defaulting to slide 0";
			Slide *s = m_sortedSlides.at(0);
			m_scene->setSlide(s,MyGraphicsScene::CrossFade);
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

Slide * SlideGroupViewer::setSlide(Slide *s)
{
	m_scene->setSlide(s,MyGraphicsScene::CrossFade);
	return s;
}

Slide * SlideGroupViewer::nextSlide()
{
	m_slideNum ++;
	if(m_slideNum >= m_sortedSlides.size())
		m_slideNum = m_sortedSlides.size() - 1;
	return setSlide(m_slideNum);
}

Slide * SlideGroupViewer::prevSlide()
{
	m_slideNum --;
	if(m_slideNum < 0)
		m_slideNum = 0;
	return setSlide(m_slideNum);
}

void SlideGroupViewer::resizeEvent(QResizeEvent *)
{
	adjustViewScaling();
}

void SlideGroupViewer::adjustViewScaling()
{
	float sx = ((float)m_view->width()) / m_scene->width();
	float sy = ((float)m_view->height()) / m_scene->height();

	float scale = qMax(sx,sy);
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
					connect(p, SIGNAL(streamStarted()), this, SLOT(videoStreamStarted()));
					
					m_videoProvidersConsumed[p->canonicalFilePath()] = false;
					
					m_videoProviders << p;
					
					if(!p->isPlaying())
						p->play();
					
					
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
		//p->pause();
	}
}
