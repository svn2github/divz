#ifndef VideoWidget_h
#define VideoWidget_h

#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QDebug>
#include <QTime>

#include <QMouseEvent>
#include <QGLWidget>

#include "VideoFrame.h"

class VideoSource;
class VideoFrame;
// class CameraServer;
class VideoWidget  : public QGLWidget
{
	Q_OBJECT
public:
	VideoWidget();
	~VideoWidget();

	void setVideoSource(VideoSource *);
	void disconnectVideoSource();
	int fps() { return m_forceFps; }
	QString overlayText() { return m_overlayText; }
	
	virtual QSize sizeHint () const;
	
	bool renderFps() { return m_renderFps; }
	
signals:
	void clicked();
	
public slots:
	//void newFrame(QImage);
	
	void setOverlayText(const QString& text);
	void showOverlayText(bool flag=true);

	void setOpacity(qreal opacity);
	
	void setSourceRectAdjust( int dx1, int dy1, int dx2, int dy2 );
	
	void setFps(int fps=-1);
	
	void setRenderFps(bool);
	
protected slots:
	void frameReady();
	
	void callUpdate();
	void updateTimer();
	
protected:
	void mouseReleaseEvent(QMouseEvent*); 
	void paintEvent(QPaintEvent*);
	void closeEvent(QCloseEvent*);
	void showEvent(QShowEvent*);
        void resizeEvent(QResizeEvent*);
        
private:
	void updateOverlay();
	void updateRects();

	VideoSource * m_thread;
	long m_frameCount;

	qreal m_opacity;
	VideoFrame m_frame;
	
	Qt::AspectRatioMode m_aspectRatioMode;
	int m_adjustDx1;
	int m_adjustDy1;
	int m_adjustDx2;
	int m_adjustDy2;


	QTime m_elapsedTime;
	
	bool m_showOverlayText;
	QString m_overlayText;

	QPixmap m_overlay;

	QRect m_targetRect;
	QRect m_sourceRect;
	QRect m_origSourceRect;
	
	QTimer m_paintTimer;
	
	int m_forceFps;
	
	bool m_renderFps;
};



#endif //VideoWidget_h
