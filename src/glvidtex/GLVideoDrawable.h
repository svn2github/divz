#ifndef GLVIDEODRAWABLE_H
#define GLVIDEODRAWABLE_H

#include <QtGui>

#include <QVideoFrame>
#include <QVideoSurfaceFormat>
#include <QGLShaderProgram>
#include "../livemix/VideoFrame.h"
#include "../livemix/VideoSource.h"

#include "GLDrawable.h"

class VideoDisplayOptions
{
public:
	VideoDisplayOptions(
		bool flipX=false, 
		bool flipY=false, 
		QPointF cropTL = QPointF(0,0), 
		QPointF cropBR = QPointF(0,0), 
		int b=0, 
		int c=0, 
		int h=0, 
		int s=0
	)
	:	flipHorizontal(flipX)
	,	flipVertical(flipY)
	,	cropTopLeft(cropTL)
	,	cropBottomRight(cropBR)
	,	brightness(b)
	,	contrast(c)
	,	hue(h)
	,	saturation(s)
	{}
	
	bool flipHorizontal;
	bool flipVertical;
	QPointF cropTopLeft;
	QPointF cropBottomRight;
	int brightness;
	int contrast;
	int hue;
	int saturation;
	
	QByteArray toByteArray();
	void fromByteArray(QByteArray);
};
QDebug operator<<(QDebug dbg, const VideoDisplayOptions &);

class GLVideoDrawable;
class VideoDisplayOptionWidget : public QWidget
{
	Q_OBJECT
public:
	VideoDisplayOptionWidget(GLVideoDrawable *drawable, QWidget *parent=0);
	VideoDisplayOptionWidget(const VideoDisplayOptions&, QWidget *parent=0);
	
signals:
	void displayOptionsChanged(const VideoDisplayOptions&);

public slots:
	void setDisplayOptions(const VideoDisplayOptions&);
	void undoChanges();
	
private slots:
	void flipHChanged(bool);
	void flipVChanged(bool);
	void cropX1Changed(int);
	void cropY1Changed(int);
	void cropX2Changed(int);
	void cropY2Changed(int);
	void bChanged(int);
	void cChanged(int);
	void hChanged(int);
	void sChanged(int);

private:
	void initUI();
	
	VideoDisplayOptions m_optsOriginal;
	VideoDisplayOptions m_opts;
	GLVideoDrawable *m_drawable;
	
	QCheckBox *m_cbFlipH;
	QCheckBox *m_cbFlipV;
	QSpinBox *m_spinCropX1;
	QSpinBox *m_spinCropX2;
	QSpinBox *m_spinCropY1;
	QSpinBox *m_spinCropY2;
	QSpinBox *m_spinB;
	QSpinBox *m_spinC;
	QSpinBox *m_spinH;
	QSpinBox *m_spinS;
	
};

class GLVideoDrawable : public GLDrawable
{
	Q_OBJECT
	
	Q_PROPERTY(int brightness READ brightness WRITE setBrightness);
	Q_PROPERTY(int contrast READ contrast WRITE setContrast);
	Q_PROPERTY(int hue READ hue WRITE setHue);
	Q_PROPERTY(int saturation READ saturation WRITE setSaturation);
	
	Q_PROPERTY(bool flipHorizontal READ flipHorizontal WRITE setFlipHorizontal);
	Q_PROPERTY(bool flipVertical READ flipVertical WRITE setFlipVertical);
	
	Q_PROPERTY(QPointF cropTopLeft READ cropTopLeft WRITE setCropTopLeft);
	Q_PROPERTY(QPointF cropBottomRight READ cropBottomRight WRITE setCropBottomRight);
	
	Q_PROPERTY(QPointF textureOffset READ textureOffset WRITE setTextureOffset);
	
	//Q_PROPERTY(VideoSource* videoSource READ videoSource WRITE setVideoSource);
	
	Q_PROPERTY(Qt::AspectRatioMode aspectRatioMode READ aspectRatioMode WRITE setAspectRatioMode);
	Q_PROPERTY(bool ignoreAspectRatio READ ignoreAspectRatio WRITE setIgnoreAspectRatio);
	
