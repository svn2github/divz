#ifndef VideoEncoderThread_H
#define VideoEncoderThread_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

#undef exit

#define STREAM_PIX_FMT PIX_FMT_YUV420P /* default pix_fmt */

class VideoEncoderData
{
public:
	AVFrame *picture, *tmp_picture;
	uint8_t *video_outbuf;
	int frame_count, video_outbuf_size;
};

class AudioEncoderData
{
public:

	float t, 
		tincr, 
		tincr2;
	int16_t *samples;
	uint8_t *audio_outbuf;
	int audio_outbuf_size;
	int audio_input_frame_size;
	
};

class VideoEncoderThread
{
public:
	VideoEncoderThread(char *file);
	
	void setupEncoder(double duration = 10.0, int frameRate = 25);
	
	void run();

protected:
	AVStream *addAudioStream(AVFormatContext *oc, enum CodecID codec_id);
	void openAudio(AVFormatContext *oc, AVStream *st);
	void getDummyAudioFrame(int16_t *samples, int frame_size, int nb_channels);
	void writeAudioFrame(AVFormatContext *oc, AVStream *st);
	void closeAudio(AVFormatContext *oc, AVStream *st);
	
	AVStream *addVideoStream(AVFormatContext *oc, enum CodecID codec_id);
	AVFrame *allocPicture(enum PixelFormat pix_fmt, int width, int height);
	void openVideo(AVFormatContext *oc, AVStream *st);
	void fillDummyYuvImage(AVFrame *pict, int frame_index, int width, int height);
	void writeVideoFrame(AVFormatContext *oc, AVStream *st);
	void closeVideo(AVFormatContext *oc, AVStream *st);
	
private:
	AudioEncoderData m_audioData;
	VideoEncoderData m_videoData;
	
	AVOutputFormat  *m_fmt;
	AVFormatContext *m_outputContext;
	AVStream *m_audioStream,
		 *m_videoStream;
	double  m_audioPts,
		m_videoPts;
		
	char *m_filename;
	
	double m_duration;
	int m_frameRate;
	int m_numFrames;
	
	SwsContext *m_imgConvertCtx;
};

#endif 
