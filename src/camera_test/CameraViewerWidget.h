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

	void setCamera(const QString& camera);
	void setOverlayText(const QString& text);
	void showOverlayText(bool flag=true);

	void setOpacity(qreal opacity);

public slots:
	void newFrame(QImage);

signals:
	void readyForNextFrame();

protected:
	void paintEvent(QPaintEvent*);

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
};



#endif //CameraViewerWidget_h
