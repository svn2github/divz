#ifndef VideoDifferenceFilter_H
#define VideoDifferenceFilter_H

#include <QtGui>
#include "VideoFilter.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"

class VideoDifferenceFilter : public VideoFilter
{
	Q_OBJECT
public:
	VideoDifferenceFilter(QObject *parent=0);
	~VideoDifferenceFilter();
	
protected:
	virtual void processFrame();
	
	QImage createDifferenceImage(QImage);
	
	IplImage* m_image;
	IplImage* m_lastImage;
	IplImage* m_diffImage;
	IplImage* m_bitImage;
};

#endif
