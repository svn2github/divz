#ifndef QVIDEODECODER_H
#define QVIDEODECODER_H

#include <QThread>

// Defn from http://gcc.gnu.org/ml/gcc-bugs/2002-10/msg00259.html
#ifndef INT64_C
# define INT64_C(c) c ## LL
#endif

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

#include "QVideo.h"
#include "QVideoBuffer.h"

// #include "SDL.h"
// #include "SDL_thread.h"

class QVideoDecoder : public QThread
{
	Q_OBJECT

public:
	QVideoDecoder(QVideo * video, QObject * parent = 0);
	~QVideoDecoder();

	bool load(const QString & filename);
	void unload();

	void run();
	void startDecoding();
	void test();

	void seek(int ms, int flags = 0);
	void restart();

	QFFMpegVideoFrame seekToFrame(int ms);

signals:
	void newFrame(QFFMpegVideoFrame frame);
	void done();

	void decodeMe();

	void reachedEnd();

	void ready(bool state);

public slots:
	void read();
	void decode();

	void readFrame();
	void decodeVideoFrame();

	QFFMpegVideoFrame getNextFrame();
	void addFrame(QFFMpegVideoFrame frame);
	void setCurrentFrame(QFFMpegVideoFrame frame);

	void flushBuffers();

protected:
	void timerEvent(QTimerEvent * te);

private:
	void calculateVideoProperties();
	quint64 calculatePTS(quint64 dts);
	void freeResources();

private:
	//ffmpeg variables
	AVFormatContext * m_av_format_context;
	AVCodecContext * m_video_codec_context;
	AVCodecContext * m_audio_codec_context;
	AVCodec * m_video_codec;
	AVCodec * m_audio_codec;
	AVFrame * m_av_frame;
	AVFrame * m_av_rgb_frame;
	SwsContext * m_sws_context;
	AVRational m_time_base_rational;
	uint8_t * m_buffer;

	int m_video_stream;
	int m_audio_stream;

	QVideoBuffer * m_video_buffer;
	QVideoBuffer * m_audio_buffer;
	
	QImage * m_frame;

// 	SDL_AudioSpec wanted_spec;
// 	SDL_AudioSpec spec;

	QVideo * m_video;
	QFFMpegVideoFrame m_current_frame;

	AVRational m_timebase;

	quint64 m_start_timestamp;
	bool m_initial_decode;

	double m_fpms;

	double m_previous_pts;

	int m_decode_timer;
	
	bool m_killed;
	
	double m_video_clock; ///<pts of last decoded frame / predicted pts of next decoded frame

};

#endif // QVIDEODECODER_H
