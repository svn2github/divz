#include "QVideoEncoder.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int * stream_duration;
int * stream_frame_rate;
int * stream_width;
int * stream_height;

#define STREAM_PIX_FMT PIX_FMT_YUV420P

int sws_flags = SWS_BICUBIC;

//non-member video functions
AVStream * add_video_stream(AVFormatContext *oc, int codec_id);
AVFrame * alloc_picture(int pix_fmt, int width, int height);
static void open_video(AVFormatContext *oc, AVStream *st);
void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height);
void write_video_frame(QImage source_frame, AVFormatContext *oc, AVStream *st);
void close_video(AVFormatContext *oc, AVStream *st);

//non-member audio functions
AVStream *add_audio_stream(AVFormatContext *oc, int codec_id);
void open_audio(AVFormatContext *oc, AVStream *st);
void get_audio_frame(int16_t *samples, int frame_size, int nb_channels);
void write_audio_frame(AVFormatContext *oc, AVStream *st);
void close_audio(AVFormatContext *oc, AVStream *st);

QVideoEncoder::QVideoEncoder(QVideo * video, QObject * parent) : QThread(parent), 
	m_output_filename("output.avi"),
	m_frame_rate(30),
	m_duration(10),
	m_output_width(720),
	m_output_height(480)
{
	m_video = video;

	stream_duration = &m_duration;
	stream_frame_rate = &m_frame_rate;
	stream_height = &m_output_height;
	stream_width = &m_output_width;
}

QVideoEncoder::~QVideoEncoder()
{}

void QVideoEncoder::run()
{
	AVOutputFormat * output_format;
	AVFormatContext * output_format_context;
	AVStream * audio_stream, * video_stream;
	double audio_pts, video_pts;
	int i;

	//detect the output format from the name.
	output_format = guess_format(NULL, qPrintable(m_output_filename), NULL);
	if (!output_format) 
	{
		emit errorStatus("Encoder: Could not deduce output format from file extension: using MPEG.", false);
		output_format = guess_format("mpeg", NULL, NULL);
	}
	if (!output_format) 
	{
		emit errorStatus("Encoder: Could not find suitable output format.", true);
		return;
	}

	//allocate the output media context
	output_format_context = av_alloc_format_context();
	if (!output_format_context) 
	{
		emit errorStatus("Decoder Memory allocationo error.", true);
		return;
	}
	output_format_context->oformat = output_format;

	//add the audio and video streams using the default format codecs and initialize
	video_stream = NULL;
	if (output_format->video_codec != CODEC_ID_NONE)
		video_stream = add_video_stream(output_format_context, output_format->video_codec);

	audio_stream = NULL;
	if (output_format->audio_codec != CODEC_ID_NONE)
		audio_stream = add_audio_stream(output_format_context, output_format->audio_codec);

	//set the output parameters (must be done even if no parameters).
	if (av_set_parameters(output_format_context, NULL) < 0) 
	{
		emit errorStatus("Invalid output format parameters.", true);
		return;
	}

	dump_format(output_format_context, 0, qPrintable(m_output_filename), 1);

	//now that all the parameters are set, we can open the audio and
	//video codecs and allocate the necessary encode buffers
	if (video_stream)
		open_video(output_format_context, video_stream);
	if (audio_stream)
		open_audio(output_format_context, audio_stream);

	//open the output file, if needed
	if (!(output_format->flags & AVFMT_NOFILE)) 
	{
		if (url_fopen(&output_format_context->pb, qPrintable(m_output_filename), URL_WRONLY) < 0) 
		{
			QString status = "Could not open " + m_output_filename;
			emit errorStatus(status, true);
			return;
		}
	}

	//write the stream header, if any
	av_write_header(output_format_context);

	m_video->restart();

	forever
	{
		//compute current audio and video time
		if (audio_stream)
			audio_pts = (double)audio_stream->pts.val * audio_stream->time_base.num / audio_stream->time_base.den;
		else
			audio_pts = 0.0;

		if (video_stream)
			video_pts = (double)video_stream->pts.val * video_stream->time_base.num / video_stream->time_base.den;
		else
			video_pts = 0.0;

		if(!video_stream || video_pts >= *stream_duration)
		{
			break;
		}

		static int last_pts = 0;
		int pts = video_pts * 1000;
		pts -= last_pts;
		last_pts = video_pts * 1000;
		//here advance source video/audio the correct pts ms
		QImage frame = m_video->advance(pts);

		//write interleaved audio and video frames
		//if (!video_stream || (video_stream && audio_stream && audio_pts < video_pts)) {
			//write_audio_frame(output_format_context, audio_stream);
		//} else {
			write_video_frame(frame, output_format_context, video_stream);
		//}
	}

	//FINISHED:
	emit encodeFinished();

	//close each codec
	if (video_stream)
		close_video(output_format_context, video_stream);
	if (audio_stream)
		close_audio(output_format_context, audio_stream);

	//write the trailer, if any
	av_write_trailer(output_format_context);

	//free the streams
	for(i = 0; i < output_format_context->nb_streams; i++) {
		av_freep(&output_format_context->streams[i]->codec);
		av_freep(&output_format_context->streams[i]);
	}

	if (!(output_format->flags & AVFMT_NOFILE)) {
		//close the output file
		url_fclose(output_format_context->pb);
	}

	//free the stream
	av_free(output_format_context);
}

