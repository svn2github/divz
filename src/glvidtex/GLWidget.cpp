#include <QtGui>
#include <QtOpenGL>
#include "GLWidget.h"
#include "GLDrawable.h"

#include <math.h>

#include <QGLFramebufferObject>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

#include "GLCommonShaders.h"

GLWidget::GLWidget(QWidget *parent, QGLWidget *shareWidget)
	: QGLWidget(QGLFormat(QGL::SampleBuffers),parent, shareWidget)
	, m_glInited(false)
	, m_fbo(0)
	, m_cornerTranslationsEnabled(true)
	, m_aspectRatioMode(Qt::KeepAspectRatio)
	, m_program(0)
	, m_useShaders(false)
	, m_colorsDirty(true)
	, m_flipHorizontal(false)
	, m_flipVertical(true)
	, m_brightness(0)
	, m_contrast(0)
	, m_hue(0)
	, m_saturation(0)
{
	
	m_cornerTranslations 
		<<  QPointF(0,0)
		<<  QPointF(0,0)
		<<  QPointF(0,0)
		<<  QPointF(0,0);
	
	setCanvasSize(QSizeF(1000.,750.));
	// setViewport() will use canvas size by default to construct a rect
	//setViewport(QRectF(QPointF(0,0),canvasSize()));
	//qDebug() << "GLWidget::doubleBuffered: "<<doubleBuffer();
	
}

GLWidget::~GLWidget()
{
	delete m_fbo;
}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(60, 45);
}

QSize GLWidget::sizeHint() const
{
	return QSize(400,300);
}

void GLWidget::setCornerTranslations(const QPolygonF& p)
{
	m_cornerTranslations = p;
	updateGL();
}
	
void GLWidget::setTopLeftTranslation(const QPointF& p)
{
	m_cornerTranslations[0] = p;
	updateGL();
}

void GLWidget::setTopRightTranslation(const QPointF& p)
{
	m_cornerTranslations[1] = p;
	updateGL();
}

void GLWidget::setBottomLeftTranslation(const QPointF& p)
{
	m_cornerTranslations[2] = p;
	updateGL();
}

void GLWidget::setBottomRightTranslation(const QPointF& p)
{
	m_cornerTranslations[3] = p;
	updateGL();
}

void GLWidget::enableCornerTranslations(bool flag)
{
	m_cornerTranslationsEnabled = flag;
}

void GLWidget::initializeGL()
{
	makeCurrent();
	
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
	
	m_fbo = new QGLFramebufferObject(QSize(16,16));
	m_program = new QGLShaderProgram(context(), this);
	
	#ifndef QT_OPENGL_ES
	glActiveTexture = (_glActiveTexture)context()->getProcAddress(QLatin1String("glActiveTexture"));
	#endif
	
	const GLubyte *str = glGetString(GL_EXTENSIONS); 
	m_useShaders = (strstr((const char *)str, "GL_ARB_fragment_shader") != NULL);
	
	if(0)
	{
		qDebug() << "GLWidget::initGL: Forcing NO GLSL shaders";
		m_useShaders = false;
	}
	
	if(0)
	{
		qDebug() << "GLWidget::initGL: Forcing GLSL shaders";
		m_useShaders = true;
	}
	
	initShaders();
	
	initAlphaMask();
		
	m_glInited = true;
	//qDebug() << "GLWidget::initializeGL()";
	foreach(GLDrawable *drawable, m_drawables)
		drawable->initGL();
		
	//resizeGL(width(),height());
	//setViewport(viewport());
	
}

