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
	
	Q_PROPERTY(VideoSource* videoSource READ videoSource WRITE setVideoSource);
	
	Q_PROPERTY(Qt::AspectRatioMode aspectRatioMode READ aspectRatioMode WRITE setAspectRatioMode);
	
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
	
	// This is the normal size of the content in pixels - independent of the specified rect().
	// E.g. if its an image, this is the size of the image, if this is text, then the size
	// of the text unscaled at natural resolution. Used for calculating alignment.
	virtual QSizeF naturalSize() { return m_sourceRect.size(); }
	
	float fpsLimit() { return m_rateLimitFps; }
	
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
	
	void setVideoSource(VideoSource*);
	void disconnectVideoSource();
	
	void setDisplayOptions(const VideoDisplayOptions&);
	
	// Returns false if format won't be acceptable
	bool setVideoFormat(const VideoFormat& format);

signals:
	void displayOptionsChanged(const VideoDisplayOptions&);

protected:
	void paintGL();
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
	
	void viewportResized(const QSize& newSize);
	void canvasResized(const QSizeF& newSize);
	void drawableResized(const QSizeF& newSize);
	
	void initGL();

	void initYv12TextureInfo(const QSize &size);
	void initYuv420PTextureInfo(const QSize &size);
	void initRgbTextureInfo(GLenum internalFormat, GLuint format, GLenum type, const QSize &size);
	
	void updateColors(int brightness, int contrast, int hue, int saturation);
	
	void updateRects();
	
	const char * resizeTextures(const QSize& size);
	
	void updateTextureOffsets();
	
	VideoFrame m_frame;
	bool m_visiblePendingFrame;
	bool m_tempVisibleValue;
	
	// QGraphicsItem::
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
 
	
	
protected slots:
	void frameReady();
	void updateTexture();
	
	
private:
	VideoFormat m_videoFormat;
	bool m_glInited;
	
	QGLShaderProgram *m_program;
	
	QList<QVideoFrame::PixelFormat> m_imagePixelFormats;
	QList<QVideoFrame::PixelFormat> m_rawPixelFormats;
	QMatrix4x4 m_colorMatrix;
	//QGLContext *m_context;
	
	QVideoSurfaceFormat::Direction m_scanLineDirection;
	GLenum m_textureFormat;
	GLuint m_textureInternalFormat;
	GLenum m_textureType;
	
	int m_textureCount;
	
	GLuint m_textureIds[3];
	int m_textureWidths[3];
	int m_textureHeights[3];
	int m_textureOffsets[3];
	
	bool m_yuv;
	
	bool m_colorsDirty;
	
	VideoDisplayOptions m_displayOpts;
	
	QSize m_frameSize;
	
	#ifndef QT_OPENGL_ES
	typedef void (APIENTRY *_glActiveTexture) (GLenum);
	_glActiveTexture glActiveTexture;
	#endif
	
	VideoSource *m_source;
	
	QTime m_time;
	int m_frameCount;
	int m_latencyAccum;
	bool m_debugFps;
	
	QTimer m_timer;
	
	QRectF m_targetRect;
	QRectF m_sourceRect;
	
	Qt::AspectRatioMode m_aspectRatioMode;
	
	bool m_validShader;
	
	QImage m_alphaMask;
	QImage m_alphaMask_preScaled;
	GLuint m_alphaTextureId;
	qint64 m_uploadedCacheKey;
	
	QPointF m_textureOffset;
	QPointF m_invertedOffset; // calculated as m_textureOffset.x() * 1/textureWidth, etc
	
	bool m_texturesInited;
	QMutex m_frameReadyLock;
	
	bool m_useShaders;
	
	float m_rateLimitFps;
	QTimer m_fpsRateLimiter;
};

#endif 

