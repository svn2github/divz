#ifndef VideoTest_h
#define VideoTest_h

#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QDebug>

#include <QGLWidget>

class VideoThread;
class CameraClient;

class VideoTest : public QGLWidget
{
	Q_OBJECT
public:
	VideoTest();
	~VideoTest();

public slots:
	void newFrame(QImage);

signals:
	void readyForNextFrame();

protected:
	void paintEvent(QPaintEvent*);

private:
	//VideoThread * m_thread;
	CameraClient * m_client;
	QImage m_frame;
};



#endif //VideoTest_h
