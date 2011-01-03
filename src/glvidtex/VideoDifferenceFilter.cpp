#include "VideoDifferenceFilter.h"

#include <QtGui>
#include <QDebug>

VideoDifferenceFilter::VideoDifferenceFilter(QObject *parent)
	: VideoFilter(parent)
	, m_includeOriginalImage(true)
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

// QImage IplImageToQImage(IplImage *iplImg)
// {
// 	int h        = iplImg->height;
// 	int w        = iplImg->width;
// 	int step     = iplImg->widthStep;
// 	char *data   = iplImg->imageData;
// 	int channels = iplImg->nChannels;
// 	
// 	QImage qimg(w, h, QImage::Format_ARGB32);
// 	for (int y = 0; y < h; y++, data += step)
// 	{
// 		for (int x = 0; x < w; x++)
// 		{
// 			char r=0, g=0, b=0, a=0;
// 			if (channels == 1)
// 			{
// 				r = data[x * channels];
// 				g = data[x * channels];
// 				b = data[x * channels];
// 			}
// 			else if (channels == 3 || channels == 4)
// 			{
// 				r = data[x * channels + 2];
// 				g = data[x * channels + 1];
// 				b = data[x * channels];
// 			}
// 			
// 			if (channels == 4)
// 			{
// 				a = data[x * channels + 3];
// 				qimg.setPixel(x, y, qRgba(r, g, b, a));
// 			}
// 			else
// 			{
// 				qimg.setPixel(x, y, qRgb(r, g, b));
// 			}
// 		}
// 	}
// 	return qimg;
// 
// }



