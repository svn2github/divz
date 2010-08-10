#include <QtGui>
#include <QtOpenGL>
#include <QVideoFrame>
#include <QAbstractVideoSurface>
#include "glwidget.h"
#include "../livemix/VideoThread.h"
#include "../livemix/CameraThread.h"

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



// class GLDrawable 
GLDrawable::GLDrawable(QObject *parent)
	: QObject(parent)
	, m_glw(0)
	, m_zIndex(0)
{}

void GLDrawable::updateGL()
{
	if(m_glw)
		m_glw->updateGL();
}
	
void GLDrawable::setRect(const QRectF& rect)
{
	m_rect = rect;
	drawableResized(rect.size());
	emit drawableResized(rect.size());
}
	
void GLDrawable::setZIndex(double z)
{
	m_zIndex = z;
	emit zIndexChanged(z);
}

void GLDrawable::setGLWidget(GLWidget* w)
{
	m_glw = w;
	if(!w)
		return;
		
	if(m_rect.isNull())
		m_rect = w->rect();
}

void GLDrawable::viewportResized(const QSize& /*newSize*/)
{
	// NOOP
}

void GLDrawable::drawableResized(const QSizeF& /*newSize*/)
{
	// NOOP
}
	
void GLDrawable::paintGL()
{
	// NOOP
}
	
void GLDrawable::initGL()
{
	// NOOP
}

GLVideoDrawable::GLVideoDrawable(QObject *parent)
	: GLDrawable(parent)
	, m_glInited(false)
	, m_colorsDirty(true)
	, m_brightness(0)
	, m_contrast(0)
	, m_hue(0)
	, m_saturation(25)
	, m_frameCount(0)
	, m_latencyAccum(0)
	, m_aspectRatioMode(Qt::KeepAspectRatio)
	
{
	#ifdef Q_OS_WIN
	QString defaultCamera = "vfwcap://0";
	#else
	QString defaultCamera = "/dev/video0";
	#endif


	CameraThread *thread = CameraThread::threadForCamera(defaultCamera);
	if(thread)
	{
		thread->setFps(30);
		//usleep(250 * 1000); // This causes a race condition to manifist itself reliably, which causes a crash every time instead of intermitently. 
		// With the crash reproducable, I can now work to fix it.
		thread->enableRawFrames(true);
		//thread->setDeinterlace(true);
		m_source = thread;
	}
	if(!thread)
	{
		VideoThread * thread = new VideoThread();
		thread->setVideo("../data/Seasons_Loop_3_SD.mpg");
		thread->start();
		m_source = thread;
	}

	m_imagePixelFormats
		<< QVideoFrame::Format_RGB32
		<< QVideoFrame::Format_ARGB32
		<< QVideoFrame::Format_RGB24
		<< QVideoFrame::Format_RGB565
		<< QVideoFrame::Format_YV12
		<< QVideoFrame::Format_YUV420P;
	
	setVideoFormat(m_source->videoFormat());
	
	connect(m_source, SIGNAL(frameReady()), this, SLOT(frameReady()));
}

GLVideoDrawable::~GLVideoDrawable()
{
}

void GLVideoDrawable::frameReady()
{
	if(!m_source)
		return;
		
	m_frame = m_source->frame();
	
	if(m_frame.rect != m_sourceRect)
	{
		resizeTextures(m_frame.size);
		updateRects();
	}
		
	updateGL();
}

/*!
*/
int GLVideoDrawable::brightness() const
{
	return m_brightness;
}

/*!
*/
void GLVideoDrawable::setBrightness(int brightness)
{
	m_brightness = brightness;
	
	m_colorsDirty = true;
}

/*!
*/
int GLVideoDrawable::contrast() const
{
	return m_contrast;
}

/*!
*/
void GLVideoDrawable::setContrast(int contrast)
{
	m_contrast = contrast;
	
	m_colorsDirty = true;
}

/*!
*/
int GLVideoDrawable::hue() const
{
	return m_hue;
}

/*!
*/
void GLVideoDrawable::setHue(int hue)
{
	m_hue = hue;
	
	m_colorsDirty = true;
}

/*!
*/
int GLVideoDrawable::saturation() const
{
	return m_saturation;
}

/*!
*/
void GLVideoDrawable::setSaturation(int saturation)
{
	m_saturation = saturation;
	
	m_colorsDirty = true;
}

void GLVideoDrawable::updateColors(int brightness, int contrast, int hue, int saturation)
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

void GLVideoDrawable::initRgbTextureInfo(GLenum internalFormat, GLuint format, GLenum type, const QSize &size)
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

