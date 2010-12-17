#include <QImage>
#include <QMutex>
#include <QMutexLocker>

#ifndef UINT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

extern "C" {
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}
#include "QVideoDecoder.h"

#define SDL_AUDIO_BUFFER_SIZE 1024

#if defined(Q_OS_WIN)
	#define RAW_PIX_FMT PIX_FMT_BGR565
#else 
	#define RAW_PIX_FMT PIX_FMT_RGB565
#endif

//uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;

/* These are called whenever we allocate a frame
* buffer. We use this to store the global_pts in
* a frame at the time it is allocated.
*/
static uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;

static int our_get_buffer(AVCodecContext *c, AVFrame *pic)
{
	int ret = avcodec_default_get_buffer(c, pic);
	uint64_t *pts = (uint64_t*)av_malloc(sizeof(uint64_t));
	*pts = global_video_pkt_pts;
	pic->opaque = pts;
	return ret;
}

static void our_release_buffer(AVCodecContext *c, AVFrame *pic)
{
	if(pic) av_freep(&pic->opaque);
	avcodec_default_release_buffer(c, pic);
}

int Round(double value);
//void audio_callback(void *userdata, Uint8 *stream, int len);
//int our_get_buffer(AVCodecContext *c, AVFrame *pic);
//void our_release_buffer(AVCodecContext *c, AVFrame *pic);

struct PacketQueue
{
	QQueue<AVPacket> audio_packets;
	QQueue<AVPacket> video_packets;
};

PacketQueue g_packet_queue;

QMutex mutex;


QVideoDecoder::QVideoDecoder(QVideo * video, QObject * parent) : QThread(parent),
	m_start_timestamp(0),
	m_initial_decode(true),
	m_previous_pts(0.0),
	m_killed(false),
	m_video_clock(0)
{
	m_video = video;

	m_time_base_rational.num = 1;
	m_time_base_rational.den = AV_TIME_BASE;

	m_sws_context = NULL;
// 	m_frame = NULL;

	m_video_buffer = new QVideoBuffer(this);
	connect(m_video_buffer, SIGNAL(nowEmpty()), this, SLOT(decode()));
	//connect(this, SIGNAL(decodeMe()), this, SLOT(decode()));
	connect(this, SIGNAL(newFrame(QFFMpegVideoFrame)), this, SLOT(addFrame(QFFMpegVideoFrame)));
	//connect(this, SIGNAL(newFrame(QFFMpegVideoFrame)), this, SLOT(setCurrentFrame(QFFMpegVideoFrame)));
}

QVideoDecoder::~QVideoDecoder()
{
	m_killed = true;
	quit();
	wait();

	if(m_video->m_video_loaded)
	{
		freeResources();
	}

	if(m_sws_context != NULL)
	{
		sws_freeContext(m_sws_context);
		m_sws_context = NULL;
	}

// 	if(m_frame != NULL)
// 	{
// 		delete m_frame;
// 		m_frame = 0;
// 	}
}

bool QVideoDecoder::load(const QString & filename)
{
	//if(!QFile::exists(filename))
	//	return false;

	//QMutexLocker locker(&mutex);
    static int debugCounter = 0;

	 AVInputFormat *inFmt = NULL;
	 AVFormatParameters formatParams;
	memset(&formatParams, 0, sizeof(AVFormatParameters));

	 QString fileTmp = filename;
	 //if(debugCounter ++ <= 0)
		//fileTmp = "vfwcap://0";
	if(fileTmp == "C:/dummy.txt")
		fileTmp = "vfwcap://0";
	qDebug() << "[DEBUG] QVideoDecoder::load(): starting with fileTmp:"<<fileTmp;
	 bool customInputFormat = false;
	 if(fileTmp.indexOf("://") > -1)
	 {
		 QStringList list = fileTmp.split("://");
		 if(list.size() == 2)
		 {
			 qDebug() << "[DEBUG] QVideoDecoder::load(): input format args:"<<list;
			 fileTmp = list[1];
			 if(fileTmp.isEmpty())
				fileTmp = "0";
			 avdevice_register_all();
			 QString fmt = list[0];
			 if(fmt == "cap")
				fmt = "vfwcap";
			 inFmt = av_find_input_format(qPrintable(list[0]));
			 if( !inFmt )
			 {
				   qDebug() << "[ERROR] QVideoDecoder::load(): Unable to find input format:"<<list[0];
				   return -1;
			 }


			 formatParams.time_base.num = 1;
			 formatParams.time_base.den = 25;

			 customInputFormat = true;
		}

	}




	// Open video file
	 //
	if(av_open_input_file(&m_av_format_context, qPrintable(fileTmp), inFmt, 0, &formatParams) != 0)
	//if(av_open_input_file(&m_av_format_context, "1", inFmt, 0, NULL) != 0)
	{
		qDebug() << "[WARN] QVideoDecoder::load(): av_open_input_file() failed, fileTmp:"<<fileTmp;
		return false;
	}

	// Retrieve stream information
	if(!customInputFormat)
	    if(av_find_stream_info(m_av_format_context) < 0)
	    {
		    qDebug() << "[WARN] QVideoDecoder::load(): av_find_stream_info() failed.";
		    return false;
	    }


	int i;

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
		qDebug() << "[WARN] QVideoDecoder::load(): Cannot find video stream.";
		return false;
	}

	// Get a pointer to the codec context for the video and audio streams
	m_video_codec_context = m_av_format_context->streams[m_video_stream]->codec;
