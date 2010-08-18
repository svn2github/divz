#include <QtGui>
#include <QtOpenGL>
#include "GLWidget.h"
#include "GLDrawable.h"

GLWidget::GLWidget(QWidget *parent, QGLWidget *shareWidget)
	: QGLWidget(parent, shareWidget)
	, m_glInited(false)
{
	setViewport(QRectF(0,0,1000.,750.));
}

GLWidget::~GLWidget()
{


}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
	return QSize(400,300);
}


void GLWidget::initializeGL()
{
	//makeCurrent();
	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
// 	glClearDepth(1.0f);						// Depth Buffer Setup
// 	glEnable(GL_DEPTH_TEST);					// Enables Depth Testing
// 	glDepthFunc(GL_LEQUAL);						// The Type Of Depth Testing To Do
// 	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		// Really Nice Perspective Calculations
	
	m_glInited = true;
	//qDebug() << "GLWidget::initializeGL()";
	foreach(GLDrawable *drawable, m_drawables)
		drawable->initGL();
	
	
}

void GLWidget::paintGL()
{
	//makeCurrent();
	
	qglClearColor(Qt::black);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
// 	int counter = 0;
	foreach(GLDrawable *drawable, m_drawables)
	{
		//qDebug() << "GLWidget::paintGL(): ["<<counter++<<"] drawable->rect: "<<drawable->rect();
		
		// Don't draw if not visible or if opacity == 0
		if(drawable->isVisible() && drawable->opacity() > 0)
			drawable->paintGL();
	}
}
	
void GLWidget::addDrawable(GLDrawable *item)
{
	//makeCurrent();
	
	item->setGLWidget(this);
	m_drawables << item;
	connect(item, SIGNAL(zIndexChanged(double)), this, SLOT(zIndexChanged()));
	if(m_glInited)
	{
		//qDebug() << "GLWidget::addDrawable()";
		item->initGL();
	}
	sortDrawables();
}

void GLWidget::removeDrawable(GLDrawable *item)
{
	m_drawables.removeAll(item);
	item->setGLWidget(0);
	disconnect(item, 0, this, 0);
	// sort not needed since order implicitly stays the same
}

void GLWidget::zIndexChanged()
{
	sortDrawables();
}

bool GLWidget_drawable_zIndex_compare(GLDrawable *a, GLDrawable *b)
{
	return (a && b) ? a->zIndex() < b->zIndex() : true;
}


void GLWidget::sortDrawables()
{
	qSort(m_drawables.begin(), m_drawables.end(), GLWidget_drawable_zIndex_compare);
}

void GLWidget::resizeGL(int width, int height)
{
	//makeCurrent();
	
// 	int side = qMin(width, height);
	//glViewport(0,0,width,height); //(width - side) / 2, (height - side) / 2, side, side);
	glViewport(0,0,width,height); //(width - side) / 2, (height - side) / 2, side, side);
	
	setViewport(viewport());
}
	
void GLWidget::setViewport(const QRectF& viewport)
{
	m_viewport = viewport;
	float sw = viewport.width();
	float sh = viewport.height();
	
	float sx = ((float)width()) / sw;
	float sy = ((float)height()) / sh;

	float scale = qMin(sx,sy);
	float scaledWidth  = sw * scale;
	float scaledHeight = sh * scale;
	int diffWidth  = width()  - (int)scaledWidth;
	int diffHeight = height() - (int)scaledHeight;
/*	qDebug() << "GLWidget::setViewport: viewport:"<<viewport;
	qDebug() << "GLWidget::setViewport: widget size:"<<width()<<"x"<<height();
	qDebug() << "GLWidget::setViewport: sx:"<<sx<<",sy:"<<sy<<", final scale:"<<scale;
	qDebug() << "GLWidget::setViewport: scaled viewport size:"<<scaledWidth<<"x"<<scaledHeight;
	qDebug() << "GLWidget::setViewport: widget size - viewport size:"<<diffWidth<<"x"<<diffHeight;*/
	
	/// WHY?? The usual centering algorithm of 'divide by 2' doesn't seem to work - at least
	/// on my work machine. The math is correct, but the output renders too far to the right and top. 
	/// The 2.75 and 1.5 were found simply thru trial and error.
	float xt = diffWidth/2  + viewport.left();
	float yt = diffHeight/2  + viewport.top();
// 	int xt = diffWidth/2.0  + (int)viewport.left();
// 	int yt = diffHeight/2.0 + (int)viewport.top();
	
	//qDebug() << "GLWidget::setViewport: xt:"<<xt<<", yt:"<<yt;
	
	setTransform(QTransform().scale(scale,scale).translate(xt,yt));
	
	//QSize size(width,height);
	//foreach(GLDrawable *drawable, m_drawables)
		//drawable->viewportResized(size);

/*#if !defined(QT_OPENGL_ES_2)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
		#ifndef QT_OPENGL_ES
			glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
		#else
			glOrthof(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
		#endif
	glMatrixMode(GL_MODELVIEW);
#endif*/
}

void GLWidget::setTransform(const QTransform& tx)
{
	m_transform = tx;
}

void GLWidget::mousePressEvent(QMouseEvent */*event*/)
{
// 	lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent */*event*/)
{
// 	int dx = event->x() - lastPos.x();
// 	int dy = event->y() - lastPos.y();
// 	
// 	if (event->buttons() & Qt::LeftButton) {
// 		rotateBy(8 * dy, 8 * dx, 0);
// 	} else if (event->buttons() & Qt::RightButton) {
// 		rotateBy(8 * dy, 0, 8 * dx);
// 	}
// 	lastPos = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */)
{
	emit clicked();
}
