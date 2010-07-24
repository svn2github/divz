#ifndef MdiCameraWidget_H
#define MdiCameraWidget_H

#include "MdiVideoChild.h"
#include "CameraThread.h"

#include <QComboBox>

class MdiCameraWidget : public MdiVideoChild
{
	Q_OBJECT
public:
	MdiCameraWidget(QWidget*parent=0);
	
protected slots:
	void deviceBoxChanged(int);	

protected:
	CameraThread *m_thread;
	QComboBox *m_deviceBox;
	QStringList m_cameras;
};

#endif
