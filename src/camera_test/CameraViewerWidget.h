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

public slots:
	void newFrame(QImage);

signals:
	void readyForNextFrame();

protected:
	void paintEvent(QPaintEvent*);

private:
	CameraThread * m_thread;
	QImage m_frame;
	CameraServer * m_server;
	QTime m_elapsedTime;
	long m_frameCount;
	
	QString m_overlayText;
	bool m_showOverlayText;
};



#endif //CameraViewerWidget_h
