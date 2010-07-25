#include "GLWidget.h"
#include <QResizeEvent>
GLWidget::GLWidget(QWidget *parent) 
	: QGLWidget(parent),
	glt(this)
{ 
	setAutoBufferSwap(false);
	resize(320, 240);
}

void GLWidget::startRendering()
{
	glt.start();
}
	
void GLWidget::stopRendering()
{
	glt.stop();
	glt.wait();
}

void GLWidget::resizeEvent(QResizeEvent *evt)
{
	glt.resizeViewport(evt->size());
}

void GLWidget::paintEvent(QPaintEvent *)
{
	// Handled by the GLThread.
}

void GLWidget::closeEvent(QCloseEvent *evt)
{
	stopRendering();
	QGLWidget::closeEvent(evt);
	deleteLater();
}
