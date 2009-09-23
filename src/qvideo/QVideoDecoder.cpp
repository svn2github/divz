#include <QImage>

extern "C" {
#include "libswscale/swscale.h"
}
#include "QVideoDecoder.h"

#define SDL_AUDIO_BUFFER_SIZE 1024


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

QVideoDecoder::QVideoDecoder(QVideo * video, QObject * parent) : QThread(parent), 
	m_start_timestamp(0), 
	m_initial_decode(true),
	m_previous_pts(0.0)
{
	m_video = video;

	m_time_base_rational.num = 1;
	m_time_base_rational.den = AV_TIME_BASE;
	
	m_sws_context = NULL;
	m_frame = NULL;

	m_video_buffer = new QVideoBuffer(this);
	connect(m_video_buffer, SIGNAL(nowEmpty()), this, SLOT(decode()));
	//connect(this, SIGNAL(decodeMe()), this, SLOT(decode()));
	connect(this, SIGNAL(newFrame(QFFMpegVideoFrame)), this, SLOT(addFrame(QFFMpegVideoFrame)));
	connect(this, SIGNAL(newFrame(QFFMpegVideoFrame)), this, SLOT(setCurrentFrame(QFFMpegVideoFrame)));
}

QVideoDecoder::~QVideoDecoder()
{
	if(m_video->m_video_loaded)
	{
		freeResources();
		
	}
	
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

bool QVideoDecoder::load(const QString & filename)
{
	if(!QFile::exists(filename))
		return false;

	// Open video file
	if(av_open_input_file(&m_av_format_context, qPrintable(filename), NULL, 0, NULL) != 0)
	{
		fprintf(stderr,"[WARN] QVideoDecoder::load(): av_open_input_file() failed.\n");
		return false;
	}

	// Retrieve stream information
	if(av_find_stream_info(m_av_format_context) < 0) 
	{
		fprintf(stderr,"[WARN] QVideoDecoder::load(): av_find_stream_info() failed.\n");
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
		fprintf(stderr,"[WARN] QVideoDecoder::load(): Cannot find video stream.\n");
		return false;
	}

	// Get a pointer to the codec context for the video and audio streams
	m_video_codec_context = m_av_format_context->streams[m_video_stream]->codec;
	//m_video_codec_context->get_buffer = our_get_buffer;
	//m_video_codec_context->release_buffer = our_release_buffer;

	// Find the decoder for the video stream
	m_video_codec =avcodec_find_decoder(m_video_codec_context->codec_id);
	if(m_video_codec == NULL) 
	{
		fprintf(stderr,"[WARN] QVideoDecoder::load(): avcodec_find_decoder() failed.\n");
		return false;
	}

	// Open codec
	if(avcodec_open(m_video_codec_context, m_video_codec) < 0)
	{
		fprintf(stderr,"[WARN] QVideoDecoder::load(): avcodec_open() failed.\n");
		return false;
	}

	// Allocate video frame
	m_av_frame = avcodec_alloc_frame();

	// Allocate an AVFrame structure
	m_av_rgb_frame =avcodec_alloc_frame();
	if(m_av_rgb_frame == NULL)
	{
		fprintf(stderr,"[WARN] QVideoDecoder::load(): avcodec_alloc_frame() failed.\n");
		return false;
	}

	// Determine required buffer size and allocate buffer
	int num_bytes = avpicture_get_size(PIX_FMT_RGB32, m_video_codec_context->width, m_video_codec_context->height);

	m_buffer = (uint8_t *)av_malloc(num_bytes * sizeof(uint8_t));

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset of AVPicture
	avpicture_fill((AVPicture *)m_av_rgb_frame, m_buffer, PIX_FMT_RGB32,
					m_video_codec_context->width, m_video_codec_context->height);

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

//uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;

/* These are called whenever we allocate a frame
* buffer. We use this to store the global_pts in
* a frame at the time it is allocated.
*/

//int our_get_buffer(AVCodecContext *c, AVFrame *pic) 
//{
//	int ret = avcodec_default_get_buffer(c, pic);
//	uint64_t *pts = (uint64_t*)av_malloc(sizeof(uint64_t));
//	*pts = global_video_pkt_pts;
//	pic->opaque = pts;
//	return ret;
//}
//
//void our_release_buffer(AVCodecContext *c, AVFrame *pic) 
//{
//	if(pic) av_freep(&pic->opaque);
//	avcodec_default_release_buffer(c, pic);
//}

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
	double seconds = (double)ms / 1000.0f;

	int64_t seek_target = (int64_t)(seconds * AV_TIME_BASE);

	seek_target = av_rescale_q(seek_target, m_time_base_rational, 
		m_av_format_context->streams[m_video_stream]->time_base);

	av_seek_frame(m_av_format_context, m_video_stream, seek_target, flags);

	avcodec_flush_buffers(m_video_codec_context);
}

void QVideoDecoder::restart()
{
	seek(0, AVSEEK_FLAG_BACKWARD);
}

QFFMpegVideoFrame QVideoDecoder::seekToFrame(int current_elapsed)
{
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
	while(!frame_finished)
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
	emit ready(false);

	AVPacket pkt1, *packet = &pkt1;
	double pts;

	int frame_finished = 0;
	while(!frame_finished)
	{
		if(av_read_frame(m_av_format_context, packet) >= 0) 
		{
			// Is this a packet from the video stream?
			if(packet->stream_index == m_video_stream) 
			{
				//global_video_pkt_pts = packet.pts;

				avcodec_decode_video(m_video_codec_context, m_av_frame, &frame_finished, packet->data, packet->size);

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
					size_t num_native_bytes = m_av_frame->linesize[0] * m_video_codec_context->height;

					size_t num_rgb_bytes = m_av_rgb_frame->linesize[0] * m_video_codec_context->height;

					// Convert the image from its native format to RGB, then copy the image data to a QImage
					if(m_sws_context == NULL)
					{
						m_sws_context = sws_getContext(m_video_codec_context->width, m_video_codec_context->height, 
							m_video_codec_context->pix_fmt, m_video_codec_context->width, m_video_codec_context->height, 
							PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL); //SWS_PRINT_INFO
						//printf("decode(): created m_sws_context\n");
					}
					//printf("decode(): got frame\n");

					sws_scale(m_sws_context, m_av_frame->data, m_av_frame->linesize, 0, 
						m_video_codec_context->height, m_av_rgb_frame->data, m_av_rgb_frame->linesize);

					

					size_t num_bytes = m_av_rgb_frame->linesize[0] * m_video_codec_context->height;

					if(m_frame == NULL)
					{
						m_frame = new QImage(m_video_codec_context->width, m_video_codec_context->height, QImage::Format_RGB32);
					}

					memcpy(m_frame->bits(), m_av_rgb_frame->data[0], num_bytes);

					av_free_packet(packet);

					QFFMpegVideoFrame video_frame;
					video_frame.frame = m_frame;
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
	while(!frame_finished)
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

	while(!frame_finished)
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
	if(m_video->m_video_loaded)
	{
		// Free the RGB image
		av_free(m_buffer);
		av_free(m_av_rgb_frame);

		// Free the YUV frame
		av_free(m_av_frame);

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
