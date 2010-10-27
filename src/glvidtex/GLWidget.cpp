#include <QtGui>
#include <QtOpenGL>
#include "GLWidget.h"
#include "GLDrawable.h"

GLWidget::GLWidget(QWidget *parent, QGLWidget *shareWidget)
	: QGLWidget(QGLFormat(QGL::SampleBuffers),parent, shareWidget)
	, m_glInited(false)
{
	setCanvasSize(QSizeF(1000.,750.));
	// setViewport() will use canvas size by default to construct a rect
	setViewport(QRectF());
	//qDebug() << "GLWidget::doubleBuffered: "<<doubleBuffer();
}

GLWidget::~GLWidget()
{

}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(60, 45);
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
	//glEnable(GL_MULTISAMPLE) 
	
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_LINE_SMOOTH);
	
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
	
	int counter = 0;
	foreach(GLDrawable *drawable, m_drawables)
	{
		//qDebug() << "GLWidget::paintGL(): ["<<counter++<<"] drawable->rect: "<<drawable->rect();
		
// 		qDebug() << "GLWidget::paintGL(): drawable:"<<((void*)drawable)<<", isvis:"<<drawable->isVisible();
		// Don't draw if not visible or if opacity == 0
		if(drawable->isVisible() && drawable->opacity() > 0)
			drawable->paintGL();
// 		qDebug() << "GLWidget::paintGL(): drawable:"<<((void*)drawable)<<", draw done";
	}


// 	GLuint	texture[1]; // Storage For One Texture
// 	QImage texOrig, texGL;
// 	if ( !texOrig.load( "me2.jpg" ) )
// 	//if ( !texOrig.load( "Pm5544.jpg" ) )
// 	{
// 		texOrig = QImage( 16, 16, QImage::Format_RGB32 );
// 		texOrig.fill( Qt::green );
// 	}
// 	
// 	// Setup inital texture
// 	texGL = QGLWidget::convertToGLFormat( texOrig );
// 	glGenTextures( 1, &texture[0] );
// 	glBindTexture( GL_TEXTURE_2D, texture[0] );
// 	glTexImage2D( GL_TEXTURE_2D, 0, 3, texGL.width(), texGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texGL.bits() );
// 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
// 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
// 	
// 	glEnable(GL_TEXTURE_2D);					// Enable Texture Mapping ( NEW )
// 	
// 	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// 	
// 	glLoadIdentity(); // Reset The View
// 	//glTranslatef(0.0f,0.0f,-3.42f);
// 	
// 	glBindTexture(GL_TEXTURE_2D, texture[0]);
// 	
// 	
// 	glBegin(GL_QUADS);
// 		// Front Face
// // 		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
// // 		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
// // 		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
// // 		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
// 	
// 		QRectF rect(0,0,764,572);
// 		
// 		qreal 
// 			vx1 = rect.left(),
// 			vx2 = rect.right(),
// 			vy1 = rect.bottom(),
// 			vy2 = rect.top();
// 		
// 		glTexCoord2f(0.0f, 0.0f); glVertex3f(vx1,vy1,  0.0f); // top left
// 		glTexCoord2f(1.0f, 0.0f); glVertex3f(vx2,vy1,  0.0f); // top right
// 		glTexCoord2f(1.0f, 1.0f); glVertex3f(vx2,vy2,  0.0f); // bottom right
// 		glTexCoord2f(0.0f, 1.0f); glVertex3f(vx1,vy2,  0.0f); // bottom left
// 
// // 		glTexCoord2f(0,0); glVertex3f( 0, 0,0); //lo
// // 		glTexCoord2f(0,1); glVertex3f(256, 0,0); //lu
// // 		glTexCoord2f(1,1); glVertex3f(256, 256,0); //ru
// // 		glTexCoord2f(1,0); glVertex3f( 0, 256,0); //ro
// 	glEnd();
// 	
	
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
// 	qDebug() << "GLWidget::removeDrawable(): drawable:"<<((void*)item);
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
	
	if(height == 0)
		height = 1;
		
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();							// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	//gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,100.0f);
	glOrtho(0, width, height, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
	glLoadIdentity();							// Reset The Modelview Matrix
	
	//qDebug() << "GLWidget::resizeGL: "<<width<<","<<height;
	setViewport(viewport());
}
	
void GLWidget::setViewport(const QRectF& rect)
{
	m_viewport = rect;
	//qDebug() << "GLWidget::setViewport: "<<rect;
	
	QRectF viewport = m_viewport;
	if(!viewport.isValid())
	{
		QSizeF canvas = m_canvasSize;
		if(canvas.isNull() || !canvas.isValid())
			canvas = QSizeF(1000.,750.);
		viewport = QRectF(QPointF(0.,0.),canvas);
	}
	
	float vw = viewport.width();
	float vh = viewport.height();
	
	// Scale viewport size to our size
	float sx = ((float)width())  / vw;
	float sy = ((float)height()) / vh;
	
	//qDebug() << "

	float scale = qMin(sx,sy);
	
	// Center viewport in our rectangle
	float scaledWidth  = vw * scale;
	float scaledHeight = vh * scale;
	
	// Calculate centering 
	float xt = (width()  - scaledWidth) /2;
	float yt = (height() - scaledHeight)/2;
	
	// Apply top-left translation for viewport location
	float xtv = xt - viewport.left() * scale;
	float ytv = yt - viewport.top()  * scale;
	
	setTransform(QTransform().translate(xtv,ytv).scale(scale,scale));
	
	//QSize size(width,height);
	QSize size = viewport.size().toSize();
	//qDebug() <<"GLWidget::setViewport(): size:"<<size;
	foreach(GLDrawable *drawable, m_drawables)
		drawable->viewportResized(size);
	
	updateGL();

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

void GLWidget::setCanvasSize(const QSizeF& size)
{
	m_canvasSize = size;
	foreach(GLDrawable *drawable, m_drawables)
		drawable->canvasResized(size);
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
