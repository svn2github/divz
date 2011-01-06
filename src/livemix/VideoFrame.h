#ifndef VideoFrame_H
#define VideoFrame_H

#include <QImage>
#include <QTime>
#include <QDebug>
#include <QVideoFrame>
#include <QQueue>
#include <QMutex>
#include <QObject>
#include <QSharedPointer>

//#define DEBUG_VIDEOFRAME_POINTERS

class VideoFrame //: public QObject
{
// 	//Q_OBJECT
public:
	VideoFrame(); 
	
	VideoFrame(int holdTime, const QTime &captureTime = QTime());
	
	VideoFrame(const QImage &frame, int holdTime, const QTime &captureTime = QTime());
/*	
QVideoFrame::Format_ARGB32	1	The frame is stored using a 32-bit ARGB format (0xAARRGGBB). This is equivalent to QImage::Format_ARGB32.
QVideoFrame::Format_ARGB32_Premultiplied	2	The frame stored using a premultiplied 32-bit ARGB format (0xAARRGGBB). This is equivalent to QImage::Format_ARGB32_Premultiplied.
QVideoFrame::Format_RGB32	3	The frame stored using a 32-bit RGB format (0xffRRGGBB). This is equivalent to QImage::Format_RGB32
QVideoFrame::Format_RGB24	4	The frame is stored using a 24-bit RGB format (8-8-8). This is equivalent to QImage::Format_RGB888
QVideoFrame::Format_RGB565	5	The frame is stored using a 16-bit RGB format (5-6-5). This is equivalent to QImage::Format_RGB16.
QVideoFrame::Format_RGB555	6	The frame is stored using a 16-bit RGB format (5-5-5). This is equivalent to QImage::Format_RGB555.*/
	
	VideoFrame(VideoFrame* other);
	
	~VideoFrame();

	void incRef();
	bool release();
	
	bool isEmpty();
	bool isValid();
	
	// Returns the approx memory consumption of this frame.
	// 'approx' because it isn't perfect, especially when using
	// BUFFER_POINTER bufferType's. 
	int byteSize() const;
	
	int holdTime() { return m_holdTime; }
	void setHoldTime(int holdTime);
	
	QTime captureTime() { return m_captureTime; }
	void setCaptureTime(const QTime& time);
	
	QVideoFrame::PixelFormat pixelFormat() { return m_pixelFormat; }
	void setPixelFormat(QVideoFrame::PixelFormat format);
	
	// Enum VideoFrameBufferType:
	//	BUFFER_INVALID	- Frame is likely uninitalized
	//	BUFFER_IMAGE	- The image data is in the .image : QImage member
	//	BUFFER_POINTER	- The image data is in the .pointer
	enum BufferType
	{
		BUFFER_INVALID = 0,
		BUFFER_IMAGE,
		BUFFER_POINTER,
	};
	
	BufferType bufferType() { return m_bufferType; }
	void setBufferType(BufferType type);
	
	QImage image() { return m_image; }
	void setImage(QImage image);
	
	bool isRaw() { return m_isRaw; }
	void setIsRaw(bool flag);
	 
	uchar *pointer() { return m_pointer; }
	int pointerLength() { return m_pointerLength; }
	void setPointer(uchar *pointer, int length);
	uchar *allocPointer(int bytes);
	
	QSize size() { return m_size; }
	void setSize(QSize size);
	
	QRect rect() { return m_rect; }
	void setRect(QRect rect);
	
	bool debugPtr() { return m_debugPtr; }
	void setDebugPtr(bool flag) { m_debugPtr = flag; }	
	
protected:
	
	// Variable holdTime : int:
	// Time in milliseconds to 'hold' this frame on screen before displaying the next frame.
	int m_holdTime;
	
	// Used for measuring latency - holds the time the frame is created.
	// The VideoWidget can then compare it to the time the frame is displayed
	// to determine how much latency there is in the pipeline.
	QTime m_captureTime;
	
	// Variable pixelFormat : QVideoFrame::PixelFormat 
	// Holds the underlying format of the data in either the image, pointers, or byte arrayf
	QVideoFrame::PixelFormat m_pixelFormat;
	
	// Variable bufferType : VideoFrameBufferType 
	// Type of data this frame holds
	BufferType m_bufferType;
	
	// Variable image : QImage:
	// The frame itself - usually RGB32_Premul.
	QImage m_image;
	
	// Variable isRaw : bool
	// Convenience variable to flag if this is a 'raw' data type frame (pointer), or if its a QImage
	bool m_isRaw;
	
	// If bufferTYpe is POINTER, then, of course, data is expected to be in this pointer
	uchar *m_pointer;
	int m_pointerLength;
	
	// Regardless of the buffer type, these members are expecte to contain the size and rect of the image, can be set both with setSize(), below
	QSize m_size;
	QRect m_rect;
	
	QMutex m_refMutex;
	int m_refCount;
	
	bool m_debugPtr;
};

typedef QSharedPointer<VideoFrame> VideoFramePtr;

class VideoFrameQueue : public QQueue<VideoFramePtr>
{
public:
	VideoFrameQueue(int maxBytes=0)
		: QQueue<VideoFramePtr>()
		, m_maxBytes(maxBytes)
		{}
		
	int byteSize() const
	{
		int sum = 0;
		for(int i=0; i<size(); i++)
		{
			sum += at(i)->byteSize();
		}
		return sum;
	}
	
	void setMaxByteSize(int bytes)
	{
		m_maxBytes = bytes;
	}
	
	int maxByteSize() { return m_maxBytes; }
	
	void enqueue(VideoFrame *frame)
	{
// 		if(m_maxBytes > 0)
// 			while(byteSize() > m_maxBytes)
// 				dequeue();	
		
		QQueue<VideoFramePtr>::enqueue(VideoFramePtr(frame));
		//enqueue(VideoFramePtr(frame));
	};
	
	void enqueue(VideoFramePtr frame)
	{
// 		if(m_maxBytes > 0)
// 			while(byteSize() > m_maxBytes)
// 				dequeue();	
		
		QQueue<VideoFramePtr>::enqueue(frame);
		//enqueue(VideoFramePtr(frame));
	};
	
protected:
	int m_maxBytes;
};



#endif