void QVideoEncoder::setOutputSize(QSize size)
{
	m_output_width = size.width();
	m_output_height = size.height();
}

/**************************************************************/
/* video output */

AVFrame *picture, *tmp_picture, *rgb_picture;
uint8_t *video_outbuf;
int frame_count, video_outbuf_size;

/* add a video output stream */
AVStream * add_video_stream(AVFormatContext *oc, int codec_id)
{
	AVCodecContext *c;
	AVStream *st;

	st = av_new_stream(oc, 0);
	if (!st) {
		fprintf(stderr, "Could not alloc stream\n");
		exit(1);
	}

	c = st->codec;
	c->codec_id = (CodecID)codec_id;
	c->codec_type = CODEC_TYPE_VIDEO;

	/* put sample parameters */
	c->bit_rate = 2048000;
	/* resolution must be a multiple of two */
	c->width = *stream_width;
	c->height = *stream_height;
	/* time base: this is the fundamental unit of time (in seconds) in terms
	of which frame timestamps are represented. for fixed-fps content,
	timebase should be 1/framerate and timestamp increments should be
	identically 1. */
	c->time_base.den = 25;
	//*stream_frame_rate;
	c->time_base.num = 1;
	c->gop_size = 12; /* emit one intra frame every twelve frames at most */
	c->pix_fmt = STREAM_PIX_FMT;
	if (c->codec_id == CODEC_ID_MPEG2VIDEO) {
		/* just for testing, we also add B frames */
		c->max_b_frames = 2;
	}
	if (c->codec_id == CODEC_ID_MPEG1VIDEO){
		/* Needed to avoid using macroblocks in which some coeffs overflow.
		This does not happen with normal video, it just happens here as
		the motion of the chroma plane does not match the luma plane. */
		c->mb_decision=2;
	}
	// some formats want stream headers to be separate
	if(!strcmp(oc->oformat->name, "mp4") || !strcmp(oc->oformat->name, "mov") || !strcmp(oc->oformat->name, "3gp"))
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;

	return st;
}

AVFrame * alloc_picture(PixelFormat pix_fmt, int width, int height)
{
	AVFrame *picture;
	uint8_t *picture_buf;
	int size;

	picture = avcodec_alloc_frame();
	if (!picture)
		return NULL;
	size = avpicture_get_size(pix_fmt, width, height);
	picture_buf = (uint8_t*)av_malloc(size);
	if (!picture_buf) {
		av_free(picture);
		return NULL;
	}
	avpicture_fill((AVPicture *)picture, picture_buf,
		pix_fmt, width, height);
	return picture;
}

