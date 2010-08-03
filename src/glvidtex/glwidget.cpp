#include <QtGui>
#include <QtOpenGL>
#include <QVideoFrame>
#include <QAbstractVideoSurface>
#include "glwidget.h"
#include "../livemix/VideoThread.h"

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)

#ifndef Q_WS_MAC
# ifndef APIENTRYP
#   ifdef APIENTRY
#     define APIENTRYP APIENTRY *
#   else
#     define APIENTRY
#     define APIENTRYP *
#   endif
# endif
#else
# define APIENTRY
# define APIENTRYP *
#endif

#ifndef GL_TEXTURE0
#  define GL_TEXTURE0    0x84C0
#  define GL_TEXTURE1    0x84C1
#  define GL_TEXTURE2    0x84C2
#endif
#ifndef GL_PROGRAM_ERROR_STRING_ARB
#  define GL_PROGRAM_ERROR_STRING_ARB       0x8874
#endif

#ifndef GL_UNSIGNED_SHORT_5_6_5
#  define GL_UNSIGNED_SHORT_5_6_5 33635
#endif

#endif

static const char *qt_glsl_vertexShaderProgram =
        "attribute highp vec4 vertexCoordArray;\n"
        "attribute highp vec2 textureCoordArray;\n"
        "uniform highp mat4 positionMatrix;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "   gl_Position = positionMatrix * vertexCoordArray;\n"
        "   textureCoord = textureCoordArray;\n"
        "}\n";

// Paints an RGB32 frame
static const char *qt_glsl_xrgbShaderProgram =
        "uniform sampler2D texRgb;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec4 color = vec4(texture2D(texRgb, textureCoord.st).bgr, 1.0);\n"
        "    gl_FragColor = colorMatrix * color;\n"
        "}\n";

// Paints an ARGB frame.
static const char *qt_glsl_argbShaderProgram =
        "uniform sampler2D texRgb;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec4 color = vec4(texture2D(texRgb, textureCoord.st).bgr, 1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, texture2D(texRgb, textureCoord.st).a);\n"
        "}\n";

// Paints an RGB(A) frame.
static const char *qt_glsl_rgbShaderProgram =
        "uniform sampler2D texRgb;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec4 color = vec4(texture2D(texRgb, textureCoord.st).rgb, 1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, texture2D(texRgb, textureCoord.st).a);\n"
        "}\n";

// Paints a YUV420P or YV12 frame.
static const char *qt_glsl_yuvPlanarShaderProgram =
        "uniform sampler2D texY;\n"
        "uniform sampler2D texU;\n"
        "uniform sampler2D texV;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec4 color = vec4(\n"
        "           texture2D(texY, textureCoord.st).r,\n"
        "           texture2D(texU, textureCoord.st).r,\n"
        "           texture2D(texV, textureCoord.st).r,\n"
        "           1.0);\n"
        "    gl_FragColor = colorMatrix * color;\n"
        "}\n";



GLWidget::GLWidget(QWidget *parent, QGLWidget *shareWidget)
	: QGLWidget(parent, shareWidget)
	, m_colorsDirty(true)
	, m_brightness(0)
	, m_contrast(0)
	, m_hue(0)
	, m_saturation(0)
	//, m_pixelFormat(QVideoFrame::Format_Invalid)
{
	clearColor = Qt::black;
	xRot = 0;
	yRot = 0;
	zRot = 0;
	//program = 0;
	
	#ifndef QT_OPENGL_ES
	glActiveTexture = (_glActiveTexture)context()->getProcAddress(QLatin1String("glActiveTexture"));
	#endif
	
	VideoThread * thread = new VideoThread();
	thread->setVideo("../data/Seasons_Loop_3_SD.mpg");
	thread->start();
	m_source = thread;
	
	connect(thread, SIGNAL(frameReady()), this, SLOT(frameReady()));

}

GLWidget::~GLWidget()
{
}

void GLWidget::frameReady()
{
	if(!m_source)
		return;
		
	m_frame = m_source->frame();
	updateGL();
}

QSize GLWidget::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
	return QSize(200, 200);
}

void GLWidget::rotateBy(int xAngle, int yAngle, int zAngle)
{
	xRot += xAngle;
	yRot += yAngle;
	zRot += zAngle;
	updateGL();
}

void GLWidget::setClearColor(const QColor &color)
{
	clearColor = color;
	updateGL();
}

