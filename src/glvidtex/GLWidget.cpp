#include <QtGui>
#include <QtOpenGL>
#include <QVideoFrame>
#include <QAbstractVideoSurface>
#include "GLWidget.h"
#include "../livemix/VideoSource.h"

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
        "uniform mediump float alpha;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec4 color = vec4(texture2D(texRgb, textureCoord.st).bgr, 1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, alpha);\n"
        "}\n";

// Paints an ARGB frame.
static const char *qt_glsl_argbShaderProgram =
        "uniform sampler2D texRgb;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "uniform mediump float alpha;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec4 color = vec4(texture2D(texRgb, textureCoord.st).bgr, 1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, texture2D(texRgb, textureCoord.st).a * alpha);\n"
        "}\n";


// Paints an RGB(A) frame.
static const char *qt_glsl_rgbShaderProgram =
        "uniform sampler2D texRgb;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "uniform mediump float alpha;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec4 color = vec4(texture2D(texRgb, textureCoord.st).rgb, 1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, texture2D(texRgb, textureCoord.st).a * alpha);\n"
        "}\n";

// Paints a YUV420P or YV12 frame.
static const char *qt_glsl_yuvPlanarShaderProgram =
        "uniform sampler2D texY;\n"
        "uniform sampler2D texU;\n"
        "uniform sampler2D texV;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "uniform mediump float alpha;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    highp vec4 color = vec4(\n"
        "           texture2D(texY, textureCoord.st).r,\n"
        "           texture2D(texU, textureCoord.st).r,\n"
        "           texture2D(texV, textureCoord.st).r,\n"
        "           1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, alpha);\n"
        "}\n";



// class GLDrawable 
GLDrawable::GLDrawable(QObject *parent)
	: QObject(parent)
	, m_glw(0)
	, m_zIndex(0)
	, m_opacity(1)
	, m_isVisible(false)
{
}

void GLDrawable::updateGL()
{
	if(m_glw)
		m_glw->updateGL();
}
	
void GLDrawable::show()
{
	setVisible(true);
}

void GLDrawable::hide()
{
	setVisible(false);
}

void GLDrawable::setVisible(bool flag)
{
	m_animDirection = flag;
	startAnimations();
}

void GLDrawable::startAnimations()
{
	if(m_animDirection)
		m_isVisible = m_animDirection;
		
	foreach(GLDrawable::AnimParam p, m_animations)
		if(( m_animDirection && p.cond == GLDrawable::OnShow) ||
		   (!m_animDirection && p.cond == GLDrawable::OnHide))
			startAnimation(p);
}

GLDrawable::AnimParam & GLDrawable::addShowAnimation(AnimationType value, int length)
{
	GLDrawable::AnimParam p;
	p.cond = GLDrawable::OnShow;
	p.type = value;
	p.startDelay = 0;
	p.length = length;
	p.curve = value == GLDrawable::AnimFade ? QEasingCurve::Linear : QEasingCurve::OutCubic;
	m_animations << p;
	
	return m_animations.last();
}

GLDrawable::AnimParam & GLDrawable::addHideAnimation(AnimationType value, int length)
{
	GLDrawable::AnimParam p;
	p.cond = GLDrawable::OnHide;
	p.type = value;
	p.startDelay = 0;
	p.length = length;
	p.curve = value == GLDrawable::AnimFade ? QEasingCurve::Linear : QEasingCurve::InCubic;
	m_animations << p;
	
	return m_animations.last();
}

void GLDrawable::removeAnimation(GLDrawable::AnimParam p)
{
	m_animations.removeAll(p);
}


bool operator==(const GLDrawable::AnimParam&a, const GLDrawable::AnimParam&b)
{
	return a.cond == b.cond &&
		a.type == b.type &&
		a.startDelay == b.startDelay &&
		a.length == b.length &&
		a.curve == b.curve;
};

void GLDrawable::startAnimation(const GLDrawable::AnimParam& p)
{
	QRectF viewport = m_glw ? m_glw->viewport() : QRectF(0,0,1000,750);
	bool inFlag = m_animDirection;
	
	QAutoDelPropertyAnimation *ani = 0;
	
	switch(p.type)
	{
		case GLDrawable::AnimFade:
			ani = new QAutoDelPropertyAnimation(this, "opacity");
			ani->setStartValue(inFlag ? 0.0 : 1.0);
			ani->setEndValue(inFlag ?   1.0 : 0.0);
			break;
		
		case GLDrawable::AnimZoom:
			ani = setupRectAnimation(m_rect.adjusted(-viewport.width(),-viewport.height(),viewport.width(),viewport.height()),inFlag);
			break;
		case GLDrawable::AnimSlideTop:
			ani = setupRectAnimation(m_rect.adjusted(0,-viewport.height(),0,-viewport.height()),inFlag);
			break;
		case GLDrawable::AnimSlideBottom:
			ani = setupRectAnimation(m_rect.adjusted(0,viewport.height(),0,viewport.height()),inFlag);
			break;
		case GLDrawable::AnimSlideLeft:
			ani = setupRectAnimation(m_rect.adjusted(-viewport.width(),0,-viewport.width(),0),inFlag);
			break;
		case GLDrawable::AnimSlideRight:
			ani = setupRectAnimation(m_rect.adjusted(viewport.width(),0,viewport.width(),0),inFlag);
			break;	
		
		default:
			break;
	}
	
	if(ani)
	{
		ani->setEasingCurve(p.curve); //inFlag ? QEasingCurve::OutCubic : QEasingCurve::InCubic);
		ani->setDuration(p.length);
		
// 		//qDebug() << "GLDrawable::startAnimation: type:"<<p.type<<", length:"<<p.length<<", curve:"<<p.curve.type();
		
		connect(ani, SIGNAL(finished()), this, SLOT(animationFinished()));
		
		if(p.startDelay > 0)
		{
			QTimer *timer = new QTimer();
			timer->setInterval(p.startDelay);
			
			connect(timer, SIGNAL(timeout()), ani, SLOT(startAutoDel()));
		}
		else
		{
			ani->startAutoDel();
		}
	}
}


QAutoDelPropertyAnimation * GLDrawable::setupRectAnimation(const QRectF& otherRect, bool inFlag)
{
	QAutoDelPropertyAnimation *ani = new QAutoDelPropertyAnimation(this, "rect");
	
	ani->setEndValue(inFlag   ? m_rect : otherRect);
	ani->setStartValue(inFlag ? otherRect : m_rect);
	
	//qDebug() << "GLDrawable::startRectAnimation: start:"<<(inFlag ? otherRect : m_rect)<<", end:"<<(inFlag   ? m_rect : otherRect)<<", other:"<<otherRect<<", duration:"<<duration<<", inFlag:"<<inFlag;
	return ani;
}

void GLDrawable::animationFinished()
{
	m_isVisible = m_animDirection;
	updateGL();
}
	
void GLDrawable::setRect(const QRectF& rect)
{
	m_rect = rect;
	//qDebug() << "GLDrawable::setRect: "<<rect;
	drawableResized(rect.size());
	emit drawableResized(rect.size());
	updateGL();
}
	
void GLDrawable::setZIndex(double z)
{
	m_zIndex = z;
	emit zIndexChanged(z);
}

void GLDrawable::setOpacity(double o)
{
	//qDebug() << "GLDrawable::setOpacity: "<<o;
	m_opacity = o;
	updateGL();
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



QByteArray GLDrawable::AnimParam::toByteArray()
{
	QByteArray array;
	QDataStream b(&array, QIODevice::WriteOnly);

	b << QVariant(cond);
	b << QVariant(type);
	b << QVariant(startDelay);
	b << QVariant(length);
	b << QVariant(curve.type());
	
	return array;
}

void GLDrawable::AnimParam::fromByteArray(QByteArray array)
{
	QDataStream b(&array, QIODevice::ReadOnly);
	QVariant x;

	b >> x; cond = (GLDrawable::AnimCondition)x.toInt();
	b >> x; type = (GLDrawable::AnimationType)x.toInt();
	b >> x; startDelay = x.toInt();
	b >> x; length = x.toInt();
	b >> x; int curveType = x.toInt();
	curve.setType((QEasingCurve::Type)curveType);
}


QByteArray VideoDisplayOptions::toByteArray()
{
	QByteArray array;
	QDataStream b(&array, QIODevice::WriteOnly);

	b << QVariant(flipHorizontal);
	b << QVariant(flipVertical);
	b << QVariant(cropTopLeft);
	b << QVariant(cropBottomRight);
	b << QVariant(brightness);
	b << QVariant(contrast);
	b << QVariant(hue);
	b << QVariant(saturation);
	
	return array;
}


void VideoDisplayOptions::fromByteArray(QByteArray array)
{
	QDataStream b(&array, QIODevice::ReadOnly);
	QVariant x;

	b >> x; flipHorizontal = x.toBool();
	b >> x; flipVertical = x.toBool();
	b >> x; cropTopLeft = x.toPointF();
	b >> x; cropBottomRight = x.toPointF();
	b >> x; brightness = x.toInt();
	b >> x; contrast = x.toInt();
	b >> x; hue = x.toInt();
	b >> x; saturation = x.toInt();
}

QDebug operator<<(QDebug dbg, const VideoDisplayOptions &opts)
{
	dbg.nospace() << "VideoDisplayOptions("<<opts.flipHorizontal<<","<<opts.flipVertical<<","<<opts.cropTopLeft<<","<<opts.cropBottomRight<<","<<opts.brightness<<","<<opts.contrast<<","<<opts.hue<<","<<opts.saturation<<")";
	
	return dbg.space();
}


GLVideoDrawable::GLVideoDrawable(QObject *parent)
	: GLDrawable(parent)
	, m_glInited(false)
	, m_colorsDirty(true)
	, m_source(0)
	, m_frameCount(0)
	, m_latencyAccum(0)
	, m_aspectRatioMode(Qt::KeepAspectRatio)
	, m_validShader(false)
{
	
	m_imagePixelFormats
		<< QVideoFrame::Format_RGB32
		<< QVideoFrame::Format_ARGB32
		<< QVideoFrame::Format_RGB24
		<< QVideoFrame::Format_RGB565
		<< QVideoFrame::Format_YV12
		<< QVideoFrame::Format_YUV420P;
}

GLVideoDrawable::~GLVideoDrawable()
{
	setVideoSource(0);
}

void GLVideoDrawable::setVideoSource(VideoSource *source)
{
	if(m_source == source)
		return;
		
	if(m_source)
		disconnectVideoSource();
	
	m_source = source;
	if(m_source)
	{	
		connect(m_source, SIGNAL(frameReady()), this, SLOT(frameReady()));
		connect(m_source, SIGNAL(destroyed()), this, SLOT(disconnectVideoSource()));
		
		setVideoFormat(m_source->videoFormat());
		
		frameReady();
	}

}

void GLVideoDrawable::disconnectVideoSource()
{
	if(!m_source)
		return;
	disconnect(m_source, 0, this, 0);
	m_source = 0;
}
	

void GLVideoDrawable::frameReady()
{
	if(!m_source)
		return;
		
	m_frame = m_source->frame();
	
	if(m_frame.rect != m_sourceRect)
	{
		//qDebug() << "GLVideoDrawable::frameReady(): \t m_frame.rect:"<<m_frame.rect<<", m_sourceRect:"<<m_sourceRect;
		if(m_videoFormat.pixelFormat != m_source->videoFormat().pixelFormat)
			setVideoFormat(m_source->videoFormat());
		resizeTextures(m_frame.size);
		updateRects();
	}
		
	if(m_glInited)
	{
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
			for (int i = 0; i < m_textureCount; ++i) 
			{
				//qDebug() << "normal: "<<i<<m_textureWidths[i]<<m_textureHeights[i];
			
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
	}
	
	updateGL();
}

/*!
*/
int GLVideoDrawable::brightness() const
{
	return m_displayOpts.brightness;
}

/*!
*/
void GLVideoDrawable::setBrightness(int brightness)
{
	m_displayOpts.brightness = brightness;
	
	m_colorsDirty = true;
}

/*!
*/
int GLVideoDrawable::contrast() const
{
	return m_displayOpts.contrast;
}

/*!
*/
void GLVideoDrawable::setContrast(int contrast)
{
	m_displayOpts.contrast = contrast;
	
	m_colorsDirty = true;
}

/*!
*/
int GLVideoDrawable::hue() const
{
	return m_displayOpts.hue;
}

/*!
*/
void GLVideoDrawable::setHue(int hue)
{
	m_displayOpts.hue = hue;
	
	m_colorsDirty = true;
}

/*!
*/
int GLVideoDrawable::saturation() const
{
	return m_displayOpts.saturation;
}

/*!
*/
void GLVideoDrawable::setSaturation(int saturation)
{
	m_displayOpts.saturation = saturation;
	
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
	bool samePixelFormat = false; //format.pixelFormat == m_videoFormat.pixelFormat;
	m_videoFormat = format;
//m_sampleTexture.load("/opt/qtsdk-2010.02/qt/examples/opengl/pbuffers/cubelogo.png"); 
	//m_sampleTexture = m_sampleTexture.scaled(640,480);
	m_sampleTexture = QImage( format.frameSize, QImage::Format_RGB32 );
	m_sampleTexture.fill( Qt::red );
	m_sampleTexture = m_sampleTexture.convertToFormat(QImage::Format_ARGB32);
	
	
	//qDebug() << "GLVideoDrawable::setVideoFormat(): \t frameSize:"<<format.frameSize<<", pixelFormat:"<<format.pixelFormat;
	
	
	if(!m_glInited)
		return m_imagePixelFormats.contains(format.pixelFormat);
		
	//qDebug() << "GLVideoDrawable::setVideoFormat(): \t Initalizing vertex and pixel shaders...";
	
	
	#define PROGRAM_VERTEX_ATTRIBUTE 0
	#define PROGRAM_TEXCOORD_ATTRIBUTE 1
	
	//QVideoSurfaceFormat format(m_sampleTexture.size(), QVideoFrame::Format_ARGB32);
	//QVideoSurfaceFormat format(m_sampleTexture.size(), QVideoFrame::Format_YUV420P);
	
	m_validShader = false;
	const char *fragmentProgram = resizeTextures(format.frameSize);
 
 	if(!samePixelFormat)
 	{
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
			return false;
			
		} 
		else 
		if (!m_program.addShaderFromSourceCode(QGLShader::Fragment, fragmentProgram)) 
		{
			qWarning("GLWidget: Shader compile error %s", qPrintable(m_program.log()));
			//error = QAbstractVideoSurface::ResourceError;
			m_program.removeAllShaders();
			return false;
		} 
		else 
		if(!m_program.link()) 
		{
			qWarning("GLWidget: Shader link error %s", qPrintable(m_program.log()));
			m_program.removeAllShaders();
			return false;
		} 
		else 
		{
			//m_handleType = format.handleType();
			m_scanLineDirection = QVideoSurfaceFormat::TopToBottom; //format.scanLineDirection();
			m_frameSize = format.frameSize;
		
			//if (m_handleType == QAbstractVideoBuffer::NoHandle)
				glGenTextures(m_textureCount, m_textureIds);
		}
		
		//qDebug() << "GLVideoDrawable::setVideoFormat(): \t Initalized"<<m_textureCount<<"textures";
	}
	m_validShader = true;
	return true;
}

const char * GLVideoDrawable::resizeTextures(const QSize& frameSize)
{
	const char * fragmentProgram = 0;
	
	qDebug() << "GLVideoDrawable::resizeTextures(): \t frameSize: "<<frameSize<<", format: "<<m_videoFormat.pixelFormat;
	m_frameSize = frameSize;

	switch (m_videoFormat.pixelFormat) 
	{
	case QVideoFrame::Format_RGB32:
		initRgbTextureInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, frameSize);
		fragmentProgram = qt_glsl_xrgbShaderProgram;
		break;
        case QVideoFrame::Format_ARGB32:
        	//qDebug() << "GLVideoDrawable::resizeTextures(): \t Format ARGB, using qt_glsl_argbShaderProgram";
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

void GLVideoDrawable::viewportResized(const QSize& /*newSize*/)
{
	// recalc rects here
	//setRect(QRectF(0,0,newSize.width(),newSize.height()));
	
	updateRects();
}

void GLVideoDrawable::drawableResized(const QSizeF& /*newSize*/)
{
	updateRects();
}


void GLVideoDrawable::updateRects()
{
	m_sourceRect = m_frame.rect;
	
	QRectF adjustedSource = m_sourceRect.adjusted(
		m_displayOpts.cropTopLeft.x(),
		m_displayOpts.cropTopLeft.y(),
		m_displayOpts.cropBottomRight.x(),
		m_displayOpts.cropBottomRight.y());
// 	m_origSourceRect = m_sourceRect;
// 
// 	m_sourceRect.adjust(m_adjustDx1,m_adjustDy1,m_adjustDx2,m_adjustDy2);

	QSizeF nativeSize = adjustedSource.size(); //m_frame.size;

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
	
	//qDebug() << "GLVideoDrawable::updateRects(): \t m_sourceRect:"<<m_sourceRect<<", m_targetRect:"<<m_targetRect;
}

// float opacity = 0.5;
// 	glColor4f(opacity,opacity,opacity,opacity);

void GLVideoDrawable::setDisplayOptions(const VideoDisplayOptions& opts)
{
	m_displayOpts = opts;
	m_colorsDirty = true;
	emit displayOptionsChanged(opts);
}
	
void GLVideoDrawable::paintGL()
{
	if(!m_validShader)
	{
		return;
	}
	
	if (m_colorsDirty) 
	{
		//qDebug() << "Updating color matrix";
		updateColors(m_displayOpts.brightness, m_displayOpts.contrast, m_displayOpts.hue, m_displayOpts.saturation);
		m_colorsDirty = false;
        }

	
	//m_frame.unmap();
	
	QRectF source = m_sourceRect; //m_sampleTexture.rect();
	QRectF target = m_targetRect; //rect();
	
	source = source.adjusted(
		m_displayOpts.cropTopLeft.x(),
		m_displayOpts.cropTopLeft.y(),
		m_displayOpts.cropBottomRight.x(),
		m_displayOpts.cropBottomRight.y());
	
	//qDebug() << "source:"<<source<<", target:"<<target;
	
	
	const int width  = QGLContext::currentContext()->device()->width();
	const int height = QGLContext::currentContext()->device()->height();

	//QPainter painter(this);
	QTransform transform =  m_glw->transform(); //= painter.deviceTransform();
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
	

	const GLfloat vertexCoordArray[] =
	{
		target.left()     , target.bottom() + 1,(GLfloat)zIndex(),
		target.right() + 1, target.bottom() + 1,(GLfloat)zIndex(),
		target.left()     , target.top(), 	(GLfloat)zIndex(),
		target.right() + 1, target.top(), 	(GLfloat)zIndex()
	};
	
	
	//qDebug() << vTop << vBottom;
	
	const GLfloat txLeft   = m_displayOpts.flipHorizontal ? source.right()  / m_frameSize.width() : source.left()  / m_frameSize.width();
	const GLfloat txRight  = m_displayOpts.flipHorizontal ? source.left()   / m_frameSize.width() : source.right() / m_frameSize.width();
	
	const GLfloat txTop    = !m_displayOpts.flipVertical //m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
		? source.top()    / m_frameSize.height()
		: source.bottom() / m_frameSize.height();
	const GLfloat txBottom = !m_displayOpts.flipVertical //m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
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
	
	m_program.setAttributeArray("vertexCoordArray",  vertexCoordArray,  3);
	m_program.setAttributeArray("textureCoordArray", textureCoordArray, 2);
	
	m_program.setUniformValue("positionMatrix",      positionMatrix);
	
	m_program.setUniformValue("alpha",               (GLfloat)opacity());

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
		
		
		qDebug() << "FPS: " + framesPerSecond + (m_frame.captureTime.isNull() ? "" : ", Latency: " + latencyPerFrame + " ms");

		m_time.start();
		m_frameCount = 0;
		m_latencyAccum = 0;
		
		//lastFrameTime = time.elapsed();
	}
	
	m_frameCount ++;
}



VideoDisplayOptionWidget::VideoDisplayOptionWidget(GLVideoDrawable *drawable, QWidget *parent)
	: QWidget(parent)
	, m_drawable(drawable)
{
	if(drawable)
	{
		m_opts = drawable->displayOptions();
		connect(this, SIGNAL(displayOptionsChanged(const VideoDisplayOptions&)), drawable, SLOT(setDisplayOptions(const VideoDisplayOptions&)));
		connect(drawable, SIGNAL(displayOptionsChanged(const VideoDisplayOptions&)), this, SLOT(setDisplayOptions(const VideoDisplayOptions&)));
	}
	
	initUI();
}

VideoDisplayOptionWidget::VideoDisplayOptionWidget(const VideoDisplayOptions& opts, QWidget *parent)
	: QWidget(parent)
{
	m_opts = opts;
	initUI();
}
	
	
void VideoDisplayOptionWidget::initUI()
{
	setWindowTitle("Video Display Options");
	
	QGridLayout *layout = new QGridLayout(this);
	m_optsOriginal = m_opts;
	
	int row = 0;
	
	QCheckBox *cb = 0;
	cb = new QCheckBox("Flip Horizontal");
	cb->setChecked(m_opts.flipHorizontal);
	connect(cb, SIGNAL(toggled(bool)), this, SLOT(flipHChanged(bool)));
	layout->addWidget(cb,row,1);
	m_cbFlipH = cb;
	
	row++;
	cb = new QCheckBox("Flip Vertical");
	cb->setChecked(m_opts.flipVertical);
	connect(cb, SIGNAL(toggled(bool)), this, SLOT(flipVChanged(bool)));
	layout->addWidget(cb,row,1);
	m_cbFlipV = cb;
	
	row++;
	QSpinBox *spinBox = 0;
	QHBoxLayout *rowLayout = new QHBoxLayout();
	layout->addWidget(new QLabel("Crop Left By:"),row,0);
	spinBox = new QSpinBox;
	spinBox->setSuffix(" px");
	spinBox->setMinimum(-1000);
	spinBox->setMaximum(1000);
	spinBox->setValue((int)m_opts.cropTopLeft.x());
	connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(cropX1Changed(int)));
	rowLayout->addWidget(spinBox);
	rowLayout->addStretch(1);
	layout->addLayout(rowLayout,row,1);
	m_spinCropX1 = spinBox;
	
	row++;
	rowLayout = new QHBoxLayout();
	layout->addWidget(new QLabel("Crop Right By:"),row,0);
	spinBox = new QSpinBox;
	spinBox->setSuffix(" px");
	spinBox->setMinimum(-1000);
	spinBox->setMaximum(1000);
	spinBox->setValue((int)m_opts.cropBottomRight.x());
	connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(cropX2Changed(int)));
	rowLayout->addWidget(spinBox);
	rowLayout->addStretch(1);
	layout->addLayout(rowLayout,row,1);
	m_spinCropX2 = spinBox;
	
	row++;
	rowLayout = new QHBoxLayout();
	layout->addWidget(new QLabel("Crop Top By:"),row,0);
	spinBox = new QSpinBox;
	spinBox->setSuffix(" px");
	spinBox->setMinimum(-1000);
	spinBox->setMaximum(1000);
	spinBox->setValue((int)m_opts.cropTopLeft.y());
	connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(cropY1Changed(int)));
	rowLayout->addWidget(spinBox);
	rowLayout->addStretch(1);
	layout->addLayout(rowLayout,row,1);
	m_spinCropY1 = spinBox;
	
	row++;
	rowLayout = new QHBoxLayout();
	layout->addWidget(new QLabel("Crop Bottom By:"),row,0);
	spinBox = new QSpinBox;
	spinBox->setSuffix(" px");
	spinBox->setMinimum(-1000);
	spinBox->setMaximum(1000);
	spinBox->setValue((int)m_opts.cropBottomRight.y());
	connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(cropY2Changed(int)));
	rowLayout->addWidget(spinBox);
	rowLayout->addStretch(1);
	layout->addLayout(rowLayout,row,1);
	m_spinCropY2 = spinBox;
	
	row++;
	QSlider *slider =0;
	rowLayout = new QHBoxLayout();
	// add label
	layout->addWidget(new QLabel("Brightness:"),row,0);
	// add slider
	slider = new QSlider;
	slider->setOrientation(Qt::Horizontal);
	slider->setMinimum(-100);
	slider->setMaximum(100);
	slider->setValue(m_opts.brightness);
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(bChanged(int)));
	rowLayout->addWidget(slider);
	// add spinBox
	spinBox = new QSpinBox;
	spinBox->setMinimum(-100);
	spinBox->setMaximum(100);
	spinBox->setValue(m_opts.brightness);
	connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
	connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
	rowLayout->addWidget(spinBox);
	// finish the row
	rowLayout->addStretch(1);
	layout->addLayout(rowLayout,row,1);
	m_spinB = spinBox;
	
	row++;
	rowLayout = new QHBoxLayout();
	layout->addWidget(new QLabel("Contrast:"),row,0);
	slider = new QSlider;
	slider->setOrientation(Qt::Horizontal);
	slider->setMinimum(-50);
	slider->setMaximum(50);
	slider->setValue(m_opts.contrast);
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(cChanged(int)));
	rowLayout->addWidget(slider);
	// add spinBox
	spinBox = new QSpinBox;
	spinBox->setMinimum(-50);
	spinBox->setMaximum(50);
	spinBox->setValue(m_opts.brightness);
	connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
	connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
	rowLayout->addWidget(spinBox);
	// finish the row
	rowLayout->addStretch(1);
	layout->addLayout(rowLayout,row,1);
	m_spinC = spinBox;
	
	row++;
	rowLayout = new QHBoxLayout();
	layout->addWidget(new QLabel("Hue:"),row,0);
	slider = new QSlider;
	slider->setOrientation(Qt::Horizontal);
	slider->setMinimum(-100);
	slider->setMaximum(100);
	slider->setValue(m_opts.hue);
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(hChanged(int)));
	rowLayout->addWidget(slider);
	// add spinBox
	spinBox = new QSpinBox;
	spinBox->setMinimum(-100);
	spinBox->setMaximum(100);
	spinBox->setValue(m_opts.brightness);
	connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
	connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
	rowLayout->addWidget(spinBox);
	// finish the row
	rowLayout->addStretch(1);
	layout->addLayout(rowLayout,row,1);
	m_spinH = spinBox;
	
	row++;
	rowLayout = new QHBoxLayout();
	layout->addWidget(new QLabel("Saturation:"),row,0);
	slider = new QSlider;
	slider->setOrientation(Qt::Horizontal);
	slider->setMinimum(-50);
	slider->setMaximum(50);
	slider->setValue(m_opts.saturation);
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sChanged(int)));
	rowLayout->addWidget(slider);
	// add spinBox
	spinBox = new QSpinBox;
	spinBox->setMinimum(-50);
	spinBox->setMaximum(50);
	spinBox->setValue(m_opts.brightness);
	connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
	connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
	rowLayout->addWidget(spinBox);
	// finish the row
	rowLayout->addStretch(1);
	layout->addLayout(rowLayout,row,1);
	m_spinS = spinBox;
	
	row++;
	QPushButton *resetButton = new QPushButton("Undo Changes");
	connect(resetButton, SIGNAL(clicked()), this, SLOT(undoChanges()));
	layout->addWidget(resetButton,row,1);
}