static void open_video(AVFormatContext *oc, AVStream *st)
{
	AVCodec *codec;
	AVCodecContext *c;

	c = st->codec;

	/* find the video encoder */
	codec = avcodec_find_encoder(c->codec_id);
	if (!codec) {
		fprintf(stderr, "codec not found\n");
		exit(1);
	}

	/* open the codec */
	if (avcodec_open(c, codec) < 0) {
		fprintf(stderr, "could not open codec\n");
		exit(1);
	}

	video_outbuf = NULL;
	if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
		/* allocate output buffer */
		/* XXX: API change will be done */
		/* buffers passed into lav* can be allocated any way you prefer,
		as long as they're aligned enough for the architecture, and
		they're freed appropriately (such as using av_free for buffers
		allocated with av_malloc) */
		video_outbuf_size = 200000;
		video_outbuf = (uint8_t*)av_malloc(video_outbuf_size);
	}

	/* allocate the encoded raw picture */
	picture = alloc_picture(c->pix_fmt, c->width, c->height);
	if (!picture) {
		fprintf(stderr, "Could not allocate picture\n");
		exit(1);
	}

	rgb_picture = alloc_picture(PIX_FMT_RGB32, c->width, c->height);
	if(!rgb_picture)
	{
		//could not allocate rgb image...
		exit(1);
	}

	/* if the output format is not YUV420P, then a temporary YUV420P
	picture is needed too. It is then converted to the required
	output format */
	tmp_picture = NULL;
	if (c->pix_fmt != PIX_FMT_YUV420P) 
	{
		tmp_picture = alloc_picture(PIX_FMT_YUV420P, c->width, c->height);
		if (!tmp_picture) 
		{
			fprintf(stderr, "Could not allocate temporary picture\n");
			exit(1);
		}
	}
}

//prepare a dummy image
void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height)
{
	int x, y, i;

	i = frame_index;

	/* Y */
	for(y=0;y<height;y++) {
		for(x=0;x<width;x++) {
			pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;
		}
	}

	/* Cb and Cr */
	for(y=0;y<height/2;y++) {
		for(x=0;x<width/2;x++) {
			pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
			pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
		}
	}
}

void write_video_frame(QImage source_frame, AVFormatContext *oc, AVStream *st)
{
	int out_size, ret;
	AVCodecContext *c;
	static struct SwsContext *img_convert_ctx;

	c = st->codec;

	int stream_num_frames = (*stream_frame_rate) * (*stream_duration);
	if (frame_count >= stream_num_frames) {
		/* no more frame to compress. The codec has a latency of a few
		frames if using B frames, so we get the last frames by
		passing the same picture again */
	} else {
		if (c->pix_fmt != PIX_FMT_YUV420P) 
		{
			/* as we only generate a YUV420P picture, we must convert it
			to the codec pixel format if needed */
			if (img_convert_ctx == NULL) 
			{
				img_convert_ctx = sws_getContext(c->width, c->height,
					PIX_FMT_YUV420P,
					c->width, c->height,
					c->pix_fmt,
					sws_flags, NULL, NULL, NULL);
				if (img_convert_ctx == NULL) 
				{
					fprintf(stderr, "Cannot initialize the conversion context\n");
					exit(1);
				}
			}
			fill_yuv_image(tmp_picture, frame_count, c->width, c->height);
			sws_scale(img_convert_ctx, tmp_picture->data, tmp_picture->linesize,
				0, c->height, picture->data, picture->linesize);
		} 
		else 
		{
			//here copy the bits from the source to picture's data
			//first you need to convert QImage RGB data to YUV
			//fill_yuv_image(picture, frame_count, c->width, c->height);

			size_t num_bytes = rgb_picture->linesize[0] * c->height;
			memcpy(rgb_picture->data[0], source_frame.bits(), num_bytes);

			SwsContext * sws_context = sws_getContext(source_frame.width(), source_frame.height(), 
				PIX_FMT_RGB32, c->width, c->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
				//SWS_PRINT_INFO

			sws_scale(sws_context, rgb_picture->data, rgb_picture->linesize, 0, 
				c->height, picture->data, picture->linesize);

			sws_freeContext(sws_context);
		}
	}

	if (oc->oformat->flags & AVFMT_RAWPICTURE) {
		/* raw video case. The API will change slightly in the near
		future for that */
		AVPacket pkt;
		av_init_packet(&pkt);

		pkt.flags |= PKT_FLAG_KEY;
		pkt.stream_index= st->index;
		pkt.data= (uint8_t *)picture;
		pkt.size= sizeof(AVPicture);

		ret = av_write_frame(oc, &pkt);
	} else {
		/* encode the image */
		out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);
		/* if zero size, it means the image was buffered */
		if (out_size > 0) {
			AVPacket pkt;
			av_init_packet(&pkt);

			pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
			if(c->coded_frame->key_frame)
				pkt.flags |= PKT_FLAG_KEY;
			pkt.stream_index= st->index;
			pkt.data= video_outbuf;
			pkt.size= out_size;

			/* write the compressed frame in the media file */
			ret = av_write_frame(oc, &pkt);
		} else {
			ret = 0;
		}
	}
	if (ret != 0) {
		fprintf(stderr, "Error while writing video frame\n");
		exit(1);
	}
	frame_count++;
}