void GLWidget::updateColors(int brightness, int contrast, int hue, int saturation)
{
	const qreal b = brightness / 200.0;
	const qreal c = contrast / 100.0 + 1.0;
	const qreal h = hue / 200.0;
	const qreal s = saturation / 100.0 + 1.0;
	
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
	
	if (m_yuv) 
	{
		m_colorMatrix = m_colorMatrix * QMatrix4x4(
			1.0,  0.000,  1.140, -0.5700,
			1.0, -0.394, -0.581,  0.4875,
			1.0,  2.028,  0.000, -1.0140,
			0.0,  0.000,  0.000,  1.0000);
	}
}

void GLWidget::initRgbTextureInfo(GLenum internalFormat, GLuint format, GLenum type, const QSize &size)
{
	m_yuv = false;
	
	m_textureInternalFormat = internalFormat;
	m_textureFormat = format;
	m_textureType   = type;
	
	m_textureCount  = 1;
	
	m_textureWidths[0]  = size.width();
	m_textureHeights[0] = size.height();
	m_textureOffsets[0] = 0;
}

void GLWidget::initYuv420PTextureInfo(const QSize &size)
{
	m_yuv = true;
	
	m_textureInternalFormat = GL_LUMINANCE;
	m_textureFormat = GL_LUMINANCE;
	m_textureType   = GL_UNSIGNED_BYTE;
	
	m_textureCount  = 3;
	
	m_textureWidths[0] = size.width();
	m_textureHeights[0] = size.height();
	m_textureOffsets[0] = 0;
	
	m_textureWidths[1] = size.width() / 2;
	m_textureHeights[1] = size.height() / 2;
	m_textureOffsets[1] = size.width() * size.height();
	
	m_textureWidths[2] = size.width() / 2;
	m_textureHeights[2] = size.height() / 2;
	m_textureOffsets[2] = size.width() * size.height() * 5 / 4;
}

void GLWidget::initYv12TextureInfo(const QSize &size)
{
	m_yuv = true;
	
	m_textureInternalFormat = GL_LUMINANCE;
	m_textureFormat = GL_LUMINANCE;
	m_textureType   = GL_UNSIGNED_BYTE;
	
	m_textureCount  = 3;
	m_textureWidths[0] = size.width();
	m_textureHeights[0] = size.height();
	m_textureOffsets[0] = 0;
	
	m_textureWidths[1] = size.width() / 2;
	m_textureHeights[1] = size.height() / 2;
	m_textureOffsets[1] = size.width() * size.height() * 5 / 4;
	
	m_textureWidths[2] = size.width() / 2;
	m_textureHeights[2] = size.height() / 2;
	m_textureOffsets[2] = size.width() * size.height();
}


void GLWidget::initializeGL()
{
	makeObject();
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	// NON-es2
	//glEnable(GL_TEXTURE_2D);
	
	qDebug() << "Sample Texture Size:"<<m_sampleTexture.size();
	
	#define PROGRAM_VERTEX_ATTRIBUTE 0
	#define PROGRAM_TEXCOORD_ATTRIBUTE 1
	
	QVideoSurfaceFormat format(m_sampleTexture.size(), QVideoFrame::Format_ARGB32);
	
	const char *fragmentProgram = 0;

	switch (format.pixelFormat()) 
	{
	case QVideoFrame::Format_RGB32:
		initRgbTextureInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, format.frameSize());
		fragmentProgram = qt_glsl_xrgbShaderProgram;
		break;
        case QVideoFrame::Format_ARGB32:
		initRgbTextureInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, format.frameSize());
		fragmentProgram = qt_glsl_argbShaderProgram;
		break;
#ifndef QT_OPENGL_ES
        case QVideoFrame::Format_RGB24:
		initRgbTextureInfo(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, format.frameSize());
		fragmentProgram = qt_glsl_rgbShaderProgram;
		break;
#endif
	case QVideoFrame::Format_RGB565:
		initRgbTextureInfo(GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, format.frameSize());
		fragmentProgram = qt_glsl_rgbShaderProgram;
		break;
	case QVideoFrame::Format_YV12:
		initYv12TextureInfo(format.frameSize());
		fragmentProgram = qt_glsl_yuvPlanarShaderProgram;
		break;
	case QVideoFrame::Format_YUV420P:
		initYuv420PTextureInfo(format.frameSize());
		fragmentProgram = qt_glsl_yuvPlanarShaderProgram;
		break;
	default:
		break;
	}
    