void VideoDisplayOptionWidget::undoChanges()
{
	setDisplayOptions(m_optsOriginal);
}

void VideoDisplayOptionWidget::setDisplayOptions(const VideoDisplayOptions& opts)
{
	m_opts = opts;
	m_cbFlipH->setChecked(opts.flipHorizontal);
	m_cbFlipV->setChecked(opts.flipVertical);
	m_spinCropX1->setValue((int)opts.cropTopLeft.x());
	m_spinCropY1->setValue((int)opts.cropTopLeft.y());
	m_spinCropX2->setValue((int)opts.cropBottomRight.x());
	m_spinCropY2->setValue((int)opts.cropBottomRight.y());
	m_spinB->setValue(opts.brightness);
	m_spinC->setValue(opts.contrast);
	m_spinH->setValue(opts.hue);
	m_spinS->setValue(opts.saturation);
	
}
	
void VideoDisplayOptionWidget::flipHChanged(bool value)
{
	m_opts.flipHorizontal = value;
	emit displayOptionsChanged(m_opts);
}

void VideoDisplayOptionWidget::flipVChanged(bool value)
{
	m_opts.flipVertical = value;
	emit displayOptionsChanged(m_opts);
}


void VideoDisplayOptionWidget::cropX1Changed(int value)
{
	m_opts.cropTopLeft.setX(value);
	emit displayOptionsChanged(m_opts);
}


