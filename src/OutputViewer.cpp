#include "OutputViewer.h"

#include "RenderOpts.h"

#include <QVBoxLayout>
#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif


OutputViewer::OutputViewer(SlideGroupViewer *output, QWidget *parent)
	    : QWidget(parent), m_scene(0), m_output(0), m_view(0)
{
	QRect sceneRect(0,0,1024,768);
	m_view = new QGraphicsView(this);
	m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	if(!RenderOpts::DisableOpenGL)
	{
		m_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	}

	if(output)
		setViewer(output);
	
	m_view->setBackgroundBrush(Qt::gray);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(m_view);
	setLayout(layout);
}

OutputViewer::~OutputViewer()
{
	if(m_view)
	{
		delete m_view;
		m_view = 0;
	}
}

void OutputViewer::setViewer(SlideGroupViewer *output)
{
	m_output = output;
	m_scene = output->scene();
	m_view->setScene(m_scene);
}

void OutputViewer::resizeEvent(QResizeEvent *)
{
	adjustViewScaling();
}

void OutputViewer::adjustViewScaling()
{
	float sx = ((float)m_view->width()) / m_scene->width();
	float sy = ((float)m_view->height()) / m_scene->height();

	float scale = qMin(sx,sy);
	m_view->setTransform(QTransform().scale(scale,scale));
	qDebug("Scaling: %.02f x %.02f = %.02f",sx,sy,scale);
	m_view->update();
}
