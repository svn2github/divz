#include <QtGui/QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QtOpenGL/QGLWidget>

#include "GVTestWindow.h"
#include "SimpleV4L2.h"


BoxItem::BoxItem(QGraphicsScene * scene, QGraphicsItem * parent) : QGraphicsItem(parent, scene),
		m_contentsRect(0,0,640,480), brush(QColor(255,0,0,255))
		, m_v4l2(0)
		, m_frameCount(0)
		, m_latencyAccum(0)
		, m_debugFps(true)
		
	{
		setFlags(QGraphicsItem::ItemIsMovable); 
		pen.setWidthF(3);
		pen.setColor(QColor(0,0,0,255));
		
		m_v4l2 = new SimpleV4L2();
		QString cameraFile = "/dev/video0";
		if(m_v4l2->openDevice(qPrintable(cameraFile)))
		{
// 			// Do the code here so that we dont have to open the device twice
// 			if(!setInput("Composite"))
// 				if(!setInput("Composite1"))
// 					setInput(0);

			m_v4l2->initDevice();
			m_v4l2->startCapturing();
// 			m_inited = true;
			
// 			qDebug() << "CameraThread::initCamera(): finish2";

// 			return 1;
		}
		readFrame();
		connect(&m_readTimer, SIGNAL(timeout()), this, SLOT(readFrame()));
		m_readTimer.setInterval(1000/35);
		m_readTimer.start();
		m_time.start();
		
	}
BoxItem::~BoxItem()
{
	if(m_v4l2)
	{
		delete m_v4l2;
		m_v4l2 = 0;
	};
}
	
QRectF BoxItem::boundingRect() const
	{
		return m_contentsRect;
	}
	
void BoxItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
	{
		painter->setPen(pen);
		painter->setBrush(brush);
		painter->drawRect(m_contentsRect);
		QImage image((const uchar*)m_frame.byteArray.constData(),m_frame.size.width(),m_frame.size.height(),QImage::Format_RGB32);
		painter->drawImage(m_contentsRect,image);
		
		
		if(!m_frame.captureTime.isNull())
		{
			int msecLatency = m_frame.captureTime.msecsTo(QTime::currentTime());
			
			m_latencyAccum += msecLatency;
		}
		
		if (!(m_frameCount % 100)) 
		{
			QString framesPerSecond;
			framesPerSecond.setNum(m_frameCount /(m_time.elapsed() / 1000.0), 'f', 2);
			
			QString latencyPerFrame;
			latencyPerFrame.setNum((((double)m_latencyAccum) / ((double)m_frameCount)), 'f', 3);
			
			if(m_debugFps && framesPerSecond!="0.00")
				qDebug() << "GLVideoDrawable::paintGL: "<<objectName()<<" FPS: " << qPrintable(framesPerSecond) << (m_frame.captureTime.isNull() ? "" : qPrintable(QString(", Latency: %1 ms").arg(latencyPerFrame)));
	
			m_time.start();
			m_frameCount = 0;
			m_latencyAccum = 0;
			
			//lastFrameTime = time.elapsed();
		}
		m_frameCount++;
	}

void BoxItem::readFrame()
	{
		VideoFrame f = m_v4l2->readFrame();
		if(f.isValid())
			m_frame = f;
		update();
	}


int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	
	QGraphicsView *graphicsView = new QGraphicsView();
 	graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
 	graphicsView->setRenderHint( QPainter::Antialiasing, true );
	
	QGraphicsScene * scene = new QGraphicsScene();
	
	graphicsView->setScene(scene);
	scene->setSceneRect(0,0,800,600);
	graphicsView->resize(800,600);
	graphicsView->setWindowTitle("Test");
	
// 	for(int i=0;i<100;i++)
// 	{
		BoxItem *x = new BoxItem(scene,0);
		//x->setPos(qrand() % 800, qrand() % 600);
		x->setPos(0,0);
// 	}
	
	graphicsView->show();
	
	return app.exec();
}

