#ifndef CameraThread_h
#define CameraThread_h


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

class CameraThread: public QThread
{
	Q_OBJECT

public:
	CameraThread(QObject *parent=0);
	~CameraThread();

	void setCamera(const QString&);
	
	static QStringList enumerateDevices();

signals:
	void newImage(QImage);

protected slots:
	void readFrame();

protected:
	void run();

	void freeResources();
	int initCamera();

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

	QImage * m_frame;

// 	SDL_AudioSpec wanted_spec;
// 	SDL_AudioSpec spec;

	//QVideo * m_video;
	//QFFMpegVideoFrame m_current_frame;

	AVRational m_timebase;

	quint64 m_start_timestamp;
	bool m_initial_decode;

	double m_fpms;

	double m_previous_pts;

	int m_decode_timer;

	bool m_killed;

	double m_video_clock; ///<pts of last decoded frame / predicted pts of next decoded frame

	bool m_inited;

	QString m_cameraFile;

};


#endif //CameraThread_h