void GLVideoDrawable::initYuv420PTextureInfo(const QSize &size)
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
	
	//qDebug() << "yuv420p tex: off2:"<<m_textureOffsets[2];
}

void GLVideoDrawable::initYv12TextureInfo(const QSize &size)
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


void GLVideoDrawable::initGL()
{
	#ifndef QT_OPENGL_ES
	glActiveTexture = (_glActiveTexture)glWidget()->context()->getProcAddress(QLatin1String("glActiveTexture"));
	#endif
	
	m_glInited = true;
	setVideoFormat(m_videoFormat);
	
	m_time.start();
}

bool GLVideoDrawable::setVideoFormat(const VideoFormat& format)
{
	m_videoFormat = format;
//m_sampleTexture.load("/opt/qtsdk-2010.02/qt/examples/opengl/pbuffers/cubelogo.png"); 
	//m_sampleTexture = m_sampleTexture.scaled(640,480);
	m_sampleTexture = QImage( format.frameSize, QImage::Format_RGB32 );
	m_sampleTexture.fill( Qt::red );
	m_sampleTexture = m_sampleTexture.convertToFormat(QImage::Format_ARGB32);
	
	if(!m_glInited)
		return m_imagePixelFormats.contains(format.pixelFormat);
	
	//qDebug() << "Sample Texture Size:"<<m_sampleTexture.size();
	
	#define PROGRAM_VERTEX_ATTRIBUTE 0
	#define PROGRAM_TEXCOORD_ATTRIBUTE 1
	
	//QVideoSurfaceFormat format(m_sampleTexture.size(), QVideoFrame::Format_ARGB32);
	//QVideoSurfaceFormat format(m_sampleTexture.size(), QVideoFrame::Format_YUV420P);
	
	const char *fragmentProgram = resizeTextures(format.frameSize);
 
	if (!fragmentProgram) 
	{
		qDebug() << "No shader program found - format not supported.";
		return false;
	} 
	else 
	if (!m_program.addShaderFromSourceCode(QGLShader::Vertex, qt_glsl_vertexShaderProgram)) 
	{
		qWarning("GLWidget: Vertex shader compile error %s",
			qPrintable(m_program.log()));
		//error = QAbstractVideoSurface::ResourceError;
		
	} 
	else 
	if (!m_program.addShaderFromSourceCode(QGLShader::Fragment, fragmentProgram)) 
	{
		qWarning("GLWidget: Shader compile error %s", qPrintable(m_program.log()));
		//error = QAbstractVideoSurface::ResourceError;
		m_program.removeAllShaders();
	} 
	else 
	if(!m_program.link()) 
	{
		qWarning("GLWidget: Shader link error %s", qPrintable(m_program.log()));
		m_program.removeAllShaders();
	} 
	else 
	{
		//m_handleType = format.handleType();
		m_scanLineDirection = QVideoSurfaceFormat::TopToBottom; //format.scanLineDirection();
		m_frameSize = format.frameSize;
	
		//if (m_handleType == QAbstractVideoBuffer::NoHandle)
			glGenTextures(m_textureCount, m_textureIds);
	}
	
	return true;
}

const char * GLVideoDrawable::resizeTextures(const QSize& frameSize)
{
	const char * fragmentProgram = 0;
	
	qDebug() << "GLVideoDrawable::resizeTextures(): frameSize: "<<frameSize<<", format: "<<m_videoFormat.pixelFormat;

	switch (m_videoFormat.pixelFormat) 
	{
	case QVideoFrame::Format_RGB32:
		initRgbTextureInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, frameSize);
		fragmentProgram = qt_glsl_xrgbShaderProgram;
		break;
        case QVideoFrame::Format_ARGB32:
		initRgbTextureInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, frameSize);
		fragmentProgram = qt_glsl_argbShaderProgram;
		break;
#ifndef QT_OPENGL_ES
        case QVideoFrame::Format_RGB24:
		initRgbTextureInfo(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, frameSize);
		fragmentProgram = qt_glsl_rgbShaderProgram;
		break;
#endif
	case QVideoFrame::Format_RGB565:
		initRgbTextureInfo(GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, frameSize);
		fragmentProgram = qt_glsl_rgbShaderProgram;
		break;
	case QVideoFrame::Format_YV12:
		initYv12TextureInfo(frameSize);
		fragmentProgram = qt_glsl_yuvPlanarShaderProgram;
		break;
	case QVideoFrame::Format_YUV420P:
		initYuv420PTextureInfo(frameSize);
		fragmentProgram = qt_glsl_yuvPlanarShaderProgram;
		break;
	default:
		break;
	}
	
	return fragmentProgram;
}