// 	QGLShader *vshader = new QGLShader(QGLShader::Vertex, this);
// 	const char *vsrc =
// 		"attribute highp vec4 vertex;\n"
// 		"attribute mediump vec4 texCoord;\n"
// 		"varying mediump vec4 texc;\n"
// 		"uniform mediump mat4 matrix;\n"
// 		"void main(void)\n"
// 		"{\n"
// 		"    gl_Position = matrix * vertex;\n"
// 		"    texc = texCoord;\n"
// 		"}\n";
// 	vshader->compileSourceCode(vsrc);
// 	
// 	QGLShader *fshader = new QGLShader(QGLShader::Fragment, this);
// 	const char *fsrc =
// 		"uniform sampler2D texture;\n"
// 		"varying mediump vec4 texc;\n"
// 		"void main(void)\n"
// 		"{\n"
// 		"    gl_FragColor = texture2D(texture, texc.st);\n"
// 		"}\n";
// 	fshader->compileSourceCode(fsrc);
// 	
	//program = new QGLShaderProgram(this);
	//program->addShader(vshader);
// 	program->addShader(fshader);
// 	program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
// 	program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
// 	program->link();

	if (!fragmentProgram) 
	{
		qDebug() << "No program found - format not supported.";
	} 
	else 
	if (!m_program.addShaderFromSourceCode(QGLShader::Vertex, qt_glsl_vertexShaderProgram)) 
	{
		qWarning("QPainterVideoSurface: Vertex shader compile error %s",
			qPrintable(m_program.log()));
		//error = QAbstractVideoSurface::ResourceError;
		
	} 
	else 
	if (!m_program.addShaderFromSourceCode(QGLShader::Fragment, fragmentProgram)) 
	{
		qWarning("QPainterVideoSurface: Shader compile error %s", qPrintable(m_program.log()));
		//error = QAbstractVideoSurface::ResourceError;
		m_program.removeAllShaders();
	} 
	else 
	if(!m_program.link()) 
	{
		qWarning("QPainterVideoSurface: Shader link error %s", qPrintable(m_program.log()));
		m_program.removeAllShaders();
	} 
	else 
	{
		//m_handleType = format.handleType();
		m_scanLineDirection = format.scanLineDirection();
		m_frameSize = format.frameSize();
	
		//if (m_handleType == QAbstractVideoBuffer::NoHandle)
			glGenTextures(m_textureCount, m_textureIds);
	}
	
// 	program->bind();
// 	program->setUniformValue("texture", 0);

}