	Q_PROPERTY(bool xfadeEnabled READ xfadeEnabled WRITE setXFadeEnabled);
	Q_PROPERTY(int xfadeLength READ xfadeLength WRITE setXFadeLength);
	
	Q_PROPERTY(bool videoSenderEnabled READ videoSenderEnabled WRITE setVideoSenderEnabled);
	Q_PROPERTY(int videoSenderPort READ videoSenderPort WRITE setVideoSenderPort);
	
	
public:
	GLVideoDrawable(QObject *parent=0);
	~GLVideoDrawable();
	
	const VideoFormat& videoFormat();
	
	int brightness() const;
	int contrast() const;
	int hue() const;
	int saturation() const;
	
	bool flipHorizontal() { return m_displayOpts.flipHorizontal; }
	bool flipVertical() { return m_displayOpts.flipVertical; }
	
	const QPointF & cropTopLeft() { return m_displayOpts.cropTopLeft; }
	const QPointF & cropBottomRight() { return m_displayOpts.cropBottomRight; }
	
	VideoSource *videoSource() { return m_source; }
	
	VideoDisplayOptions displayOptions() { return m_displayOpts; }
	
	QImage alphaMask() { return m_alphaMask; }
	
	QPointF textureOffset() { return m_textureOffset; }
	
	Qt::AspectRatioMode aspectRatioMode() { return m_aspectRatioMode; }
	bool ignoreAspectRatio() { return m_ignoreAspectRatio; }
	
	// This is the normal size of the content in pixels - independent of the specified rect().
	// E.g. if its an image, this is the size of the image, if this is text, then the size
	// of the text unscaled at natural resolution. Used for calculating alignment.
	virtual QSizeF naturalSize() { return m_sourceRect.size(); }
	
	float fpsLimit() { return m_rateLimitFps; }
	
	bool xfadeEnabled() { return m_xfadeEnabled; }
	int xfadeLength() { return m_xfadeLength; }
	
	bool videoSenderEnabled() { return m_videoSenderEnabled; }
	int videoSenderPort() { return m_videoSenderPort; }
	
public slots:
	void setFpsLimit(float);
	void setVisible(bool flag, bool waitOnFrameSignal=false);
	
	void setAlphaMask(const QImage&);
	void setTextureOffset(double x, double y);
	void setTextureOffset(const QPointF&);

	void setBrightness(int brightness);
	void setContrast(int contrast);
	void setHue(int hue);
	void setSaturation(int saturation);
	
	void setFlipHorizontal(bool);
	void setFlipVertical(bool);
	void setCropTopLeft(QPointF);
	void setCropBottomRight(QPointF);
	
	void setAspectRatioMode(Qt::AspectRatioMode mode);
	void setIgnoreAspectRatio(bool flag);
	
	void setXFadeEnabled(bool flag);
	void setXFadeLength(int length);
	
	/// PS
	void setVideoSource(VideoSource*);
	void disconnectVideoSource();
	
	void setDisplayOptions(const VideoDisplayOptions&);
	
	/// PS
	// Returns false if format won't be acceptable
	bool setVideoFormat(const VideoFormat& format, bool secondSource=false);
	
	void setVideoSenderEnabled(bool);
	void setVideoSenderPort(int);

signals:
	void displayOptionsChanged(const VideoDisplayOptions&);
	void sourceDiscarded(VideoSource*);

protected:
	/// PS
	void paintGL();
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
	
	virtual void viewportResized(const QSize& newSize);
	virtual void canvasResized(const QSizeF& newSize);
	virtual void drawableResized(const QSizeF& newSize);
	
	void initGL();

	/// PS
	void initYv12TextureInfo(const QSize &size, bool secondSource=false);
	void initYuv420PTextureInfo(const QSize &size, bool secondSource=false);
	void initRgbTextureInfo(GLenum internalFormat, GLuint format, GLenum type, const QSize &size, bool secondSource=false);
	
	void updateColors(int brightness, int contrast, int hue, int saturation);
	
	/// PS
	virtual void updateRects(bool secondSource=false);
	
	/// PS
	const char * resizeTextures(const QSize& size, bool secondSource=false);
	