void GLWidget::initShaders()
{
	
	const char *fragmentProgram = 
		qt_glsl_rgbShaderProgram;
		//qt_glsl_argbShaderProgram;
		//qt_glsl_xrgbShaderProgram;
	
	if(!m_program->shaders().isEmpty())
		m_program->removeAllShaders();
	
	if(!QGLShaderProgram::hasOpenGLShaderPrograms())
	{
		qDebug() << "GLWidget::initShaders: GLSL Shaders Not Supported by this driver, this program will NOT function as expected and will likely crash.";
		return;// false;
	}

	if (!fragmentProgram) 
	{
		qDebug() << "GLWidget::initShaders: No shader program found - format not supported.";
		return;// false;
	} 
	else 
	if (!m_program->addShaderFromSourceCode(QGLShader::Vertex, qt_glsl_vertexShaderProgram)) 
	{
		qWarning("GLWidget::initShaders: Vertex shader compile error %s",
			qPrintable(m_program->log()));
		//error = QAbstractVideoSurface::ResourceError;
		return;// false;
		
	} 
	else 
	if (!m_program->addShaderFromSourceCode(QGLShader::Fragment, fragmentProgram)) 
	{
		qWarning("GLWidget::initShaders: Shader compile error %s", qPrintable(m_program->log()));
		//error = QAbstractVideoSurface::ResourceError;
		m_program->removeAllShaders();
		return;// false;
	} 
	else 
	if(!m_program->link()) 
	{
		qWarning("GLWidget::initShaders: Shader link error %s", qPrintable(m_program->log()));
		m_program->removeAllShaders();
		return;// false;
	} 
}

void GLWidget::initAlphaMask()
{
	glGenTextures(1, &m_alphaTextureId);
	
	// Alpha mask may have been set prior to initAlphaMask() due to the fact init...() is called from initalizeGL()
	if(m_alphaMask.isNull())
	{
		m_alphaMask = QImage(1,1,QImage::Format_RGB32);
		m_alphaMask.fill(Qt::black);
 		//qDebug() << "GLVideoDrawable::initGL: BLACK m_alphaMask.size:"<<m_alphaMask.size();
 		setAlphaMask(m_alphaMask);
	}
	else
	{
 		//qDebug() << "GLVideoDrawable::initGL: Alpha mask already set, m_alphaMask.size:"<<m_alphaMask.size();
		setAlphaMask(m_alphaMask_preScaled);
	}
}

void GLWidget::showEvent(QShowEvent *)
{
	QTimer::singleShot(50,this,SLOT(postInitGL()));
}


void GLWidget::postInitGL()
{
	resizeGL(width(),height());
}


void GLWidget::makeRenderContextCurrent()
{
	if(m_fbo)
		m_fbo->bind();
	else
		makeCurrent();
}

void GLWidget::setAlphaMask(const QImage &mask)
{
	m_alphaMask_preScaled = mask;
	m_alphaMask = mask;
	
	if(mask.isNull())
	{
		//qDebug() << "GLVideoDrawable::setAlphaMask: "<<this<<",  Got null mask, size:"<<mask.size();
		//return;
		m_alphaMask = QImage(1,1,QImage::Format_RGB32);
		m_alphaMask.fill(Qt::black);
 		//qDebug() << "GLVideoDrawable::initGL: BLACK m_alphaMask.size:"<<m_alphaMask.size();
 		setAlphaMask(m_alphaMask);
 		return;
	}
	
	if(m_glInited)
	{
		QSize targetSize = m_fbo->size();
		if(targetSize == QSize(0,0))
		{
			//qDebug() << "GLVideoDrawable::setAlphaMask: "<<this<<", Not scaling or setting mask, video size is 0x0";
			return;
		}
		
		makeCurrent();
		if(m_alphaMask.size() != targetSize)
		{
			//qDebug() << "GLVideoDrawable::setAlphaMask: "<<this<<",  Mask size and source size different, scaling";
			m_alphaMask = m_alphaMask.scaled(targetSize);
		}
		
		if(m_alphaMask.format() != QImage::Format_ARGB32)
		{
			//qDebug() << "GLVideoDrawable::setAlphaMask: "<<this<<",  Mask format not ARGB32, reformatting";
			m_alphaMask = m_alphaMask.convertToFormat(QImage::Format_ARGB32);
		}
			
		if(m_alphaMask.cacheKey() == m_uploadedCacheKey)
		{
			//qDebug() << "GLVideoDrawable::setAlphaMask: "<<this<<",  Current mask already uploaded to GPU, not re-uploading.";
			return;
		}
			
		
		m_uploadedCacheKey = m_alphaMask.cacheKey();

		//qDebug() << "GLVideoDrawable::setAlphaMask: "<<this<<",  Valid mask, size:"<<m_alphaMask.size()<<", null?"<<m_alphaMask.isNull();
		
		glBindTexture(GL_TEXTURE_2D, m_alphaTextureId);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			mask.width(),
			mask.height(),
			0,
			GL_RGBA, 
			GL_UNSIGNED_BYTE,
			mask.scanLine(0)
			);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	
// 	qDebug() << "GLVideoDrawable::setAlphaMask: "<<this<<",  AT END :"<<m_alphaMask.size()<<", null?"<<m_alphaMask.isNull();
	
}

