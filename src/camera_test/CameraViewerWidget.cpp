#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QDebug>

#include "CameraViewerWidget.h"
#include "CameraThread.h"
#include "CameraServer.h"
#include <QPainter>
#include <QApplication>
#include <QMessageBox>

CameraViewerWidget::CameraViewerWidget()
	: QGLWidget()
	, m_frameCount(0)
	, m_opacity(1)
{
	//setWindowTitle("Camera Test");

}

void CameraViewerWidget::setCamera(const QString& camera)
{
	m_camera = camera;
	m_thread = CameraThread::threadForCamera(camera);
	if(!m_thread)
	{
		repaint();
		return;
	}
		
	connect(m_thread, SIGNAL(newImage(QImage)), this, SLOT(newFrame(QImage)));
	connect(this, SIGNAL(readyForNextFrame()), m_thread, SLOT(readFrame()));

	m_elapsedTime.start();
	
// 	m_server = new CameraServer();
// 	m_server->setProvider(m_thread, SIGNAL(newImage(QImage)));
// 		
// 	if (!m_server->listen(QHostAddress::Any,8088)) 
// 	{
// 		qDebug() << "CameraServer could not start: "<<m_server->errorString();
// 	}
// 
// 	resize(100,100);

	emit readyForNextFrame();
}

void CameraViewerWidget::setOverlayText(const QString& text)
{
	m_overlayText = text;
}

void CameraViewerWidget::showOverlayText(bool flag)
{
	m_showOverlayText = flag;
}


CameraViewerWidget::~CameraViewerWidget()
{
	if(m_thread)
		m_thread->release();
// 	m_thread->quit();
// 	m_thread->wait();
// 	delete m_thread;
// 	m_thread = 0;
	
// 	delete m_server;
// 	m_server = 0;
}

void CameraViewerWidget::setOpacity(qreal opac)
{
	m_opacity = opac;
	update();
}

void CameraViewerWidget::newFrame(QImage frame)
{
	m_frame = frame;
	if(frame.size().width() > size().width() ||
	    frame.size().height() > size().height() )
	{
		//qDebug() << "Frame Size:"<<frame.size();
		//QMessageBox::information(this, "Debug", QString("Frame: %1 x %2").arg(frame.size().width()).arg(frame.size().height()));
		//resize(frame.size());
		resize(QSize(1024,768));
	}
	repaint();
	QTimer::singleShot(0,this,SIGNAL(readyForNextFrame()));
}

void CameraViewerWidget::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform);
	p.setRenderHint(QPainter::Antialiasing);
	
	if(m_opacity>0 && m_opacity<1)
	{
		p.fillRect(rect(),Qt::black);
		p.setOpacity(m_opacity);
	}
	else
	if(m_opacity <= 0)
	{
		p.fillRect(rect(),Qt::black);
		return;
	}
	
	if(!m_thread)
	{
		p.fillRect(rect(),Qt::black);
		p.setPen(Qt::white);
		p.drawText(5,15,QString("Error: Cannot find camera \"%1\"").arg(m_camera));
	}
	else
	{
		
		p.drawImage(rect(),m_frame);
	
	// 	int sec = (m_elapsedTime.elapsed() / 1000);
	// 
	// 	m_frameCount ++;
	// 	int fps = (m_frameCount <= 0 ? 1 : m_frameCount) / (sec <= 0 ? 1 : sec);
	// 	p.drawText(5,15,QString("fps: %1, frames: %3, time: %2").arg(fps).arg(sec).arg(m_frameCount));
	// 	
		if(m_showOverlayText)
		{
			// render m_overlayText neatly and nicely
		}
	}
}
