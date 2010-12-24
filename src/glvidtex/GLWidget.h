#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtGui>
#include <QGLWidget>
#include <QGLShaderProgram>

#include "../livemix/VideoSource.h"

class QGLFramebufferObject;

class GLDrawable;

enum GLRotateValue
{
	GLRotateLeft =-1,
	GLRotateNone = 0,
	GLRotateRight=+1
};
	

class GLWidget;
class GLWidgetOutputStream : public VideoSource
{
	Q_OBJECT
	
protected:
	friend class GLWidget;
	GLWidgetOutputStream(GLWidget *parent=0);
	void setImage(QImage);

public:
	//virtual ~StaticVideoSource() {}

	VideoFormat videoFormat() { return VideoFormat(VideoFrame::BUFFER_IMAGE,QVideoFrame::Format_ARGB32); }
	//VideoFormat videoFormat() { return VideoFormat(VideoFrame::BUFFER_BYTEARRAY,QVideoFrame::Format_ARGB32); }
	
	const QImage & image() { return m_image; }
	int fps() { return m_fps; }

public slots:
	void setFps(int);

signals:
	void frameReady();

private:
	GLWidget *m_glWidget;
	QImage m_image;
	int m_fps;
	QTimer m_frameReadyTimer;
};

class GLWidgetSubview : public QObject
{
	Q_OBJECT
public:
	GLWidgetSubview();
	
	QByteArray toByteArray();
	void fromByteArray(QByteArray&);
	
	int subviewId();
	QString title() const { return m_title; }
	
	GLWidget *glWidget() { return m_glw; }
	
	/* TLBR is stored as a fraction of the FBO size.
	   E.g. TLBR of (0,0,.5,.5) means the subview covers the top-left 50% of the FBO, or the upper-left 1/4th of the FBO */
	double top() const { return m_viewTop; }
	double left() const { return m_viewLeft; }
	double bottom() const { return m_viewBottom; }
	double right() const { return m_viewRight; }
	
	
	bool flipHorizontal() { return m_flipHorizontal; }
	bool flipVertical() { return m_flipVertical; }
	
	// When this is changed, the GLWidget must re-upload to the GPU.
	// When a subview is created, a cooresponding texture ID must be
	// created and stored in the subview
	QImage alphaMask() { return m_alphaMask; }
	QString alphaMaskFile() { return m_alphaMaskFile; }
	
	// When colors are changed, the color matrix needs recalculated
	// and stored in the subview
	int brightness() const { return m_brightness; }
	int contrast() const { return m_contrast; }
	int hue() const { return m_hue; }
	int saturation() const { return m_saturation; }
	
	// When corner translations are changed, the warp matrix
	// must be reclaculated and stored in the subview 
	const QPolygonF & cornerTranslations() { return m_cornerTranslations; }
	
	GLRotateValue cornerRotation() { return m_cornerRotation; }
	
	// for use in re-transmitting to players
signals:
	void changed(GLWidgetSubview *);
	
public slots:	
	void setTitle(const QString&);
	
	void setTop(double);
	void setLeft(double);
	void setBottom(double);
	void setRight(double);
	
	void setTopPercent(double d)    { setTop(d/100.); }
	void setLeftPercent(double d)   { setLeft(d/100.); }
	void setBottomPercent(double d) { setBottom(d/100.); }
	void setRightPercent(double d)  { setRight(d/100.); }
	
	void setAlphaMaskFile(const QString&);
	void setAlphaMask(const QImage&);
	
	void setBrightness(int brightness);
	void setContrast(int contrast);
	void setHue(int hue);
	void setSaturation(int saturation);
	
	void setFlipHorizontal(bool flip);
	void setFlipVertical(bool flip);
	
	void setCornerTranslations(const QPolygonF&);
	
	void setCornerRotation(GLRotateValue);
	
protected:
	friend class GLWidget;
	// Calling setGLWidget initalizes the alpha mask if GL already inited,
	// otherwise initGL takes care of the alpha init 
	void setGLWidget(GLWidget*);
	void initGL();
	
	void updateWarpMatrix();
	void upateAlphaMask();
	void initAlphaMask();
	void updateColors();
	
	QRectF targetRect();
	
	GLfloat m_warpMatrix[4][4];
	
	QMatrix4x4 m_colorMatrix;
	bool m_colorsDirty;
	
	QImage m_alphaMask;
	QImage m_alphaMask_preScaled;
	GLuint m_alphaTextureId;
		
	QPolygonF m_cornerTranslations;
	
	double m_viewTop;
	double m_viewLeft;
	double m_viewBottom;
	double m_viewRight;
	
	bool m_flipHorizontal;
	bool m_flipVertical;
// 	QPointF cropTopLeft;
// 	QPointF cropBottomRight;
	int m_brightness;
	int m_contrast;
	int m_hue;
	int m_saturation;
	
	GLRotateValue m_cornerRotation;
	
private:
	QString m_alphaMaskFile;
	
	GLWidget *m_glw;
	
	int m_id;
	QString m_title;
	
	qint64 m_uploadedCacheKey;

};


class GLWidget : public QGLWidget
{
	Q_OBJECT

