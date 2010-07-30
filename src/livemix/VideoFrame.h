#ifndef VideoFrame_H
#define VideoFrame_H

#include <QImage>
#include <QTime>

class VideoFrame
{
public:
	VideoFrame() { image = QImage(1,1,QImage::Format_Mono); holdTime = -1; captureTime = QTime(); }
	VideoFrame(const QImage &frame, int holdTime, const QTime &captureTime = QTime())
		: image(frame), holdTime(holdTime), captureTime(captureTime) {}
	VideoFrame(const VideoFrame& other)
		: image(other.image), holdTime(other.holdTime), captureTime(other.captureTime) {}
	
	void operator=(const VideoFrame& other)
	{
		image = other.image;
		holdTime = other.holdTime;
		captureTime = other.captureTime;
	}
	
	bool isEmpty() { return image.isNull() || holdTime < 0; }
	
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
};


#endif