void GLWidget::paintGL()
{
	if (m_colorsDirty) 
	{
		qDebug() << "Updating color matrix";
		updateColors(m_brightness, m_contrast, m_hue, m_saturation);
		m_colorsDirty = false;
        }

        qglClearColor(clearColor);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// #if !defined(QT_OPENGL_ES_2)
	// 
	//     glLoadIdentity();
	//     glTranslatef(0.0f, 0.0f, -10.0f);
	//     glRotatef(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
	//     glRotatef(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
	//     glRotatef(zRot / 16.0f, 0.0f, 0.0f, 1.0f);
	// 
	//     glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
	//     glTexCoordPointer(2, GL_FLOAT, 0, texCoords.constData());
	//     glEnableClientState(GL_VERTEX_ARRAY);
	//     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	// 
	// #else
	
// 	QMatrix4x4 m;
// 	m.ortho(-0.5f, +0.5f, +0.5f, -0.5f, 4.0f, 15.0f);
// 	m.translate(0.0f, 0.0f, -10.0f);
// /*	m.rotate(xRot / 16.0f, 1.0f, 0.0f, 0.0f);
// 	m.rotate(yRot / 16.0f, 0.0f, 1.0f, 0.0f);
// 	m.rotate(zRot / 16.0f, 0.0f, 0.0f, 1.0f);*/
// 	
// 	program->setUniformValue("matrix", m);
// 	program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
// 	program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
// 	program->setAttributeArray
// 		(PROGRAM_VERTEX_ATTRIBUTE, vertices.constData());
// 	program->setAttributeArray
// 		(PROGRAM_TEXCOORD_ATTRIBUTE, texCoords.constData());
// 	
// 	//#endif
// 	
// // 	for (int i = 0; i < 6; ++i) 
// // 	{
// // 		glBindTexture(GL_TEXTURE_2D, textures[i]);
// // 		glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
// // 	}
// 	
// 	int i=0;
// 	glBindTexture(GL_TEXTURE_2D, textures[i]);
// 	glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);


	//makeCurrent();

	for (int i = 0; i < m_textureCount; ++i) 
	{
		glBindTexture(GL_TEXTURE_2D, m_textureIds[i]);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			m_textureInternalFormat,
			m_textureWidths[i],
			m_textureHeights[i],
			0,
			m_textureFormat,
			m_textureType,
			//m_frame.bits() + m_textureOffsets[i]
			(m_frame.isValid() ? m_frame.image.convertToFormat(QImage::Format_ARGB32).scanLine(0) : m_sampleTexture.scanLine(0)) + m_textureOffsets[i]
			);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	//m_frame.unmap();
	
	QRectF source = m_sampleTexture.rect();
	QRectF target = rect();
	
	
	
	const int width = QGLContext::currentContext()->device()->width();
	const int height = QGLContext::currentContext()->device()->height();

	const QTransform transform;// = painter->deviceTransform();

	const GLfloat wfactor = 2.0 / width;
	const GLfloat hfactor = -2.0 / height;

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

	const GLfloat vTop = m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
		? target.top()
		: target.bottom() + 1;
	const GLfloat vBottom = m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
		? target.bottom() + 1
		: target.top();


	const GLfloat vertexCoordArray[] =
	{
		target.left()     , vBottom,
		target.right() + 1, vBottom,
		target.left()     , vTop,
		target.right() + 1, vTop
	};

	const GLfloat txLeft   = source.left()  / m_frameSize.width();
	const GLfloat txRight  = source.right() / m_frameSize.width();
	
	const GLfloat txTop    = m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
		? source.top()    / m_frameSize.height()
		: source.bottom() / m_frameSize.height();
	const GLfloat txBottom = m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
		? source.bottom() / m_frameSize.height()
		: source.top()    / m_frameSize.height();

	const GLfloat textureCoordArray[] =
	{
		txLeft , txBottom,
		txRight, txBottom,
		txLeft , txTop,
		txRight, txTop
	};

	m_program.bind();

	m_program.enableAttributeArray("vertexCoordArray");
	m_program.enableAttributeArray("textureCoordArray");
	
	m_program.setAttributeArray("vertexCoordArray",  vertexCoordArray,  2);
	m_program.setAttributeArray("textureCoordArray", textureCoordArray, 2);
	
	m_program.setUniformValue("positionMatrix",      positionMatrix);

	if (m_textureCount == 3) 
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_textureIds[1]);
		
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_textureIds[2]);
		
		glActiveTexture(GL_TEXTURE0);
	
		m_program.setUniformValue("texY", 0);
		m_program.setUniformValue("texU", 1);
		m_program.setUniformValue("texV", 2);
	} 
	else 
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
	
		m_program.setUniformValue("texRgb", 0);
	}
	m_program.setUniformValue("colorMatrix", m_colorMatrix);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	m_program.release();



}

void GLWidget::resizeGL(int width, int height)
{
	int side = qMin(width, height);
	glViewport((width - side) / 2, (height - side) / 2, side, side);

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

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - lastPos.x();
	int dy = event->y() - lastPos.y();
	
	if (event->buttons() & Qt::LeftButton) {
		rotateBy(8 * dy, 8 * dx, 0);
	} else if (event->buttons() & Qt::RightButton) {
		rotateBy(8 * dy, 0, 8 * dx);
	}
	lastPos = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */)
{
	emit clicked();
}

void GLWidget::makeObject()
{
// 	static const int coords[6][4][3] = 
// 	{
// 		{ { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
// 		{ { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
// 		{ { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
// 		{ { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
// 		{ { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
// 		{ { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
// 	};
	
	//m_sampleTexture = QImage(QString(":/images/side%1.png").arg(1)).convertToFormat(QImage::Format_ARGB32);
	m_sampleTexture = QImage( 640, 480, QImage::Format_RGB32 );
	m_sampleTexture.fill( Qt::green );
	m_sampleTexture = m_sampleTexture.convertToFormat(QImage::Format_ARGB32);
	
	/*
	for (int j=0; j < 6; ++j) 
	{
		textures[j] = bindTexture
			(QPixmap(QString(":/images/side%1.png").arg(j + 1)), GL_TEXTURE_2D);
		//(QPixmap(j == 0 ? "me2.jpg" : QString(":/images/side%1.png").arg(j + 1)), GL_TEXTURE_2D);
	}
	
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 4; ++j) {
			texCoords.append
				(QVector2D(j == 0 || j == 3, j == 0 || j == 1));
			vertices.append
				(QVector3D(0.2 * coords[i][j][0], 0.2 * coords[i][j][1],
					0.2 * coords[i][j][2]));
		}
	}*/
}

