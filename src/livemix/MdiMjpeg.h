#ifndef MdiMjpeg_H
#define MdiMjpeg_H

#include "MdiVideoSource.h"
#include "MjpegThread.h"

class MdiMjpeg : public MdiVideoSource
{
	Q_OBJECT
public:
	MdiMjpeg(QWidget*parent=0);
	
protected slots:
	void urlReturnPressed();
	
protected:
// 	virtual void setupGui();

	QLineEdit *m_urlInput;
	MjpegThread *m_thread;
};

#endif
