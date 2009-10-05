#include "QVideo.h"

#include "QVideoBuffer.h"
#include "QVideoDecoder.h"

#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
//#include "QVideoEncoder.h"
/*
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <Winbase.h>*/
QMutex qvideo_mutex;

QVideo::QVideo(const QString & filename, QObject * parent) : QObject(parent), 
	m_frame_rate(0.0f), 
	m_looped(true),
	m_max_buffer_size(25000000),
	m_ready_to_play(false),
	m_advance_mode(QVideo::RealTime),
	m_play_timer(0),
	m_video_loaded(false)
{
	QMutexLocker locker(&qvideo_mutex);
	av_register_all();
	/*
	m_video_decoder = new QVideoDecoder(this, this);
	m_video_decoder->start();
	connect(this, SIGNAL(startDecode()), m_video_decoder, SLOT(decode()));
	connect(m_video_decoder, SIGNAL(reachedEnd()), this, SLOT(determineVideoEndAction()));
	connect(m_video_decoder, SIGNAL(ready(bool)), this, SLOT(setReady(bool)));
	*/
	// just a random default value
	m_expectedDelay = 999;
	m_last_frame_shown = 999;

	setAdvanceMode(QVideo::RealTime);

	m_screen = new QLabel();

	if(QFile::exists(filename))
		load(filename);
}

QVideo::QVideo(QObject *parent) : QObject(parent), 
	m_frame_rate(0.0f), 
	m_looped(true),
	m_max_buffer_size(25000000),
	m_ready_to_play(false),
	m_advance_mode(QVideo::RealTime),
	m_play_timer(0),
	m_video_loaded(false)
{
	QMutexLocker locker(&qvideo_mutex);
	av_register_all();

	//m_video_encoder = new QVideoEncoder(this, this);
	
	m_video_decoder = new QVideoDecoder(this, this);
	m_video_decoder->start();
	//connect(this, SIGNAL(startDecode()), m_video_decoder, SLOT(decode()));
	connect(m_video_decoder, SIGNAL(reachedEnd()), this, SLOT(determineVideoEndAction()));
        connect(m_video_decoder, SIGNAL(ready(bool)), this, SLOT(setReady(bool)));
        

        //connect(&m_nextImageTimer, SIGNAL(timeout()), this, SLOT(displayFrame()));
        //m_nextImageTimer.setSingleShot(true);


	setAdvanceMode(QVideo::RealTime);

// 	m_screen = new QLabel();

        // just a random default value
	m_expectedDelay = 999;
	m_last_frame_shown = 999;
        m_frame_counter = 0;
}

QVideo::~QVideo()
{
/*	delete m_screen;
	m_screen = 0;*/
	
	unload();
}


bool QVideo::load(const QString & filename)
{
	if(!m_video_decoder)
		return true;
	//qDebug() << "QVideo::load:"<<filename<<", entering lock...";
	QMutexLocker locker(&qvideo_mutex);
	//qDebug() << "QVideo::load:"<<filename<<", got lock.";
	
	bool flag = m_video_decoder->load(filename);
	
	//qDebug() << "QVideo::load:"<<filename<<", done loading, flag:"<<flag;
	return flag;
}

void QVideo::unload()
{
	if(m_video_decoder)
		m_video_decoder->unload();
}

void QVideo::setAdvanceMode(AdvanceMode mode)
{
	//switch(mode)
	//{
	//	case QVideo::RealTime:
	//		m_video_buffer_one = new QVideoBuffer(this);
	//		m_video_buffer_two = new QVideoBuffer(this);
	//		connect(m_video_decoder, SIGNAL(newFrame(QFFMpegVideoFrame)), this, SLOT(addFrame(QFFMpegVideoFrame)));
	//		connect(this, SIGNAL(startDecode()), m_video_decoder, SLOT(decode()));
	//		break;
	//	case QVideo::Manual:
	//		if(m_video_buffer_one)
	//		{
	//			delete m_video_buffer_one;
	//			m_video_buffer_one = 0;
	//		}
	//		if(m_video_buffer_two)
	//		{
	//			delete m_video_buffer_two;
	//			m_video_buffer_two = 0;
	//		}
	//		disconnect(m_video_decoder, SIGNAL(newFrame(QFFMpegVideoFrame)), this, SLOT(addFrame(QFFMpegVideoFrame)));
	//		disconnect(this, SIGNAL(startDecode()), m_video_decoder, SLOT(decode()));
	//		break;
	//	default:
	//		break;
	//}

	m_advance_mode = mode;
}

void QVideo::setOutputSource(uchar * source_bits)
{

}

void QVideo::setOutputDuration(int seconds)
{
	//m_video_encoder->setDuration(seconds);
}

void QVideo::setOutputFPS(int fps)
{
	//m_video_encoder->setFrameRate(fps);
}

void QVideo::setOutputFile(const QString & output_file)
{
	//m_video_encoder->setOutputFile(output_file);
}

void QVideo::setOutputSize(QSize size)
{
	//m_video_encoder->setOutputSize(size);
}

void QVideo::play()
{
	m_status = Running;
	if(!m_play_timer)
	{
		m_play_time.start();
		m_play_timer = startTimer(1);
	}
	emit movieStateChanged(QMovie::Running);
}

void QVideo::seek(int ms)
{
	if(m_video_decoder)
		m_video_decoder->seek(ms);
}

void QVideo::pause()
{
	m_status = Paused;
	//emit startDecode();
	killTimer(m_play_timer);
	m_play_timer = 0;
	emit movieStateChanged(QMovie::Paused);
}

