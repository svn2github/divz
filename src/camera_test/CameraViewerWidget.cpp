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

	updateOverlay();

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

void CameraViewerWidget::updateOverlay()
{
	if(m_overlay.rect() != rect())
	    m_overlay = QPixmap(rect().width(), rect().height());

	m_overlay.fill(QColor(0,0,0,0));
	QPainter painter(&m_overlay);

	painter.setRenderHint(QPainter::Antialiasing, true);
	//painter.setPen(QPen(Qt::black, 12, Qt.ashDotLine, Qt::RoundCap));
	//painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
	//painter.drawEllipse(80, 80, 400, 240);

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
	if(frame.size() != m_sourceRect.size() ||
	   rect().size() != m_cachedFrameRect.size())
	{
		m_cachedFrameRect = rect();
		//qDebug() << "Frame Size:"<<frame.size();
		//QMessageBox::information(this, "Debug", QString("Frame: %1 x %2").arg(frame.size().width()).arg(frame.size().height()));
		//resize(frame.size());

		//resize(QSize(1024,768));
		m_sourceRect = frame.rect();
		m_targetRect = QRect(0,0,0,0);
		if(m_sourceRect.height() > m_sourceRect.width())
		{
			qreal ar = (qreal)m_sourceRect.width() / (qreal)m_sourceRect.height();
			int width = (int)((qreal)rect().height() * ar);
			if(width > rect().width())
			{
				int height = (int)((qreal)rect().width() / .75);
				m_targetRect = QRect(0, rect().height()/2 - height/2, rect().width(), height);
			}
			else
				m_targetRect = QRect(rect().width()/2 - width/2, 0, width, rect().height());
		}
		else
		{
			qreal ar = (qreal)m_sourceRect.height() / (qreal)m_sourceRect.width();
			int height = (int)((qreal)rect().width() * ar);
			if(height > rect().height())
			{
				int width = (int)((qreal)rect().height() / .75);
				m_targetRect = QRect(rect().width()/2 - width/2, 0, width, rect().height());
			}
			else
				m_targetRect = QRect(0, rect().height()/2 - height/2, rect().width(), height);


			//qDebug() << "WIDE: m_sourceRect:"<<m_sourceRect<<", m_targetRect:"<<m_targetRect<<", ar:"<<ar<<", rect:"<<rect()<<", height: "<<height;
		}

	}
	//repaint();
	update();
	QTimer::singleShot(0,this,SIGNAL(readyForNextFrame()));
}

void CameraViewerWidget::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	p.setRenderHint(QPainter::SmoothPixmapTransform);
	p.setRenderHint(QPainter::Antialiasing);

	if(m_opacity <= 0)
		return;

	if(m_opacity < 1 || m_targetRect != rect())
		p.fillRect(rect(),Qt::black);

	if(m_opacity>0 && m_opacity<1)
		p.setOpacity(m_opacity);

	if(!m_thread)
	{
		p.fillRect(rect(),Qt::black);
		p.setPen(Qt::white);
		p.drawText(5,15,QString("Error: Cannot find camera \"%1\"").arg(m_camera));
	}
	else
	{

		p.drawImage(m_targetRect,m_frame,m_sourceRect);

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

		//p.drawPixmap(rect(),m_overlay);
	}
}