// 	m_video_codec_context->get_buffer = our_get_buffer;
// 	m_video_codec_context->release_buffer = our_release_buffer;

	// Find the decoder for the video stream
	m_video_codec =avcodec_find_decoder(m_video_codec_context->codec_id);
	if(m_video_codec == NULL)
	{
		qDebug() << "[WARN] QVideoDecoder::load(): avcodec_find_decoder() failed.";
		return false;
	}

	// Open codec
	if(avcodec_open(m_video_codec_context, m_video_codec) < 0)
	{
		qDebug() << "[WARN] QVideoDecoder::load(): avcodec_open() failed.";
		return false;
	}

	// Allocate video frame
	m_av_frame = avcodec_alloc_frame();

	// Allocate an AVFrame structure
	m_av_rgb_frame =avcodec_alloc_frame();
	if(m_av_rgb_frame == NULL)
	{
		qDebug() << "[WARN] QVideoDecoder::load(): avcodec_alloc_frame() failed.";
		return false;
	}

	// Determine required buffer size and allocate buffer
	//int num_bytes = avpicture_get_size(PIX_FMT_RGB32, m_video_codec_context->width, m_video_codec_context->height);
	int num_bytes = avpicture_get_size(RAW_PIX_FMT, m_video_codec_context->width, m_video_codec_context->height);


	m_buffer = (uint8_t *)av_malloc(num_bytes * sizeof(uint8_t));

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset of AVPicture
	//avpicture_fill((AVPicture *)m_av_rgb_frame, m_buffer, PIX_FMT_RGB32, m_video_codec_context->width, m_video_codec_context->height);
	avpicture_fill((AVPicture *)m_av_rgb_frame, m_buffer, RAW_PIX_FMT, m_video_codec_context->width, m_video_codec_context->height);

	if(m_audio_stream != -1)
	{
		m_audio_codec_context = m_av_format_context->streams[m_audio_stream]->codec;

// 		// Set audio settings from codec info
// 		wanted_spec.freq = m_audio_codec_context->sample_rate;
// 		wanted_spec.format = AUDIO_S16SYS;
// 		wanted_spec.channels = m_audio_codec_context->channels;
// 		wanted_spec.silence = 0;
// 		wanted_spec.samples = SDL_AUDIO_BUFFER_SIZE;
// 		//wanted_spec.callback = audio_callback;
// 		wanted_spec.userdata = m_audio_codec_context;
//
// 		if(SDL_OpenAudio(&wanted_spec, &spec) < 0)
// 		{
// 			//error
// 			return false;
// 		}
//
		m_audio_codec = avcodec_find_decoder(m_audio_codec_context->codec_id);
		if(!m_audio_codec)
		{
			//unsupported codec
			return false;
		}
		avcodec_open(m_audio_codec_context, m_audio_codec);
	}

	m_timebase = m_av_format_context->streams[m_video_stream]->time_base;

	calculateVideoProperties();

	m_initial_decode = true;

	decode();

	m_video->m_video_loaded = true;

	return true;
}



void QVideoDecoder::unload()
{
	freeResources();
}

void QVideoDecoder::run()
{
	//forever
	//{
	//	if(g_packet_queue.audio_packets.count() < 100 || g_packet_queue.video_packets.count() < 100)
	//	{
	//		readFrame();
	//	}
	//	if(m_video_buffer->needsFrame())
	//	{
	//		decodeVideoFrame();
	//	}
	//}
// 	forever
// 	{
// 		qApp->processEvents();
// 		decode();
// 	}
	
	exec();
}

void QVideoDecoder::startDecoding()
{
	emit decodeMe();
	//m_decode_timer = startTimer(10);
}

