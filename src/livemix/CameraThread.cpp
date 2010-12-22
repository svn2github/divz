
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QStringList>
#include <QDebug>
#include <QApplication>

#include <assert.h>

#include <QImageWriter>

#include "CameraThread.h"

extern "C" {
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}

#include "SimpleV4L2.h"

//#include "ccvt/ccvt.h"

//#define DEINTERLACE 1

#if defined(Q_OS_WIN)
// 	#ifdef DEINTERLACE
		#define RAW_PIX_FMT PIX_FMT_BGR32
// 	#else
// 		#define RAW_PIX_FMT PIX_FMT_BGR565
// 	#endif
#else
// 	#ifdef DEINTERLACE
		#define RAW_PIX_FMT PIX_FMT_RGB32
// 	#else
// 		#define RAW_PIX_FMT PIX_FMT_RGB565
// 	#endif
#endif

namespace
{
	// NOTE: This code was copied from the iLab saliency project by USC
	// found at http://ilab.usc.edu/source/. The note following is from
	// the USC code base. I've just converted it to use uchar* instead of
	// the iLab (I assume) specific typedef 'byte'. - Josiah 20100730

void bobDeinterlace(const uchar* src, const uchar* const srcend,
		    uchar* dest, uchar* const destend,
		    const int height, const int stride,
		    const bool in_bottom_field)
{

	// NOTE: this deinterlacing code was derived from and/or inspired by
	// code from tvtime (http://tvtime.sourceforge.net/); original
	// copyright notice here:

	/**
	* Copyright (c) 2001, 2002, 2003 Billy Biggs <vektor@dumbterm.net>.
	*
	* This program is free software; you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation; either version 2, or (at your option)
	* any later version.
	*
	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	* GNU General Public License for more details.
	*
	* You should have received a copy of the GNU General Public License
	* along with this program; if not, write to the Free Software Foundation,
	* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
	*/

	assert(height > 0);
	assert(stride > 0);

	// NOTE: on x86 machines with glibc 2.3.6 and g++ 3.4.4, it looks
	// like std::copy is faster than memcpy, so we use that to do the
	// copying here:

#if 1
#  define DOCOPY(dst,src,n) std::copy((src),(src)+(n),(dst))
#else
#  define DOCOPY(dst,src,n) memcpy((dst),(src),(n))
#endif

	if (in_bottom_field)
	{
		src += stride;

		DOCOPY(dest, src, stride);

		dest += stride;
	}

	DOCOPY(dest, src, stride);

	dest += stride;

	const int N = (height / 2) - 1;
	for (int i = 0; i < N; ++i)
	{
		const uchar* const src2 = src + (stride*2);

		for (int k = 0; k < stride; ++k)
			dest[k] = (src[k] + src2[k]) / 2;

		dest += stride;

		DOCOPY(dest, src2, stride);

		src += stride*2;
		dest += stride;
	}

	if (!in_bottom_field)
	{
		DOCOPY(dest, src, stride);

		src += stride*2;
		dest += stride;
	}
	else
		src += stride;

	// consistency check: make sure we've done all our counting right:

	if (src != srcend)
		qFatal("deinterlacing consistency check failed: %d src %p-%p=%d",
			int(in_bottom_field), src, srcend, int(src-srcend));

	if (dest != destend)
		qFatal("deinterlacing consistency check failed: %d dst %p-%p=%d",
			int(in_bottom_field), dest, destend, int(dest-destend));
}

#undef DOCOPY

}


QMap<QString,CameraThread *> CameraThread::m_threadMap;
QStringList CameraThread::m_enumeratedDevices;
bool CameraThread::m_devicesEnumerated = false;
QMutex CameraThread::threadCacheMutex;

CameraThread::CameraThread(const QString& camera, QObject *parent)
	: VideoSource(parent)
	, m_fps(30)
	, m_inited(false)
	, m_cameraFile(camera)
	, m_frameCount(0)
	, m_deinterlace(false)
	, m_v4l2(0)
	, m_error(false)
{
	m_time_base_rational.num = 1;
	m_time_base_rational.den = AV_TIME_BASE;

	m_sws_context = NULL;
	m_frame = NULL;

	m_rawFrames = false;

	setIsBuffered(false);
}

void CameraThread::destroySource()
{
	#ifdef DEBUG
	qDebug() << "CameraThread::destroySource(): "<<this;
	#endif
	QMutexLocker lock(&threadCacheMutex);
	m_threadMap.remove(m_cameraFile);

	VideoSource::destroySource();
}