void VideoDisplayOptionWidget::cropY1Changed(int value)
{
	m_opts.cropTopLeft.setY(value);
	emit displayOptionsChanged(m_opts);
}


void VideoDisplayOptionWidget::cropX2Changed(int value)
{
	m_opts.cropBottomRight.setX(value);
	emit displayOptionsChanged(m_opts);
}


void VideoDisplayOptionWidget::cropY2Changed(int value)
{
	m_opts.cropBottomRight.setY(value);
	emit displayOptionsChanged(m_opts);
}


void VideoDisplayOptionWidget::bChanged(int value)
{
	//qDebug() << "b changed: "<<value;
	m_opts.brightness = value;
	emit displayOptionsChanged(m_opts);
}


void VideoDisplayOptionWidget::cChanged(int value)
{
	m_opts.contrast = value;
	emit displayOptionsChanged(m_opts);
}


void VideoDisplayOptionWidget::hChanged(int value)
{
	m_opts.hue = value;
	emit displayOptionsChanged(m_opts);
}


void VideoDisplayOptionWidget::sChanged(int value)
{
	m_opts.saturation = value;
	emit displayOptionsChanged(m_opts);
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



StaticVideoSource::StaticVideoSource(QObject *parent)
	: VideoSource(parent)
{
	setImage(QImage());
}

void StaticVideoSource::setImage(const QImage& img)
{
	m_image = img.convertToFormat(QImage::Format_ARGB32);
	m_frame = VideoFrame(m_image,1000/30);
	enqueue(m_frame);
}

void StaticVideoSource::run()
{
	while(!m_killed)
	{
		//qDebug() << "Frame ready";
		enqueue(m_frame);
		emit frameReady();
		msleep(m_frame.holdTime);
	}
}



GLWidget::GLWidget(QWidget *parent, QGLWidget *shareWidget)
	: QGLWidget(parent, shareWidget)
	, m_glInited(false)
{
	setViewport(QRectF(0,0,1000.,750.));
	
	
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
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
// 	glClearDepth(1.0f);						// Depth Buffer Setup
// 	glEnable(GL_DEPTH_TEST);					// Enables Depth Testing
// 	glDepthFunc(GL_LEQUAL);						// The Type Of Depth Testing To Do
// 	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		// Really Nice Perspective Calculations
	
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
	//glViewport(0,0,width,height); //(width - side) / 2, (height - side) / 2, side, side);
	glViewport(0,0,width,height); //(width - side) / 2, (height - side) / 2, side, side);
	
	setViewport(viewport());
}
	
void GLWidget::setViewport(const QRectF& viewport)
{
	m_viewport = viewport;
	float sw = viewport.width();
	float sh = viewport.height();;
	
	float sx = ((float)width()) / sw;
	float sy = ((float)height()) / sh;

	float scale = qMin(sx,sy);
	float scaledWidth = sw * scale;
	float scaledHeight = sh * scale;
	float diffWidth = width() - scaledWidth;
	float diffHeight = height() - scaledHeight;
	//qDebug() << "scaledWH:"<<scaledWidth<<scaledHeight<<", diffWH:"<<diffWidth<<diffHeight;
	
	/// WHY?? The usual centering algorithm of 'divide by 2' doesn't seem to work - at least
	/// on my work machine. The math is correct, but the output renders too far to the right and top. 
	/// The 2.75 and 1.5 were found simply thru trial and error.
	float xt = diffWidth/2.75  + viewport.left();
	float yt = diffHeight/1.5  + viewport.top();
	//qDebug() << "GLWidget::resizeGL: width:"<<width<<",height:"<<height<<", scale:"<<scale<<", trans:"<<xt<<yt;
	
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