void GLVideoDrawable::viewportResized(const QSize& newSize)
{
	// recalc rects here
	setRect(QRectF(0,0,newSize.width(),newSize.height()));
	
	updateRects();
}

void GLVideoDrawable::updateRects()
{
	m_sourceRect = m_frame.rect;
// 	m_origSourceRect = m_sourceRect;
// 
// 	m_sourceRect.adjust(m_adjustDx1,m_adjustDy1,m_adjustDx2,m_adjustDy2);

	QSize nativeSize = m_frame.size;

	if (nativeSize.isEmpty())
	{
		m_targetRect = QRectF();
	}
	else
	if (m_aspectRatioMode == Qt::IgnoreAspectRatio)
	{
		m_targetRect = rect();
	}
	else
	if (m_aspectRatioMode == Qt::KeepAspectRatio)
	{
		QSizeF size = nativeSize;
		size.scale(rect().size(), Qt::KeepAspectRatio);

		m_targetRect = QRectF(0, 0, size.width(), size.height());
		m_targetRect.moveCenter(rect().center());
	}
	else
	if (m_aspectRatioMode == Qt::KeepAspectRatioByExpanding)
	{
		m_targetRect = rect();

		QSizeF size = rect().size();
		size.scale(nativeSize, Qt::KeepAspectRatio);

		m_sourceRect = QRectF(QPointF(0,0),size);
		m_sourceRect.moveCenter(QPointF(size.width() / 2, size.height() / 2));
	}
}


void GLVideoDrawable::paintGL()
{
	if (m_colorsDirty) 
	{
		//qDebug() << "Updating color matrix";
		updateColors(m_brightness, m_contrast, m_hue, m_saturation);
		m_colorsDirty = false;
        }

	
	
	//m_frame.isValid() && 
	if(m_frame.isRaw)
	{
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
				m_frame.bufferType == VideoFrame::BUFFER_POINTER ? m_frame.data[i] :
					(uint8_t*)m_frame.byteArray.constData() + m_textureOffsets[i]
			);
				 
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
	}
	else
	{
		//qDebug() << "normal";
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
				(!m_frame.image.isNull() ? m_frame.image.scanLine(0) : m_sampleTexture.scanLine(0)) + m_textureOffsets[i]
				);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
	}

	
	//m_frame.unmap();
	
	QRectF source = m_sourceRect; //m_sampleTexture.rect();
	QRectF target = m_targetRect; //rect();
	
	//qDebug() << "source:"<<source<<", target:"<<target;
	
	
	const int width = QGLContext::currentContext()->device()->width();
	const int height = QGLContext::currentContext()->device()->height();

	//QPainter painter(this);
	QTransform transform; // = painter.deviceTransform();
	//transform = transform.scale(1.25,1.);

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
	
	
	//qDebug() << vTop << vBottom;
	
	bool flipHorizontal = true;
	
	const GLfloat txLeft   = flipHorizontal ? source.right()  / m_frameSize.width() : source.left()  / m_frameSize.width();
	const GLfloat txRight  = flipHorizontal ? source.left()   / m_frameSize.width() : source.right() / m_frameSize.width();
	
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
	
	
	
	//renderText(10, 10, qPrintable(QString("%1 fps").arg(framesPerSecond)));
	
	if(!m_frame.captureTime.isNull())
	{
		int msecLatency = m_frame.captureTime.msecsTo(QTime::currentTime());
		
		m_latencyAccum += msecLatency;
	}
					
	if (!(m_frameCount % 100)) 
	{
		QString framesPerSecond;
		framesPerSecond.setNum(m_frameCount /(m_time.elapsed() / 1000.0), 'f', 2);
		
		QString latencyPerFrame;
		latencyPerFrame.setNum((((double)m_latencyAccum) / ((double)m_frameCount)), 'f', 3);
		
		qDebug() << "FPS: "<<framesPerSecond<<", Latency: "<<latencyPerFrame<<" ms";

		m_time.start();
		m_frameCount = 0;
		m_latencyAccum = 0;
		
		//lastFrameTime = time.elapsed();
	}
	
	m_frameCount ++;
}


class GLBugDrawable : public GLDrawable
{
public:
	GLBugDrawable(QObject *parent=0);
	
protected:
	void initGL();
	void paintGL();
	void viewportResized(const QSize& /*newSize*/);
	void updateRects();
	
private:
	GLuint m_textureId;
	QRectF m_targetRect;
	QImage m_image;
};

GLBugDrawable::GLBugDrawable(QObject *parent)
	: GLDrawable(parent)
{
	//m_image.load("/opt/qtsdk-2010.02/qt/examples/opengl/pbuffers/cubelogo.png");
	m_image = QImage("../qq06-glthread/me2.jpg");
}