void QVideo::stop()
{
	m_status = NotRunning;
	killTimer(m_play_timer);
	m_play_timer = 0;

// 	m_screen->clear();

	if(m_video_decoder)
	{
		m_video_decoder->restart();
		m_video_decoder->flushBuffers();
	}
	emit movieStateChanged(QMovie::NotRunning);
	
}

void QVideo::setStatus(Status s)
{
	m_status = s;
	if(s == NotRunning)
		stop();
	else
	if(s == Paused)
		pause();
	else
	if(s == Running)
		play();
}

QImage QVideo::advance(int ms)
{
	if(!m_video_decoder)
		return QImage();
	QFFMpegVideoFrame video_frame = m_video_decoder->seekToFrame(ms);
// 	QImage frame = *video_frame.frame;
// 
// 	delete video_frame.frame;
// 	video_frame.frame = 0;

	return QImage(); //frame;
}

void QVideo::restart()
{
	if(m_video_decoder)
		m_video_decoder->restart();
}

void QVideo::setReady(bool ready)
{
	m_ready_to_play = ready;
}

void QVideo::determineVideoEndAction()
{
	if(m_looped)
	{
		if(m_video_decoder)
			m_video_decoder->restart();
	}
	else
	{
		stop();
	}
}


/*void CALLBACK QVideo::TimerProc(void* lpParametar,
                                    BOOLEAN TimerOrWaitFired)
{
    // This is used only to call QueueTimerHandler
    // Typically, this function is static member of CTimersDlg
    QVideo* obj = (QVideo*) lpParametar;
    obj->displayFrame();
}
*/
void QVideo::timerEvent(QTimerEvent * te)
{
	if(te->timerId() == m_play_timer)
	{
                consumeFrame();
	}
}

void QVideo::consumeFrame()
{
	if(!m_video_decoder)
		return;
		
	if(m_ready_to_play)
	{
		//stop timer until displayFrame() has completed
		killTimer(m_play_timer);

		m_current_frame = m_video_decoder->getNextFrame();

		static double previous_pts_delay = 0.04;

		//calculate the pts delay
		double pts_delay = m_current_frame.pts - m_current_frame.previous_pts;
		if((pts_delay < 0.0) || (pts_delay > 1.0))
			pts_delay = previous_pts_delay;

		previous_pts_delay = pts_delay;

		//calculate the global delay
		int global_delay = (m_current_frame.pts * 1000) - m_play_time.elapsed();

		//convert to milliseconds for comparison
		pts_delay *= 1000;

		//we want the lesser of the two delays, so as not to fall behind, that will be our actual_delay
		int actual_delay = (global_delay < pts_delay && global_delay > 0) ? global_delay : pts_delay;
		//actual_delay = 15;
		//qDebug("Sleeping %d till next frame...",actual_delay);
		int min = 10;
		#if !defined(Q_OS_UNIX)
			min=33 * 2;
		#endif
		
		if(actual_delay<min)
		{
			actual_delay = min;
		//qDebug("***************** Keeping out of the water: %d",min);
		}


		//use the actual_delay to schedule a refresh to display the current frame

		// trying this from : http://lists.trolltech.com/qt-interest/2000-04/thread00530-0.html
		#ifndef UNIX
		//qt_win_use_simple_timers = false;
		#endif
		
		//printf("actual_delay=%d\n",actual_delay);
		

		m_frame_counter++;
		//qDebug("START frame: %d",m_frame_counter);
		m_frameDebug.start();
		m_expectedDelay = actual_delay;
		QTimer::singleShot(actual_delay, this, SLOT(displayFrame()));
		//m_nextImageTimer.start(actual_delay);
		//displayFrame();
		//HANDLE handle;
/*
				BOOL CreateTimerQueueTimer(PHANDLE phNewTimer, HANDLE TimerQueue ,
	WAITORTIMERCALLBACK Callback, PVOID Parameter, DWORD DueTime,
	DWORD Period, ULONG Flags);
*/
		/*
		BOOL success = ::CreateTimerQueueTimer(
		&handle,
		NULL,
		TimerProc,
		this,
		0,
		actual_delay,
		WT_EXECUTEINTIMERTHREAD | WT_EXECUTEONLYONCE
		);
		*/


		#ifndef UNIX
		//qt_win_use_simple_timers = true;
		#endif

	}
}


void QVideo::displayFrame()
{
	if(status() != Running)
	{
		qDebug("Not running, not showing frame");
		return;
	}
        //qDebug("Done sleeping, showing next frame");

        //qDebug("    END frame: %d",m_frame_counter);
        int xflag = 0;
        if(m_frameDebug.elapsed() > m_expectedDelay * 3)
        {
            qDebug(" * * * elapsed: %d, expected: %d", m_frameDebug.elapsed(), m_expectedDelay);
            xflag ++;
        }

//         if(m_frame_counter - m_last_frame_shown > 1)
//         {
//             qDebug("*************** DROPED %d",m_frame_counter - m_last_frame_shown);
//         }

        m_last_frame_shown = m_frame_counter;
        QImage img = *m_current_frame.frame;
	emit newPixmap(QPixmap::fromImage(img));

 	//delete m_current_frame.frame;
 	//m_current_frame.frame = 0;

	//resume playing
        m_play_timer = startTimer(1);
        //consumeFrame();
}

void QVideo::makeMovie()
{
	//m_video_encoder->start();
}
