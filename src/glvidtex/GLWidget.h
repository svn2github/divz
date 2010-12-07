#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtGui>
#include <QGLWidget>
#include <QGLShaderProgram>

class QGLFramebufferObject;

class GLDrawable;

class GLWidget;
class GLWidgetSubview
{
public:
	GLWidgetSubview();
	GLWidgetSubview(QByteArray&);
	
	QByteArray toByteArray();
	void fromByteArray(QByteArray&);
	
	int subviewId();
	
	QString title() const { return m_title; }
	double top() const { return m_viewTop; }
	double left() const { return m_viewLeft; }
	double bottom() const { return m_viewBottom; }
	double right() const { return m_viewRight; }
	
	bool flipHorizontal() { return m_flipHorizontal; }
	bool flipVertical() { return m_flipVertical; }
	
	QImage alphaMask() { return m_alphaMask; }
	
	int brightness() const { return m_brightness; }
	int contrast() const { return m_contrast; }
	int hue() const { return m_hue; }
	int saturation() const { return m_saturation; }
	
	const QPolygonF & cornerTranslations() { return m_cornerTranslations; }
	
	
	void setTitle(const QString&);
	
	void setTop(double);
	void setLeft(double);
	void setBottom(double);
	void setRight(double);
	
	void setAlphaMask(const QImage&);
	
	void setBrightness(int brightness);
	void setContrast(int contrast);
	void setHue(int hue);
	void setSaturation(int saturation);
	
	void setFlipHorizontal(bool flip);
	void setFlipVertical(bool flip);
	
	void setCornerTranslations(const QPolygonF&);
	
protected:
	friend class GLWidget;
	void uploadAlphaMask(GLWidget*);
	
private:
	void updateColors(int brightness, int contrast, int hue, int saturation);
	
	int m_id;
	QString m_title;
	
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
	
	QImage m_alphaMask;
	QImage m_alphaMask_preScaled;
	GLuint m_alphaTextureId;
	qint64 m_uploadedCacheKey;
	
	QMatrix4x4 m_colorMatrix;
	bool m_colorsDirty;

	QPolygonF m_cornerTranslations;
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
	
	void addDrawable(GLDrawable *);
	void removeDrawable(GLDrawable*);
	
	QList<GLDrawable*> drawables() { return m_drawables; }
	
	QTransform transform() { return m_transform; }
	void setTransform(const QTransform&);
	
	const QRectF & viewport() const { return m_viewport; }
	const QSizeF & canvasSize() const { return m_canvasSize; }
	const QRectF canvasRect() const { return QRectF(QPointF(0,0), m_canvasSize); }
	
	void makeRenderContextCurrent();
	
	const QPolygonF & cornerTranslations() { return m_cornerTranslations; }
	bool cornerTranslationsEnabled() { return m_cornerTranslationsEnabled; }
	
	Qt::AspectRatioMode aspectRatioMode() { return m_aspectRatioMode; }
	
	QImage alphaMask() { return m_alphaMask; }
	
	int brightness() const { return m_brightness; }
	int contrast() const { return m_contrast; }
	int hue() const { return m_hue; }
	int saturation() const { return m_saturation; }
	
	bool flipHorizontal() { return m_flipHorizontal; }
	bool flipVertical() { return m_flipVertical; }
	
	typedef enum RotateValue
	{
		RotateLeft =-1,
		RotateNone = 0,
		RotateRight=+1
	};
	
	RotateValue cornerRotation() { return m_cornerRotation; }
	
	QList<GLWidgetSubview*> subviews() { return m_subviews; }

signals:
	void clicked();
	void canvasSizeChanged(const QSizeF&);
	void viewportChanged(const QRectF&);

public slots:
	void setViewport(const QRectF&);
	void setCanvasSize(const QSizeF&);
	void setCanvasSize(double x,double y) { setCanvasSize(QSizeF(x,y)); }
	
	void setCornerTranslations(const QPolygonF&);
	
	void setTopLeftTranslation(const QPointF&);
	void setTopRightTranslation(const QPointF&);
	void setBottomLeftTranslation(const QPointF&);
	void setBottomRightTranslation(const QPointF&);
	
	void enableCornerTranslations(bool flag=true);
	
	void setAspectRatioMode(Qt::AspectRatioMode mode);
	
	void setAlphaMask(const QImage&);
	
	void setBrightness(int brightness);
	void setContrast(int contrast);
	void setHue(int hue);
	void setSaturation(int saturation);
	
	void setFlipHorizontal(bool flip);
	void setFlipVertical(bool flip);
	
	void setCornerRotation(RotateValue);
	
// 	void addSubview(GLWidgetSubview*);
// 	void removeSubview(int subviewId);
// 	void removeSubview(GLWidgetSubview*);
	
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
	
	void updateColors(int brightness, int contrast, int hue, int saturation);

private:
	void initShaders();
	void initAlphaMask();
	void updateWarpMatrix();
	
	QList<GLDrawable*> m_drawables;
	
	bool m_glInited;
	
	QTransform m_transform;
	QRectF m_viewport;
	QSizeF m_canvasSize;
	
	QGLFramebufferObject * m_fbo;
	
	bool m_cornerTranslationsEnabled;
	QPolygonF m_cornerTranslations;
	
	Qt::AspectRatioMode m_aspectRatioMode;
	
	QGLShaderProgram *m_program;
	bool m_useShaders;
	
	#ifndef QT_OPENGL_ES
	typedef void (APIENTRY *_glActiveTexture) (GLenum);
	_glActiveTexture glActiveTexture;
	#endif
	
	QImage m_alphaMask;
	QImage m_alphaMask_preScaled;
	GLuint m_alphaTextureId;
	qint64 m_uploadedCacheKey;
	
	QMatrix4x4 m_colorMatrix;
	
	bool m_colorsDirty;
	
	bool m_flipHorizontal;
	bool m_flipVertical;
// 	QPointF cropTopLeft;
// 	QPointF cropBottomRight;
	int m_brightness;
	int m_contrast;
	int m_hue;
	int m_saturation;
	
	RotateValue m_cornerRotation;
	
	GLfloat m_warpMatrix[4][4];
	
	QList<GLWidgetSubview*> m_subviews;
	QHash<int,GLWidgetSubview*> m_subviewLookup;
};

#endif
