#ifndef NativeViewerWebKit_H
#define NativeViewerWebKit_H


#include "SlideGroupViewer.h"
class WebSlideGroup;



// TODO - do we need to code a non-OpenGL version for compat with non-GL systems?
#include <QtGui>
#include <QGLWidget>
class NativeViewerWebKit;

// Based on the GLRenderWidgetImplementation in Qt's /src/3rdparty/phonon/gstreamer/glrenderer.h 
class GLFrameRenderWidget : public QWidget
{
    	Q_OBJECT
public:
	 GLFrameRenderWidget(const QGLFormat &format, QWidget *parent=0);
	~GLFrameRenderWidget();
	
	// Parent viewer is just used to get the fade settings right now
	void setParentViewer(NativeViewerWebKit*);
	NativeViewerWebKit * parentViewer() { return m_viewer; }
	
	QPixmap snapshot();
	
public slots:
// 	void updateScreen(const QPixmap& pixmap, const QRegion& region = QRegion());
	void fadeBlack(bool);
	void setWidget(QWidget*);
	
protected:
	bool eventFilter(QObject *obj, QEvent *event);
	void paintEvent(QPaintEvent*);
	
	void paintWidget(QPainter*);
	
protected slots:
	void fadeStart();
	void fadeStep();
	void fadeDone();
	
private:
	qreal m_blackOpacity;
	qreal m_fadeInc;
	int m_fadeDir;
	QTimer m_fadeTimer;
	 
	QTimer m_copyTimer;
	
	//QPixmap m_frame;
	QRegion m_region;
	
	QWidget * m_view;
	
	bool m_lockPainter;
	
	QPointF m_viewTopLeft;
	QPointF m_viewScale;
	
	QPixmap m_pixmap;
	bool m_pixmapAllDirty;
	
	NativeViewerWebKit * m_viewer;
	
	bool m_blackFlag;
};



class NativeViewerWebKit : public NativeViewer
{
	Q_OBJECT
public:
	NativeViewerWebKit();
	~NativeViewerWebKit();

	void setSlideGroup(SlideGroup*);
	void setSlide(int);

	int numSlides() { return 1; }
	int currentSlide() { return 0; }
	
	void show();
	void close();
	void hide();
	
	QPixmap snapshot();

	void setState(NativeShowState);
	NativeShowState state() {  return m_state; }
	
	QWidget * renderWidget() { return (QWidget*)m_widget; }
	
private:
	NativeShowState m_state;
	WebSlideGroup * m_webGroup;
	GLFrameRenderWidget * m_widget;

};

#endif