CameraThread * CameraThread::threadForCamera(const QString& camera)
{
	if(camera.isEmpty())
		return 0;

	QStringList devices = enumerateDevices();

	if(!devices.contains(camera))
		return 0;

	QMutexLocker lock(&threadCacheMutex);

	if(m_threadMap.contains(camera))
	{
		CameraThread *v = m_threadMap[camera];
		v->m_refCount++;
		#ifdef DEBUG
 		qDebug() << "CameraThread::threadForCamera(): "<<v<<": "<<camera<<": [CACHE HIT] +";
 		#endif
		return v;
	}
	else
	{
		CameraThread *v = new CameraThread(camera);
		m_threadMap[camera] = v;
		v->m_refCount=1;
 		#ifdef DEBUG
 		qDebug() << "CameraThread::threadForCamera(): "<<v<<": "<<camera<<": [CACHE MISS] -";
 		#endif
//  		v->initCamera();
		v->start(QThread::HighPriority);
		usleep(750 * 1000); // give it half a sec or so to init

		return v;
	}
}


QStringList CameraThread::enumerateDevices(bool forceReenum)
{
	VideoSource::initAV();

	if(!forceReenum && m_devicesEnumerated)
		return m_enumeratedDevices;

	m_enumeratedDevices.clear();
	m_devicesEnumerated = true;

	#ifdef Q_OS_WIN32
		QString deviceBase = "vfwcap://";
		QString formatName = "vfwcap";
	#else
		QString deviceBase = "/dev/video";
		QString formatName = "video4linux";
	#endif
	QStringList list;


	AVInputFormat *inFmt = NULL;
	AVFormatParameters formatParams;

	for(int i=0; i<10; i++)
	{
		memset(&formatParams, 0, sizeof(AVFormatParameters));

		#ifdef Q_OS_WIN32
			QString file = QString::number(i);
		#else
			QString file = QString("/dev/video%1").arg(i);
		#endif

		inFmt = av_find_input_format(qPrintable(formatName));
		if( !inFmt )
		{
			qDebug() << "[ERROR] CameraThread::load(): Unable to find input format:"<<formatName;
			break;
		}

		formatParams.time_base.num = 1;
		formatParams.time_base.den = 29; //25;
		formatParams.channel = 0;
		formatParams.standard = "ntsc";

		//formatParams.width = 352;
		//formatParams.height = 288;
		//formatParams.channel = 0;
		//formatParams.pix_fmt = PIX_FMT_RGB24 ;

		// Open video file
		//
		AVFormatContext * formatCtx;
		if(av_open_input_file(&formatCtx, qPrintable(file), inFmt, 0, &formatParams) != 0)
		//if(av_open_input_file(&m_av_format_context, "1", inFmt, 0, NULL) != 0)
		{
			//qDebug() << "[WARN] CameraThread::load(): av_open_input_file() failed, file:"<<file;
			break;
		}
		else
		{
			list << QString("%1%2").arg(deviceBase).arg(i);
			av_close_input_file(formatCtx);
		}
	}

	#ifdef DEBUG
	qDebug() << "enumerateDevices: Found: "<<list;
	#endif

	m_enumeratedDevices = list;
	return list;
}

QStringList CameraThread::inputs()
{
	#if !defined(Q_OS_LINUX)
		return QStringList();
	#endif

	SimpleV4L2 * api = m_v4l2;
	bool deleteIt = false;
	if(!api)
	{
		api = new SimpleV4L2();
		deleteIt = true;
		if(!api->openDevice(qPrintable(m_cameraFile)))
		{
			delete api;
			return QStringList();
		}
	}

	QStringList inputs = api->inputs();

	if(deleteIt)
		delete api;
	return inputs;
}

int CameraThread::input()
{
	#if !defined(Q_OS_LINUX)
		return -1;
	#endif

	SimpleV4L2 * api = m_v4l2;
	bool deleteIt = false;
	if(!api)
	{
		api = new SimpleV4L2();
		deleteIt = true;
		if(!api->openDevice(qPrintable(m_cameraFile)))
		{
			delete api;
			return -1;
		}
	}

	int idx = api->input();

	if(deleteIt)
		delete api;

	return idx;
}

