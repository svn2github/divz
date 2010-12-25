#include "GLVideoDrawable.h"

#include "GLWidget.h"

#include <QVideoFrame>
#include <QAbstractVideoSurface>
#include "../livemix/VideoSource.h"

#include "VideoSender.h"

#include <QImageWriter>

#include <string.h>

#include "GLCommonShaders.h"

#ifndef GL_UNSIGNED_INT_8_8_8_8_REV
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#endif

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

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


int GLVideoDrawable::m_videoSenderPortAllocator = 7755;

GLVideoDrawable::GLVideoDrawable(QObject *parent)
	: GLDrawable(parent)
	, m_frame(0)
	, m_frame2(0)
	, m_visiblePendingFrame(false)
	, m_aspectRatioMode(Qt::KeepAspectRatio)
	, m_glInited(false)
	, m_program(0)
	, m_program2(0)
	, m_textureCount(1)
	, m_textureCount2(1)
	, m_colorsDirty(true)
	, m_source(0)
	, m_source2(0)
	, m_frameCount(0)
	, m_latencyAccum(0)
	, m_debugFps(true)
	, m_validShader(false)
	, m_validShader2(false)
	, m_uploadedCacheKey(0)
	, m_textureOffset(0,0)
	, m_texturesInited(false)
	, m_useShaders(true)
	, m_rateLimitFps(0.0)
	, m_xfadeEnabled(true)
	, m_xfadeLength(300)
	, m_fadeValue(0.)
	, m_fadeActive(false)
	, m_videoSender(0)
	, m_videoSenderEnabled(false)
	, m_videoSenderPort(-1)
	, m_ignoreAspectRatio(false)
{

	m_imagePixelFormats
		<< QVideoFrame::Format_RGB32
		<< QVideoFrame::Format_ARGB32
		<< QVideoFrame::Format_RGB24
		<< QVideoFrame::Format_RGB565
		<< QVideoFrame::Format_YV12
		<< QVideoFrame::Format_YUV420P;

	connect(&m_fpsRateLimiter, SIGNAL(timeout()), this, SLOT(updateGL()));
	m_fpsRateLimiter.setInterval(1000/30);
	// only start if setFpsLimit(float) is called

	connect(&m_fadeTick, SIGNAL(timeout()), this, SLOT(xfadeTick()));
	m_fadeTick.setInterval(1000/25);
}

GLVideoDrawable::~GLVideoDrawable()
{
	setVideoSource(0);
}

void GLVideoDrawable::setFpsLimit(float fps)
{
	m_rateLimitFps = fps;
	if(fps > 0.0)
	{
		m_fpsRateLimiter.setInterval((int)(1000./fps));
		m_fpsRateLimiter.start();
	}
	else
	{
		m_fpsRateLimiter.stop();
	}
}

void GLVideoDrawable::setVideoSenderEnabled(bool flag)
{
	m_videoSenderEnabled = flag;

	if(m_videoSenderEnabled)
	{

		if(!m_videoSender)
			m_videoSender = new VideoSender();


		if(m_videoSenderPort == -1)
		{
			bool done = false;
			while(!done)
			{
				m_videoSenderPort = m_videoSenderPortAllocator ++;
				if(m_videoSender->listen(QHostAddress::Any,m_videoSenderPort))
				{
					done = true;
				}
			}
		}
		else
		{
			if(!m_videoSender->listen(QHostAddress::Any,m_videoSenderPort))
			{
				qDebug() << "VideoServer could not start on port"<<m_videoSenderPort<<": "<<m_videoSender->errorString();
				//return -1;
			}
		}

		if(m_source)
		{
			m_videoSender->setVideoSource(m_source);
		}
	}
	else
	if(m_videoSender)
	{
		m_videoSender->close();
		delete m_videoSender;
		m_videoSender = 0;
	}
}

void GLVideoDrawable::setVideoSenderPort(int port)
{
	m_videoSenderPort = port;

	if(!m_videoSender)
		// Port will be applied when video sender is enabled (above)
		return;

	if(!m_videoSender->listen(QHostAddress::Any,m_videoSenderPort))
	{
		qDebug() << "VideoServer could not start on port"<<m_videoSenderPort<<": "<<m_videoSender->errorString();
		//return -1;
	}
}

void GLVideoDrawable::setVideoSource(VideoSource *source)
{
	if(m_source == source)
		return;

	if(m_source)
	{
		if(!m_xfadeEnabled)
		{
			disconnectVideoSource();
		}
		else
		{
			disconnect(m_source, 0, this, 0);

			m_source2 = m_source;
			connect(m_source2, SIGNAL(frameReady()), this, SLOT(frameReady2()));
			connect(m_source2, SIGNAL(destroyed()),  this, SLOT(disconnectVideoSource2()));

// 			if(m_frame2 &&
// 			   m_frame2->release())
// 			{
// 				#ifdef DEBUG_VIDEOFRAME_POINTERS
// 				qDebug() << "GLVideoDrawable::setVideoSource(): Deleting old m_frame2:"<<m_frame2;
// 				#endif
// 				delete m_frame2;
// 				m_frame2 = 0;
// 			}
			m_frame2 = m_frame;
			#ifdef DEBUG_VIDEOFRAME_POINTERS
			qDebug() << "GLVideoDrawable::setVideoSource(): Copied m_frame:"<<m_frame<<"to m_frame2:"<<m_frame2<<", calling incRef() on m_frame2";
			#endif
			m_frame2->incRef();
			updateTexture(true);

			xfadeStart();
		}
	}

	m_source = source;
	if(m_source)
	{
		connect(m_source, SIGNAL(frameReady()), this, SLOT(frameReady()));
		connect(m_source, SIGNAL(destroyed()),  this, SLOT(disconnectVideoSource()));

		//qDebug() << "GLVideoDrawable::setVideoSource(): "<<(QObject*)this()<<" m_source:"<<m_source;
		setVideoFormat(m_source->videoFormat());

		frameReady();

		if(m_videoSender)
			m_videoSender->setVideoSource(m_source);
	}
	else
	{
		qDebug() << "GLVideoDrawable::setVideoSource(): "<<(QObject*)this<<" Source is NULL";
	}

}

void GLVideoDrawable::xfadeStart()
{
	m_fadeTick.start();
	m_fadeTime.start();
	m_fadeActive = true;
	m_fadeValue = 0.0;
	//qDebug() << "GLVideoDrawable::xfadeStart()";
}

void GLVideoDrawable::xfadeTick()
{
	int elapsed = m_fadeTime.elapsed();
	m_fadeValue = ((double)elapsed) / ((double)m_xfadeLength);
	//qDebug() << "GLVideoDrawable::xfadeTick(): elapsed:"<<elapsed<<", length:"<<m_xfadeLength<<", fadeValue:"<<m_fadeValue;

	if(elapsed >= m_xfadeLength)
		xfadeStop();

	updateGL();
}

void GLVideoDrawable::xfadeStop()
{
	//qDebug() << "GLVideoDrawable::xfadeStop()";
	m_fadeActive = false;
	m_fadeTick.stop();

	disconnectVideoSource2();
	updateGL();
}

void GLVideoDrawable::setXFadeEnabled(bool flag)
{
	m_xfadeEnabled = flag;
}

void GLVideoDrawable::setXFadeLength(int length)
{
	m_xfadeLength = length;
}

void GLVideoDrawable::disconnectVideoSource()
{
	if(!m_source)
		return;
	disconnect(m_source, 0, this, 0);
	m_source = 0;
}

void GLVideoDrawable::disconnectVideoSource2()
{
	if(!m_source2)
		return;
	disconnect(m_source2, 0, this, 0);
	emit sourceDiscarded(m_source2);
	m_source2 = 0;
}

