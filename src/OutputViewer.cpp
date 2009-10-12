#include "OutputViewer.h"

#include "MainWindow.h"
#include "AppSettings.h"

#include <QVBoxLayout>
#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif


OutputViewer::OutputViewer(SlideGroupViewer *output, QWidget *parent)
	    : QWidget(parent), m_output(0), m_scene(0), m_view(0), m_usingGL(false)
{
	m_view = new QGraphicsView(this);
	m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
	
	// check to see if we need to set the GL viewport
	appSettingsChanged();

	if(output)
		setViewer(output);
	
	m_view->setBackgroundBrush(Qt::gray);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(m_view);
	setLayout(layout);
	
	if(MainWindow::mw())
	{
		connect(MainWindow::mw(), SIGNAL(appSettingsChanged()), this, SLOT(appSettingsChagned()));
// 		connect(mw, SIGNAL(aspectRatioChanged(Document*)), this, SLOT(docSettingsChagned(Document*)));
	}
}

void OutputViewer::appSettingsChanged()
{
	if(AppSettings::useOpenGL() && !m_usingGL)
	{
		m_usingGL = true;
		m_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	}
	else
	if(!AppSettings::useOpenGL() && m_usingGL)
	{
		m_usingGL = false;
		m_view->setViewport(new QWidget());
	}
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

	float scale = qMax(sx,sy);
	m_view->setTransform(QTransform().scale(scale,scale));
	//qDebug("Scaling: %.02f x %.02f = %.02f",sx,sy,scale);
	m_view->update();
}
