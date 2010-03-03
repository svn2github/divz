#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QDebug>

#include "CameraTest.h"
#include "CameraThread.h"
#include "CameraServer.h"
#include <QPainter>

CameraTest::CameraTest()
	: QGLWidget()
{
	setWindowTitle("Camera Test");

	m_thread = new CameraThread(this);
	connect(m_thread, SIGNAL(newImage(QImage)), this, SLOT(newFrame(QImage)));
	connect(this, SIGNAL(readyForNextFrame()), m_thread, SLOT(readFrame()));

	m_thread->setCamera("vfwcap://0");
	m_thread->start();
	
	m_server = new CameraServer();
	m_server->setProvider(m_thread, SIGNAL(newImage(QImage)));
		
	if (!m_server->listen(QHostAddress::Any,8088)) 
	{
		qDebug() << "CameraServer could not start: "<<m_server->errorString();
	}

	resize(320,240);

	emit readyForNextFrame();
}

CameraTest::~CameraTest()
{
	m_thread->quit();
	m_thread->wait();
	delete m_thread;
	m_thread = 0;
	
	delete m_server;
	m_server = 0;
}

void CameraTest::newFrame(QImage frame)
{
	m_frame = frame;
	if(frame.size().width() > size().width() ||
	    frame.size().height() > size().height() )
		resize(frame.size());
	repaint();
	QTimer::singleShot(0,this,SIGNAL(readyForNextFrame()));
}

void CameraTest::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	p.drawImage(rect(),m_frame);
}