QImage VideoDifferenceFilter::createDifferenceImage(QImage image)
{
	if(image.isNull())
		return image;
		
	QSize smallSize = image.size();
	smallSize.scale(200,150,Qt::KeepAspectRatio);
	//smallSize.scale(4,1,Qt::KeepAspectRatio);
	//smallSize.scale(160,120,Qt::KeepAspectRatio);
	
	QImage origScaled = image.scaled(smallSize);
	if(origScaled.format() != QImage::Format_RGB32)
		origScaled = origScaled.convertToFormat(QImage::Format_RGB32);
		
	if(m_lastImage.isNull())
		m_lastImage = origScaled.copy();
	
	QImage outputImage(smallSize, QImage::Format_RGB32);
	memset(outputImage.scanLine(0),0,outputImage.byteCount());
	
	QColor hsvConverter;
	int h1,s1,v1,
	    h2,s2,v2,
	    h,s,v,gray;
	
	int hsvMin[3] = {255,255,255};
	int hsvMax[3] = {0,0,0};
	int hsvAvg[3] = {0,0,0};
	int avgCounter = 0;
	
	// Convert each pixel to grayscale and count the number of times each
	// grayscale value appears in the image
	int bytesPerLine = origScaled.bytesPerLine();
	for(int y=0; y<smallSize.height(); y++)
	{
		const uchar *lastLine = (const uchar*)m_lastImage.scanLine(y);
		const uchar *thisLine  = (const uchar*)origScaled.scanLine(y);
		uchar *destLine = outputImage.scanLine(y);
		for(int x=0; x<bytesPerLine; x+=4)
		{
			
// 			const uchar a = line[x];
// 			const uchar r = x+1 >= bytesPerLine ? 0 : line[x+1];
// 			const uchar g = x+2 >= bytesPerLine ? 0 : line[x+2];
// 			const uchar b = x+3 >= bytesPerLine ? 0 : line[x+3];

			// Docs say QImage::Format_RGB32 stores colors as 0xffRRGGBB - 
			// - but when testing with a 4x1 image of (white,red,green,blue),
			//   I find that the colors (on linux 32bit anyway) were stored BGRA...is that only linux or is that all Qt platforms with Format_RGB32?  
			const uchar xb = lastLine[x];
			const uchar xg = x+1 >= bytesPerLine ? 0 : lastLine[x+1];
			const uchar xr = x+2 >= bytesPerLine ? 0 : lastLine[x+2];
			
			const uchar tb = thisLine[x];
			const uchar tg = x+1 >= bytesPerLine ? 0 : thisLine[x+1];
			const uchar tr = x+2 >= bytesPerLine ? 0 : thisLine[x+2];
// 			const uchar a = x+3 >= bytesPerLine ? 0 : line[x+3];
				
				// These grayscale conversion values are just rough estimates based on my google research - adjust to suit your tastes
				int thisGray = (int)( tr * .30 + tg * .59 + tb * .11 );
				int lastGray = (int)( xr * .30 + xg * .59 + xb * .11 );
				//qDebug() << "val:"<<(int)val<<", r:"<<qRed(pixel)<<", g:"<<qGreen(pixel)<<", b:"<<qBlue(pixel)<<", gray:"<<gray;
				//qDebug() << "r:"<<r<<", g:"<<g<<", b:"<<b<<", gray:"<<gray;
				
				hsvConverter.setRgb(tr,tg,tb);
				hsvConverter.getHsv(&h1,&s1,&v1);
				
				hsvConverter.setRgb(xr,xg,xb);
				hsvConverter.getHsv(&h2,&s2,&v2);
				
				h = abs(h1-h2);
				s = abs(s1-s2);
				v = abs(v1-v2);
				gray = thisGray - lastGray;
				
				//h = (int) ( (((double)h)/359.)*255 );
				
				
				if(h > hsvMax[0])
					hsvMax[0] = h;
				if(s > hsvMax[1])
					hsvMax[1] = s;
				if(v > hsvMax[2])
					hsvMax[2] = v;
				if(h && h < hsvMin[0])
					hsvMin[0] = h;
				if(s && s < hsvMin[1])
					hsvMin[1] = s;
				if(v && v < hsvMin[2])
					hsvMin[2] = v;
				
				hsvAvg[0] += h;
				hsvAvg[1] += s;
				hsvAvg[2] += v;
				
				avgCounter ++;
				
				// threshold
				if(gray > 23)
				{
					destLine[x+0] = 255; //b
					destLine[x+1] = 255; //g
					destLine[x+2] = 255; //r
					destLine[x+3] = 0;   //a
				}
				
				//qDebug() << "r:"<<r<<" \t g:"<<g<<" \t b:"<<b<<" \t gray:"<<gray<<", h:"<<h<<", s:"<<s<<", v:"<<v<<", x:"<<x<<",y:"<<y; 
		}
	}
	
	//qDebug() << "avgCounter:"<<avgCounter;
	//if(m_calcHsvStats)
	{
		if(avgCounter == 0)
			avgCounter = 1;
		hsvAvg[0] /= avgCounter;
		hsvAvg[1] /= avgCounter;
		hsvAvg[2] /= avgCounter;
		
		//emit hsvStatsUpdated(hsvMin[0],hsvMax[0],hsvAvg[0],hsvMin[1],hsvMax[1],hsvAvg[1],hsvMin[2],hsvMax[2],hsvAvg[2]);
		
		qDebug() << "HSV Deltas min/max/avg: " <<
			"\t H: "<<hsvMin[0]<<hsvMax[0]<<hsvAvg[0]<<
			"\t S: "<<hsvMin[1]<<hsvMax[1]<<hsvAvg[1]<<
			"\t V: "<<hsvMin[2]<<hsvMax[2]<<hsvAvg[2];
	
	}
	
	if(m_includeOriginalImage)
	{
		smallSize.scale(255,128,Qt::KeepAspectRatio);
		origScaled = origScaled.scaled(smallSize);
		
		// Render side by side with the original image
		QImage histogramOutput(smallSize.width() + 255, smallSize.height(), QImage::Format_RGB32);
		QPainter p2(&histogramOutput);
		p2.fillRect(histogramOutput.rect(),Qt::gray);
		p2.drawImage(QPointF(0,0),origScaled);
		p2.drawImage(QPointF(smallSize.width(),0),outputImage);
	
		return histogramOutput;
	}
	
	return outputImage;
}