void GLWidget::paintGL()
{
	// Render all drawables into the FBO
	m_fbo->bind();
	
	qglClearColor(Qt::black);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	QRectF viewport = m_viewport;
	if(!viewport.isValid())
	{
		QSizeF canvas = m_canvasSize;
		if(canvas.isNull() || !canvas.isValid())
			canvas = QSizeF(1000.,750.);
		viewport = QRectF(QPointF(0.,0.),canvas);
	}
	
	//int counter = 0;
	foreach(GLDrawable *drawable, m_drawables)
	{
		//qDebug() << "GLWidget::paintGL(): ["<<counter++<<"] drawable->rect: "<<drawable->rect();
		
// 		qDebug() << "GLWidget::paintGL(): drawable:"<<((void*)drawable)<<", isvis:"<<drawable->isVisible();
		// Don't draw if not visible or if opacity == 0
		if(drawable->isVisible() && 
		   drawable->opacity() > 0 &&
		   drawable->rect().intersects(viewport))
			drawable->paintGL();
// 		qDebug() << "GLWidget::paintGL(): drawable:"<<((void*)drawable)<<", draw done";
	}

	glFlush();
	
	m_fbo->release();
	

	// Now render the FBO to the screen, applying a variety of transforms/effects:
	// 1. Corner distortion ("keystoning") - can move any of the four corners individually
	// 2. Alpha masking - using a PNG image as a mask, using only the alpha channel to blend/black out areas of the final image
	// 3. Brightness/Contrast/Hue/Saturation adjustments - Adjust the B/C/H/S over the entire output image, not just individual drawables
	// The alpha masking and BCHS adjustments require pixel shaders - therefore, if the system does not support them (<OpenGL 2), then
	// only the first item (corner distortion) will work.
	qglClearColor(Qt::black);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); // Reset The View
	
	glEnable(GL_TEXTURE_2D);
	
	if(m_useShaders)
	{
		if (m_colorsDirty) 
		{
			//qDebug() << "Updating color matrix";
			updateColors(m_brightness, m_contrast, m_hue, m_saturation);
			m_colorsDirty = false;
		}
		
		QRectF source = QRect(0,0,m_fbo->size().width(),m_fbo->size().height());
		QRectF target = rect();
		
// 		source = source.adjusted(
// 			m_displayOpts.cropTopLeft.x(),
// 			m_displayOpts.cropTopLeft.y(),
// 			m_displayOpts.cropBottomRight.x(),
// 			m_displayOpts.cropBottomRight.y());
		
		const int width  = QGLContext::currentContext()->device()->width();
		const int height = QGLContext::currentContext()->device()->height();
	
		QTransform transform =  QTransform(); //m_glw->transform(); //= painter.deviceTransform();
		//transform = transform.scale(1.25,1.);
// 		if(!translation().isNull())
// 			transform *= QTransform().translate(translation().x(),translation().y());
		
		const GLfloat wfactor =  2.0 / width;
		const GLfloat hfactor = -2.0 / height;
	
// 		if(!rotation().isNull())
// 		{
// 			qreal tx = target.width()  * rotationPoint().x() + target.x();
// 			qreal ty = target.height() * rotationPoint().y() + target.y();
// 			qreal x, y;
// 			transform.map(tx,ty,&x,&y);
// 			
// 			QVector3D rot = rotation();
// 			transform *= QTransform()
// 				.translate(x,y)
// 				.rotate(rot.x(),Qt::XAxis)
// 				.rotate(rot.y(),Qt::YAxis)
// 				.rotate(rot.z(),Qt::ZAxis)
// 				.translate(-x,-y);
// 		}
		const GLfloat positionMatrix[4][4] =
		{
			{
				/*(0,0)*/ wfactor * transform.m11() - transform.m13(),
				/*(0,1)*/ hfactor * transform.m12() + transform.m13(),
				/*(0,2)*/ 0.0,
				/*(0,3)*/ transform.m13()
			}, {
				/*(1,0)*/ wfactor * transform.m21() - transform.m23(),
				/*(1,1)*/ hfactor * transform.m22() + transform.m23(),
				/*(1,2)*/ 0.0,
				/*(1,3)*/ transform.m23()
			}, {
				/*(2,0)*/ 0.0,
				/*(2,1)*/ 0.0,
				/*(2,2)*/ -1.0,
				/*(2,3)*/ 0.0
			}, {
				/*(3,0)*/ wfactor * transform.dx() - transform.m33(),
				/*(3,1)*/ hfactor * transform.dy() + transform.m33(),
				/*(3,2)*/ 0.0,
				/*(3,3)*/ transform.m33()
			}
		};
		
		
		//QVector3D list[] = 
	
		const GLfloat vertexCoordArray[] =
		{
			target.left()      + m_cornerTranslations[2].x(), target.bottom() + 1 - m_cornerTranslations[2].y(), //(GLfloat)zIndex(),
			target.right() + 1 - m_cornerTranslations[3].x(), target.bottom() + 1 - m_cornerTranslations[3].y(), //(GLfloat)zIndex(),
			target.left()      + m_cornerTranslations[0].x(), target.top()        + m_cornerTranslations[0].y(), 	//(GLfloat)zIndex(),
			target.right() + 1 - m_cornerTranslations[1].x(), target.top()        + m_cornerTranslations[1].y()//, 	(GLfloat)zIndex()
		};
		
		
		const GLfloat txLeft   = m_flipHorizontal ? source.right()  / m_fbo->size().width() : source.left()  / m_fbo->size().width();
		const GLfloat txRight  = m_flipHorizontal ? source.left()   / m_fbo->size().width() : source.right() / m_fbo->size().width();
		
		const GLfloat txTop    = m_flipVertical //m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
			? source.top()    / m_fbo->size().height()
			: source.bottom() / m_fbo->size().height();
		const GLfloat txBottom = m_flipVertical //m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
			? source.bottom() / m_fbo->size().height()
			: source.top()    / m_fbo->size().height();
	
		const GLfloat textureCoordArray[] =
		{
			txLeft , txBottom,
			txRight, txBottom,
			txLeft , txTop,
			txRight, txTop
		};
		
		double liveOpacity = 1.;//(opacity() * (m_fadeActive ? m_fadeValue : 1.));
	
		m_program->bind();
	
		m_program->enableAttributeArray("vertexCoordArray");
		m_program->enableAttributeArray("textureCoordArray");
		
		m_program->setAttributeArray("vertexCoordArray",  vertexCoordArray,  2);
		m_program->setAttributeArray("textureCoordArray", textureCoordArray, 2);
		
		m_program->setUniformValue("positionMatrix",      positionMatrix);
	// 	QMatrix4x4 mat4(
	// 		positionMatrix[0][0], positionMatrix[0][1], positionMatrix[0][2], positionMatrix[0][3],
	// 		positionMatrix[1][0], positionMatrix[1][1], positionMatrix[1][2], positionMatrix[1][3], 
	// 		positionMatrix[2][0], positionMatrix[2][1], positionMatrix[2][2], positionMatrix[2][3], 
	// 		positionMatrix[3][0], positionMatrix[3][1], positionMatrix[3][2], positionMatrix[3][3]
	// 		); 
	// 	m_program->setUniformValue("positionMatrix",      mat4);
		
		//qDebug() << "GLVideoDrawable:paintGL():"<<this<<", rendering with opacity:"<<opacity();
		m_program->setUniformValue("alpha",               (GLfloat)liveOpacity);
		m_program->setUniformValue("texOffsetX",          (GLfloat)0.);//m_invertedOffset.x());
		m_program->setUniformValue("texOffsetY",          (GLfloat)0.);//m_invertedOffset.y());
			
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_fbo->texture());
		
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
	
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_alphaTextureId);
		
		glActiveTexture(GL_TEXTURE0);
	
		m_program->setUniformValue("texRgb", 0);
		m_program->setUniformValue("alphaMask", 1);
		
		m_program->setUniformValue("colorMatrix", m_colorMatrix);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
		m_program->release();
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, m_fbo->texture());
	
		//glTranslatef(0.0f,0.0f,-3.42f);
		
		glBegin(GL_QUADS);
			// Front Face
	//              glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	//              glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	//              glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
	//              glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	
			//QRectF rect(10,10,320,240);
			QRectF rect(0,0,width(),height());
	
			qreal
				vx1 = rect.left(),
				vx2 = rect.right(),
				vy1 = rect.bottom(),
				vy2 = rect.top();
	
	//                 glTexCoord2f(0.0f, 0.0f); glVertex3f(vx1,vy1,  0.0f); // top left
	//                 glTexCoord2f(1.0f, 0.0f); glVertex3f(vx2,vy1,  0.0f); // top right
	//                 glTexCoord2f(1.0f, 1.0f); glVertex3f(vx2,vy2,  0.0f); // bottom right
	//                 glTexCoord2f(0.0f, 1.0f); glVertex3f(vx1,vy2,  0.0f); // bottom left
	
			if(1)
			{
				glTexCoord2f(0.0f, 0.0f); glVertex3f(vx1 + m_cornerTranslations[3].x(),vy1 + m_cornerTranslations[3].y(),  0.0f); // bottom left // 3
				glTexCoord2f(1.0f, 0.0f); glVertex3f(vx2 - m_cornerTranslations[2].x(),vy1 + m_cornerTranslations[2].y(),  0.0f); // bottom right // 2
				glTexCoord2f(1.0f, 1.0f); glVertex3f(vx2 - m_cornerTranslations[1].x(),vy2 - m_cornerTranslations[1].y(),  0.0f); // top right  // 1
				glTexCoord2f(0.0f, 1.0f); glVertex3f(vx1 + m_cornerTranslations[0].x(),vy2 - m_cornerTranslations[0].y(),  0.0f); // top left // 0
			}
	
			if(0)
			{
				qreal inc = fabs(vy2-vy1)/10.;
				qreal dx = fabs(vx2-vx1);
				qreal dy = fabs(vy2-vy1);
				qreal xf = inc/dx;
				qreal yf = inc/dy;
				//qDebug() << "params:"<<xf<<yf;
				for(qreal x=vx1; x<vx2; x+=inc)
				{
					qreal tx = x/dx;
					//qDebug() << "tx:"<<tx;
					for(qreal y=vy1; y>=vy2; y-=inc)
					{
						qreal ty = 1.-(y/dy);
						//qDebug() << "Y:"<<y;
						//qDebug() << "at:"<<x<<",y:"<<y;
	// 					glTexCoord2f(0.0f, 0.0f); glVertex3f(x,y,  0.0f); // bottom left
	// 					glTexCoord2f(1.0f, 0.0f); glVertex3f(x+inc,y,  0.0f); // bottom right
	// 					glTexCoord2f(1.0f, 1.0f); glVertex3f(x+inc,y+inc,  0.0f); // top right
	// 					glTexCoord2f(0.0f, 1.0f); glVertex3f(x,y+inc,  0.0f); // top left
						
						
	// 					glTexCoord2f(0.0f, 0.0f); glVertex3f(x,y+inc,  0.0f); // bottom left
	// 					glTexCoord2f(1.0f, 0.0f); glVertex3f(x+inc,y+inc,  0.0f); // bottom right
	// 					glTexCoord2f(1.0f, 1.0f); glVertex3f(x+inc,y,  0.0f); // top right
	// 					glTexCoord2f(0.0f, 1.0f); glVertex3f(x,y,  0.0f); // top left
	
						glTexCoord2f(tx, ty);
									glVertex3f(x,y+inc,  0.0f); // bottom left
						
						glTexCoord2f(tx+xf, ty);	
									glVertex3f(x+inc,y+inc,  0.0f); // bottom right
						
						glTexCoord2f(tx+xf, ty+yf); 
									glVertex3f(x+inc,y,  0.0f); // top right
						
						glTexCoord2f(tx, ty+yf); 
									glVertex3f(x,y,  0.0f); // top left
					}
				}
			}
	
	//              glTexCoord2f(0,0); glVertex3f( 0, 0,0); //lo
	//              glTexCoord2f(0,1); glVertex3f(256, 0,0); //lu
	//              glTexCoord2f(1,1); glVertex3f(256, 256,0); //ru
	//              glTexCoord2f(1,0); glVertex3f( 0, 256,0); //ro
		glEnd();
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


