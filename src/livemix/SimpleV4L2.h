#ifndef SimpleV4L2_H
#define SimpleV4L2_H

typedef enum {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
} v4l2_simple_io_method;

struct v4l2_simple_buffer {
	void *                  start;
	size_t                  length;
};

class VideoFrame;
class SimpleV4L2
{
public:
	SimpleV4L2();
	~SimpleV4L2();
	
	bool openDevice(const char *dev_name);
	void initDevice();
	void startCapturing();
	VideoFrame readFrame();
	void stopCapturing();
	void uninitDevice();
	void closeDevice();
	
private:
	void io_init_read(unsigned int buffer_size);
	void io_init_mmap();
	void io_init_userp(unsigned int buffer_size);
	
	const char *		  m_devName;//		= NULL;
	v4l2_simple_io_method	  m_io;//		= IO_METHOD_MMAP;
	int			  m_fd;//		= -1;
	v4l2_simple_buffer	* m_buffers;//		= NULL;
	unsigned int		  m_numBuffers;//	= 0;
	QSize			  m_imageSize;//	= QSize(0,0)
};


#endif
