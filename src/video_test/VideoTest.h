#ifndef VideoTest_h
#define VideoTest_h

#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QDebug>
#include <QTime>

#include <QGLWidget>

#include <QGraphicsView>

class VideoThread;
class CameraClient;


class QGraphicsView2 : public QGraphicsView
{
public:
	QGraphicsView2() 
	{
		m_frames=0;
		m_timeTotal  =0;
	};
	

protected:
	friend class VideoTest;
	QImage m_bg;
	QTime m_time;
	int m_timeTotal;
	int m_frames;
	
	void drawBackground(QPainter* p, const QRectF &er)
	{
// 		qDebug() << ":drawBackground: nothing done";
		if(!m_bg.isNull())
		{
			p->drawImage(er,m_bg);
		}
	}
};


class VideoTest : public QGLWidget
{
	Q_OBJECT
public:
	VideoTest();
	~VideoTest();
	
	//void setView(QGraphicsView2*v) { m_view=v; }

public slots:
	void newFrame(QImage,QTime);

signals:
	void readyForNextFrame();

protected:
	void paintEvent(QPaintEvent*);

private:
	VideoThread * m_thread;
	CameraClient * m_client;
	QImage m_frame;
// 	QTime m_time;
// 	int m_timeTotal;
// 	int m_frames;
	
	QTime m_elapsedTime;
	long m_frameCount;
	
};



#endif //VideoTest_h
