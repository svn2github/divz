#include "FaceDetectFilter.h"

#include "EyeCounter.h"

FaceDetectFilter::FaceDetectFilter(QObject *parent)
	: VideoFilter(parent)
{
	m_counter = new EyeCounter(this);
}

FaceDetectFilter::~FaceDetectFilter()
{
}

void FaceDetectFilter::processFrame()
{
	QImage image = frameImage();	
	QImage histo = highlightFaces(image);
	
	enqueue(new VideoFrame(histo,m_frame->holdTime()));
}

QImage FaceDetectFilter::highlightFaces(QImage img)
{
	QList<EyeCounterResult> results = m_counter->detectEyes(img,true); // true = faces, no eyes
	
	QImage imageCopy = img.copy();
	QPainter p(&imageCopy);
	qDebug() << "FaceDetectFilter::highlightFaces: Result size:"<<results.size();
	foreach(EyeCounterResult r, results)
	{
		p.setPen(Qt::red);
		p.drawRect(r.face);
		qDebug() << "FaceDetectFilter::highlightFaces: Face at:"<<r.face<<", eyes:"<<r.leftEye<<"/"<<r.rightEye;
		
		p.setPen(Qt::green);
		p.drawRect(r.leftEye);
		
		p.setPen(Qt::blue);
		p.drawRect(r.rightEye);
	}
		
	return imageCopy;
}