void GLWidget::setFlipHorizontal(bool value)
{
	m_flipHorizontal = value;
	updateGL();
}

void GLWidget::setFlipVertical(bool value)
{
	m_flipVertical = value;
	updateGL();
}

/*!
*/
void GLWidget::setBrightness(int brightness)
{
	m_brightness = brightness;
	m_colorsDirty = true;
	updateGL();
}

/*!
*/
void GLWidget::setContrast(int contrast)
{
	m_contrast = contrast;
	m_colorsDirty = true;
	updateGL();
}

/*!
*/
void GLWidget::setHue(int hue)
{
	m_hue = hue;
	m_colorsDirty = true;
	updateGL();
}

/*!
*/
void GLWidget::setSaturation(int saturation)
{
	m_saturation = saturation;
	m_colorsDirty = true;
	updateGL();
}

void GLWidget::updateColors(int brightness, int contrast, int hue, int saturation)
{
	//qDebug() << "GLWidget::updateColors: b:"<<brightness<<", c:"<<contrast<<", h:"<<hue<<", s:"<<saturation;
	
	const qreal b = brightness / 200.0;
	const qreal c = contrast / 200.0 + 1.0;
	const qreal h = hue / 200.0;
	const qreal s = saturation / 200.0 + 1.0;
	
	const qreal cosH = qCos(M_PI * h);
	const qreal sinH = qSin(M_PI * h);
	
	const qreal h11 = -0.4728 * cosH + 0.7954 * sinH + 1.4728;
	const qreal h21 = -0.9253 * cosH - 0.0118 * sinH + 0.9523;
	const qreal h31 =  0.4525 * cosH + 0.8072 * sinH - 0.4524;
	
	const qreal h12 =  1.4728 * cosH - 1.3728 * sinH - 1.4728;
	const qreal h22 =  1.9253 * cosH + 0.5891 * sinH - 0.9253;
	const qreal h32 = -0.4525 * cosH - 1.9619 * sinH + 0.4525;
	
	const qreal h13 =  1.4728 * cosH - 0.2181 * sinH - 1.4728;
	const qreal h23 =  0.9253 * cosH + 1.1665 * sinH - 0.9253;
	const qreal h33 =  0.5475 * cosH - 1.3846 * sinH + 0.4525;
	
	const qreal sr = (1.0 - s) * 0.3086;
	const qreal sg = (1.0 - s) * 0.6094;
	const qreal sb = (1.0 - s) * 0.0820;
	
	const qreal sr_s = sr + s;
	const qreal sg_s = sg + s;
	const qreal sb_s = sr + s;
	
	const float m4 = (s + sr + sg + sb) * (0.5 - 0.5 * c + b);
	
	m_colorMatrix(0, 0) = c * (sr_s * h11 + sg * h21 + sb * h31);
	m_colorMatrix(0, 1) = c * (sr_s * h12 + sg * h22 + sb * h32);
	m_colorMatrix(0, 2) = c * (sr_s * h13 + sg * h23 + sb * h33);
	m_colorMatrix(0, 3) = m4;
	
	m_colorMatrix(1, 0) = c * (sr * h11 + sg_s * h21 + sb * h31);
	m_colorMatrix(1, 1) = c * (sr * h12 + sg_s * h22 + sb * h32);
	m_colorMatrix(1, 2) = c * (sr * h13 + sg_s * h23 + sb * h33);
	m_colorMatrix(1, 3) = m4;
	
	m_colorMatrix(2, 0) = c * (sr * h11 + sg * h21 + sb_s * h31);
	m_colorMatrix(2, 1) = c * (sr * h12 + sg * h22 + sb_s * h32);
	m_colorMatrix(2, 2) = c * (sr * h13 + sg * h23 + sb_s * h33);
	m_colorMatrix(2, 3) = m4;
	
	m_colorMatrix(3, 0) = 0.0;
	m_colorMatrix(3, 1) = 0.0;
	m_colorMatrix(3, 2) = 0.0;
	m_colorMatrix(3, 3) = 1.0;
}

