#include "MdiCamera.h"

#include <QMessageBox>

MdiCamera::MdiCamera(QWidget *parent)
	: MdiVideoSource(parent)
{

#ifdef Q_OS_WIN
	QString defaultCamera = "vfwcap://0";
#else
	QString defaultCamera = "/dev/video0";
#endif
	
	m_thread = CameraThread::threadForCamera(defaultCamera);
	
	qDebug() << "MdiCamera: m_thread:"<<m_thread;
	 
	if(m_thread)
		m_thread->setFps(30);
	else
		QMessageBox::critical(this,"Missing Camera","Sorry, cannot connect to the default camera!");
	
	m_videoWidget->setSourceRectAdjust(11,0,-6,-3);
	
	setWindowTitle(defaultCamera);
	
	setVideoSource(m_thread);
	
	
}
