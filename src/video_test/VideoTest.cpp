#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include <QPainter>

#include "VideoTest.h"
#include "VideoThread.h"
#include "CameraClient.h"

VideoTest::VideoTest()
	: QGLWidget()
{
	setWindowTitle("Video Test");

	//m_thread = new VideoThread(this);
	//connect(m_thread, SIGNAL(newImage(QImage)), this, SLOT(newFrame(QImage)));
	//connect(this, SIGNAL(readyForNextFrame()), m_thread, SLOT(readFrame()));
	
	m_client = new CameraClient(this);
	connect(m_client, SIGNAL(newImage(QImage)), this, SLOT(newFrame(QImage)));

// 	m_thread->setVideo("085_JumpBack.mov");
// 	m_thread->start();
	m_client->connectTo("10.1.5.68",8088);

	resize(320,240);

	emit readyForNextFrame();
}

VideoTest::~VideoTest()
{
// 	m_thread->quit();
// 	m_thread->wait();
// 	delete m_thread;
// 	m_thread = 0;
	delete m_client;
}

void VideoTest::newFrame(QImage frame)
{
	m_frame = frame;
	if(frame.size().width() > size().width() ||
	    frame.size().height() > size().height() )
		resize(frame.size());
	update(); //repaint();
	QTimer::singleShot(0,this,SIGNAL(readyForNextFrame()));
}

void VideoTest::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	p.drawImage(rect(),m_frame);
}