	void updateTextureOffsets();
	
	/// PS
	VideoFrame m_frame;
	VideoFrame m_frame2;
	bool m_visiblePendingFrame;
	bool m_tempVisibleValue;
	
	Qt::AspectRatioMode m_aspectRatioMode;
	
	VideoDisplayOptions m_displayOpts;
	
	/// PS
	QRectF m_targetRect;
	QRectF m_targetRect2;
	/// PS
	QRectF m_sourceRect;
	QRectF m_sourceRect2;
	
	
	// QGraphicsItem::
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
 
	void xfadeStart();
	void xfadeStop();
	
protected slots:
	/// PS
	void frameReady();
	void frameReady2();
	void updateTexture(bool secondSource=false);
	
	void disconnectVideoSource2();
	void xfadeTick();
	
	
private:
	/// PS
	VideoFormat m_videoFormat;
	VideoFormat m_videoFormat2;
	bool m_glInited;
	
	/// PS
	QGLShaderProgram *m_program;
	QGLShaderProgram *m_program2;
	
	QList<QVideoFrame::PixelFormat> m_imagePixelFormats;
	QList<QVideoFrame::PixelFormat> m_rawPixelFormats;
	
	QMatrix4x4 m_colorMatrix;
	//QGLContext *m_context;
	
	/// PS
	QVideoSurfaceFormat::Direction m_scanLineDirection;
	/// PS
	GLenum m_textureFormat;
	GLenum m_textureFormat2;
	/// PS
	GLuint m_textureInternalFormat;
	GLuint m_textureInternalFormat2;
	/// PS
	GLenum m_textureType;
	GLenum m_textureType2;
	
	/// PS
	int m_textureCount;
	int m_textureCount2;
	
	/// PS
	GLuint m_textureIds[3];
	GLuint m_textureIds2[3];
	/// PS
	int m_textureWidths[6];
	/// PS
	int m_textureHeights[6];
	/// PS
	int m_textureOffsets[6];
	
	/// PS
	bool m_yuv;
	bool m_yuv2;
	
	bool m_colorsDirty;
	
	/// PS
	QSize m_frameSize;
	QSize m_frameSize2;
	
	#ifndef QT_OPENGL_ES
	typedef void (APIENTRY *_glActiveTexture) (GLenum);
	_glActiveTexture glActiveTexture;
	#endif
	
	/// PS
	VideoSource *m_source;
	VideoSource *m_source2;
	
	QTime m_time;
	int m_frameCount;
	int m_latencyAccum;
	bool m_debugFps;
	
	QTimer m_timer;
	
	/// PS
	bool m_validShader;
	bool m_validShader2;
	
	QImage m_alphaMask;
	QImage m_alphaMask_preScaled;
	GLuint m_alphaTextureId;
	qint64 m_uploadedCacheKey;
	
	QPointF m_textureOffset;
	QPointF m_invertedOffset; // calculated as m_textureOffset.x() * 1/textureWidth, etc
	
	/// PS
	bool m_texturesInited;
	bool m_texturesInited2;
	
	QMutex m_frameReadyLock;
	
	bool m_useShaders;
	
	float m_rateLimitFps;
	QTimer m_fpsRateLimiter;
	
	// If m_xfadeEnabled, then when setVideoSource() is called,
	// m_fadeTick will be started at 1000/25 ms interval and
	// m_fadeTime will be started. The xfadeTick() slot will 
	// be called to update m_fadeValue while m_fadeTime.elapsed()
	// is less than m_xfadeLength; While fade is active,
	// m_fadeActive is true, which will tell the paint routines
	// to honor m_fadeValue. When fade is finished, xfadeDone()
	// slot is called and the second source is discarded. 
	bool m_xfadeEnabled;
	int m_xfadeLength; // in ms
	QTimer m_fadeTick;
	QTime m_fadeTime;
	double m_fadeValue;
	bool m_fadeActive;
	
	class VideoSender *m_videoSender;
	bool m_videoSenderEnabled;
	int m_videoSenderPort;
	
	bool m_ignoreAspectRatio;
	
	static int m_videoSenderPortAllocator;
};

#endif 

