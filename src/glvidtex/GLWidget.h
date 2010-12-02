#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtGui>
#include <QGLWidget>

class QGLFramebufferObject;

class GLDrawable;
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
	const QSizeF & canvasSize() const { return m_canvasSize; }
	
	void makeRenderContextCurrent();
	
	const QPolygonF & cornerTranslations() { return m_cornerTranslations; }
	bool cornerTranslationsEnabled() { return m_cornerTranslationsEnabled; }
	
	Qt::AspectRatioMode aspectRatioMode() { return m_aspectRatioMode; }
	
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
	void setBottomLeft(const QPointF&);
	void setBottomRight(const QPointF&);
	
	void enableCornerTranslations(bool flag=true);
	
	void setAspectRatioMode(Qt::AspectRatioMode mode);
	
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
	
	

private:
// 	QPoint lastPos;
// 	int xRot;
// 	int yRot;
// 	int zRot;
	void initShaders();
	
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
	
};

#endif
