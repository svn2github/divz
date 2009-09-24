#include "SlideGroupViewer.h"

#include "RenderOpts.h"

#include <QVBoxLayout>
#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif


SlideGroupViewer::SlideGroupViewer(QWidget *parent)
	    : QWidget(parent), m_scene(0), m_slideGroup(0), m_view(0)
{
	QRect sceneRect(0,0,1024,768);
	m_view = new QGraphicsView(this);
	m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	if(!RenderOpts::DisableOpenGL)
	{
		m_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	}

	m_scene = new MyGraphicsScene();
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

bool slide_group_viewer_slide_num_compare(Slide *a, Slide *b)
{
	return (a && b) ? a->slideNumber() < b->slideNumber() : true;
}

void SlideGroupViewer::setSlideGroup(SlideGroup *g, int startSlide)
{
	//qDebug() << "SlideGroupListModel::setSlideGroup: setting slide group:"<<g->groupNumber();
	if(m_slideGroup && m_slideGroup != g)
	{
		//disconnect(m_slideGroup,0,this,0);
	}

	//if(m_slideGroup != g)
	//	connect(g,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));

	m_slideGroup = g;

	QList<Slide*> slist = g->slideList();
	qSort(slist.begin(), slist.end(), slide_group_viewer_slide_num_compare);
	m_sortedSlides = slist;

	if(startSlide)
	{
		Slide *s = m_sortedSlides.at(startSlide);
		m_scene->setSlide(s);
		//m_slideListView->setCurrentIndex(m_slideModel->indexForSlide(curSlide));
	}
	else
	{
		QList<Slide*> slist = g->slideList();
		if(slist.size() > 0)
		{
			Slide *s = m_sortedSlides.at(0);
			m_scene->setSlide(s);
			//m_slideListView->setCurrentIndex(m_slideModel->indexForRow(0));
		}
		else
		{
			qDebug("SlideGroupViewer::setSlideGroup: Group[0] has 0 slides");
		}
	}
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
	qDebug("Scaling: %.02f x %.02f = %.02f",sx,sy,scale);
	m_view->update();



}
