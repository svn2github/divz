#ifndef VideoFrame_H
#define VideoFrame_H

#include <QImage>
#include <QTime>
#include <QDebug>
#include <QVideoFrame>

class VideoFrame
{
public:
	VideoFrame() 
	{ 
		//image = QImage(1,1,QImage::Format_Mono);
		holdTime = -1; 
		captureTime = QTime(); 
		isRaw=false; 
		pixelFormat = QVideoFrame::Format_Invalid;
		bufferType = BUFFER_INVALID;
	}
	
	VideoFrame(int holdTime, const QTime &captureTime = QTime())
		: holdTime(holdTime)
		, captureTime(captureTime) 
		, pixelFormat(QVideoFrame::Format_Invalid)
		, bufferType(BUFFER_INVALID)
		, isRaw(false)
	{}
	
	VideoFrame(const QImage &frame, int holdTime, const QTime &captureTime = QTime())
		: holdTime(holdTime)
		, captureTime(captureTime) 
		, pixelFormat(QVideoFrame::Format_Invalid)
		, bufferType(BUFFER_IMAGE)
		, image(frame)
		, isRaw(false)
	{
		setSize(frame.size());
		
		QImage::Format format = image.format();
		pixelFormat =
			format == QImage::Format_ARGB32 ? QVideoFrame::Format_ARGB32 :
			format == QImage::Format_RGB32  ? QVideoFrame::Format_RGB32  :
			format == QImage::Format_RGB888 ? QVideoFrame::Format_RGB24  :
			format == QImage::Format_RGB16  ? QVideoFrame::Format_RGB565 :
			format == QImage::Format_RGB555 ? QVideoFrame::Format_RGB555 :
			//format == QImage::Format_ARGB32_Premultiplied ? QVideoFrame::Format_ARGB32_Premultiplied :
			// GLVideoDrawable doesn't support premultiplied - so the format conversion below will convert it to ARGB32 automatically
			pixelFormat;
			
		if(pixelFormat == QVideoFrame::Format_Invalid)
		{
			qDebug() << "VideoFrame: image was not in an acceptable format, converting to ARGB32 automatically.";
			image = image.convertToFormat(QImage::Format_ARGB32);
			pixelFormat = QVideoFrame::Format_ARGB32;
		}
	}
	
/*	
QVideoFrame::Format_ARGB32	1	The frame is stored using a 32-bit ARGB format (0xAARRGGBB). This is equivalent to QImage::Format_ARGB32.
QVideoFrame::Format_ARGB32_Premultiplied	2	The frame stored using a premultiplied 32-bit ARGB format (0xAARRGGBB). This is equivalent to QImage::Format_ARGB32_Premultiplied.
QVideoFrame::Format_RGB32	3	The frame stored using a 32-bit RGB format (0xffRRGGBB). This is equivalent to QImage::Format_RGB32
QVideoFrame::Format_RGB24	4	The frame is stored using a 24-bit RGB format (8-8-8). This is equivalent to QImage::Format_RGB888
QVideoFrame::Format_RGB565	5	The frame is stored using a 16-bit RGB format (5-6-5). This is equivalent to QImage::Format_RGB16.
QVideoFrame::Format_RGB555	6	The frame is stored using a 16-bit RGB format (5-5-5). This is equivalent to QImage::Format_RGB555.*/
	
	VideoFrame(const VideoFrame& other)
		: holdTime(other.holdTime)
		, captureTime(other.captureTime)
		, pixelFormat(other.pixelFormat)
		, bufferType(other.bufferType)
		, image(other.image)
		, isRaw(other.isRaw)
	{
		setSize(other.size);
		if(bufferType == BUFFER_POINTER)
			setPointerData(other.data, other.linesize);
		if(bufferType == BUFFER_BYTEARRAY)
			byteArray = other.byteArray;
	}

	void operator=(const VideoFrame& other)
	{
		image = other.image;
		holdTime = other.holdTime;
		captureTime = other.captureTime;
		isRaw = other.isRaw;
		pixelFormat = other.pixelFormat;
		bufferType = other.bufferType;
		setSize(other.size);
		if(bufferType == BUFFER_POINTER)
			setPointerData(other.data, other.linesize);
		if(bufferType == BUFFER_BYTEARRAY)
			byteArray = other.byteArray;
	}
	
	bool isEmpty() { return bufferType == BUFFER_INVALID; }
	bool isValid() { return bufferType != BUFFER_INVALID; }
	
	// Variable holdTime : int:
	// Time in milliseconds to 'hold' this frame on screen before displaying the next frame.
	int holdTime;
	
	// Used for measuring latency - holds the time the frame is created.
	// The VideoWidget can then compare it to the time the frame is displayed
	// to determine how much latency there is in the pipeline.
	QTime captureTime;
	
	// Variable pixelFormat : QVideoFrame::PixelFormat 
	// Holds the underlying format of the data in either the image, pointers, or byte arrayf
	QVideoFrame::PixelFormat pixelFormat;
	
	// Enum VideoFrameBufferType:
	//	BUFFER_INVALID	- Frame is likely uninitalized
	//	BUFFER_IMAGE	- The image data is in the .image : QImage member
	//	BUFFER_POINTER	- The image data is in the .data[4] : unit8_t* member (related: linesize[4] : int)
	//	BUFFER_BYTEARRAY- The image data is in the .byteArray : QByteArray member
	typedef enum BufferType
	{
		BUFFER_INVALID = 0,
		BUFFER_IMAGE,
		BUFFER_POINTER,
		BUFFER_BYTEARRAY,
	};
	
	// Variable bufferType : VideoFrameBufferType 
	// Type of data this frame holds
	BufferType bufferType;
	
	// Variable image : QImage:
	// The frame itself - usually RGB32_Premul.
	QImage image;
	
	// Variable isRaw : bool
	// Convenience variable to flag if this is a 'raw' data type frame (pointer or bytearray), or if its a QImage
	bool isRaw;
	
	// If bufferType is POINTER, then the data is expected to reside in these pointers.
	uint8_t *data[4];
	int linesize[4];
	
	// If bufferTYpe is BYTEARRAY, then, of course, data is expected to be in this bytearray
	QByteArray byteArray;
	
	// Regardless of the buffer type, these members are expecte to contain the size and rect of the image, can be set both with setSize(), below
	QSize size;
	QRect rect;
	
	void setSize(const QSize& newSize)
	{
		size = newSize;
		rect = QRect(0,0,size.width(),size.height());
	}

	void setPointerData(uint8_t* const src[], const int stride[])
	{
		isRaw = true;
		bufferType = BUFFER_POINTER;
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
