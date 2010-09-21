#include "GLVideoDrawable.h"

#include "GLWidget.h"

#include <QVideoFrame>
#include <QAbstractVideoSurface>
#include "../livemix/VideoSource.h"

#include <QImageWriter>

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
#  define GL_TEXTURE3    0x84C3
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
        "uniform sampler2D alphaMask;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "uniform mediump float alpha;\n"
        "uniform mediump float texOffsetX;\n"
        "uniform mediump float texOffsetY;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    mediump vec2 texPoint = vec2(textureCoord.s + texOffsetX, textureCoord.t + texOffsetY);\n"
        "    highp vec4 color = vec4(texture2D(texRgb, texPoint).bgr, 1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, alpha * texture2D(alphaMask, textureCoord.st).a);\n"
        "}\n";

//
// Paints an ARGB frame.
static const char *qt_glsl_argbShaderProgram =
	"uniform sampler2D texRgb;\n"
	"uniform sampler2D alphaMask;\n"
	"uniform mediump mat4 colorMatrix;\n"
	"uniform mediump float alpha;\n"
	"uniform mediump float texOffsetX;\n"
        "uniform mediump float texOffsetY;\n"
	"varying highp vec2 textureCoord;\n"
	"void main(void)\n"
	"{\n"
	"    mediump vec2 texPoint = vec2(textureCoord.s + texOffsetX, textureCoord.t + texOffsetY);\n"
	"    highp vec4 color = vec4(texture2D(texRgb, texPoint).bgr, 1.0);\n"
	"    color = colorMatrix * color;\n"
	"    gl_FragColor = vec4(color.rgb, texture2D(texRgb, texPoint).a * alpha * texture2D(alphaMask, textureCoord.st).a);\n"
	"}\n";


//  	"void main(void)\n"
//  	"{\n"
// 		"vec4 result = 0.0;\n"
// 		"float stepU = 1.0 / 1024.0;\n"
// 		"float stepV = 1.0 / 818.0;\n"
// 		"vec2 texCoord;\n"
// 		"\n"
// 		"mat3 gaussianCoef = {\n"
// 		"	1.0,	2.0,	1.0,\n"
// 		"	2.0,	4.0,	2.0,\n"
// 		"	1.0,	2.0,	1.0};\n"
// 		"\n"
// 		"for(int i=0;i<3;i++) {\n"
// 		"	for(int j=0;j<3;j++) {\n"
// 		"		texCoord = vec2(textureCoord.s + (i-1)*stepU, textureCoord.t + (j-1)*stepV);\n"
// 		"		result += gaussianCoef[i][j] * vec4(texture2D(texRgb,texCoord).bgr, 1.0);\n" //, texture2D(texRgb,texCoord).a * alpha);\n"
// 		"	}\n"
// 		"}\n"
// 		"result /= 16.0;\n"
// 		"gl_FragColor = result;\n"
// 	"}\n";
	
// 	"void main(void)\n"
// 	"{\n"
// 	"    highp vec4 color = vec4(texture2D(texRgb, textureCoord.st).bgr, 1.0);\n"
// 	"    color = colorMatrix * color;\n"
// 	"    gl_FragColor = vec4(color.rgb, texture2D(texRgb, textureCoord.st).a * alpha);\n"
// 	"}\n";

