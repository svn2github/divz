#ifndef _QVIDEO_H
#define _QVIDEO_H

#include <QtCore>
#include <QtGui>
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

#include "QVideoBuffer.h"
#include <QtGui/QMovie>

// #include <windows.h>

class QVideoDecoder;
class QVideoEncoder;

/////////////////////////////////////////////
//Qt class using the ffmpeg API and the 
//libavcodec for video playback. Audio is
//not supported at present. MSVC++ compiled
//libraries can be found at this location:
//http://arrozcru.no-ip.org/ffmpeg_builds/

//Author: Daniel Stepp - July 18, 2008
/////////////////////////////////////////////

class QVideo : public QObject
{
	Q_OBJECT

public:
	QVideo(const QString & filename, QObject * parent = 0);
	QVideo(QObject * parent = 0);
	~QVideo();

	bool load(const QString & filename);
	void unload();

	enum AdvanceMode { RealTime, Manual };

	//------------------------------------------------------------------------------------
	//AdvanceMode: determines how client interfaces with class

	//RealTime is the default mode, its interface being play(), seek(int ms), pause(), stop()
	//Manual mode is for clients needing to advance frame by frame according to 
	//outside time-steps. Manual mode's interface is advance(int ms) and restart(). Note:
	//Interfaces are not interchangeable, as RealTime depends on buffers and Manual does
	//not, so if you need to switch, be sure to call setAdvanceMode() first.
	//------------------------------------------------------------------------------------

	int advanceMode()							{ return m_advance_mode; }
	void setAdvanceMode(AdvanceMode mode);

	//in RealTime mode, this label will be refreshed with the current video frame
	QLabel * screen()							{ return m_screen; }

	QString fileName()							{ return m_filename; }
	QSize frameSize()							{ return m_frame_size; }
	int duration()								{ return m_duration * 1000; } //in milliseconds
	int fps()								{ return m_frame_rate; }
	bool loaded()								{ return m_video_loaded; }
	
	bool readyToPlay()							{ return m_ready_to_play; }
	void setLooped(bool looped)						{ m_looped = looped; }

	quint64 bufferSize()							{ return m_max_buffer_size; }
	void setBufferSize(quint64 size_in_bytes)				{ m_max_buffer_size = size_in_bytes; }

	void setOutputSource(uchar * source_bits);
	void setOutputDuration(int seconds);
	void setOutputFPS(int fps);
	void setOutputFile(const QString & output_file);
	void setOutputSize(QSize size);
	
	typedef enum Status {Running, NotRunning, Paused};
	Status status() { return m_status; }
	void setStatus(Status);
	

public slots:
	void play();
	void seek(int ms);
	void pause();
	void stop();

	QImage advance(int ms);
	void restart();

	void setReady(bool ready);

	void determineVideoEndAction();

	void displayFrame();

	void makeMovie();

signals:
	void startDecode();
	void newPixmap(const QPixmap &);
	void movieStateChanged(QMovie::MovieState state);

protected:
	void timerEvent(QTimerEvent * te);

//         static void TimerProc(void* lpParametar, BOOLEAN TimerOrWaitFired);

private:
	friend class QVideoDecoder;
	QVideoDecoder * m_video_decoder;

	friend class QVideoEncoder;
	QVideoEncoder * m_video_encoder;

	QString m_filename;

	QSize m_frame_size;

	bool m_video_loaded;
	bool m_looped;
	bool m_ready_to_play;

	int m_frame_rate;
	int m_duration;

	quint64 m_max_buffer_size;

	AdvanceMode m_advance_mode;

	QLabel * m_screen;

	int m_play_timer;

	QTime m_play_time;

	QFFMpegVideoFrame m_current_frame;

        int m_frame_counter;
        int m_last_frame_shown;

        void consumeFrame();

        QTimer m_nextImageTimer;
        QTime m_frameDebug;
        int m_expectedDelay;
        
        double m_frameTimer;
        
        // used to reset the elapsed time and frame timer back to in sync
        int m_skipFrameCount;
        
        Status m_status;
};

#endif // _QVIDEO_H
