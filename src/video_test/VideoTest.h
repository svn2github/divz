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

#include "QPainterVideoSurface.h"
#include <QtMultimedia/qvideosurfaceformat.h>


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
	void formatChanged(const QVideoSurfaceFormat &);

signals:
	void readyForNextFrame();

protected:
	void paintEvent(QPaintEvent*);
	void resizeEvent(QResizeEvent*) { updateRects(); } // HACK

private:
	void updateRects();
	
	VideoThread * m_thread;
	CameraClient * m_client;
	QImage m_frame;
	QGraphicsView2 *m_view;
	QPainterVideoSurface *m_surface;
	QRectF m_boundingRect;
	QRectF m_sourceRect;
	bool m_updatePaintDevice;
	QSize m_videoNativeSize;
	Qt::AspectRatioMode m_aspectRatioMode;
};



#endif //VideoTest_h
