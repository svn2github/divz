#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtGui>
#include <QGLWidget>

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
	
signals:
	void clicked();
	void canvasSizeChanged(const QSizeF&);
	void viewportChanged(const QRectF&);

public slots:
	void setViewport(const QRectF&);
	void setCanvasSize(const QSizeF&);
	void setCanvasSize(double x,double y) { setCanvasSize(QSizeF(x,y)); }
	
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
	QSizeF m_canvasSize;
	
};

#endif
