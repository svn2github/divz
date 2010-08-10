#ifndef VideoFrame_H
#define VideoFrame_H

#include <QImage>
#include <QTime>
#include <QDebug>

class VideoFrame
{
public:
	VideoFrame() 
	{ 
		//image = QImage(1,1,QImage::Format_Mono);
		holdTime = -1; 
		captureTime = QTime(); 
		isRaw=false; 
		isYuv=false;
		isPlanar=false;
		useByteArray=false;
	}
	VideoFrame(int holdTime, const QTime &captureTime = QTime())
		: holdTime(holdTime)
		, captureTime(captureTime) 
		, isRaw(false)
		, isYuv(false)
		, isPlanar(false)
		, useByteArray(false)
		{}
	VideoFrame(const QImage &frame, int holdTime, const QTime &captureTime = QTime())
		: image(frame)
		, holdTime(holdTime)
		, captureTime(captureTime) 
		, isRaw(false)
		, isYuv(false)
		, isPlanar(false)
		, useByteArray(false)
		, size(frame.size())
		{}
	VideoFrame(const VideoFrame& other)
		: image(other.image)
		, holdTime(other.holdTime)
		, captureTime(other.captureTime)
		, isRaw(other.isRaw)
		, isYuv(other.isYuv)
		, isPlanar(other.isPlanar)
		, useByteArray(other.useByteArray)
		, size(other.size)
		{
			if(isRaw)
			{
				bits = other.bits;
				setRawData(other.data, other.linesize);
			}
			if(useByteArray)
				byteArray = other.byteArray;
		}
	
	void operator=(const VideoFrame& other)
	{
		image = other.image;
		holdTime = other.holdTime;
		captureTime = other.captureTime;
		isRaw = other.isRaw;
		isYuv = other.isYuv;
		isPlanar = other.isPlanar;
		size = other.size;
		if(isRaw)
		{
			bits = other.bits;
			setRawData(other.data, other.linesize);
		}
		useByteArray = other.useByteArray;
		if(useByteArray)
			byteArray = other.byteArray;
	}
	
	bool isEmpty() { return holdTime < 0; }
	bool isValid() { return !isEmpty(); }
	
	// Variable image : QImage:
	// The frame itself - usually RGB32_Premul.
	QImage image;
	
	// Variable holdTime : int:
	// Time in milliseconds to 'hold' this frame on screen before displaying the next frame.
	int holdTime;
	
	// Used for measuring latency - holds the time the frame is created.
	// The VideoWidget can then compare it to the time the frame is displayed
	// to determine how much latency there is in the pipeline.
	QTime captureTime;
	
	bool isRaw;
	bool isYuv;
	bool isPlanar;
	
	uint8_t *data[4];
	int linesize[4];
	uchar *bits;
	
	QByteArray byteArray;
	bool useByteArray;
	
	QSize size;

	void setRawBits(uchar *dataBits)
	{
		isRaw = true;
		isPlanar = false;
		isYuv = true;
		bits = dataBits;
	}
	
	void setRawData(uint8_t* const src[], const int stride[])
	{
		//qDebug() << "setRawData!";
		isRaw = true;
		isPlanar = true;
		isYuv = true;
		data[0] = src[0];
		data[1] = src[1];
		data[2] = src[2];
		data[3] = src[3];
		linesize[0] = stride[0];
		linesize[1] = stride[1];
		linesize[2] = stride[2];
		linesize[3] = stride[3];
	}


};


#endif