void GLBugDrawable::initGL()
{
	if(!m_glw)
		return;
	qDebug() << "GLBugDrawable::initGL()";
	m_textureId = m_glw->bindTexture(m_image);
// 	QImage texture = m_glw->convertToGLFormat(m_image);
// 	glGenTextures( 1, &m_textureId );
// 	glBindTexture( GL_TEXTURE_2D, m_textureId );
// 	glTexImage2D( GL_TEXTURE_2D, 0, 3, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits() );
// 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
// 	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	updateRects();
}

void GLBugDrawable::viewportResized(const QSize& /*newSize*/)
{
	updateRects();
	updateGL();
}

void GLBugDrawable::updateRects()
{
// 	QRect viewport = m_glw->rect();
// 	m_targetRect = QRectF(  viewport.width() - m_image.width() - 10, 
// 				viewport.height() - m_image.height() - 10,
// 				m_image.width(),
// 				m_image.height());
// 				
// 	qDebug() << "GLBugDrawable::updateRects(): new target: "<<m_targetRect;

}

void GLBugDrawable::paintGL()
{
// 	glLoadIdentity();									// Reset The View
	
// 	glMatrixMode(GL_PROJECTION);
// 	glLoadIdentity();
// 	glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
// 	glMatrixMode(GL_MODELVIEW);
// 	
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
// 	glLoadIdentity();									// Reset The View
// 	glTranslatef(0.0f,0.0f,-5.0f);
	
	m_glw->drawTexture(QPointF(0,0),m_textureId);
// 	glBindTexture(GL_TEXTURE_2D, m_textureId);
// 	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
// 	//qDebug() << "Texture rect: "<<m_targetRect;
// 	glEnable(GL_TEXTURE_2D);
// 		
// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
// 	glLoadIdentity();									// Reset The View
// 	glTranslatef(0.0f,0.0f,-5.0f);
// 	
// 	glBindTexture(GL_TEXTURE_2D, m_textureId);
// 	
// 		
// 	glBegin(GL_QUADS);
// 		// Front Face
// 		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  m_targetRect.left(),  m_targetRect.top());
// 		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  m_targetRect.right(),  m_targetRect.top());
// 		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  m_targetRect.right(),  m_targetRect.bottom());
// 		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  m_targetRect.left(),  m_targetRect.bottom());
// // 		// Back Face
// // 		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
// // 		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
// // 		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
// // 		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
// // 		// Top Face
// // 		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
// // 		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
// // 		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
// // 		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
// // 		// Bottom Face
// // 		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
// // 		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
// // 		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
// // 		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
// // 		// Right face
// // 		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
// // 		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
// // 		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
// // 		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
// // 		// Left Face
// // 		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
// // 		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
// // 		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
// // 		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
// 	glEnd();

}


GLWidget::GLWidget(QWidget *parent, QGLWidget *shareWidget)
	: QGLWidget(parent, shareWidget)
	, m_glInited(false)
{
	GLVideoDrawable *video = new GLVideoDrawable(this);
	addDrawable(video);
	
// 	GLBugDrawable *bug = new GLBugDrawable();
// 	bug->setZIndex(1);
// 	
// 	addDrawable(bug);
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
	return QSize(200, 200);
}


void GLWidget::initializeGL()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	m_glInited = true;
	foreach(GLDrawable *drawable, m_drawables)
		drawable->initGL();
	
	
}

// NOTE: on x86 machines with glibc 2.3.6 and g++ 3.4.4, it looks
	// like std::copy is faster than memcpy, so we use that to do the
	// copying here:

#ifndef fast_memcpy
#  if 1
#    define fast_memcpy(dst,src,n) std::copy((src),(src)+(n),(dst))
#  else
#    define fast_memcpy(dst,src,n) memcpy((dst),(src),(n))
#  endif
#endif

void GLWidget::paintGL()
{
	qglClearColor(Qt::black);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	foreach(GLDrawable *drawable, m_drawables)
		drawable->paintGL();
}
	
void GLWidget::addDrawable(GLDrawable *item)
{
	item->setGLWidget(this);
	m_drawables << item;
	connect(item, SIGNAL(zIndexChanged(double)), this, SLOT(zIndexChanged()));
	if(m_glInited)
		item->initGL();
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
// 	int side = qMin(width, height);
	glViewport(0,0,width,height); //(width - side) / 2, (height - side) / 2, side, side);
	//glViewport((width - side) / 2, (height - side) / 2, side, side);
	
	QSize size(width,height);
	
// 	qDebug() << "GLWidget::resizeGL(): size:"<<size;
	
	foreach(GLDrawable *drawable, m_drawables)
		drawable->viewportResized(size);

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