void GLVideoDrawable::setVisible(bool flag, bool pendingFrame)
{
	//qDebug() << "GLVideoDrawable::setVisible: "<<this<<", flag:"<<flag<<", pendingFrame:"<<pendingFrame;
	m_tempVisibleValue = flag;
	if(flag && pendingFrame)
	{
		m_visiblePendingFrame = true;
	}
	else
	{
		if(m_visiblePendingFrame)
			m_visiblePendingFrame = false;

		GLDrawable::setVisible(flag);
	}
}

void GLVideoDrawable::frameReady()
{
	// This seems to prevent crashes during startup of an application when a thread is pumping out frames
	// before the app has finished initalizing.
	QMutexLocker lock(&m_frameReadyLock);

// 	qDebug() << "GLVideoDrawable::frameReady(): "<<objectName()<<" m_source:"<<m_source;
	if(m_source)
	{
		VideoFramePtr f = m_source->frame();
// 		qDebug() << "GLVideoDrawable::frameReady(): "<<objectName()<<" f:"<<f;
		if(!f)
			return;
		if(f->isValid())
		{
// 			if(m_frame && 
// 			   m_frame->release())
// 			{
// 				#ifdef DEBUG_VIDEOFRAME_POINTERS
//  				qDebug() << "GLVideoDrawable::frameReady(): "<<objectName()<<" deleting old frame:"<<m_frame;
//  				#endif
// 				delete m_frame;
// 			}

			m_frame = f;
		}
// 		else
// 		{
// 			if(f->release())
// 			{
// 				#ifdef DEBUG_VIDEOFRAME_POINTERS
//  				qDebug() << "GLVideoDrawable::frameReady(): "<<objectName()<<" deleting invalid frame:"<<f;
//  				#endif
// 				delete f;
// 			}
// 		}
	}

// 	qDebug() << "GLVideoDrawable::frameReady(): "<<objectName()<<" going to updateTexture";
	updateTexture();

	if(m_rateLimitFps <= 0.0)
		updateGL();
	//else
	//	qDebug() << "GLVideoDrawable::frameReady(): "<<(QObject*)this<<" rate limited, waiting on timer";

	if(m_visiblePendingFrame)
	{
		//qDebug() << "GLVideoDrawable::frameReady: "<<this<<", pending visible set, calling setVisible("<<m_tempVisibleValue<<")";
		m_visiblePendingFrame = false;
		GLDrawable::setVisible(m_tempVisibleValue);
	}
}