void QVideoDecoder::test()
{
	while(true)
	{
	}
}

void QVideoDecoder::seek(int ms, int flags)
{
// 	QMutexLocker locker(&mutex);

	double seconds = (double)ms / 1000.0f;

	int64_t seek_target = (int64_t)(seconds * AV_TIME_BASE);

	seek_target = av_rescale_q(seek_target, m_time_base_rational,
		m_av_format_context->streams[m_video_stream]->time_base);

	av_seek_frame(m_av_format_context, m_video_stream, seek_target, flags);

	avcodec_flush_buffers(m_video_codec_context);
}

void QVideoDecoder::restart()
{
// 	if(m_frame != NULL)
// 	{
// 		delete m_frame;
// 		m_frame = 0;
// 	}
//
	if(!m_video->m_video_loaded)
		return;

	seek(0, AVSEEK_FLAG_BACKWARD);
}

QFFMpegVideoFrame QVideoDecoder::seekToFrame(int current_elapsed)
{
// 	QMutexLocker locker(&mutex);

	int num_frames_to_advance = m_fpms * current_elapsed;

	for(int i = 0; i < num_frames_to_advance; i++)
	{
		read();
	}

	decode();

	return m_current_frame;
}

void QVideoDecoder::read()
{
	emit ready(false);

	AVPacket packet;

	int frame_finished = 0;
	while(!frame_finished && !m_killed)
	{
		if(av_read_frame(m_av_format_context, &packet) >= 0)
		{
			// Is this a packet from the video stream?
			if(packet.stream_index == m_video_stream)
			{
				//global_video_pkt_pts = packet.pts;

				avcodec_decode_video(m_video_codec_context, m_av_frame, &frame_finished, packet.data, packet.size);

				// Did we get a video frame?
				if(frame_finished)
				{
					av_free_packet(&packet);
				}
			}
			else if(packet.stream_index == m_audio_stream)
			{
				//decode audio packet, store in queue
				av_free_packet(&packet);
			}
			else
			{
				av_free_packet(&packet);
			}
		}
		else
		{
			emit reachedEnd();
			this->restart();
		}
	}
}

void QVideoDecoder::decode()
{
	if(m_video->m_status == QVideo::NotRunning)
		return;

	emit ready(false);

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
				global_video_pkt_pts = packet->pts;

// 				mutex.lock();
				avcodec_decode_video(m_video_codec_context, m_av_frame, &frame_finished, packet->data, packet->size);
// 				mutex.unlock();

				if(packet->dts == AV_NOPTS_VALUE &&
					      m_av_frame->opaque &&
				  *(uint64_t*)m_av_frame->opaque != AV_NOPTS_VALUE)
				{
					pts = *(uint64_t *)m_av_frame->opaque;
				}
				else if(packet->dts != AV_NOPTS_VALUE)
				{
					pts = packet->dts;
				}
				else
				{
					pts = 0;
				}

				pts *= av_q2d(m_timebase);

				// Did we get a video frame?
				if(frame_finished)
				{
// 					size_t num_native_bytes = m_av_frame->linesize[0]     * m_video_codec_context->height;
// 					size_t num_rgb_bytes    = m_av_rgb_frame->linesize[0] * m_video_codec_context->height;

					// Convert the image from its native format to RGB, then copy the image data to a QImage
					if(m_sws_context == NULL)
					{
						mutex.lock();
						m_sws_context = sws_getContext(
							m_video_codec_context->width, m_video_codec_context->height,
							m_video_codec_context->pix_fmt,
							m_video_codec_context->width, m_video_codec_context->height,
							//PIX_FMT_RGB32,SWS_BICUBIC,
							RAW_PIX_FMT, SWS_FAST_BILINEAR,
							NULL, NULL, NULL); //SWS_PRINT_INFO
						mutex.unlock();
						//printf("decode(): created m_sws_context\n");
					}
					//printf("decode(): got frame\n");

// 					mutex.lock();
					sws_scale(m_sws_context,
						  m_av_frame->data,
						  m_av_frame->linesize, 0,
						  m_video_codec_context->height,
						  m_av_rgb_frame->data,
						  m_av_rgb_frame->linesize);
// 					mutex.unlock();

// 					size_t num_bytes = m_av_rgb_frame->linesize[0] * m_video_codec_context->height;

// 					if(m_frame)
// 						delete m_frame;

					m_frame = QImage(m_av_rgb_frame->data[0],
								m_video_codec_context->width,
								m_video_codec_context->height,
								QImage::Format_RGB16);

					av_free_packet(packet);

					// This block from the synchronize_video(VideoState *is, AVFrame *src_frame, double pts) : double
					// function given at: http://www.dranger.com/ffmpeg/tutorial05.html
					{
						// update the frame pts
						double frame_delay;

						if(pts != 0)
						{
							/* if we have pts, set video clock to it */
							m_video_clock = pts;
						} else {
							/* if we aren't given a pts, set it to the clock */
							pts = m_video_clock;
						}
						/* update the video clock */
						frame_delay = av_q2d(m_timebase);
						/* if we are repeating a frame, adjust clock accordingly */
						frame_delay += m_av_frame->repeat_pict * (frame_delay * 0.5);
						m_video_clock += frame_delay;
						//qDebug() << "Frame Dealy: "<<frame_delay;
					}


					QFFMpegVideoFrame video_frame;
					video_frame.frame = &m_frame;
					video_frame.pts = pts;
					video_frame.previous_pts = m_previous_pts;

					m_current_frame = video_frame;

					emit newFrame(video_frame);

					m_previous_pts = pts;

                                        //QTimer::singleShot(5, this, SLOT(decode()));
				}
			}
			else if(packet->stream_index == m_audio_stream)
			{
// 				mutex.lock();
				//decode audio packet, store in queue
				av_free_packet(packet);
// 				mutex.unlock();

			}
			else
			{
// 				mutex.lock();
				av_free_packet(packet);
// 				mutex.unlock();

			}
		}
		else
		{
			emit reachedEnd();
		}
	}
}

