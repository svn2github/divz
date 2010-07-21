#ifndef VideoFrame_H
#define VideoFrame_H

#include <QImage>

class VideoFrame
{
public:
	VideoFrame() { image = QImage(1,1,QImage::Format_Mono); holdTime = -1; }
	VideoFrame(const QImage &frame, int holdTime)
		: image(frame), holdTime(holdTime) {}
	VideoFrame(const VideoFrame& other)
		: image(other.image), holdTime(other.holdTime) {}
	
	void operator=(const VideoFrame& other)
	{
		image = other.image;
		holdTime = other.holdTime;
	}
	
	bool isEmpty() { return image.isNull() || holdTime < 0; }
	
	QImage image;
	int holdTime;
};


#endif