	// Coordinates go through several stages:
	// The physical window on the screen is often a fixed size that can't be configured or changed - a GLWidget fit to the screen or a window.
	// The GLWidget than computes a transform to fit the viewport (or the entire canvas if the viewport is invalid) into an aspect-ratio correct
	// rectangle. If the viewport doesnt match the canvas, it will set up a transform to first translate the viewport into the canvas, then 
	// apply the scaling and translation for the window.
	
	// This is the usable area for arranging layers.
	Q_PROPERTY(QSizeF canvasSize READ canvasSize WRITE setCanvasSize);
	
	// Viewport is an "instructive" property - can be used by GLWidgets to look at a subset of the canvas.
	// By default, the viewport is an invalid rectangle, which will instruct the GLWidget to look at the entire canvas.
	Q_PROPERTY(QRectF viewport READ viewport WRITE setViewport);
	
	Q_PROPERTY(Qt::AspectRatioMode aspectRatioMode READ aspectRatioMode WRITE setAspectRatioMode);
	
	Q_PROPERTY(int brightness READ brightness WRITE setBrightness);
	Q_PROPERTY(int contrast READ contrast WRITE setContrast);
	Q_PROPERTY(int hue READ hue WRITE setHue);
	Q_PROPERTY(int saturation READ saturation WRITE setSaturation);
	
	Q_PROPERTY(bool flipHorizontal READ flipHorizontal WRITE setFlipHorizontal);
	Q_PROPERTY(bool flipVertical READ flipVertical WRITE setFlipVertical);
	
public:
	GLWidget(QWidget *parent = 0, QGLWidget *shareWidget = 0);
	~GLWidget();
	
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
	
	GLWidgetSubview *defaultSubview(); 

	void addDrawable(GLDrawable *);
	void removeDrawable(GLDrawable*);
	
 	QList<GLDrawable*> drawables() { return m_drawables; }
	
 	QTransform transform() { return m_transform; }
	void setTransform(const QTransform&);
	
 	const QRectF & viewport() const { return m_viewport; }
 	const QSizeF & canvasSize() const { return m_canvasSize; }
 	const QRectF canvasRect() const { return QRectF(QPointF(0,0), m_canvasSize); }
	
	void makeRenderContextCurrent();
	
	const QPolygonF & cornerTranslations() { return defaultSubview()->cornerTranslations(); }
	
 	Qt::AspectRatioMode aspectRatioMode() { return m_aspectRatioMode; }
	
	QImage alphaMask() { return defaultSubview()->alphaMask(); }
	
	int brightness() { return defaultSubview()->brightness(); }
	int contrast() { return defaultSubview()->contrast(); }
	int hue() { return defaultSubview()->hue(); }
	int saturation() { return defaultSubview()->saturation(); }
	
	bool flipHorizontal() { return defaultSubview()->flipHorizontal(); }
	bool flipVertical() { return defaultSubview()->flipVertical(); }
	
	GLRotateValue cornerRotation() { return defaultSubview()->cornerRotation(); }
	
	QList<GLWidgetSubview*> subviews() { return m_subviews; }
	GLWidgetSubview *subview(int subviewId);
	
	bool glInited() { return m_glInited; }
	
	GLWidgetOutputStream *outputStream();
	
	QImage toImage();

signals:
	void clicked();
	void canvasSizeChanged(const QSizeF&);
	void viewportChanged(const QRectF&);
	void updated();

public slots:
	void setViewport(const QRectF&);
	void setCanvasSize(const QSizeF&);
	void setCanvasSize(double x,double y) { setCanvasSize(QSizeF(x,y)); }
	
	void setCornerTranslations(const QPolygonF&);
	
	void setTopLeftTranslation(const QPointF&);
	void setTopRightTranslation(const QPointF&);
	void setBottomLeftTranslation(const QPointF&);
	void setBottomRightTranslation(const QPointF&);
	
	void setAspectRatioMode(Qt::AspectRatioMode mode);
	
	void setAlphaMask(const QImage&);
	
	void setBrightness(int brightness);
	void setContrast(int contrast);
	void setHue(int hue);
	void setSaturation(int saturation);
	
	void setFlipHorizontal(bool flip);
	void setFlipVertical(bool flip);
	
	void setCornerRotation(GLRotateValue);
	
 	void addSubview(GLWidgetSubview*);
 	void removeSubview(GLWidgetSubview*);
	
protected slots:
	void zIndexChanged();
	
	void postInitGL();
	
protected:
	void sortDrawables();
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void showEvent(QShowEvent *);
	
	//void updateColors(int brightness, int contrast, int hue, int saturation);

private:
	void initShaders();
	//void initAlphaMask();
	//void updateWarpMatrix();
	
	QList<GLDrawable*> m_drawables;
	
	bool m_glInited;
	
	QTransform m_transform;
	QRectF m_viewport;
	QSizeF m_canvasSize;
	
	QGLFramebufferObject * m_fbo;
	
	Qt::AspectRatioMode m_aspectRatioMode;
	
	QGLShaderProgram *m_program;
	bool m_useShaders;
	bool m_shadersLinked;
	
	#ifndef QT_OPENGL_ES
	typedef void (APIENTRY *_glActiveTexture) (GLenum);
	_glActiveTexture glActiveTexture;
	#endif
	
	QList<GLWidgetSubview*> m_subviews;
	QHash<int,GLWidgetSubview*> m_subviewLookup;
	
	GLWidgetOutputStream *m_outputStream;
};

#endif
