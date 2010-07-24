#include "MdiCameraWidget.h"

#include <QMessageBox>

MdiCameraWidget::MdiCameraWidget(QWidget *parent)
	: MdiVideoChild(parent)
{

	m_deviceBox = new QComboBox(this);
	m_layout->addWidget(m_deviceBox);

	m_cameras = CameraThread::enumerateDevices();
 	if(!m_cameras.size())
 	{
 		QMessageBox::critical(this,tr("No Cameras Found"),tr("Sorry, but no camera devices were found attached to this computer."));
 		setWindowTitle("No Camera");
 		return;
 	}
 	
 	
	QStringList items;
	int counter = 1;
	foreach(QString dev, m_cameras)
		items << QString("Camera # %1").arg(counter);
	
	m_deviceBox->addItems(items);
	
	connect(m_deviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceBoxChanged(int)));
	
	deviceBoxChanged(0);
	
}

void MdiCameraWidget::deviceBoxChanged(int idx)
{
	

/*#ifdef Q_OS_WIN
	QString defaultCamera = "vfwcap://0";
#else
	QString defaultCamera = "/dev/video0";
#endif*/

	if(idx < 0 || idx >= m_cameras.size())
		return;
		
	QString camera = m_cameras[idx];
	
	m_thread = CameraThread::threadForCamera(camera);
	
	qDebug() << "MdiCamera: m_thread:"<<m_thread;
	 
	if(m_thread)
		m_thread->setFps(30);
	else
	{
		QMessageBox::critical(this,"Missing Camera",QString("Sorry, cannot connect to %1!").arg(camera));
		setWindowTitle("Camera Error");
	}
	
	m_videoWidget->setSourceRectAdjust(11,0,-6,-3);
	
	setWindowTitle(camera);
	
	setVideoSource(m_thread);	
}
