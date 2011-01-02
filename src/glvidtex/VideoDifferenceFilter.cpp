#include "VideoDifferenceFilter.h"

#include <QtGui>
#include <QDebug>

#include "opencv/cv.h"
#include "opencv/highgui.h"

VideoDifferenceFilter::VideoDifferenceFilter(QObject *parent)
	: VideoFilter(parent)
{
}

VideoDifferenceFilter::~VideoDifferenceFilter()
{
}

void VideoDifferenceFilter::processFrame()
{
	QImage image = frameImage();	
	QImage histo = createDifferenceImage(image);
	
	enqueue(new VideoFrame(histo,m_frame->holdTime()));
}

QImage IplImageToQImage(IplImage *iplImg)
{
	int h        = iplImg->height;
	int w        = iplImg->width;
	int step     = iplImg->widthStep;
	char *data   = iplImg->imageData;
	int channels = iplImg->nChannels;
	
	QImage qimg(w, h, QImage::Format_ARGB32);
	for (int y = 0; y < h; y++, data += step)
	{
		for (int x = 0; x < w; x++)
		{
			char r=0, g=0, b=0, a=0;
			if (channels == 1)
			{
				r = data[x * channels];
				g = data[x * channels];
				b = data[x * channels];
			}
			else if (channels == 3 || channels == 4)
			{
				r = data[x * channels + 2];
				g = data[x * channels + 1];
				b = data[x * channels];
			}
			
			if (channels == 4)
			{
				a = data[x * channels + 3];
				qimg.setPixel(x, y, qRgba(r, g, b, a));
			}
			else
			{
				qimg.setPixel(x, y, qRgb(r, g, b));
			}
		}
	}
	return qimg;

}



QImage VideoDifferenceFilter::createDifferenceImage(QImage image)
{
	IplImage *frame;
	qDebug() << "VideoDifferenceFilter::createDifferenceImage: mark1"; 
	
	if(image.format() != QImage::Format_ARGB32)
	//image = image.copy();
		image = image.convertToFormat(QImage::Format_ARGB32);
		
	frame = cvCreateImageHeader( cvSize(image.width(), image.width()), IPL_DEPTH_8U, 4);
	frame->imageData = (char*) image.bits();
	
	qDebug() << "VideoDifferenceFilter::createDifferenceImage: mark2";
	
	//If is the first frame
	if(!m_image)
	{
		//Create image header same as frame but with 1 channel to gray
		m_image = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
		m_bitImage = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
	}
	
	qDebug() << "VideoDifferenceFilter::createDifferenceImage: mark3";
	
	//Convert frame to gray and store in image
	cvCvtColor(frame, m_image,CV_BGR2GRAY);
	//m_image = cvCloneImage(frame);
	
	qDebug() << "VideoDifferenceFilter::createDifferenceImage: mark4";
	
	//If is the first frame
	if(!m_lastImage)
	{
		//If no lastImage clone actual image;
		m_lastImage = cvCloneImage(m_image);
	}
	
	qDebug() << "VideoDifferenceFilter::createDifferenceImage: mark5";
	
	if(!m_diffImage)
	{
		//Create image header same as frame but with 1 channel to gray
		m_diffImage = cvCreateImage(cvSize(frame->width,frame->height),frame->depth,1);
	}
	
	qDebug() << "VideoDifferenceFilter::createDifferenceImage: mark6";
	
	//cvShowImage( "CamSub", frame );
	
	//Differences with actual and last image
	cvAbsDiff(m_image,m_lastImage,m_diffImage);
	
	qDebug() << "VideoDifferenceFilter::createDifferenceImage: mark7";
	
	//threshold image
	// 23 = arbitrary threshold
	cvThreshold(m_diffImage,m_bitImage,23,255,CV_THRESH_BINARY);
	
	qDebug() << "VideoDifferenceFilter::createDifferenceImage: mark8";
	
	//Change datas;
	m_lastImage = cvCloneImage(m_image);
	
	//cvShowImage("CamSub 1",bitImage);
	qDebug() << "VideoDifferenceFilter::createDifferenceImage: mark9";

	// convert bitImage to imageCopy();
	QImage imageCopy = IplImageToQImage(m_bitImage);
	
	qDebug() << "VideoDifferenceFilter::createDifferenceImage: mark10";
	
	return imageCopy;
}




