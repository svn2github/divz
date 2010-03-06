#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include <QPainter>

#include <QGraphicsView>
#include <QGraphicsScene>

#include "VideoTest.h"
#include "VideoThread.h"
#include "CameraClient.h"

VideoTest::VideoTest()
	: QGLWidget()
{
	setWindowTitle("Video Test");
	
	
	m_thread = new VideoThread(this);
	connect(m_thread, SIGNAL(newImage(QImage,QTime)), this, SLOT(newFrame(QImage,QTime)));
// 	connect(this, SIGNAL(readyForNextFrame()), m_thread, SLOT(readFrame()));
	
// 	m_client = new CameraClient(this);
// 	connect(m_client, SIGNAL(newImage(QImage)), this, SLOT(newFrame(QImage)));

 	m_thread->setVideo("105_JumpBack.mov");
 	//m_thread->setVideo("/home/josiah/Download/qt-master/qt-qt/examples/dviz/src/samples/jumpbacks/105_JumpBack.mov");
 	m_thread->start();
	//m_client->connectTo("10.1.5.68",8088);

	resize(320,240);

	emit readyForNextFrame();



}

VideoTest::~VideoTest()
{
	m_thread->quit();
	m_thread->wait();
	delete m_thread;
	m_thread = 0;
// 	delete m_client;
}

void VideoTest::newFrame(QImage frame,QTime time)
{
// 	m_frame = frame;
// 	if(frame.size().width() > size().width() ||
// 	    frame.size().height() > size().height() )
// 		resize(frame.size());
// 	qDebug() << "Got new frame";
//  	repaint();

   	QPainter p(this);
//   	p.drawImage(rect(),m_frame);
  	
 	m_view->m_bg = frame;
 	m_view->m_time = time;
  	m_view->render(&p);
  	
//  	m_view->repaint();
	
	int ms = m_view->m_time.msecsTo(QTime::currentTime());
	m_view->m_frames++;
	m_view->m_timeTotal += ms;
	int avg = m_view->m_timeTotal / m_view->m_frames;
	qDebug() << "Time: inst:"<<ms<<", avg:"<<avg;
	
	QTimer::singleShot(0,this,SIGNAL(readyForNextFrame()));
}

void VideoTest::paintEvent(QPaintEvent*)
{
// 	qDebug() << "paint event";
// 	QPainter p(this);
// 	p.drawImage(rect(),m_frame);
}



