#ifndef VideoTest_h
#define VideoTest_h

#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QDebug>

#include <QGLWidget>

#include <QGraphicsView>

class VideoThread;
class CameraClient;


class QGraphicsView2 : public QGraphicsView
{

protected:
	friend class VideoTest;
	QImage m_bg;
	
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
	
	void setView(QGraphicsView2*v) { m_view=v; }

public slots:
	void newFrame(QImage);

signals:
	void readyForNextFrame();

protected:
	void paintEvent(QPaintEvent*);

private:
	VideoThread * m_thread;
	CameraClient * m_client;
	QImage m_frame;
	QGraphicsView2 *m_view;
};



#endif //VideoTest_h
