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
		items << QString("Camera # %1").arg(counter++);
	
	m_deviceBox->addItems(items);
	
	connect(m_deviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceBoxChanged(int)));
	
	deviceBoxChanged(0);
}

void MdiCameraWidget::deviceBoxChanged(int idx)
{
	if(idx < 0 || idx >= m_cameras.size())
		return;
		
	QString camera = m_cameras[idx];
	
	m_thread = CameraThread::threadForCamera(camera);
	
	if(!m_thread)
	{
		QMessageBox::critical(this,"Missing Camera",QString("Sorry, cannot connect to %1!").arg(camera));
		setWindowTitle("Camera Error");
	}
	
	// Default clip rect to compensate for oft-seen video capture 'bugs'
	m_videoWidget->setSourceRectAdjust(11,0,-6,-3);
	
	// Reduce the video widgets FPS (not the camera threads fps)
	// inorder to make a more responsive UI
	m_videoWidget->setFps(15);
	
	setWindowTitle(camera);
	
	setVideoSource(m_thread);	
}