// 	"void main(void)\n"
// 	"{\n"
// 	"    vec4 sample[9];\n"
// 	"    sample[0] = vec4(texture2D(texRgb, vec2(textureCoord.s - 1.0, textureCoord.t - 1.0)).bgr, 1.0);\n"
// 	"    sample[1] = vec4(texture2D(texRgb, vec2(textureCoord.s - 0.0, textureCoord.t - 1.0)).bgr, 1.0);\n"
// 	"    sample[2] = vec4(texture2D(texRgb, vec2(textureCoord.s + 1.0, textureCoord.t - 1.0)).bgr, 1.0);\n"
// 	"    sample[3] = vec4(texture2D(texRgb, vec2(textureCoord.s - 1.0, textureCoord.t - 0.0)).bgr, 1.0);\n"
// 	"    sample[4] = vec4(texture2D(texRgb, vec2(textureCoord.s - 0.0, textureCoord.t - 0.0)).bgr, 1.0);\n"
// 	"    sample[5] = vec4(texture2D(texRgb, vec2(textureCoord.s + 1.0, textureCoord.t - 0.0)).bgr, 1.0);\n"
// 	"    sample[6] = vec4(texture2D(texRgb, vec2(textureCoord.s - 1.0, textureCoord.t + 1.0)).bgr, 1.0);\n"
// 	"    sample[7] = vec4(texture2D(texRgb, vec2(textureCoord.s - 0.0, textureCoord.t + 1.0)).bgr, 1.0);\n"
// 	"    sample[8] = vec4(texture2D(texRgb, vec2(textureCoord.s + 1.0, textureCoord.t + 1.0)).bgr, 1.0);\n"
// 	"    highp vec4 color = (sample[0] + (2.0*sample[1]) + sample[2] + \n"
// 	"                       (2.0*sample[3]) + sample[4] + (2.0*sample[5]) + \n"
// 	"                        sample[6] + (2.0*sample[7]) + sample[8]) / 13.0;\n"
// 	"    color = colorMatrix * color;\n"
// 	"    gl_FragColor = vec4(color.rgb, texture2D(texRgb, textureCoord.st).a * alpha);\n"
// 	"}\n";

//   1 2 1
//   2 1 2   / 13
//   1 2 1


        // blur (low-pass) 3x3 kernel




// Paints an RGB(A) frame.
static const char *qt_glsl_rgbShaderProgram =
        "uniform sampler2D texRgb;\n"
        "uniform sampler2D alphaMask;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "uniform mediump float alpha;\n"
        "uniform mediump float texOffsetX;\n"
        "uniform mediump float texOffsetY;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    mediump vec2 texPoint = vec2(textureCoord.s + texOffsetX, textureCoord.t + texOffsetY);\n"
        "    highp vec4 color = vec4(texture2D(texRgb, texPoint).rgb, 1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, texture2D(texRgb, texPoint).a * alpha * texture2D(alphaMask, textureCoord.st).a);\n"
        "}\n";

// Paints a YUV420P or YV12 frame.
static const char *qt_glsl_yuvPlanarShaderProgram =
        "uniform sampler2D texY;\n"
        "uniform sampler2D texU;\n"
        "uniform sampler2D texV;\n"
        "uniform sampler2D alphaMask;\n"
        "uniform mediump mat4 colorMatrix;\n"
        "uniform mediump float alpha;\n"
        "uniform mediump float texOffsetX;\n"
        "uniform mediump float texOffsetY;\n"
        "varying highp vec2 textureCoord;\n"
        "void main(void)\n"
        "{\n"
        "    mediump vec2 texPoint = vec2(textureCoord.s + texOffsetX, textureCoord.t + texOffsetY);\n"
        "    highp vec4 color = vec4(\n"
        "           texture2D(texY, texPoint).r,\n"
        "           texture2D(texU, texPoint).r,\n"
        "           texture2D(texV, texPoint).r,\n"
        "           1.0);\n"
        "    color = colorMatrix * color;\n"
        "    gl_FragColor = vec4(color.rgb, alpha * texture2D(alphaMask, textureCoord.st).a);\n"
        "}\n";


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
	, m_debugFps(false)
	, m_aspectRatioMode(Qt::KeepAspectRatio)
	, m_validShader(false)
	, m_program(0)
	, m_uploadedCacheKey(0)
	, m_textureOffset(0,0)
	, m_texturesInited(false)
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
		
		//qDebug() << "GLVideoDrawable::setVideoSource(): "<<objectName()<<" m_source:"<<m_source;
		setVideoFormat(m_source->videoFormat());
		
		frameReady();
	}
	else
	{
		qDebug() << "GLVideoDrawable::setVideoSource(): "<<this<<" Source is NULL";
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
	
	// This seems to prevent crashes during startup of an application when a thread is pumping out frames
	// before the app has finished initalizing.
	QMutexLocker lock(&m_frameReadyLock);
	
	m_frame = m_source->frame();
	
	if(m_glInited && glWidget())
	{
		//if(objectName() != "StaticBackground")
			//qDebug() << "GLVideoDrawable::frameReady(): "<<this<<" Got a frame, size:"<<m_frame.size;
			
		if(m_frame.rect != m_sourceRect || !m_texturesInited)
		{
// 			qDebug() << "GLVideoDrawable::frameReady(): m_frame.rect:"<<m_frame.rect<<", m_sourceRect:"<<m_sourceRect;
// 			qDebug() << "GLVideoDrawable::frameReady(): frame size changed or !m_texturesInited, resizing and adjusting pixels...";
			if(m_videoFormat.pixelFormat != m_source->videoFormat().pixelFormat)
				setVideoFormat(m_source->videoFormat());
			resizeTextures(m_frame.size);
			updateRects();
			updateAlignment();
		}
		
		glWidget()->makeCurrent();
		
		if(m_frame.isEmpty())
		{
			qDebug() << "GLVideoDrawable::frameReady(): Got empty frame, ignoring.";
		}
		else
		if(m_frame.isRaw)
		{
			for (int i = 0; i < m_textureCount; ++i) 
			{
				//qDebug() << "raw: "<<i<<m_textureWidths[i]<<m_textureHeights[i]<<m_textureOffsets[i]<<m_textureInternalFormat<<m_textureFormat<<m_textureType;
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
// 				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			}
		}
		else
		if(!m_frame.image.isNull())
		{
			for (int i = 0; i < m_textureCount; ++i) 
			{
				//qDebug() << (this) << "normal: "<<i<<m_textureWidths[i]<<m_textureHeights[i]<<m_textureOffsets[i]<<m_textureInternalFormat<<m_textureFormat<<m_textureType;
// 				QImageWriter writer("test.jpg");
// 				writer.write(m_frame.image);
			
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
					m_frame.image.scanLine(0) + m_textureOffsets[i]
					);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			}
		}
	}
	
	updateGL();
}

