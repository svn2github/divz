#ifndef VideoWidget_h
#define VideoWidget_h

#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QDebug>
#include <QTime>

#include <QGLWidget>

class VideoSource;
// class CameraServer;
class VideoWidget  : public QGLWidget
{
	Q_OBJECT
public:
	VideoWidget();
	~VideoWidget();

	void setVideoSource(VideoSource *);
	void disconnectVideoSource();
	
	void setOverlayText(const QString& text);
	void showOverlayText(bool flag=true);

	void setOpacity(qreal opacity);
	
	void setSourceRectAdjust( int dx1, int dy1, int dx2, int dy2 );
	
public slots:
	//void newFrame(QImage);
	void frameReady(int frameHoldTime);

private slots:
	void callUpdate();
	void updateTimer();
	
protected:
	void paintEvent(QPaintEvent*);
	void closeEvent(QCloseEvent*);
	void showEvent(QShowEvent*);
        void resizeEvent(QResizeEvent*);
        
private:
	void updateOverlay();
	void updateRects();

	qreal m_opacity;
	QString m_camera;
	VideoSource * m_thread;
	QImage m_frame;
// 	CameraServer * m_server;
	QTime m_elapsedTime;
	long m_frameCount;

	QString m_overlayText;
	bool m_showOverlayText;

	QPixmap m_overlay;

	QRect m_targetRect;
	QRect m_sourceRect;
	QRect m_origSourceRect;
	int m_adjustDx1;
	int m_adjustDy1;
	int m_adjustDx2;
	int m_adjustDy2;
	
	int m_readFrameCount;
	bool m_lockRepaint;
	QTimer m_paintTimer;

	int m_frameHoldTime; 

	
	Qt::AspectRatioMode m_aspectRatioMode;

};



#endif //VideoWidget_h
