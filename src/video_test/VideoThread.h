#ifndef VideoThread_h
#define VideoThread_h


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

#include <QImage>
#include <QTimer>
#include <QTime>
#include <QMovie>

#include <QAbstractVideoSurface>
#include <QHash>
#include <QVideoSurfaceFormat> 

class VideoThread: public QThread
{
	Q_OBJECT

public:
	VideoThread(QObject *parent=0);
	~VideoThread();

	void setVideo(const QString&);

	typedef enum Status {Running, NotRunning, Paused};
	Status status() { return m_status; }
	
	void setSurface(QAbstractVideoSurface *);
	QAbstractVideoSurface * surface() { return m_surface; }
	
	void start(bool asThread = false);
	
signals:
	void newImage(QImage);
	void movieStateChanged(QMovie::MovieState);

public slots:
	void seek(int ms, int flags);
	void restart();
	void play();
	void pause();
	void stop();
	void setStatus(Status);

protected slots:
	void readFrame();
	void releaseCurrentFrame();
	
protected:
	void run();

	void freeResources();
	int initVideo();
	
	void initSurface();


	void calculateVideoProperties();
	
	
private:
	QTimer *m_readTimer;

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

	//QVideoBuffer * m_video_buffer;
	//QVideoBuffer * m_audio_buffer;

	QImage m_frame;

// 	SDL_AudioSpec wanted_spec;
// 	SDL_AudioSpec spec;

	//QVideo * m_video;
	//QFFMpegVideoFrame m_current_frame;

	AVRational m_timebase;

	quint64 m_start_timestamp;
	bool m_initial_decode;

	double m_previous_pts;

	int m_decode_timer;

	bool m_killed;

	double m_video_clock; ///<pts of last decoded frame / predicted pts of next decoded frame

	bool m_inited;

	QString m_videoFile;
	
	double m_previous_pts_delay;
	QSize m_frame_size;
	double m_duration;
	double m_fpms;
	double m_frame_rate;

	QTime m_run_time;
	int m_total_runtime;
	
	Status m_status;
	
	double m_frameTimer;
	int m_skipFrameCount;
	
	QHash<PixelFormat,QVideoFrame::PixelFormat> m_formatMap;
	QAbstractVideoSurface * m_surface;
	QVideoSurfaceFormat m_format;
	bool m_needCpuConversion;
	int m_raw_num_bytes;
	int m_conv_num_bytes;
	QVideoFrame m_videoFrame;
};


#endif //VideoThread_h