void GLVideoDrawable::frameReady2()
{
	if(m_source2)
	{
		VideoFramePtr f = m_source2->frame();
		if(!f)
			return;
		if(f->isValid())
		{
// 			if(m_frame2 && 
// 			   m_frame2->release())
// 			   {
// 			   	#ifdef DEBUG_VIDEOFRAME_POINTERS
// 			   	qDebug() << "GLVideoDrawable::frameReady2(): "<<objectName()<<" deleting old m_frame2:"<<m_frame2;
// 			   	#endif
// 			   	delete m_frame2;
// 			   }

			m_frame2 = f;
		}
// 		else
// 		{
// 			if(f->release())
// 			{
// 				#ifdef DEBUG_VIDEOFRAME_POINTERS
// 				qDebug() << "GLVideoDrawable::frameReady2(): "<<objectName()<<" deleting invalid frame:"<<f;
// 				#endif
// 				delete f;
// 			}
// 		}
	}

	updateTexture(true);
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

		glWidget()->makeRenderContextCurrent();
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

void GLVideoDrawable::setIgnoreAspectRatio(bool flag)
{
	m_ignoreAspectRatio = flag;
	setAspectRatioMode(flag ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio);
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

void GLVideoDrawable::initRgbTextureInfo(GLenum internalFormat, GLuint format, GLenum type, const QSize &size, bool secondSource)
{
	if(!secondSource)
		m_yuv = false;
	else
		m_yuv2 = false;

	if(!secondSource)
	{
		m_textureInternalFormat = internalFormat;
		m_textureFormat = format;
		m_textureType   = type;

		m_textureCount  = 1;
	}
	else
	{
		m_textureInternalFormat2 = internalFormat;
		m_textureFormat2 = format;
		m_textureType2   = type;

		m_textureCount2  = 1;
	}

	int idx = secondSource ? 3:0;

	m_textureWidths[0+idx]  = size.width();
	m_textureHeights[0+idx] = size.height();
	m_textureOffsets[0+idx] = 0;
}

void GLVideoDrawable::initYuv420PTextureInfo(const QSize &size, bool secondSource)
{
	if(!secondSource)
		m_yuv = true;
	else
		m_yuv2 = true;

	if(!secondSource)
	{
		m_textureInternalFormat = GL_LUMINANCE;
		m_textureFormat = GL_LUMINANCE;
		m_textureType   = GL_UNSIGNED_BYTE;
		m_textureCount  = 3;
	}
	else
	{
		m_textureInternalFormat2 = GL_LUMINANCE;
		m_textureFormat2 = GL_LUMINANCE;
		m_textureType2   = GL_UNSIGNED_BYTE;
		m_textureCount2  = 3;
	}

	int idx = secondSource ? 3:0;

	m_textureWidths[0+idx] = size.width();
	m_textureHeights[0+idx] = size.height();
	m_textureOffsets[0+idx] = 0;

	m_textureWidths[1+idx] = size.width() / 2;
	m_textureHeights[1+idx] = size.height() / 2;
	m_textureOffsets[1+idx] = size.width() * size.height();
	//qDebug() << "GLVideoDrawable::initYuv420PTextureInfo: size:"<<size;

	m_textureWidths[2+idx] = size.width() / 2;
	m_textureHeights[2+idx] = size.height() / 2;
	m_textureOffsets[2+idx] = size.width() * size.height() * 5 / 4;

	//qDebug() << "yuv420p tex: off2:"<<m_textureOffsets[2];
}

void GLVideoDrawable::initYv12TextureInfo(const QSize &size, bool secondSource)
{
	if(!secondSource)
		m_yuv = true;
	else
		m_yuv2 = true;

	if(!secondSource)
	{
		m_textureInternalFormat = GL_LUMINANCE;
		m_textureFormat = GL_LUMINANCE;
		m_textureType   = GL_UNSIGNED_BYTE;
		m_textureCount  = 3;
	}
	else
	{
		m_textureInternalFormat2 = GL_LUMINANCE;
		m_textureFormat2 = GL_LUMINANCE;
		m_textureType2   = GL_UNSIGNED_BYTE;
		m_textureCount2  = 3;
	}

	int idx = secondSource ? 3:0;

	m_textureWidths[0+idx] = size.width();
	m_textureHeights[0+idx] = size.height();
	m_textureOffsets[0+idx] = 0;

	m_textureWidths[1+idx] = size.width() / 2;
	m_textureHeights[1+idx] = size.height() / 2;
	m_textureOffsets[1+idx] = size.width() * size.height() * 5 / 4;

	m_textureWidths[2+idx] = size.width() / 2;
	m_textureHeights[2+idx] = size.height() / 2;
	m_textureOffsets[2+idx] = size.width() * size.height();
}


 QVariant GLVideoDrawable::itemChange(GraphicsItemChange change, const QVariant &value)
 {
	if (change == ItemSceneChange)
	{
// 		// value is the new position.
// 		QPointF newPos = value.toPointF();
// 		QRectF rect = scene()->sceneRect();
// 		if (!rect.contains(newPos)) {
// 		// Keep the item inside the scene rect.
// 		newPos.setX(qMin(rect.right(), qMax(newPos.x(), rect.left())));
// 		newPos.setY(qMin(rect.bottom(), qMax(newPos.y(), rect.top())));
// 		return newPos;
// 		}
/*
		if(videoSource() && dynamic_cast<QtVideoSource*>(videoSource()))// && !m_videoWidgetItem)
		{
			QGraphicsProxyWidget *m_videoWidgetProxy = new QGraphicsProxyWidget(this);
			QVideoWidget *m_videoWidgetItem = new QVideoWidget();
			m_videoWidgetProxy->setWidget(m_videoWidgetItem);
			//m_videoWidgetProxy->setVisible(true);
			m_videoWidgetItem->show();

			QtVideoSource *src = dynamic_cast<QtVideoSource*>(videoSource());
			src->player()->setVideoOutput(m_videoWidgetItem);


			qDebug() << "Setup Video Proxy";
		}
		else
		{
			qDebug() << "Added to scene, but no qt video source";
		}
*/
	}
	return QGraphicsItem::itemChange(change, value);
 }

void GLVideoDrawable::initGL()
{
	// Just to be safe, dont re-initalize this 'widgets' GL state if already passed thru here once.
	if(m_glInited)
		return;

	if(glWidget())
		glWidget()->makeRenderContextCurrent();

	//qDebug() << "GLVideoDrawable::initGL(): "<<(QObject*)this;

	#ifndef QT_OPENGL_ES
	glActiveTexture = (_glActiveTexture)glWidget()->context()->getProcAddress(QLatin1String("glActiveTexture"));
	#endif

	const GLubyte *str = glGetString(GL_EXTENSIONS);
	m_useShaders = (strstr((const char *)str, "GL_ARB_fragment_shader") != NULL);

	if(0)
	{
		qDebug() << "GLVideoDrawable::initGL: Forcing NO GLSL shaders";
		m_useShaders = false;
	}

	if(m_useShaders)
	{
		m_program = new QGLShaderProgram(glWidget()->context(), this);
		m_program2 = new QGLShaderProgram(glWidget()->context(), this);
	}

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
	else
		if(m_frame && m_frame->isValid())
			QTimer::singleShot(0,this,SLOT(updateTexture()));


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

bool GLVideoDrawable::setVideoFormat(const VideoFormat& format, bool secondSource)
{
	bool samePixelFormat = false; //format.pixelFormat == m_videoFormat.pixelFormat;
	if(!secondSource)
		m_videoFormat = format;
	else
		m_videoFormat2 = format;

	//qDebug() << "GLVideoDrawable::setVideoFormat(): "<<(QObject*)this<<" \t frameSize:"<<format.frameSize<<", pixelFormat:"<<format.pixelFormat;

	if(!m_glInited || !glWidget())
		return m_imagePixelFormats.contains(format.pixelFormat);

	//qDebug() << "GLVideoDrawable::setVideoFormat(): \t Initalizing vertex and pixel shaders...";

	glWidget()->makeRenderContextCurrent();

	if(!secondSource)
		m_validShader = false;
	else
		m_validShader2 = false;
	const char *fragmentProgram = resizeTextures(format.frameSize, secondSource);
	
  	if(!samePixelFormat)
  	{
  		if(m_useShaders)
		{
			Q_UNUSED(qt_glsl_warpingVertexShaderProgram);
			
			QGLShaderProgram *program = !secondSource ? m_program : m_program2;

			if(!program->shaders().isEmpty())
				program->removeAllShaders();

			if(!QGLShaderProgram::hasOpenGLShaderPrograms())
			{
				qDebug() << "GLSL Shaders Not Supported by this driver, this program will NOT function as expected and will likely crash.";
				return false;
			}

			if (!fragmentProgram)
			{
				if(format.pixelFormat != QVideoFrame::Format_Invalid)
					qDebug() << "GLVideoDrawable: No shader program found - format not supported.";
				return false;
			}
			else
			if (!program->addShaderFromSourceCode(QGLShader::Vertex, qt_glsl_vertexShaderProgram))
			{
				qWarning("GLVideoDrawable: Vertex shader compile error %s",
					qPrintable(program->log()));
				//error = QAbstractVideoSurface::ResourceError;
				return false;

			}
			else
			if (!program->addShaderFromSourceCode(QGLShader::Fragment, fragmentProgram))
			{
				qWarning("GLVideoDrawable: Shader compile error %s", qPrintable(program->log()));
				//error = QAbstractVideoSurface::ResourceError;
				program->removeAllShaders();
				return false;
			}
			else
			if(!program->link())
			{
				qWarning("GLVideoDrawable: Shader link error %s", qPrintable(program->log()));
				program->removeAllShaders();
				return false;
			}
			else
			{
				//m_handleType = format.handleType();
				m_scanLineDirection = QVideoSurfaceFormat::TopToBottom; //format.scanLineDirection();

				if(!secondSource)
				{
					m_frameSize = format.frameSize;
					glGenTextures(m_textureCount, m_textureIds);
					m_texturesInited = true;
				}
				else
				{
					m_frameSize2 = format.frameSize;
					glGenTextures(m_textureCount2, m_textureIds2);
					m_texturesInited2 = true;
				}
			}

			//qDebug() << "GLVideoDrawable::setVideoFormat(): \t Initalized"<<m_textureCount<<"textures";
		}
		else
		{
			m_scanLineDirection = QVideoSurfaceFormat::TopToBottom; //format.scanLineDirection();

			if(!secondSource)
			{
				m_frameSize = format.frameSize;
				glGenTextures(1, m_textureIds);
				m_texturesInited = true;
			}
			else
			{
				m_frameSize2 = format.frameSize;
				glGenTextures(1, m_textureIds2);
				m_texturesInited2 = true;
			}

			//qDebug() << "GLVideoDrawable::setVideoFormat(): \t Initalized 1 textures, no shader used";
		}
 	}
	m_validShader = true;
	return true;
}

const char * GLVideoDrawable::resizeTextures(const QSize& frameSize, bool secondSource)
{
	const char * fragmentProgram = 0;

	//qDebug() << "GLVideoDrawable::resizeTextures(): "<<objectName()<<" \t frameSize: "<<frameSize<<", format: "<<m_videoFormat.pixelFormat;
	//qDebug() << "GLVideoDrawable::resizeTextures(): secondSource:"<<secondSource;
	if(!secondSource)
		m_frameSize = frameSize;
	else
		m_frameSize2 = frameSize;

	bool debugShaderName = false;
	switch (m_videoFormat.pixelFormat)
	{
	case QVideoFrame::Format_RGB32:
 		if(debugShaderName)
 			qDebug() << "GLVideoDrawable::resizeTextures(): \t Format RGB32, using qt_glsl_xrgbShaderProgram";
		initRgbTextureInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, frameSize, secondSource);
		fragmentProgram = qt_glsl_xrgbShaderProgram;
		break;
        case QVideoFrame::Format_ARGB32:
         	if(debugShaderName)
         		qDebug() << "GLVideoDrawable::resizeTextures(): \t Format ARGB, using qt_glsl_argbShaderProgram";
		initRgbTextureInfo(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, frameSize, secondSource);
		fragmentProgram = qt_glsl_argbShaderProgram;
		break;
#ifndef QT_OPENGL_ES
        case QVideoFrame::Format_RGB24:
        	if(debugShaderName)
         		qDebug() << "GLVideoDrawable::resizeTextures(): \t Format RGB24, using qt_glsl_rgbShaderProgram";
		initRgbTextureInfo(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, frameSize, secondSource);
		fragmentProgram = qt_glsl_rgbShaderProgram;
		break;
#endif
	case QVideoFrame::Format_RGB565:
		if(debugShaderName)
			qDebug() << "GLVideoDrawable::resizeTextures(): \t Format RGB565, using qt_glsl_rgbShaderProgram";
		initRgbTextureInfo(GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, frameSize, secondSource);
		fragmentProgram = qt_glsl_rgbShaderProgram;
		break;
	case QVideoFrame::Format_YV12:
		if(debugShaderName)
			qDebug() << "GLVideoDrawable::resizeTextures(): \t Format YV12, using qt_glsl_yuvPlanarShaderProgram";
		initYv12TextureInfo(frameSize, secondSource);
		fragmentProgram = qt_glsl_yuvPlanarShaderProgram;
		break;
	case QVideoFrame::Format_YUV420P:
		if(debugShaderName)
			qDebug() << "GLVideoDrawable::resizeTextures(): \t Format YUV420P, using qt_glsl_yuvPlanarShaderProgram";
		initYuv420PTextureInfo(frameSize, secondSource);
		fragmentProgram = qt_glsl_yuvPlanarShaderProgram;
		break;
	default:
		if(debugShaderName)
			qDebug() << "GLVideoDrawable::resizeTextures(): \t Unknown pixel format:"<<m_videoFormat.pixelFormat;
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

void GLVideoDrawable::updateRects(bool secondSource)
{
// 	if(!m_glInited)
// 		return;
	if(!m_frame)
		return;

	QRectF sourceRect = m_frame->rect();
	//if(m_frame->rect() != m_sourceRect)

	updateTextureOffsets();

	// force mask to be re-scaled
	//qDebug() << "GLVideoDrawable::updateRects(): "<<(this)<<",  New source rect: "<<m_sourceRect<<", mask size:"<<m_alphaMask.size()<<", isNull?"<<m_alphaMask.isNull();


	QRectF adjustedSource = sourceRect.adjusted(
		m_displayOpts.cropTopLeft.x(),
		m_displayOpts.cropTopLeft.y(),
		m_displayOpts.cropBottomRight.x(),
		m_displayOpts.cropBottomRight.y());
// 	m_origSourceRect = m_sourceRect;
//
// 	m_sourceRect.adjust(m_adjustDx1,m_adjustDy1,m_adjustDx2,m_adjustDy2);

	QSizeF nativeSize = adjustedSource.size(); //m_frame->size();

	QRectF targetRect;

	if (nativeSize.isEmpty())
	{
		targetRect = QRectF();
	}
	else
	if (m_aspectRatioMode == Qt::IgnoreAspectRatio)
	{
		targetRect = rect();
	}
	else
	if (m_aspectRatioMode == Qt::KeepAspectRatio)
	{
		QSizeF size = nativeSize;
		size.scale(rect().size(), Qt::KeepAspectRatio);

		targetRect = QRectF(0, 0, size.width(), size.height());
		targetRect.moveCenter(rect().center());
	}
	else
	if (m_aspectRatioMode == Qt::KeepAspectRatioByExpanding)
	{
		targetRect = rect();

		QSizeF size = rect().size();
		size.scale(nativeSize, Qt::KeepAspectRatio);

		sourceRect = QRectF(QPointF(0,0),size);
		sourceRect.moveCenter(QPointF(size.width() / 2, size.height() / 2));
	}

	if(!secondSource)
	{
		m_sourceRect = sourceRect;
		m_targetRect = targetRect;
	}
	else
	{
		m_sourceRect2 = sourceRect;
		m_targetRect2 = targetRect;
	}

	setAlphaMask(m_alphaMask_preScaled);
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

// returns the highest number closest to v, which is a power of 2
// NB! assumes 32 bit ints
int qt_next_power_of_two(int v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    ++v;
    return v;
}


// map from Qt's ARGB endianness-dependent format to GL's big-endian RGBA layout
static inline void qgl_byteSwapImage(QImage &img, GLenum pixel_type)
{
    const int width = img.width();
    const int height = img.height();

    if (pixel_type == GL_UNSIGNED_INT_8_8_8_8_REV
        || (pixel_type == GL_UNSIGNED_BYTE && QSysInfo::ByteOrder == QSysInfo::LittleEndian))
    {
        for (int i = 0; i < height; ++i) {
            uint *p = (uint *) img.scanLine(i);
            for (int x = 0; x < width; ++x)
                p[x] = ((p[x] << 16) & 0xff0000) | ((p[x] >> 16) & 0xff) | (p[x] & 0xff00ff00);
        }
    } else {
        for (int i = 0; i < height; ++i) {
            uint *p = (uint *) img.scanLine(i);
            for (int x = 0; x < width; ++x)
                p[x] = (p[x] << 8) | ((p[x] >> 24) & 0xff);
        }
    }
}

//#define QGL_BIND_TEXTURE_DEBUG

static void uploadTexture(GLuint tx_id, const QImage &image)
{
	 GLenum target = GL_TEXTURE_2D;
	 GLint internalFormat = GL_RGBA;
	 QGLContext::BindOptions options;
	 //const qint64 key = 0;

    //Q_Q(QGLContext);

#ifdef QGL_BIND_TEXTURE_DEBUG
    printf("GLVideoDrawable / uploadTexture(), imageSize=(%d,%d), internalFormat =0x%x, options=%x\n",
           image.width(), image.height(), internalFormat, int(options));
    QTime time;
    time.start();
#endif

#ifndef QT_NO_DEBUG
    // Reset the gl error stack...git
    while (glGetError() != GL_NO_ERROR) ;
#endif

    // Scale the pixmap if needed. GL textures needs to have the
    // dimensions 2^n+2(border) x 2^m+2(border), unless we're using GL
    // 2.0 or use the GL_TEXTURE_RECTANGLE texture target
    int tx_w = qt_next_power_of_two(image.width());
    int tx_h = qt_next_power_of_two(image.height());

    QImage img = image;

    if (// !(QGLExtensions::glExtensions() & QGLExtensions::NPOTTextures)
        //&&
        !(QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_ES_Version_2_0)
        && (target == GL_TEXTURE_2D && (tx_w != image.width() || tx_h != image.height())))
    {
        img = img.scaled(tx_w, tx_h);
#ifdef QGL_BIND_TEXTURE_DEBUG
        printf(" - upscaled to %dx%d (%d ms)\n", tx_w, tx_h, time.elapsed());

#endif
    }


    GLuint filtering = options & QGLContext::LinearFilteringBindOption ? GL_LINEAR : GL_NEAREST;

    //GLuint tx_id;
    //glGenTextures(1, &tx_id);
    glBindTexture(target, tx_id);
    glTexParameterf(target, GL_TEXTURE_MAG_FILTER, filtering);

    //qDebug() << "Upload, mark1";

#if defined(QT_OPENGL_ES_2)
    bool genMipmap = false;
#endif

//     if (glFormat.directRendering()
//         && (QGLExtensions::glExtensions() & QGLExtensions::GenerateMipmap)
//         && target == GL_TEXTURE_2D
//         && (options & QGLContext::MipmapBindOption))
//     {
// #ifdef QGL_BIND_TEXTURE_DEBUG
//         printf(" - generating mipmaps (%d ms)\n", time.elapsed());
// #endif
// #if !defined(QT_OPENGL_ES_2)
//         glHint(GL_GENERATE_MIPMAP_HINT_SGIS, GL_NICEST);
// #ifndef QT_OPENGL_ES
//         glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
// #else
//         glTexParameterf(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
// #endif
// #else
//         glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
//         genMipmap = true;
// #endif
//         glTexParameterf(target, GL_TEXTURE_MIN_FILTER, options & QGLContext::LinearFilteringBindOption
//                         ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_NEAREST);
//     } else {
        glTexParameterf(target, GL_TEXTURE_MIN_FILTER, filtering);
    //}

    QImage::Format target_format = img.format();
    bool premul = options & QGLContext::PremultipliedAlphaBindOption;
    GLenum externalFormat;
    GLuint pixel_type;
//     if (QGLExtensions::glExtensions() & QGLExtensions::BGRATextureFormat) {
//         externalFormat = GL_BGRA;
//         if (QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_2)
//             pixel_type = GL_UNSIGNED_INT_8_8_8_8_REV;
//         else
//             pixel_type = GL_UNSIGNED_BYTE;
//     } else {
        externalFormat = GL_RGBA;
        pixel_type = GL_UNSIGNED_BYTE;
    //}

    switch (target_format) {
    case QImage::Format_ARGB32:
        if (premul) {
            img = img.convertToFormat(target_format = QImage::Format_ARGB32_Premultiplied);
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - converting ARGB32 -> ARGB32_Premultiplied (%d ms) \n", time.elapsed());
#endif
        }
        break;
    case QImage::Format_ARGB32_Premultiplied:
        if (!premul) {
            img = img.convertToFormat(target_format = QImage::Format_ARGB32);
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - converting ARGB32_Premultiplied -> ARGB32 (%d ms)\n", time.elapsed());
#endif
        }
        break;
    case QImage::Format_RGB16:
        pixel_type = GL_UNSIGNED_SHORT_5_6_5;
        externalFormat = GL_RGB;
        internalFormat = GL_RGB;
        break;
    case QImage::Format_RGB32:
        break;
    default:
        if (img.hasAlphaChannel()) {
            img = img.convertToFormat(premul
                                      ? QImage::Format_ARGB32_Premultiplied
                                      : QImage::Format_ARGB32);
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - converting to 32-bit alpha format (%d ms)\n", time.elapsed());
#endif
        } else {
            img = img.convertToFormat(QImage::Format_RGB32);
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - converting to 32-bit (%d ms)\n", time.elapsed());
#endif
        }
    }

    if (options & QGLContext::InvertedYBindOption) {
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - flipping bits over y (%d ms)\n", time.elapsed());
#endif
        if (img.isDetached()) {
            int ipl = img.bytesPerLine() / 4;
            int h = img.height();
            for (int y=0; y<h/2; ++y) {
                int *a = (int *) img.scanLine(y);
                int *b = (int *) img.scanLine(h - y - 1);
                for (int x=0; x<ipl; ++x)
                    qSwap(a[x], b[x]);
            }
        } else {
            // Create a new image and copy across.  If we use the
            // above in-place code then a full copy of the image is
            // made before the lines are swapped, which processes the
            // data twice.  This version should only do it once.
            img = img.mirrored();
        }
    }

    if (externalFormat == GL_RGBA) {
#ifdef QGL_BIND_TEXTURE_DEBUG
            printf(" - doing byte swapping (%d ms)\n", time.elapsed());
#endif
        // The only case where we end up with a depth different from
        // 32 in the switch above is for the RGB16 case, where we set
        // the format to GL_RGB
        Q_ASSERT(img.depth() == 32);
        qgl_byteSwapImage(img, pixel_type);
        //qDebug() << "Upload, mark2 - unable to byte swap - dont hvae helper";
    }
#ifdef QT_OPENGL_ES
    // OpenGL/ES requires that the internal and external formats be
    // identical.
    internalFormat = externalFormat;
#endif
#ifdef QGL_BIND_TEXTURE_DEBUG
    printf(" - uploading, image.format=%d, externalFormat=0x%x, internalFormat=0x%x, pixel_type=0x%x\n",
           img.format(), externalFormat, internalFormat, pixel_type);
#endif

    const QImage &constRef = img; // to avoid detach in bits()...
    glTexImage2D(target, 0, internalFormat, img.width(), img.height(), 0, externalFormat,
                 pixel_type, constRef.bits());
#if defined(QT_OPENGL_ES_2)
    if (genMipmap)
        glGenerateMipmap(target);
#endif
#ifndef QT_NO_DEBUG
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        qWarning(" - texture upload failed, error code 0x%x\n", error);
    }
#endif

#ifdef QGL_BIND_TEXTURE_DEBUG
    static int totalUploadTime = 0;
    totalUploadTime += time.elapsed();
    printf(" - upload done in (%d ms) time=%d\n", time.elapsed(), totalUploadTime);
#endif


    // this assumes the size of a texture is always smaller than the max cache size
//     int cost = img.width()*img.height()*4/1024;
//     QGLTexture *texture = new QGLTexture(q, tx_id, target, options);
//     QGLTextureCache::instance()->insert(q, key, texture, cost);
    //return texture;
}


void GLVideoDrawable::updateTexture(bool secondSource)
{
//  	qDebug() << "GLVideoDrawable::updateTexture(): "<<(QObject*)this<<" secondSource:"<<secondSource;
	if(!secondSource ? (!m_frame || !m_frame->isValid()) : (!m_frame2 || !m_frame2->isValid()))
	{
		qDebug() << "GLVideoDrawable::updateTexture(): "<<(QObject*)this<<" Frame not valid";
		return;
	}

	if(m_glInited && glWidget())
	{
		//if(objectName() != "StaticBackground")
// 		qDebug() << "GLVideoDrawable::updateTexture(): "<<(QObject*)this<<" Got a frame, size:"<<m_frame->size();
		//if()
			//m_frameSize = m_frame->size();
// 		qDebug() << "GLVideoDrawable::updateTexture(): "<<objectName()<<" Got frame size:"<<m_frame->size();

		if(!secondSource ? (m_frameSize != m_frame->size()   || m_frame->rect() != m_sourceRect   || !m_texturesInited) :
				   (m_frameSize2 != m_frame2->size() || m_frame2->rect() != m_sourceRect2 || !m_texturesInited2))
		{
 			//qDebug() << "GLVideoDrawable::paintGL(): m_frame->rect():"<<m_frame->rect()<<", m_sourceRect:"<<m_sourceRect<<", m_frame->size():"<<m_frame->size();
//   			qDebug() << "GLVideoDrawable::updateTexture(): "<<(QObject*)this<<" frame size changed or !m_texturesInited, resizing and adjusting pixels...";
			//if(m_videoFormat.pixelFormat != m_source->videoFormat().pixelFormat)

			if(!secondSource)
			{
				if(m_videoFormat.pixelFormat != m_frame->pixelFormat())
					setVideoFormat(VideoFormat(m_frame->bufferType(), m_frame->pixelFormat(), m_frame->size()), secondSource);
			}
			else
			{
				if(m_videoFormat2.pixelFormat != m_frame2->pixelFormat())
					setVideoFormat(VideoFormat(m_frame2->bufferType(), m_frame2->pixelFormat(), m_frame2->size()), secondSource);
			}

			resizeTextures(!secondSource ? m_frame->size() : m_frame2->size(), secondSource);
			updateRects(secondSource);
			updateAlignment();
		}

		if(!m_validShader)
		{
			qDebug() << "GLVideoDrawable::updateTexture(): "<<(QObject*)this<<" No valid shader, not painting";
			return;
		}

		glWidget()->makeRenderContextCurrent();

		if(!secondSource ? m_frame->isEmpty() : m_frame2->isEmpty())
		{
			qDebug() << "GLVideoDrawable::updateTexture(): Got empty frame, ignoring.";
		}
		else
		if(!secondSource ? m_frame->isRaw() : m_frame2->isRaw())
		{
// 			qDebug() << "GLVideoDrawable::updateTexture(): "<<objectName()<<" Mark: raw frame";
			for (int i = 0; i < (!secondSource ? m_textureCount : m_textureCount2); ++i)
			{
				//qDebug() << "raw: "<<i<<m_textureWidths[i]<<m_textureHeights[i]<<m_textureOffsets[i]<<m_textureInternalFormat<<m_textureFormat<<m_textureType;
				glBindTexture(GL_TEXTURE_2D, (!secondSource ? m_textureIds[i] : m_textureIds2[i]));
				if(m_useShaders)
				{
					if(!secondSource)
					{
						glTexImage2D(
							GL_TEXTURE_2D,
							0,
							m_textureInternalFormat,
							m_textureWidths[i],
							m_textureHeights[i],
							0,
							m_textureFormat,
							m_textureType,
							//m_frame->byteArray().constData() + m_textureOffsets[i]
							m_frame->pointer() + m_textureOffsets[i]
							//m_frame->bufferType() == VideoFrame::BUFFER_POINTER ? m_frame->data()[i] :
								//(uint8_t*)m_frame->byteArray().constData() + m_textureOffsets[i]
						);
					}
					else
					{
						glTexImage2D(
							GL_TEXTURE_2D,
							0,
							m_textureInternalFormat2,
							m_textureWidths[i+3],
							m_textureHeights[i+3],
							0,
							m_textureFormat2,
							m_textureType2,
							m_frame2->pointer() + m_textureOffsets[i+3]
							//m_frame->bufferType() == VideoFrame::BUFFER_POINTER ? m_frame->data()[i] :
								//(uint8_t*)m_frame->byteArray().constData() + m_textureOffsets[i]
						);
					}
				}
				else
				{
					if(!secondSource)
					{
						glTexImage2D(
							GL_TEXTURE_2D,
							0,
							3, //m_textureInternalFormat,
							m_textureWidths[i],
							m_textureHeights[i],
							0,
							GL_BGRA, //m_textureFormat,
							GL_UNSIGNED_BYTE, //m_textureType,
							m_frame->pointer() + m_textureOffsets[i]
							//m_frame->bufferType() == VideoFrame::BUFFER_POINTER ? m_frame->data()[i] :
								//(uint8_t*)m_frame->byteArray().constData() + m_textureOffsets[i]
						);
					}
					else
					{
						glTexImage2D(
							GL_TEXTURE_2D,
							0,
							3, //m_textureInternalFormat,
							m_textureWidths[i+3],
							m_textureHeights[i+3],
							0,
							GL_BGRA, //m_textureFormat,
							GL_UNSIGNED_BYTE, //m_textureType,
							m_frame2->pointer() + m_textureOffsets[i]
							//m_frame->bufferType() == VideoFrame::BUFFER_POINTER ? m_frame->data()[i] :
								//(uint8_t*)m_frame->byteArray().constData() + m_textureOffsets[i]
						);
					}
				}

				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				
			}
		}
		else
		if(!m_frame->image().isNull())
		{
//  			qDebug() << "GLVideoDrawable::updateTexture(): "<<(QObject*)this<<" Mark: QImage frame";
			for (int i = 0; i < (!secondSource ? m_textureCount : m_textureCount2); ++i)
			{
				//qDebug() << (QObject*)(this) << "normal: "<<i<<m_textureWidths[i]<<m_textureHeights[i]<<m_textureOffsets[i]<<m_textureInternalFormat<<m_textureFormat<<m_textureType;
// 				QImageWriter writer("test.jpg");
// 				writer.write(m_frame->image());

				const QImage &constRef = !secondSource ? m_frame->image() : m_frame2->image(); // avoid detach in .bits()

				glBindTexture(GL_TEXTURE_2D, !secondSource ? m_textureIds[i] : m_textureIds2[i]);
				if(m_useShaders)
				{
					if(!secondSource)
					{
						glTexImage2D(
							GL_TEXTURE_2D,
							0,
							m_textureInternalFormat,
							m_textureWidths[i],
							m_textureHeights[i],
							0,
							m_textureFormat,
							m_textureType,
							constRef.bits() + m_textureOffsets[i]
							);
					}
					else
					{
						glTexImage2D(
							GL_TEXTURE_2D,
							0,
							m_textureInternalFormat2,
							m_textureWidths[i+3],
							m_textureHeights[i+3],
							0,
							m_textureFormat2,
							m_textureType2,
							constRef.bits() + m_textureOffsets[i]
							);
					}
				}
				else
				{
					//m_frame->image() = m_frame->image().convertToFormat(QImage::Format_ARGB32);
					//qDebug() << "No shader, custom glTexImage2D arguments";

 					//QImage texGL = m_frame->image();
// 					//glTexImage2D( GL_TEXTURE_2D, 0, 3, texGL.width(), texGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texGL.bits() );
 					//glTexImage2D( GL_TEXTURE_2D, 0, 3, texGL.width(), texGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texGL.bits() );
//  					qDebug() << (QObject*)(this) << "my args:      "<<m_textureInternalFormat<<m_textureFormat<<m_textureType;
//  					qDebug() << (QObject*)(this) << "working args: "<<3<<GL_RGBA<<GL_UNSIGNED_BYTE;
//  					qDebug() << (QObject*)(this) << "image format#:"<<m_frame->image().format();

					// Why does this work?? m_frame->image().format == #4, RGB32, but tex format seems to require BGRA when using non-GLSL texture rendering...wierd...
					m_textureFormat = GL_BGRA;

					if(0)
					{
						m_textureIds[i] = m_glw->bindTexture(m_frame->image());
					}

					if(1)
					{
						uploadTexture(m_textureIds[i],!secondSource ? m_frame->image() : m_frame2->image());
					}

					if(0)
					{
						glTexImage2D(
							GL_TEXTURE_2D,
							0,
							m_textureInternalFormat,
							m_textureWidths[i],
							m_textureHeights[i],
							0,
							m_textureFormat,
							m_textureType,
							(const uchar*)constRef.bits()
							//m_frame->image().scanLine(0)
						);
					}

					//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
 					//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				}

				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			}
		}
	}
	else
	{
		if(!secondSource ? (m_frame->rect()  != m_sourceRect) :
		                   (m_frame2->rect() != m_sourceRect2))
		{
			updateRects(secondSource);
			updateAlignment(secondSource);
		}
	}
	
//  	qDebug() << "GLVideoDrawable::updateTexture(): "<<(QObject*)this<<" Done update";
}


void GLVideoDrawable::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
// 	painter->setPen(pen);
// 	painter->setBrush(brush);
// 	painter->drawRect(m_contentsRect);


	QRectF source = m_sourceRect;
	QRectF target = QRectF(m_targetRect.topLeft() - rect().topLeft(),m_targetRect.size());

	source = source.adjusted(
		m_displayOpts.cropTopLeft.x(),
		m_displayOpts.cropTopLeft.y(),
		m_displayOpts.cropBottomRight.x(),
		m_displayOpts.cropBottomRight.y());

	//const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());

	painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
	painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

	painter->setOpacity(opacity() * (m_fadeActive ? m_fadeValue:1));
	
	if(!m_frame)
		return;

	if(!m_frame->image().isNull())
	{
		painter->drawImage(target,m_frame->image(),source);
		//qDebug() << "GLVideoDrawablle::paint: Painted frame, size:" << m_frame->image().size()<<", source:"<<source<<", target:"<<target;
	}
	else
	{
		const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(m_frame->pixelFormat());
		if(imageFormat != QImage::Format_Invalid)
		{
			QImage image(m_frame->pointer(),
				m_frame->size().width(),
				m_frame->size().height(),
				m_frame->size().width() *
					(imageFormat == QImage::Format_RGB16  ||
					 imageFormat == QImage::Format_RGB555 ||
					 imageFormat == QImage::Format_RGB444 ||
					 imageFormat == QImage::Format_ARGB4444_Premultiplied ? 2 :
					 imageFormat == QImage::Format_RGB888 ||
					 imageFormat == QImage::Format_RGB666 ||
					 imageFormat == QImage::Format_ARGB6666_Premultiplied ? 3 :
					 4),
				imageFormat);
				
			if(m_displayOpts.flipHorizontal || m_displayOpts.flipVertical)
				image = image.mirrored(m_displayOpts.flipHorizontal, m_displayOpts.flipVertical);

			painter->drawImage(target,image,source);
			//qDebug() << "GLVideoDrawable::paint: Painted RAW frame, size:" << image.size()<<", source:"<<source<<", target:"<<target;
		}
		else
		{
			//qDebug() << "GLVideoDrawable::paint: Unable to convert pixel format to image format, cannot paint frame. Pixel Format:"<<m_frame->pixelFormat();
		}
	}

	if(m_fadeActive)
	{
		QRectF source2 = m_sourceRect2;
		QRectF target2 = QRectF(m_targetRect2.topLeft() - rect().topLeft(),m_targetRect2.size());

		source2 = source2.adjusted(
			m_displayOpts.cropTopLeft.x(),
			m_displayOpts.cropTopLeft.y(),
			m_displayOpts.cropBottomRight.x(),
			m_displayOpts.cropBottomRight.y());



		painter->setOpacity(opacity() * (m_fadeActive ? (1.-m_fadeValue):1));

		if(!m_frame2->image().isNull())
		{
			painter->drawImage(target2,m_frame2->image(),source2);
			//qDebug() << "GLVideoDrawablle::paint: Painted frame, size:" << m_frame->image().size()<<", source:"<<source<<", target:"<<target;
		}
		else
		{
			const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(m_frame2->pixelFormat());
			if(imageFormat != QImage::Format_Invalid)
			{
				QImage image(m_frame2->pointer(),
					m_frame2->size().width(),
					m_frame2->size().height(),
					m_frame2->size().width() *
						(imageFormat == QImage::Format_RGB16  ||
						imageFormat == QImage::Format_RGB555 ||
						imageFormat == QImage::Format_RGB444 ||
						imageFormat == QImage::Format_ARGB4444_Premultiplied ? 2 :
						imageFormat == QImage::Format_RGB888 ||
						imageFormat == QImage::Format_RGB666 ||
						imageFormat == QImage::Format_ARGB6666_Premultiplied ? 3 :
						4),
					imageFormat);

				painter->drawImage(target2,image,source2);
				//qDebug() << "GLVideoDrawable::paint: Painted RAW frame, size:" << image.size()<<", source:"<<source<<", target:"<<target;
			}
			else
			{
				//qDebug() << "GLVideoDrawable::paint: Unable to convert pixel format to image format, cannot paint frame. Pixel Format:"<<m_frame->pixelFormat();
			}
		}
	}


	if(!m_frame->captureTime().isNull())
	{
		int msecLatency = m_frame->captureTime().msecsTo(QTime::currentTime());

		m_latencyAccum += msecLatency;
	}

	if (!(m_frameCount % 100))
	{
		QString framesPerSecond;
		framesPerSecond.setNum(m_frameCount /(m_time.elapsed() / 1000.0), 'f', 2);

		QString latencyPerFrame;
		latencyPerFrame.setNum((((double)m_latencyAccum) / ((double)m_frameCount)), 'f', 3);

		if(m_debugFps && framesPerSecond!="0.00")
			qDebug() << "GLVideoDrawable::paint: "<<objectName()<<" FPS: " << qPrintable(framesPerSecond) << (m_frame->captureTime().isNull() ? "" : qPrintable(QString(", Latency: %1 ms").arg(latencyPerFrame)));

		m_time.start();
		m_frameCount = 0;
		m_latencyAccum = 0;

		//lastFrameTime = time.elapsed();
	}

	m_frameCount ++;

	painter->setOpacity(1.);
	GLDrawable::paint(painter, option, widget);

}


void GLVideoDrawable::paintGL()
{
	if(!m_validShader)
	{
		qDebug() << "GLVideoDrawable::paintGL(): "<<(QObject*)this<<" No valid shader, not painting";
		return;
	}

	if(!m_texturesInited)
	{
		qDebug() << "GLVideoDrawable::paintGL(): "<<(QObject*)this<<" Textures not inited, not painting.";
		return;
	}

	if (m_colorsDirty)
	{
		//qDebug() << "Updating color matrix";
		updateColors(m_displayOpts.brightness, m_displayOpts.contrast, m_displayOpts.hue, m_displayOpts.saturation);
		m_colorsDirty = false;
        }

// 	qDebug() << "GLVideoDrawable::paintGL(): "<<(QObject*)this<<" Mark - start";


	//m_frame->unmap()();

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
	//qDebug() << "m_frameSize:"<<m_frameSize;

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

	double liveOpacity = (opacity() * (m_fadeActive ? m_fadeValue - .1 : 1.));

	if(m_useShaders)
	{

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

	}
	else
	{
		if(0)
		{
			QImage texOrig, texGL;
			if ( !texOrig.load( "me2.jpg" ) )
			//if ( !texOrig.load( "Pm5544.jpg" ) )
			{
				texOrig = QImage( 16, 16, QImage::Format_RGB32 );
				texOrig.fill( Qt::green );
			}

			qDebug() << "Loaded test me2.jpg manually using glTexImage2D";
			// Setup inital texture
			texGL = QGLWidget::convertToGLFormat( texOrig );
			glGenTextures( 1, &m_textureIds[0] );
			glBindTexture( GL_TEXTURE_2D, m_textureIds[0] );
			glTexImage2D( GL_TEXTURE_2D, 0, 3, texGL.width(), texGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texGL.bits() );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}

		glColor4f(liveOpacity,liveOpacity,liveOpacity,liveOpacity);

		glEnable(GL_TEXTURE_2D);

		glActiveTexture(GL_TEXTURE0);

	// 	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity(); // Reset The View
		//glTranslatef(0.0f,0.0f,-3.42f);

		glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);

		QPolygonF points = transform.map(QPolygonF(target));
 		//qDebug() << "target: "<<target;
 		//qDebug() << "texture: "<<txLeft<<txBottom<<txTop<<txRight;
		glBegin(GL_QUADS);
// 			qreal
// 				vx1 = target.left(),
// 				vx2 = target.right(),
// 				vy1 = target.bottom(),
// 				vy2 = target.top();

	// 		const GLfloat txLeft   = m_displayOpts.flipHorizontal ? source.right()  / m_frameSize.width() : source.left()  / m_frameSize.width();
	// 		const GLfloat txRight  = m_displayOpts.flipHorizontal ? source.left()   / m_frameSize.width() : source.right() / m_frameSize.width();
	//
	// 		const GLfloat txTop    = !m_displayOpts.flipVertical //m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
	// 			? source.top()    / m_frameSize.height()
	// 			: source.bottom() / m_frameSize.height();
	// 		const GLfloat txBottom = !m_displayOpts.flipVertical //m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
	// 			? source.bottom() / m_frameSize.height()
	// 			: source.top()    / m_frameSize.height();

			glTexCoord2f(txLeft, txBottom); 	glVertex3f(points[2].x(),points[2].y(),  0.0f); // top left
			glTexCoord2f(txRight, txBottom); 	glVertex3f(points[1].x(),points[1].y(),  0.0f); // top right
			glTexCoord2f(txRight, txTop); 		glVertex3f(points[0].x(),points[0].y(),  0.0f); // bottom right
			glTexCoord2f(txLeft, txTop); 		glVertex3f(points[3].x(),points[3].y(),  0.0f); // bottom left

				/*
			glTexCoord2f(0.0f, 0.0f); glVertex3f(vx1,vy1,  0.0f); // bottom left 2
			glTexCoord2f(1.0f, 0.0f); glVertex3f(vx2,vy1,  0.0f); // bottom right 1
			glTexCoord2f(1.0f, 1.0f); glVertex3f(vx2,vy2,  0.0f); // top right 0
			glTexCoord2f(0.0f, 1.0f); glVertex3f(vx1,vy2,  0.0f); // top left 3
	*/
	// 		glTexCoord2f(0,0); glVertex3f( 0, 0,0); //lo
	// 		glTexCoord2f(0,1); glVertex3f(256, 0,0); //lu
	// 		glTexCoord2f(1,1); glVertex3f(256, 256,0); //ru
	// 		glTexCoord2f(1,0); glVertex3f( 0, 256,0); //ro
		glEnd();
	}

 	if(m_fadeActive)
 	{
 		double fadeOpacity = opacity() * (1.0-m_fadeValue);

		QRectF source2 = m_sourceRect2;
		QRectF target2 = m_targetRect2;

		source2 = source2.adjusted(
			m_displayOpts.cropTopLeft.x(),
			m_displayOpts.cropTopLeft.y(),
			m_displayOpts.cropBottomRight.x(),
			m_displayOpts.cropBottomRight.y());


		//QPainter painter(this);
		QTransform transform =  m_glw->transform(); //= painter.deviceTransform();
		//transform = transform.scale(1.25,1.);
		if(!translation().isNull())
			transform *= QTransform().translate(translation().x(),translation().y());

		if(!rotation().isNull())
		{
			qreal tx = target2.width()  * rotationPoint().x() + target2.x();
			qreal ty = target2.height() * rotationPoint().y() + target2.y();
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
			target2.left()     , target2.bottom() + 1, //(GLfloat)zIndex(),
			target2.right() + 1, target2.bottom() + 1, //(GLfloat)zIndex(),
			target2.left()     , target2.top(), 	//(GLfloat)zIndex(),
			target2.right() + 1, target2.top()//, 	(GLfloat)zIndex()
		};

		const GLfloat txLeft   = m_displayOpts.flipHorizontal ? source2.right()  / m_frameSize2.width() : source2.left()  / m_frameSize2.width();
		const GLfloat txRight  = m_displayOpts.flipHorizontal ? source2.left()   / m_frameSize2.width() : source2.right() / m_frameSize2.width();

		const GLfloat txTop    = !m_displayOpts.flipVertical //m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
			? source2.top()    / m_frameSize2.height()
			: source2.bottom() / m_frameSize2.height();
		const GLfloat txBottom = !m_displayOpts.flipVertical //m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
			? source2.bottom() / m_frameSize2.height()
			: source2.top()    / m_frameSize2.height();

		const GLfloat textureCoordArray[] =
		{
			txLeft , txBottom,
			txRight, txBottom,
			txLeft , txTop,
			txRight, txTop
		};

		if(m_useShaders)
		{

			m_program2->bind();

			m_program2->enableAttributeArray("vertexCoordArray");
			m_program2->enableAttributeArray("textureCoordArray");

			m_program2->setAttributeArray("vertexCoordArray",  vertexCoordArray,  2);
			m_program2->setAttributeArray("textureCoordArray", textureCoordArray, 2);

			m_program2->setUniformValue("positionMatrix",      positionMatrix);
		// 	QMatrix4x4 mat4(
		// 		positionMatrix[0][0], positionMatrix[0][1], positionMatrix[0][2], positionMatrix[0][3],
		// 		positionMatrix[1][0], positionMatrix[1][1], positionMatrix[1][2], positionMatrix[1][3],
		// 		positionMatrix[2][0], positionMatrix[2][1], positionMatrix[2][2], positionMatrix[2][3],
		// 		positionMatrix[3][0], positionMatrix[3][1], positionMatrix[3][2], positionMatrix[3][3]
		// 		);
		// 	m_program->setUniformValue("positionMatrix",      mat4);

			//qDebug() << "GLVideoDrawable:paintGL():"<<this<<", rendering with opacity:"<<opacity();
			m_program2->setUniformValue("alpha",               (GLfloat)fadeOpacity);
			m_program2->setUniformValue("texOffsetX",          (GLfloat)m_invertedOffset.x());
			m_program2->setUniformValue("texOffsetY",          (GLfloat)m_invertedOffset.y());


			if (m_textureCount2 == 3)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_textureIds2[0]);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, m_textureIds2[1]);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, m_textureIds2[2]);

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
				glBindTexture(GL_TEXTURE_2D, m_textureIds2[0]);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, m_alphaTextureId);

				glActiveTexture(GL_TEXTURE0);

				m_program->setUniformValue("texRgb", 0);
				m_program->setUniformValue("alphaMask", 1);
			}
			m_program->setUniformValue("colorMatrix", m_colorMatrix);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			m_program->release();

		}
		else
		{
			if(0)
			{
				QImage texOrig, texGL;
				if ( !texOrig.load( "me2.jpg" ) )
				//if ( !texOrig.load( "Pm5544.jpg" ) )
				{
					texOrig = QImage( 16, 16, QImage::Format_RGB32 );
					texOrig.fill( Qt::green );
				}

				qDebug() << "Loaded test me2.jpg manually using glTexImage2D";
				// Setup inital texture
				texGL = QGLWidget::convertToGLFormat( texOrig );
				glGenTextures( 1, &m_textureIds2[0] );
				glBindTexture( GL_TEXTURE_2D, m_textureIds2[0] );
				glTexImage2D( GL_TEXTURE_2D, 0, 3, texGL.width(), texGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texGL.bits() );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			}

			glColor4f(fadeOpacity,fadeOpacity,fadeOpacity,fadeOpacity);

			glEnable(GL_TEXTURE_2D);

			glActiveTexture(GL_TEXTURE0);

		// 	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
		// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glLoadIdentity(); // Reset The View
			//glTranslatef(0.0f,0.0f,-3.42f);

			glBindTexture(GL_TEXTURE_2D, m_textureIds2[0]);

			target2 = transform.mapRect(target2);
			//qDebug() << "target: "<<target;
			//qDebug() << "texture: "<<txLeft<<txBottom<<txTop<<txRight;
			glBegin(GL_QUADS);
				qreal
					vx1 = target2.left(),
					vx2 = target2.right(),
					vy1 = target2.bottom(),
					vy2 = target2.top();

		// 		const GLfloat txLeft   = m_displayOpts.flipHorizontal ? source.right()  / m_frameSize.width() : source.left()  / m_frameSize.width();
		// 		const GLfloat txRight  = m_displayOpts.flipHorizontal ? source.left()   / m_frameSize.width() : source.right() / m_frameSize.width();
		//
		// 		const GLfloat txTop    = !m_displayOpts.flipVertical //m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
		// 			? source.top()    / m_frameSize.height()
		// 			: source.bottom() / m_frameSize.height();
		// 		const GLfloat txBottom = !m_displayOpts.flipVertical //m_scanLineDirection == QVideoSurfaceFormat::TopToBottom
		// 			? source.bottom() / m_frameSize.height()
		// 			: source.top()    / m_frameSize.height();

				glTexCoord2f(txLeft, txBottom); 	glVertex3f(vx1,vy1,  0.0f); // top left
				glTexCoord2f(txRight, txBottom); 	glVertex3f(vx2,vy1,  0.0f); // top right
				glTexCoord2f(txRight, txTop); 		glVertex3f(vx2,vy2,  0.0f); // bottom right
				glTexCoord2f(txLeft, txTop); 		glVertex3f(vx1,vy2,  0.0f); // bottom left

		// 		glTexCoord2f(0,0); glVertex3f( 0, 0,0); //lo
		// 		glTexCoord2f(0,1); glVertex3f(256, 0,0); //lu
		// 		glTexCoord2f(1,1); glVertex3f(256, 256,0); //ru
		// 		glTexCoord2f(1,0); glVertex3f( 0, 256,0); //ro
			glEnd();
		}
 	}


	//renderText(10, 10, qPrintable(QString("%1 fps").arg(framesPerSecond)));

	if(!m_frame->captureTime().isNull())
	{
		int msecLatency = m_frame->captureTime().msecsTo(QTime::currentTime());

		m_latencyAccum += msecLatency;
	}

	if (!(m_frameCount % 100))
	{
		QString framesPerSecond;
		framesPerSecond.setNum(m_frameCount /(m_time.elapsed() / 1000.0), 'f', 2);

		QString latencyPerFrame;
		latencyPerFrame.setNum((((double)m_latencyAccum) / ((double)m_frameCount)), 'f', 3);

		if(m_debugFps && framesPerSecond!="0.00")
			qDebug() << "GLVideoDrawable::paintGL: "<<objectName()<<" FPS: " << qPrintable(framesPerSecond) << (m_frame->captureTime().isNull() ? "" : qPrintable(QString(", Latency: %1 ms").arg(latencyPerFrame)));

		m_time.start();
		m_frameCount = 0;
		m_latencyAccum = 0;

		//lastFrameTime = time.elapsed();
	}

	m_frameCount ++;
	
// 	qDebug() << "GLVideoDrawable::paintGL(): "<<(QObject*)this<<" Mark - end";
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

