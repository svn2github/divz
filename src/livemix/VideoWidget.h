#ifndef VideoWidget_h
#define VideoWidget_h

#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QDebug>
#include <QTime>

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
	
	void setOverlayText(const QString& text);
	void showOverlayText(bool flag=true);

	void setOpacity(qreal opacity);
	
	void setSourceRectAdjust( int dx1, int dy1, int dx2, int dy2 );
	
public slots:
	//void newFrame(QImage);
	void frameReady();

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
};



#endif //VideoWidget_h
