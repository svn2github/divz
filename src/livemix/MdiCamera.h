#ifndef MdiCameraSource_H
#define MdiCameraSource_H

#include "MdiVideoSource.h"
#include "CameraThread.h"

class MdiCamera : public MdiVideoSource
{
	Q_OBJECT
public:
	MdiCamera(QWidget*parent=0);
	
protected:
	CameraThread *m_thread;
};

#endif