void QVideoDecoder::readFrame()
{
	emit ready(false);

	AVPacket packet;
	double pts;

	int frame_finished = 0;
	while(!frame_finished && !m_killed)
	{
		if(av_read_frame(m_av_format_context, &packet) >= 0)
		{
			// Is this a packet from the video stream?
			if(packet.stream_index == m_video_stream)
			{
				//global_video_pkt_pts = packet.pts;

				avcodec_decode_video(m_video_codec_context, m_av_frame, &frame_finished, packet.data, packet.size);

				if(packet.dts == AV_NOPTS_VALUE && m_av_frame->opaque && *(uint64_t*)m_av_frame->opaque != AV_NOPTS_VALUE)
				{
					pts = *(uint64_t *)m_av_frame->opaque;
				}
				else if(packet.dts != AV_NOPTS_VALUE)
				{
					pts = packet.dts;
				}
				else
				{
					pts = 0;
				}

				pts *= av_q2d(m_timebase);

				// Did we get a video frame?
				if(frame_finished)
				{
					// Convert the image from its native format to RGB, then copy the image data to a QImage
					if(m_sws_context == NULL)
					{
						m_sws_context = sws_getContext(m_video_codec_context->width, m_video_codec_context->height,
							m_video_codec_context->pix_fmt, m_video_codec_context->width, m_video_codec_context->height,
							PIX_FMT_RGB32, SWS_PRINT_INFO, NULL, NULL, NULL);
						//printf("readFrame(): created m_sws_context\n");
					}
					printf("readFrame(): got frame\n");

					sws_scale(m_sws_context, m_av_frame->data, m_av_frame->linesize, 0,
						m_video_codec_context->height, m_av_rgb_frame->data, m_av_rgb_frame->linesize);

					size_t num_bytes = m_av_rgb_frame->linesize[0] * m_video_codec_context->height;

					QImage * frame = new QImage(m_video_codec_context->width, m_video_codec_context->height, QImage::Format_RGB32);

					memcpy(frame->bits(), m_av_rgb_frame->data[0], num_bytes);

					av_free_packet(&packet);

					QFFMpegVideoFrame video_frame;
					video_frame.frame = frame;
					video_frame.pts = pts;
					video_frame.previous_pts = m_previous_pts;

					emit newFrame(video_frame);

					m_previous_pts = pts;
				}
			}
			else if(packet.stream_index == m_audio_stream)
			{
				//decode audio packet, store in queue
				av_free_packet(&packet);
			}
			else
			{
				av_free_packet(&packet);
			}
		}
		else
		{
			emit reachedEnd();
		}
	}
}