void GLVideoDrawable::setAlphaMask(const QImage &mask)
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
	
	if(m_glInited && glWidget())
	{
		if(m_sourceRect.size().toSize() == QSize(0,0))
		{
			//qDebug() << "GLVideoDrawable::setAlphaMask: "<<this<<", Not scaling or setting mask, video size is 0x0";
			return;
		}
		
		glWidget()->makeCurrent();
		if(m_alphaMask.size() != m_sourceRect.size().toSize())
		{
			//qDebug() << "GLVideoDrawable::setAlphaMask: "<<this<<",  Mask size and source size different, scaling";
			m_alphaMask = m_alphaMask.scaled(m_sourceRect.size().toSize());
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

void GLVideoDrawable::setFlipHorizontal(bool value)
{
	m_displayOpts.flipHorizontal = value;
	emit displayOptionsChanged(m_displayOpts);
	updateGL();
}

void GLVideoDrawable::setFlipVertical(bool value)
{
	m_displayOpts.flipVertical = value;
	emit displayOptionsChanged(m_displayOpts);
	updateGL();
}

void GLVideoDrawable::setCropTopLeft(QPointF value)
{
	m_displayOpts.cropTopLeft = value;
	emit displayOptionsChanged(m_displayOpts);
	updateGL();
}

void GLVideoDrawable::setCropBottomRight(QPointF value)
{
	m_displayOpts.cropBottomRight = value;
	emit displayOptionsChanged(m_displayOpts);
	updateGL();
}

void GLVideoDrawable::setAspectRatioMode(Qt::AspectRatioMode mode)
{
	//qDebug() << "GLVideoDrawable::setAspectRatioMode: "<<this<<", mode:"<<mode<<", int:"<<(int)mode;
	m_aspectRatioMode = mode;
	updateRects();
	updateAlignment();
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
	emit displayOptionsChanged(m_displayOpts);
	m_colorsDirty = true;
	updateGL();
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
	emit displayOptionsChanged(m_displayOpts);
	m_colorsDirty = true;
	updateGL();
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
	emit displayOptionsChanged(m_displayOpts);
	m_colorsDirty = true;
	updateGL();
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
	emit displayOptionsChanged(m_displayOpts);
	m_colorsDirty = true;
	updateGL();
}

void GLVideoDrawable::updateColors(int brightness, int contrast, int hue, int saturation)
{
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
	//qDebug() << "GLVideoDrawable::initYuv420PTextureInfo: size:"<<size;
	
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
	// Just to be safe, dont re-initalize this 'widgets' GL state if already passed thru here once.
	if(m_glInited)
		return;
		
	if(glWidget())
		glWidget()->makeCurrent();
		
	//qDebug() << "GLVideoDrawable::initGL(): "<<objectName();
		
	#ifndef QT_OPENGL_ES
	glActiveTexture = (_glActiveTexture)glWidget()->context()->getProcAddress(QLatin1String("glActiveTexture"));
	#endif
	
	m_program = new QGLShaderProgram(glWidget()->context(), this);
	
	m_glInited = true;
	//qDebug() << "GLVideoDrawable::initGL(): "<<objectName();
	setVideoFormat(m_videoFormat);
	
	// frameReady() updates textures, so it seems to trigger
	// some sort of loop by causing the GLWidget::initializeGL() function to be called again,
	// which calls this class's initGL(), which calls frameReady(), which triggers 
	// GLWidget::initalizeGL() again...so we just put it in a singleshot timer to 
	// fire as soon as control returns to the event loop
	if(m_source)
		QTimer::singleShot(0,this,SLOT(frameReady()));
	
	
	// create the alpha texture
	glGenTextures(1, &m_alphaTextureId);
	
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
	
	m_time.start();
}

bool GLVideoDrawable::setVideoFormat(const VideoFormat& format)
{
	bool samePixelFormat = false; //format.pixelFormat == m_videoFormat.pixelFormat;
	m_videoFormat = format;
	
	//qDebug() << "GLVideoDrawable::setVideoFormat(): "<<objectName()<<" \t frameSize:"<<format.frameSize<<", pixelFormat:"<<format.pixelFormat;
	
	if(!m_glInited || !glWidget())
		return m_imagePixelFormats.contains(format.pixelFormat);
		
	//qDebug() << "GLVideoDrawable::setVideoFormat(): \t Initalizing vertex and pixel shaders...";
	
	glWidget()->makeCurrent();
	
	m_validShader = false;
	const char *fragmentProgram = resizeTextures(format.frameSize);
 
 	if(!samePixelFormat)
 	{
 		if(!m_program->shaders().isEmpty())
 			m_program->removeAllShaders();
 			
		if (!fragmentProgram) 
		{
			qDebug() << "No shader program found - format not supported.";
			return false;
		} 
		else 
		if (!m_program->addShaderFromSourceCode(QGLShader::Vertex, qt_glsl_vertexShaderProgram)) 
		{
			qWarning("GLWidget: Vertex shader compile error %s",
				qPrintable(m_program->log()));
			//error = QAbstractVideoSurface::ResourceError;
			return false;
			
		} 
		else 
		if (!m_program->addShaderFromSourceCode(QGLShader::Fragment, fragmentProgram)) 
		{
			qWarning("GLWidget: Shader compile error %s", qPrintable(m_program->log()));
			//error = QAbstractVideoSurface::ResourceError;
			m_program->removeAllShaders();
			return false;
		} 
		else 
		if(!m_program->link()) 
		{
			qWarning("GLWidget: Shader link error %s", qPrintable(m_program->log()));
			m_program->removeAllShaders();
			return false;
		} 
		else 
		{
			//m_handleType = format.handleType();
			m_scanLineDirection = QVideoSurfaceFormat::TopToBottom; //format.scanLineDirection();
			m_frameSize = format.frameSize;
		
			//if (m_handleType == QAbstractVideoBuffer::NoHandle)
				glGenTextures(m_textureCount, m_textureIds);
				
			m_texturesInited = true;
		}
		
		//qDebug() << "GLVideoDrawable::setVideoFormat(): \t Initalized"<<m_textureCount<<"textures";
	}
	m_validShader = true;
	return true;
}

const char * GLVideoDrawable::resizeTextures(const QSize& frameSize)
{
	const char * fragmentProgram = 0;
	
	//qDebug() << "GLVideoDrawable::resizeTextures(): "<<objectName()<<" \t frameSize: "<<frameSize<<", format: "<<m_videoFormat.pixelFormat;
	m_frameSize = frameSize;

	bool debugShaderName = false;
	switch (m_videoFormat.pixelFormat) 
	{
	case QVideoFrame::Format_RGB32:
 		if(debugShaderName)
 			qDebug() << "GLVideoDrawable::resizeTextures(): \t Format RGB32, using qt_glsl_xrgbShaderProgram";
		initRgbTextureInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, frameSize);
		fragmentProgram = qt_glsl_xrgbShaderProgram;
		break;
        case QVideoFrame::Format_ARGB32:
         	if(debugShaderName)
         		qDebug() << "GLVideoDrawable::resizeTextures(): \t Format ARGB, using qt_glsl_argbShaderProgram";
		initRgbTextureInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, frameSize);
		fragmentProgram = qt_glsl_argbShaderProgram;
		break;
#ifndef QT_OPENGL_ES
        case QVideoFrame::Format_RGB24:
        	if(debugShaderName)
         		qDebug() << "GLVideoDrawable::resizeTextures(): \t Format RGB24, using qt_glsl_rgbShaderProgram";
		initRgbTextureInfo(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, frameSize);
		fragmentProgram = qt_glsl_rgbShaderProgram;
		break;
#endif
	case QVideoFrame::Format_RGB565:
		if(debugShaderName)
			qDebug() << "GLVideoDrawable::resizeTextures(): \t Format RGB565, using qt_glsl_rgbShaderProgram";
		initRgbTextureInfo(GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, frameSize);
		fragmentProgram = qt_glsl_rgbShaderProgram;
		break;
	case QVideoFrame::Format_YV12:
		if(debugShaderName)
			qDebug() << "GLVideoDrawable::resizeTextures(): \t Format YV12, using qt_glsl_yuvPlanarShaderProgram";
		initYv12TextureInfo(frameSize);
		fragmentProgram = qt_glsl_yuvPlanarShaderProgram;
		break;
	case QVideoFrame::Format_YUV420P:
		if(debugShaderName)
			qDebug() << "GLVideoDrawable::resizeTextures(): \t Format YUV420P, using qt_glsl_yuvPlanarShaderProgram";
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
	updateAlignment();
	updateRects();
}

void GLVideoDrawable::canvasResized(const QSizeF& /*newSize*/)
{
	updateAlignment();
	updateRects();
}

void GLVideoDrawable::drawableResized(const QSizeF& /*newSize*/)
{
	//qDebug() << "GLVideoDrawable::drawableResized()";
	//updateAlignment();
	updateRects();
}

void GLVideoDrawable::updateTextureOffsets()
{
	m_invertedOffset = QPointF(m_textureOffset.x() * 1/m_sourceRect.width(),
				   m_textureOffset.y() * 1/m_sourceRect.height());
}

void GLVideoDrawable::updateRects()
{
	if(!m_glInited)
		return;
		
	m_sourceRect = m_frame.rect;
	//if(m_frame.rect != m_sourceRect)
	setAlphaMask(m_alphaMask_preScaled);
	
	updateTextureOffsets();
	
	// force mask to be re-scaled
	//qDebug() << "GLVideoDrawable::updateRects(): "<<(this)<<",  New source rect: "<<m_sourceRect<<", mask size:"<<m_alphaMask.size()<<", isNull?"<<m_alphaMask.isNull();
	
	
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
	
	//qDebug() << "GLVideoDrawable::updateRects(): "<<(this)<<" m_sourceRect:"<<m_sourceRect<<", m_targetRect:"<<m_targetRect;
}

// float opacity = 0.5;
// 	glColor4f(opacity,opacity,opacity,opacity);

void GLVideoDrawable::setDisplayOptions(const VideoDisplayOptions& opts)
{
	m_displayOpts = opts;
	m_colorsDirty = true;
	emit displayOptionsChanged(opts);
	updateGL();
}

void GLVideoDrawable::setTextureOffset(double x, double y)
{
	setTextureOffset(QPointF(x,y));
}

void GLVideoDrawable::setTextureOffset(const QPointF& point)
{
	m_textureOffset = point;
	updateTextureOffsets();
	updateGL();
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
	
	QRectF source = m_sourceRect;
	QRectF target = m_targetRect;
	
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
	if(!translation().isNull())
		transform *= QTransform().translate(translation().x(),translation().y());
	
	const GLfloat wfactor =  2.0 / width;
	const GLfloat hfactor = -2.0 / height;

	if(!rotation().isNull())
	{
 		qreal tx = target.width()  * rotationPoint().x() + target.x();
 		qreal ty = target.height() * rotationPoint().y() + target.y();
 		qreal x, y;
 		transform.map(tx,ty,&x,&y);
 		
 		QVector3D rot = rotation();
  		transform *= QTransform()
 			.translate(x,y)
 			.rotate(rot.x(),Qt::XAxis)
 			.rotate(rot.y(),Qt::YAxis)
 			.rotate(rot.z(),Qt::ZAxis)
 			.translate(-x,-y);
	}
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
		target.left()     , target.bottom() + 1, //(GLfloat)zIndex(),
		target.right() + 1, target.bottom() + 1, //(GLfloat)zIndex(),
		target.left()     , target.top(), 	//(GLfloat)zIndex(),
		target.right() + 1, target.top()//, 	(GLfloat)zIndex()
	};
	
	
// 	QVector3D v1(target.left(),      target.bottom() + 1, 1);
// 	QVector3D v2(target.right() + 1, target.bottom() + 1, 1);
// 	QVector3D v3(target.left(),      target.top(), 1);
// 	QVector3D v4(target.right() + 1, target.top(), 1);
// 	
// 	if(applyRotTx)
// 	{
// 		v1 = rotTx.map(v1);
// 		v2 = rotTx.map(v2);
// 		v3 = rotTx.map(v3);
// 		v4 = rotTx.map(v4);
// 	}
// 	
// 	
// 	const GLfloat vertexCoordArray[] =
// 	{
// 		v1.x(), v1.y(), v1.z(),
// 		v2.x(), v2.y(), v2.z(),
// 		v3.x(), v3.y(), v3.z(),
// 		v4.x(), v4.y(), v4.z(),
// 	};
	
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
	m_program->setUniformValue("alpha",               (GLfloat)opacity());
	m_program->setUniformValue("texOffsetX",          (GLfloat)m_invertedOffset.x());
	m_program->setUniformValue("texOffsetY",          (GLfloat)m_invertedOffset.y());
		

	if (m_textureCount == 3) 
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_textureIds[1]);
		
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_textureIds[2]);
		
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_alphaTextureId);
		
		glActiveTexture(GL_TEXTURE0);
	
		m_program->setUniformValue("texY", 0);
		m_program->setUniformValue("texU", 1);
		m_program->setUniformValue("texV", 2);
		m_program->setUniformValue("alphaMask", 3);
	} 
	else 
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
	
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_alphaTextureId);
		
		glActiveTexture(GL_TEXTURE0);
	
		m_program->setUniformValue("texRgb", 0);
		m_program->setUniformValue("alphaMask", 1);
	}
	m_program->setUniformValue("colorMatrix", m_colorMatrix);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	m_program->release();
	
	
	
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
		
		if(m_debugFps && framesPerSecond!="0.00")
			qDebug() << "GLVideoDrawable::paintGL: "<<objectName()<<" FPS: " << qPrintable(framesPerSecond) << (m_frame.captureTime.isNull() ? "" : qPrintable(QString(", Latency: %1 ms").arg(latencyPerFrame)));

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
	QString name(m_drawable ? m_drawable->objectName() : "");
	setWindowTitle(QString("%1Display Options").arg(name.isEmpty() ? "" : name + " - "));
	
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
	slider->setMinimum(-100);
	slider->setMaximum(100);
	slider->setValue(m_opts.contrast);
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(cChanged(int)));
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
	slider->setMinimum(-100);
	slider->setMaximum(100);
	slider->setValue(m_opts.saturation);
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sChanged(int)));
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

