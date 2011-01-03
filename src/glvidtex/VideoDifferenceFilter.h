#ifndef VideoDifferenceFilter_H
#define VideoDifferenceFilter_H

#include <QtGui>
#include "VideoFilter.h"

class VideoDifferenceFilter : public VideoFilter
{
	Q_OBJECT
public:
	VideoDifferenceFilter(QObject *parent=0);
	~VideoDifferenceFilter();
	
protected:
	virtual void processFrame();
	
	QImage createDifferenceImage(QImage);
	
	QImage m_lastImage;
	bool m_includeOriginalImage;
};

#endif