void GLWidget::addDrawable(GLDrawable *item)
{
	//makeCurrent();
	
	QString newName = QString("%1/%2").arg(objectName()).arg(item->objectName());
	item->setObjectName(qPrintable(newName));
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
	glViewport(0,0,width,height); //(width - side) / 2, (height - side) / 2, side, side);
	
	//qDebug() << "GLWidget::resizeGL(): width:"<<width<<", height:"<<height;
	
	if(m_fbo && 
		(m_fbo->size().width()  != width ||
		 m_fbo->size().height() != height))
		
	{
		delete m_fbo;
		QSize size(width,height);
		m_fbo = new QGLFramebufferObject(size);
		
		//qDebug() << "GLWidget::resizeGL(): New FBO size:"<<size;
	}
	
	if(height == 0)
		height = 1;
		
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();							// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	//gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,100.0f);
	glOrtho(0, width, height, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
	glLoadIdentity();							// Reset The Modelview Matrix
	
	
	
// 	glMatrixMode(GL_PROJECTION);
// 	glLoadIdentity();
// 	qreal aspect = (qreal)width / (qreal)(height ? height : 1);
// 	#ifdef QT_OPENGL_ES
// 	glFrustumf(-aspect, +aspect, -1.0, +1.0, 4.0, 15.0);
// 	#else
// 	glFrustum(-aspect, +aspect, -1.0, +1.0, 4.0, 15.0);
// 	#endif
// 	glMatrixMode(GL_MODELVIEW);
	
	
	//qDebug() << "GLWidget::resizeGL: "<<width<<","<<height;
	setViewport(viewport());
	setAlphaMask(m_alphaMask_preScaled);

}
	
	
void GLWidget::setAspectRatioMode(Qt::AspectRatioMode mode)
{
	//qDebug() << "GLVideoDrawable::setAspectRatioMode: "<<this<<", mode:"<<mode<<", int:"<<(int)mode;
	m_aspectRatioMode = mode;
	setViewport(viewport());
	updateGL();
}


void GLWidget::setViewport(const QRectF& rect)
{
	m_viewport = rect;
	
	QRectF viewport = m_viewport;
	if(!viewport.isValid())
	{
		QSizeF canvas = m_canvasSize;
		if(canvas.isNull() || !canvas.isValid())
			canvas = QSizeF(1000.,750.);
		viewport = QRectF(QPointF(0.,0.),canvas);
	}
	
	//qDebug() << "GLWidget::setViewport: "<<viewport;
	
	float vw = viewport.width();
	float vh = viewport.height();
	
	// Scale viewport size to our size
	float winWidth  = (float)(m_fbo ? m_fbo->size().width()  : width());
	float winHeight = (float)(m_fbo ? m_fbo->size().height() : height());
	
	float sx = winWidth  / vw;
	float sy = winHeight / vh;
	
	//qDebug() << "

	//float scale = qMin(sx,sy);
	if (m_aspectRatioMode != Qt::IgnoreAspectRatio)
	{
		if(sx < sy)
			sy = sx;
		else
			sx = sy;
	}
	
	// Center viewport in our rectangle
	float scaledWidth  = vw * sx;//scale;
	float scaledHeight = vh * sy;//scale;
	
	// Calculate centering 
	float xt = (winWidth  - scaledWidth) /2;
	float yt = (winHeight - scaledHeight)/2;
	
	// Apply top-left translation for viewport location
	float xtv = xt - viewport.left() * sx;//scale;
	float ytv = yt - viewport.top()  * sy;//scale;
	
	setTransform(QTransform().translate(xtv,ytv).scale(sx,sy));//scale,scale));
	
	//QSize size(width,height);
	QSize size = viewport.size().toSize();
	//qDebug() <<"GLWidget::setViewport:  size:"<<size<<", sx:"<<sx<<",sy:"<<sy<<", xtv:"<<xtv<<", ytv:"<<ytv<<", scaled size:"<<scaledWidth<<scaledHeight;
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
