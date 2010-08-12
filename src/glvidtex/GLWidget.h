#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtGui>
#include <QGLWidget>
#include <QVideoSurfaceFormat>
#include <QGLShaderProgram>
#include "../livemix/VideoFrame.h"
#include "../livemix/VideoSource.h"


class QGLShaderProgram;
class GLWidget;

class GLDrawable : public QObject
{
	Q_OBJECT
	//Q_PROPERTY(QString itemName READ itemName WRITE setItemName);
	Q_PROPERTY(QRectF rect READ rect WRITE setRect);
	Q_PROPERTY(double zIndex READ zIndex WRITE setZIndex);
	Q_PROPERTY(double opacity READ opacity WRITE setOpacity);
public:
	GLDrawable(QObject *parent=0);
	
	void updateGL();
	
	GLWidget *glWidget() { return m_glw; }
	
	const QRectF & rect() const { return m_rect; }
	double zIndex() { return m_zIndex; }
	
	double opacity() { return m_opacity; }
	
public slots:
	void setRect(const QRectF& rect);
	void setOpacity(double i);
	void setZIndex(double z);

signals:
	void zIndexChanged(double newZIndex);
	void drawableResized(const QSize& newSize);
	
protected:
	friend class GLWidget;
	virtual void setGLWidget(GLWidget*); // when update is called, it calls GLWidget::updateGL()
	
	virtual void viewportResized(const QSize& newSize);
	virtual void drawableResized(const QSizeF& newSize);
	
	virtual void paintGL();
	virtual void initGL();
		
	GLWidget *m_glw;

private:
	QRectF m_rect;
	double m_zIndex;
	double m_opacity;
	
};



class StaticVideoSource : public VideoSource
{
	Q_OBJECT
	
	Q_PROPERTY(QImage image READ image WRITE setImage);

public:
	StaticVideoSource(QObject *parent=0);
	//virtual ~StaticVideoSource() {}

	VideoFormat videoFormat() { return VideoFormat(VideoFrame::BUFFER_IMAGE,QVideoFrame::Format_ARGB32); }
	
	const QImage & image() { return m_image; }

public slots:
	void setImage(const QImage&);
	
signals:
	void frameReady();

protected:
	void run();
	
private:
	QImage m_image;
	VideoFrame m_frame;
};

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


class GLVideoDrawable : public GLDrawable
{
	Q_OBJECT
	
	Q_PROPERTY(int brightness READ brightness WRITE setBrightness);
	Q_PROPERTY(int contrast READ contrast WRITE setContrast);
	Q_PROPERTY(int hue READ hue WRITE setHue);
	Q_PROPERTY(int saturation READ saturation WRITE setSaturation);
	
public:
	GLVideoDrawable(QObject *parent=0);
	~GLVideoDrawable();
	
	const VideoFormat& videoFormat();
	
	int brightness() const;
	int contrast() const;
	int hue() const;
	int saturation() const;
	
	VideoSource *videoSource();
	
	VideoDisplayOptions displayOptions() { return m_displayOpts; }
	
public slots:
	void setBrightness(int brightness);
	void setContrast(int contrast);
	void setHue(int hue);
	void setSaturation(int saturation);
	
	void setVideoSource(VideoSource*);
	void disconnectVideoSource();
	
	void setDisplayOptions(const VideoDisplayOptions&);
	
	// Returns false if format won't be acceptable
	bool setVideoFormat(const VideoFormat& format);

signals:
	void displayOptionsChanged(const VideoDisplayOptions&);

protected:
	void paintGL();
	
	void viewportResized(const QSize& newSize);
	
	void initGL();

	void initYv12TextureInfo(const QSize &size);
	void initYuv420PTextureInfo(const QSize &size);
	void initRgbTextureInfo(GLenum internalFormat, GLuint format, GLenum type, const QSize &size);
	
	void updateColors(int brightness, int contrast, int hue, int saturation);
	
	void updateRects();
	
	const char * resizeTextures(const QSize& size);

protected slots:
	void frameReady();
	
private:
	VideoFormat m_videoFormat;
	bool m_glInited;
	
	QGLShaderProgram m_program;
	
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
	
	QImage m_sampleTexture;
	
	QSize m_frameSize;
	
	#ifndef QT_OPENGL_ES
	typedef void (APIENTRY *_glActiveTexture) (GLenum);
	_glActiveTexture glActiveTexture;
	#endif
	
	VideoFrame m_frame;
	VideoSource *m_source;
	
	QTime m_time;
	int m_frameCount;
	int m_latencyAccum;
	
	QTimer m_timer;
	
	QRectF m_targetRect;
	QRectF m_sourceRect;
	
	Qt::AspectRatioMode m_aspectRatioMode;
	
	bool m_validShader;
};

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

class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent = 0, QGLWidget *shareWidget = 0);
	~GLWidget();
	
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	
	void addDrawable(GLDrawable *);
	void removeDrawable(GLDrawable*);
	
	QTransform transform() { return m_transform; }
	void setTransform(const QTransform&);
	
	const QRectF & viewport() const { return m_viewport; }
	void setViewport(const QRectF&);
	
signals:
	void clicked();

protected slots:
	void zIndexChanged();
	
protected:
	void sortDrawables();
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	

private:
// 	QPoint lastPos;
// 	int xRot;
// 	int yRot;
// 	int zRot;
	
	QList<GLDrawable*> m_drawables;
	
	bool m_glInited;
	
	QTransform m_transform;
	QRectF m_viewport;
	
};

#endif