void CameraThread::setInput(int idx)
{
	#if !defined(Q_OS_LINUX)
		return;
	#endif

	SimpleV4L2 * api = m_v4l2;
	bool deleteIt = false;
	if(!api)
	{
		api = new SimpleV4L2();
		deleteIt = true;
		if(!api->openDevice(qPrintable(m_cameraFile)))
		{
			delete api;
			return;
		}
	}

	api->setInput(idx);
	api->setStandard("NTSC");

	if(deleteIt)
		delete api;
}

bool CameraThread::setInput(const QString& name)
{
	#if !defined(Q_OS_LINUX)
		return false;
	#else
	
	m_inputName = name;


	SimpleV4L2 * api = m_v4l2;
	bool deleteIt = false;
	if(!api)
	{
		api = new SimpleV4L2();
		deleteIt = true;
		if(!api->openDevice(qPrintable(m_cameraFile)))
		{
			delete api;
			return false;
		}
	}

	bool flag = api->setInput(name);
	if(flag)
		api->setStandard("NTSC");

	if(deleteIt)
		delete api;

	return flag;

	#endif
}



int CameraThread::initCamera()
{
// 	qDebug() << "CameraThread::initCamera(): start";
	QMutexLocker lock(&m_initMutex);
	
	
	m_inited = false;
	
	#if defined(Q_OS_LINUX)
	if(m_rawFrames)
	{
		m_v4l2 = new SimpleV4L2();
		if(m_v4l2->openDevice(qPrintable(m_cameraFile)))
		{
			// Do the code here so that we dont have to open the device twice
			if(!m_inputName.isEmpty())
			{
				setInput(m_inputName);
			}
			else
			{
				if(!setInput("Composite"))
					if(!setInput("Composite1"))
						setInput(0);
			}

			m_v4l2->initDevice();
			if(!m_v4l2->startCapturing())
			{
				m_error = true;
				return 0;
			}
			else
			{
				m_inited = true;
				
	// 			qDebug() << "CameraThread::initCamera(): finish2";
	
				return 1;
			}
		}
		else
		{
			m_error = true;
			return 0;
		}
	}
	#endif
	
	if(m_inited)
	{
// 		qDebug() << "CameraThread::initCamera(): finish3";
		return 1;
	}

	// And do it here even if we're not using raw frames because setInput() will use SimpleV4L2 on linux,
	// and its a NOOP on windows.
	if(!setInput("Composite"))
		if(!setInput("Composite1"))
			setInput(0);


	AVInputFormat *inFmt = NULL;
	AVFormatParameters formatParams;
	memset(&formatParams, 0, sizeof(AVFormatParameters));

	QString fileTmp = m_cameraFile;

	#ifdef Q_OS_WIN32
	QString fmt = "vfwcap";
	if(fileTmp.startsWith("vfwcap://"))
		fileTmp = fileTmp.replace("vfwcap://","");
	#else
	QString fmt = "video4linux";
	#endif

	#ifdef DEBUG
	qDebug() << "[DEBUG] CameraThread::load(): fmt:"<<fmt<<", filetmp:"<<fileTmp;
	#endif

	inFmt = av_find_input_format(qPrintable(fmt));
	if( !inFmt )
	{
		qDebug() << "[ERROR] CameraThread::load(): Unable to find input format:"<<fmt;
		return -1;
	}

	formatParams.time_base.num = 1;
	formatParams.time_base.den = 35; //25;
// 	formatParams.width = 352;
// 	formatParams.height = 288;
	formatParams.width = 640;
	formatParams.height = 480;
// 	formatParams.channel = 1;
	//formatParams.pix_fmt = PIX_FMT_RGB24 ;


	// Open video file
	 //
	if(av_open_input_file(&m_av_format_context, qPrintable(fileTmp), inFmt, 0, &formatParams) != 0)
	//if(av_open_input_file(&m_av_format_context, "1", inFmt, 0, NULL) != 0)
	{
		qDebug() << "[WARN] CameraThread::load(): av_open_input_file() failed, fileTmp:"<<fileTmp;
		return false;
	}

	//dump_format(m_av_format_context, 0, qPrintable(m_cameraFile), 0);
	#ifdef DEBUG
	qDebug() << "[DEBUG] dump_format():";
	dump_format(m_av_format_context, 0, qPrintable(fileTmp), false);
	#endif

	uint i;

	// Find the first video stream
	m_video_stream = -1;
	m_audio_stream = -1;
	for(i = 0; i < m_av_format_context->nb_streams; i++)
	{
		if(m_av_format_context->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
		{
			m_video_stream = i;
		}
		if(m_av_format_context->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
		{
			m_audio_stream = i;
		}
	}
	if(m_video_stream == -1)
	{
		qDebug() << "[WARN] CameraThread::load(): Cannot find video stream.";
		return false;
	}

	// Get a pointer to the codec context for the video and audio streams
	m_video_codec_context = m_av_format_context->streams[m_video_stream]->codec;
// 	m_video_codec_context->get_buffer = our_get_buffer;
// 	m_video_codec_context->release_buffer = our_release_buffer;

	// Find the decoder for the video stream
	m_video_codec = avcodec_find_decoder(m_video_codec_context->codec_id);
	if(m_video_codec == NULL)
	{
		qDebug() << "[WARN] CameraThread::load(): avcodec_find_decoder() failed for codec_id:" << m_video_codec_context->codec_id;
		//return false;
	}

	// Open codec
	if(avcodec_open(m_video_codec_context, m_video_codec) < 0)
	{
		qDebug() << "[WARN] CameraThread::load(): avcodec_open() failed.";
		//return false;
	}

	// Allocate video frame
	m_av_frame = avcodec_alloc_frame();

	// Allocate an AVFrame structure
	m_av_rgb_frame =avcodec_alloc_frame();
	if(m_av_rgb_frame == NULL)
	{
		qDebug() << "[WARN] CameraThread::load(): avcodec_alloc_frame() failed.";
		return false;
	}

	#ifdef DEBUG
	qDebug() << "[DEBUG] codec context size:"<<m_video_codec_context->width<<"x"<<m_video_codec_context->height;
	#endif

	// Determine required buffer size and allocate buffer
	int num_bytes = avpicture_get_size(RAW_PIX_FMT, m_video_codec_context->width, m_video_codec_context->height);

	m_buffer = (uint8_t *)av_malloc(num_bytes * sizeof(uint8_t));

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset of AVPicture
	avpicture_fill((AVPicture *)m_av_rgb_frame, m_buffer, RAW_PIX_FMT,
					m_video_codec_context->width, m_video_codec_context->height);

	if(m_audio_stream != -1)
	{
		m_audio_codec_context = m_av_format_context->streams[m_audio_stream]->codec;

		m_audio_codec = avcodec_find_decoder(m_audio_codec_context->codec_id);
		if(!m_audio_codec)
		{
			//unsupported codec
			return false;
		}
		avcodec_open(m_audio_codec_context, m_audio_codec);
	}

	m_timebase = m_av_format_context->streams[m_video_stream]->time_base;

	m_inited = true;
// 	qDebug() << "CameraThread::initCamera(): finish";
	return 0;
}

void CameraThread::start(QThread::Priority priority)
{
	QThread::start(priority);
	
	/*
	connect(&m_readTimer, SIGNAL(timeout()), this, SLOT(readFrame()));
	double finalFps = m_fps * 1.5 * (m_deinterlace ? 2 : 1);
	qDebug() << "CameraThread::start: m_fps:"<<m_fps<<", finalFps:"<<finalFps;
	m_readTimer.setInterval(1000 / finalFps);
	initCamera();
	m_readTimer.start();
	*/
}

void CameraThread::run()
{
 	initCamera();

	//qDebug() << "CameraThread::run: In Thread ID "<<QThread::currentThreadId();
// 	int counter = 0;
	while(!m_killed)
	{
		readFrame();

// 		counter ++;
// 		if(m_singleFrame.holdTime>0)
// 		{
// 			QString file = QString("frame-%1.jpg").arg(counter %2 == 0?"even":"odd");
// 			qDebug() << "CameraThread::run(): frame:"<<counter<<", writing to file:"<<file;
// 			QImageWriter writer(file, "jpg");
// 			writer.write(m_singleFrame.image);
// 		}

		msleep(int(1000 / m_fps / 1.5 / (m_deinterlace ? 1 : 2)));
		//msleep(int(1000 / m_fps / 1.5));// (m_deinterlace ? 1 : 2)));
	};
}

void CameraThread::setDeinterlace(bool flag)
{
	m_deinterlace = flag;
}

void CameraThread::setFps(int fps)
{
	m_fps = fps;
}

CameraThread::~CameraThread()
{

	m_killed = true;
	quit();
	wait();

	#if defined(Q_OS_LINUX)
	if(m_v4l2)
	{
		delete m_v4l2;
		m_v4l2 = 0;
	}
	#endif

	freeResources();

	if(m_sws_context != NULL)
	{
		sws_freeContext(m_sws_context);
		m_sws_context = NULL;
	}

	if(m_frame != NULL)
	{
		delete m_frame;
		m_frame = 0;
	}
}

void CameraThread::freeResources()
{
	if(!m_inited)
		return;

	// Free the RGB image
	if(m_buffer != NULL)
		av_free(m_buffer);
	if(m_av_rgb_frame != NULL)
		av_free(m_av_rgb_frame);

	// Free the YUV frame
	//av_free(m_av_frame);
	//mutex.unlock();

	// Close the codec
	if(m_video_codec_context != NULL)
		avcodec_close(m_video_codec_context);

	// Close the video file
	if(m_av_format_context != NULL)
		av_close_input_file(m_av_format_context);
}

void CameraThread::enableRawFrames(bool enable)
{
	QMutexLocker lock(&m_readMutex);
	
	bool old = m_rawFrames;
	m_rawFrames = enable;

	
	if(old != enable)
	{
 		//qDebug() << "CameraThread::enableRawFrames(): start, flag:"<<enable;
		m_initMutex.lock(); // make sure init isnt running, block while it is
		// switch from raw V4L2 to LibAV* (or visa versa based on m_rawFrames, since SimpleV4L2 only outputs raw ARGB32)
		//qDebug() << "CameraThread::enableRawFrames: flag changed, status: "<<m_rawFrames;
		
		freeResources();
		
		m_initMutex.unlock(); // dont block init now
		
// 		qDebug() << "CameraThread::enableRawFrames(): mark1";
		initCamera();
		
 		//qDebug() << "CameraThread::enableRawFrames(): finish";
	}

}

VideoFormat CameraThread::videoFormat()
{

	return VideoFormat(
		m_rawFrames ?
				 VideoFrame::BUFFER_POINTER :
				 VideoFrame::BUFFER_IMAGE,
		m_rawFrames ?
			#if defined(Q_OS_LINUX)
				 QVideoFrame::Format_RGB32  :
			#else
				 QVideoFrame::Format_YUV420P :
			#endif
				 QVideoFrame::Format_ARGB32 //_Premultiplied
	);

	// Size defaults to 640,480
}

void CameraThread::readFrame()
{
	QMutexLocker lock(&m_readMutex);

	QTime capTime = QTime::currentTime();
	//qDebug() << "CameraThread::readFrame(): My Frame Count # "<<m_frameCount ++;
	m_frameCount ++;

	#if defined(Q_OS_LINUX)
	if(m_v4l2 && m_rawFrames)
	{
		if(!m_inited)
		{
			//qDebug() << "CameraThread::readFrame(): Unable to read raw from from V4L interface because not inited, error? "<<m_error;
			return;
		}
			
		VideoFrame *frame = m_v4l2->readFrame();
		if(frame->isValid())
		{
			frame->setCaptureTime(capTime);
			frame->setHoldTime(1000/m_fps);

			// We can do deinterlacing on these frames because SimpleV4L2 provides raw ARGB32 frames
			//m_deinterlace = false;
			if(m_deinterlace && frame->pointerLength() > 0)
			{
				VideoFrame *deinterlacedFrame = new VideoFrame();
				deinterlacedFrame->setCaptureTime ( frame->captureTime() );
 				deinterlacedFrame->setHoldTime    ( frame->holdTime()    );
 				deinterlacedFrame->setSize	  ( frame->size()    );
 				deinterlacedFrame->setIsRaw(true);
 				deinterlacedFrame->setBufferType(VideoFrame::BUFFER_POINTER);
				
				 // give us a new array, dont mudge the original image
				uchar * dest      = deinterlacedFrame->allocPointer(frame->pointerLength());
				uchar * src       = frame->pointer();
				const int h       = frame->size().height();
				const int stride  = frame->size().width()*4; // I can  cheat because I know SimpleV4L2 sends ARGB32 frames, with 4 bytes per pixel
				bool bottomFrame  = m_frameCount % 2 == 1;
				
				bobDeinterlace( (uchar*)src,  (uchar*)src +h*stride,
						(uchar*)dest, (uchar*)dest+h*stride,
						h, stride, bottomFrame);
						
				//qDebug() << "CameraThread::enqueue call: deinterlaced raw V4L2 frame:"<<deinterlacedFrame;
				enqueue(deinterlacedFrame);
				
				delete frame;
			}
			else
			{
				//qDebug() << "CameraThread::enqueue call: raw V4L2 frame:"<<frame;
				enqueue(frame);
			}
		}
		return;
	}
	#endif

	if(!m_inited)
	{
		//emit newImage(QImage());
		//emit frameReady(1000/m_fps);
		return;
	}
	AVPacket pkt1, *packet = &pkt1;
	double pts;



	int frame_finished = 0;
	while(!frame_finished && !m_killed)
	{
		if(av_read_frame(m_av_format_context, packet) >= 0)
		{
			// Is this a packet from the video stream?
			if(packet->stream_index == m_video_stream)
			{
				//global_video_pkt_pts = packet->pts;

//				mutex.lock();
				avcodec_decode_video(m_video_codec_context, m_av_frame, &frame_finished, packet->data, packet->size);
// 				mutex.unlock();

				if(packet->dts == (uint)AV_NOPTS_VALUE &&
						  m_av_frame->opaque &&
				  *(uint64_t*)m_av_frame->opaque != (uint)AV_NOPTS_VALUE)
				{
					pts = *(uint64_t *)m_av_frame->opaque;
				}
				else if(packet->dts != (uint)AV_NOPTS_VALUE)
				{
					pts = packet->dts;
				}
				else
				{
					pts = 0;
				}

				pts *= av_q2d(m_timebase);

				// Did we get a video frame?
				//frame_finished = 1;
				if(frame_finished)
				{

// 					if(m_rawFrames)
// 					{
//  						//qDebug() << "Decode Time: "<<capTime.msecsTo(QTime::currentTime())<<" ms";
// 						VideoFrame *frame = new VideoFrame(1000/m_fps,capTime);
// 						frame->setPointerData(m_av_frame->data, m_av_frame->linesize);
// 						frame->setPixelFormat(QVideoFrame::Format_YUV420P);
// 						frame->setSize(QSize(m_video_codec_context->width, m_video_codec_context->height));
// 						//qDebug() << "CameraThread::enqueue call: raw LibAV YUV420P frame";
// 						enqueue(frame);
// 					}
// 					else
					{
						// Convert the image from its native format to RGB, then copy the image data to a QImage
						if(m_sws_context == NULL)
						{
							//mutex.lock();
							//qDebug() << "Creating software scaler for pix_fmt: "<<m_video_codec_context->pix_fmt;
							m_sws_context = sws_getContext(
								m_video_codec_context->width, m_video_codec_context->height,
								m_video_codec_context->pix_fmt,
								m_video_codec_context->width, m_video_codec_context->height,
								//PIX_FMT_RGB32,SWS_BICUBIC,
								RAW_PIX_FMT, SWS_FAST_BILINEAR,
								NULL, NULL, NULL); //SWS_PRINT_INFO
							//mutex.unlock();
							//printf("decode(): created m_sws_context\n");
						}

						sws_scale(m_sws_context,
							m_av_frame->data,
							m_av_frame->linesize, 0,
							m_video_codec_context->height,
							m_av_rgb_frame->data,
							m_av_rgb_frame->linesize);

						if(m_deinterlace)
						{
							QImage frame(m_video_codec_context->width,
								m_video_codec_context->height,
								QImage::Format_ARGB32);//_Premultiplied);
							// I can cheat and claim premul because I know the video (should) never have alpha

							bool bottomFrame = m_frameCount % 2 == 1;

							uchar * dest = frame.scanLine(0); // use scanLine() instead of bits() to prevent deep copy
							uchar * src  = (uchar*)m_av_rgb_frame->data[0];
							const int h  = m_video_codec_context->height;
							const int stride = frame.bytesPerLine();

							bobDeinterlace( src,  src +h*stride,
									dest, dest+h*stride,
									h, stride, bottomFrame);

							//qDebug() << "CameraThread::enqueue call: deinterlaced QImage ARGB32 frame";
							enqueue(new VideoFrame(frame.copy(),1000/m_fps,capTime));
						}
						else
						{
							QImage frame(m_av_rgb_frame->data[0],
								m_video_codec_context->width,
								m_video_codec_context->height,
								//QImage::Format_RGB16);
								QImage::Format_ARGB32); //_Premultiplied);

							//qDebug() << "CameraThread::enqueue call: QImage ARGB32 frame";
							enqueue(new VideoFrame(frame.copy(),1000/m_fps,capTime));
						}
					}

					av_free_packet(packet);


				}

			}
			else if(packet->stream_index == m_audio_stream)
			{
				//decode audio packet, store in queue
				av_free_packet(packet);
			}
			else
			{
				av_free_packet(packet);
			}
		}
		else
		{
			//emit reachedEnd();
// 			qDebug() << "reachedEnd()";
		}
	}
}