void close_video(AVFormatContext *oc, AVStream *st)
{
	avcodec_close(st->codec);
	av_free(picture->data[0]);
	av_free(picture);
	if (tmp_picture) 
	{
		av_free(tmp_picture->data[0]);
		av_free(tmp_picture);
	}
	if(rgb_picture)
	{
		av_free(rgb_picture->data[0]);
		av_free(rgb_picture);
	}
	av_free(video_outbuf);
}

/**************************************************************/
/* audio output */

float t, tincr, tincr2;
int16_t *samples;
uint8_t *audio_outbuf;
int audio_outbuf_size;
int audio_input_frame_size;

/*
* add an audio output stream
*/
AVStream * add_audio_stream(AVFormatContext *oc, int codec_id)
{
	AVCodecContext *c;
	AVStream *st;

	st = av_new_stream(oc, 1);
	if (!st) {
		fprintf(stderr, "Could not alloc stream\n");
		exit(1);
	}

	c = st->codec;
	c->codec_id = (CodecID)codec_id;
	c->codec_type = CODEC_TYPE_AUDIO;

	/* put sample parameters */
	c->bit_rate = 64000;
	c->sample_rate = 44100;
	c->channels = 2;
	return st;
}

void open_audio(AVFormatContext *oc, AVStream *st)
{
	AVCodecContext *c;
	AVCodec *codec;

	c = st->codec;

	/* find the audio encoder */
	codec = avcodec_find_encoder(c->codec_id);
	if (!codec) {
		fprintf(stderr, "codec not found\n");
		exit(1);
	}

	/* open it */
	if (avcodec_open(c, codec) < 0) {
		fprintf(stderr, "could not open codec\n");
		exit(1);
	}

	/* init signal generator */
	t = 0;
	tincr = 2 * M_PI * 110.0 / c->sample_rate;
	/* increment frequency by 110 Hz per second */
	tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

	audio_outbuf_size = 10000;
	audio_outbuf = (uint8_t*)av_malloc(audio_outbuf_size);

	/* ugly hack for PCM codecs (will be removed ASAP with new PCM
	support to compute the input frame size in samples */
	if (c->frame_size <= 1) {
		audio_input_frame_size = audio_outbuf_size / c->channels;
		switch(st->codec->codec_id) {
		case CODEC_ID_PCM_S16LE:
		case CODEC_ID_PCM_S16BE:
		case CODEC_ID_PCM_U16LE:
		case CODEC_ID_PCM_U16BE:
			audio_input_frame_size >>= 1;
			break;
		default:
			break;
		}
	} else {
		audio_input_frame_size = c->frame_size;
	}
	samples = (int16_t*)av_malloc(audio_input_frame_size * 2 * c->channels);
}

/* prepare a 16 bit dummy audio frame of 'frame_size' samples and
'nb_channels' channels */
void get_audio_frame(int16_t *samples, int frame_size, int nb_channels)
{
	int j, i, v;
	int16_t *q;

	q = samples;
	for(j=0;j<frame_size;j++) {
		v = (int)(sin(t) * 10000);
		for(i = 0; i < nb_channels; i++)
			*q++ = v;
		t += tincr;
		tincr += tincr2;
	}
}

void write_audio_frame(AVFormatContext *oc, AVStream *st)
{
	AVCodecContext *c;
	AVPacket pkt;
	av_init_packet(&pkt);

	c = st->codec;

	get_audio_frame(samples, audio_input_frame_size, c->channels);

	pkt.size= avcodec_encode_audio(c, audio_outbuf, audio_outbuf_size, samples);

	pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
	pkt.flags |= PKT_FLAG_KEY;
	pkt.stream_index= st->index;
	pkt.data= audio_outbuf;

	/* write the compressed frame in the media file */
	if (av_write_frame(oc, &pkt) != 0) {
		fprintf(stderr, "Error while writing audio frame\n");
		exit(1);
	}
}

void close_audio(AVFormatContext *oc, AVStream *st)
{
	avcodec_close(st->codec);

	av_free(samples);
	av_free(audio_outbuf);
}