void QVideoDecoder::decodeVideoFrame()
{
	//get next video frame from global queue
	//decode it and add to video buffer
	//freepacket
	//global_video_pkt_pts = packet.pts;

	int frame_finished = 0;
	long double pts;

	while(!frame_finished && !m_killed)
	{
		AVPacket packet;
		int num_packets = g_packet_queue.video_packets.count();
		if(num_packets > 0)
			packet = g_packet_queue.video_packets.dequeue();
		else
			return;

		avcodec_decode_video(m_video_codec_context, m_av_frame, &frame_finished, packet.data, packet.size);

		if(packet.dts == AV_NOPTS_VALUE && m_av_frame->opaque && *(uint64_t*)m_av_frame->opaque != AV_NOPTS_VALUE)
		{
			pts = *(uint64_t *)m_av_frame->opaque;
		}
		else if(packet.dts != AV_NOPTS_VALUE)
		{
			pts = packet.dts;
		}
		else
		{
			pts = 0;
		}

		pts *= av_q2d(m_timebase);

		// Did we get a video frame?
		if(frame_finished)
		{
			// Convert the image from its native format to RGB, then copy the image data to a QImage
			if(m_sws_context == NULL)
			{
				m_sws_context = sws_getContext(m_video_codec_context->width, m_video_codec_context->height,
					m_video_codec_context->pix_fmt, m_video_codec_context->width, m_video_codec_context->height,
					PIX_FMT_RGB32, SWS_PRINT_INFO, NULL, NULL, NULL);
				//printf("decodeVideoFrame(): created m_sws_context\n");
			}
			printf("decodeVideoFrame(): got frame\n");

			sws_scale(m_sws_context, m_av_frame->data, m_av_frame->linesize, 0,
				m_video_codec_context->height, m_av_rgb_frame->data, m_av_rgb_frame->linesize);

			size_t num_bytes = m_av_rgb_frame->linesize[0] * m_video_codec_context->height;

			QImage * frame = new QImage(m_video_codec_context->width, m_video_codec_context->height, QImage::Format_RGB32);

			memcpy(frame->bits(), m_av_rgb_frame->data[0], num_bytes);

			av_free_packet(&packet);

			QFFMpegVideoFrame video_frame;
			video_frame.frame = frame;
			video_frame.pts = pts;
			video_frame.previous_pts = m_previous_pts;

			emit newFrame(video_frame);

			m_previous_pts = pts;
		}
	}
}

QFFMpegVideoFrame QVideoDecoder::getNextFrame()
{
	QFFMpegVideoFrame video_frame = m_video_buffer->getNextFrame();
	return video_frame;
}

void QVideoDecoder::addFrame(QFFMpegVideoFrame video_frame)
{
	m_video_buffer->addFrame(video_frame);
	emit ready(true);
}

void QVideoDecoder::setCurrentFrame(QFFMpegVideoFrame frame)
{
	m_current_frame = frame;
}

void QVideoDecoder::flushBuffers()
{
	m_video_buffer->flush();
}

void QVideoDecoder::timerEvent(QTimerEvent * te)
{
	if(te->timerId() == m_decode_timer)
	{
		if(g_packet_queue.audio_packets.count() < 100 || g_packet_queue.video_packets.count() < 100)
		{
			readFrame();
		}
		else
		{
			if(g_packet_queue.video_packets.count() > 1)
			{
				decodeVideoFrame();
			}
		}
	}
}

void QVideoDecoder::calculateVideoProperties()
{
	//filename
	m_video->m_filename = QString(m_av_format_context->filename);

	//frame rate
	m_video->m_frame_rate = Round(av_q2d(m_av_format_context->streams[m_video_stream]->r_frame_rate));
	m_fpms = (double)m_video->m_frame_rate / 1000.0f;
	//printf("m_fpms = %.02f, frame_rate=%d\n",m_fpms,m_video->m_frame_rate);

	//duration
	m_video->m_duration = (m_av_format_context->duration / AV_TIME_BASE);

	//framesize
	m_video->m_frame_size = QSize(m_video_codec_context->width, m_video_codec_context->height);
}

quint64 QVideoDecoder::calculatePTS(quint64 dts)
{
	double timebase_d = av_q2d(m_timebase);
	double test = dts * timebase_d;
	double display_timestamp = ((double)dts * timebase_d) - (double)m_start_timestamp;

	return (quint64)display_timestamp;
}

void QVideoDecoder::freeResources()
{
 	//QMutexLocker locker(&mutex);
	if(m_video->m_video_loaded)
	{
		//mutex.lock();
		// Free the RGB image
		av_free(m_buffer);
		av_free(m_av_rgb_frame);

		// Free the YUV frame
		//av_free(m_av_frame);
		//mutex.unlock();

		// Close the codec
		avcodec_close(m_video_codec_context);

		// Close the video file
		av_close_input_file(m_av_format_context);
	}
	m_video->m_video_loaded = false;
}

int Round(double value)
{
	return (int)(value + 0.5f);
}

//void audio_callback(void *userdata, Uint8 *stream, int len)
//{
//
//}
