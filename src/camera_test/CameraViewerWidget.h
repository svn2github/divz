#ifndef CameraViewerWidget_h
#define CameraViewerWidget_h

#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QDebug>
#include <QTime>

#include <QGLWidget>

class CameraThread;
class CameraServer;
class CameraViewerWidget : public QGLWidget
{
	Q_OBJECT
public:
	CameraViewerWidget();
	~CameraViewerWidget();

	void setCamera(const QString& camera, int fps=30);
	void setFps(int fps);
	int fps() { return m_fps; }
	
	void setOverlayText(const QString& text);
	void showOverlayText(bool flag=true);

	void setOpacity(qreal opacity);

	void setPrimaryConsumer(bool flag=true);
	bool isPrimaryConsumer() { return m_isPrimaryConsumer; }

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

private:
	void updateOverlay();

	qreal m_opacity;
	QString m_camera;
	CameraThread * m_thread;
	QImage m_frame;
	CameraServer * m_server;
	QTime m_elapsedTime;
	long m_frameCount;

	QString m_overlayText;
	bool m_showOverlayText;

	QPixmap m_overlay;

	QRect m_targetRect;
	QRect m_sourceRect;
	QRect m_cachedFrameRect;

	int m_readFrameCount;
	bool m_lockRepaint;
	QTimer m_paintTimer;

	bool m_isPrimaryConsumer;
	
	int m_fps;
};



#endif //CameraViewerWidget_h
