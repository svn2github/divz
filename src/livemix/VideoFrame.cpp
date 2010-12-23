#include "VideoFrame.h"

VideoFrame::VideoFrame()
{
	m_holdTime = -1; 
	m_captureTime = QTime(); 
	m_isRaw=false; 
	m_pixelFormat = QVideoFrame::Format_Invalid;
	m_bufferType = BUFFER_INVALID;
	m_refCount = 0;
	m_pointer = 0;
	m_pointerLength = 0;
	qDebug() << "VideoFrame::VideoFrame(): constructor(1): "<<this;
}

VideoFrame::VideoFrame(int holdTime, const QTime &captureTime)
	: m_holdTime(holdTime)
	, m_captureTime(captureTime) 
	, m_pixelFormat(QVideoFrame::Format_Invalid)
	, m_bufferType(BUFFER_INVALID)
	, m_isRaw(false)
	, m_refCount(0)
	, m_pointer(0)
	, m_pointerLength(0)
{
	qDebug() << "VideoFrame::VideoFrame(): constructor(2): "<<this;
}


VideoFrame::VideoFrame(const QImage &frame, int holdTime, const QTime &captureTime)
	: m_holdTime(holdTime)
	, m_captureTime(captureTime) 
	, m_pixelFormat(QVideoFrame::Format_Invalid)
	, m_bufferType(BUFFER_IMAGE)
	, m_image(frame)
	, m_isRaw(false)
	, m_refCount(0)
	, m_pointer(0)
	, m_pointerLength(0)
{
	qDebug() << "VideoFrame::VideoFrame(): constructor(3): "<<this;
	
	setSize(frame.size());
	
	QImage::Format format = m_image.format();
	m_pixelFormat =
		format == QImage::Format_ARGB32 ? QVideoFrame::Format_ARGB32 :
		format == QImage::Format_RGB32  ? QVideoFrame::Format_RGB32  :
		format == QImage::Format_RGB888 ? QVideoFrame::Format_RGB24  :
		format == QImage::Format_RGB16  ? QVideoFrame::Format_RGB565 :
		format == QImage::Format_RGB555 ? QVideoFrame::Format_RGB555 :
		//format == QImage::Format_ARGB32_Premultiplied ? QVideoFrame::Format_ARGB32_Premultiplied :
		// GLVideoDrawable doesn't support premultiplied - so the format conversion below will convert it to ARGB32 automatically
		m_pixelFormat;
		
	if(m_pixelFormat == QVideoFrame::Format_Invalid)
	{
		qDebug() << "VideoFrame: image was not in an acceptable format, converting to ARGB32 automatically.";
		m_image = m_image.convertToFormat(QImage::Format_ARGB32);
		m_pixelFormat = QVideoFrame::Format_ARGB32;
	}
}

VideoFrame::VideoFrame(VideoFrame *other)
	: m_holdTime(other->m_holdTime)
	, m_captureTime(other->m_captureTime)
	, m_pixelFormat(other->m_pixelFormat)
	, m_bufferType(other->m_bufferType)
	, m_image(other->m_image)
	, m_isRaw(other->m_isRaw)
	, m_refCount(0)
	, m_pointer(other->m_pointer)
	, m_pointerLength(other->m_pointerLength)

{
	qDebug() << "VideoFrame::VideoFrame(): constructor(4): "<<this;
	setSize(other->m_size);
}

VideoFrame::~VideoFrame()
{
	qDebug() << "VideoFrame::~VideoFrame(): "<<this;
	if(m_pointer)
	{
		qDebug() << "VideoFrame::~VideoFrame(): "<<this<<" deleting m_pointer:"<<m_pointer; 
		free(m_pointer);
		m_pointer = 0;
	}
}
	
void VideoFrame::incRef()
{
	QMutexLocker lock(&m_refMutex);
	m_refCount++;
	qDebug() << "VideoFrame::incRef(): "<<this<<": m_refCount:"<<m_refCount; 	
}

bool VideoFrame::release()
{
	QMutexLocker lock(&m_refMutex);
	m_refCount --;
	qDebug() << "VideoFrame::release(): "<<this<<": m_refCount:"<<m_refCount;
	if(m_refCount <= 0)
		//deleteLater();
		return true;
	return false;
}

uchar *VideoFrame::allocPointer(int bytes)
{
// 	if(m_pointer)
// 		delete m_pointer;
	m_pointer = (uchar*)malloc(sizeof(uchar) * bytes);
	qDebug() << "VideoFrame::allocPointer(): "<<this<<" allocated m_pointer:"<<m_pointer<<", bytes:"<<bytes;
	m_pointerLength = bytes;
	return m_pointer;
}

bool VideoFrame::isEmpty() { return m_bufferType == BUFFER_INVALID; }
bool VideoFrame::isValid() { return m_bufferType != BUFFER_INVALID && (m_bufferType == BUFFER_IMAGE ? !m_image.isNull() : m_pointer != NULL); }

// Returns the approx memory consumption of this frame.
// 'approx' because it isn't perfect, especially when using
// BUFFER_POINTER bufferType's. 
int VideoFrame::byteSize() const
{
	int mem = 0;
	switch(m_bufferType)
	{
		case BUFFER_IMAGE:
			mem = m_image.byteCount();
			break;
		case BUFFER_POINTER:
			mem = m_pointerLength;
			break;
		default:
			break;
	}
	return mem + sizeof(VideoFrame);
}

void VideoFrame::setHoldTime(int holdTime)
{
	m_holdTime = holdTime; 
}
	
void VideoFrame::setCaptureTime(const QTime& time)
{
	m_captureTime = time;
}

void VideoFrame::setPixelFormat(QVideoFrame::PixelFormat format)
{
	m_pixelFormat = format;
}

void VideoFrame::setBufferType(BufferType type)
{
	m_bufferType = type;
}

void VideoFrame::setImage(const QImage& image)
{
	m_image = image; // .copy();
}

void VideoFrame::setIsRaw(bool flag)
{
	m_isRaw = flag;
}
	
void VideoFrame::setSize(QSize newSize)
{
	m_size = newSize;
	m_rect = QRect(0,0,m_size.width(),m_size.height());
}

void VideoFrame::setRect(QRect rect)
{
	setSize(rect.size());
}

void VideoFrame::setPointer(uchar *dat, int len)
{
	m_isRaw = true;
	m_bufferType = BUFFER_POINTER;
	m_pointer = dat;
	m_pointerLength = len;
}
