#include "GLVideoFileDrawable.h"

#ifdef HAS_QT_VIDEO_SOURCE
#include "QtVideoSource.h"
#endif

#include <QPropertyAnimation>
#include "GLEditorGraphicsScene.h"

GLVideoFileDrawable::GLVideoFileDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
	, m_videoLength(-1)
#ifdef HAS_QT_VIDEO_SOURCE
	, m_qtSource(0)
#endif
{
	if(!file.isEmpty())
		setVideoFile(file);
	
	playlist()->setDurationProperty("videoLength");
	
	connect(this, SIGNAL(sourceDiscarded(VideoSource*)), this, SLOT(deleteSource(VideoSource*)));
}
	
void GLVideoFileDrawable::testXfade()
{
	qDebug() << "GLVideoFileDrawable::testXfade(): loading file #2";
	setVideoFile("/data/appcluster/dviz-old/dviz-r62-b2/src/data/Seasons_Loop_2_SD.mpg");
}
	
bool GLVideoFileDrawable::setVideoFile(const QString& file)
{
	qDebug() << "GLVideoFileDrawable::setVideoFile(): "<<(QObject*)this<<" file:"<<file;
	
	QFileInfo fileInfo(file);
	if(!fileInfo.exists())
	{
		qDebug() << "GLVideoFileDrawable::setVideoFile: "<<(QObject*)this<<" "<<file<<" does not exist!";
		return false;
	}
	
	m_videoFile = file;
	
#ifdef HAS_QT_VIDEO_SOURCE
	if(m_qtSource)
	{
		QPropertyAnimation *anim = new QPropertyAnimation(this, "volume");
		anim->setEndValue(0);
		anim->setDuration(xfadeLength());
		anim->start();
		
		disconnect(m_qtSource->player(), 0, this, 0);
	}
	
	m_qtSource = new QtVideoSource();
	m_qtSource->setFile(file);
	m_qtSource->start();
	
	connect(m_qtSource->player(), SIGNAL(positionChanged(qint64)), this, SIGNAL(positionChanged(qint64))); 
	
	connect(m_qtSource->player(), SIGNAL(durationChanged ( qint64 )), this, SLOT(durationChanged ( qint64 )));
	connect(m_qtSource->player(), SIGNAL(error ( QMediaPlayer::Error )), this, SLOT(error ( QMediaPlayer::Error )));
	connect(m_qtSource->player(), SIGNAL(stateChanged ( QMediaPlayer::State )), this, SLOT(stateChanged ( QMediaPlayer::State )));
	
	// Reset length for next query to videoLength(), below 
	m_videoLength = -1;
	
	setVideoSource(m_qtSource);
	
	// Volume will be raised in setLiveStatus() below
	m_qtSource->player()->setVolume(0);
	
	if(liveStatus())
		setLiveStatus(true);

#else

	qDebug() << "GLVideoFileDrawable::setVideoFile: "<<file<<": GLVidTex Graphics Engine not compiled with QtMobility support, therefore, unable to play back video files with sound. Use GLVideoLoopDrawable to play videos as loops without QtMobility.";

#endif
	
	emit videoFileChanged(file);
	return true;
	
}

#ifdef HAS_QT_VIDEO_SOURCE
void GLVideoFileDrawable::durationChanged ( qint64 duration )
{
	duration /= 1000;
	qDebug() << "GLVideoFileDrawable::durationChanged: "<<m_videoFile<<" Duration changed to:"<<duration;
	m_videoLength = (double)duration;
}

void GLVideoFileDrawable::error ( QMediaPlayer::Error error )
{
	qDebug() << "GLVideoFileDrawable::error: "<<m_videoFile<<" "<<error;
}

void GLVideoFileDrawable::stateChanged ( QMediaPlayer::State state )
{
	qDebug() << "GLVideoFileDrawable::stateChanged: "<<m_videoFile<<" "<<state;
	
	if(state == QMediaPlayer::StoppedState)
	{
		//if(playlist()->isPlaying())
			playlist()->nextItem();
		//else
		//	qDebug() << "GLVideoFileDrawable::stateChanged: "<<m_videoFile<<" Stopped, but no playlist going to move on in.";
	}
}
#endif

double GLVideoFileDrawable::videoLength()
{
#ifdef HAS_QT_VIDEO_SOURCE
	if(m_videoLength < 0)
	{
		if(!m_qtSource)
		{
			qDebug() << "GLVideoFileDrawable::videoLength: No source set, unable to find length.";
		}
		else
		{ 
			// Duration is in milleseconds, we store length in seconds
			m_videoLength = m_qtSource->player()->duration() / 1000.;
			qDebug() << "GLVideoFileDrawable::videoLength: "<<m_qtSource->file()<<": Duration: "<<m_videoLength;
		}
	}
#endif	
	return m_videoLength;
}	
		

void GLVideoFileDrawable::deleteSource(VideoSource *source)
{
#ifdef HAS_QT_VIDEO_SOURCE
	QtVideoSource *vt = dynamic_cast<QtVideoSource*>(source);
	if(vt)
	{
		qDebug() << "GLVideoFileDrawable::deleteSource: Deleting video thread:" <<vt;
		delete vt;
		vt = 0;
		source = 0;
	}
	else
	{
		qDebug() << "GLVideoFileDrawable::deleteSource: Source not deleted because its not a 'QtVideoSource':" <<source;
	}
#endif
}

void GLVideoFileDrawable::setVolume(int v)
{ 
#ifdef HAS_QT_VIDEO_SOURCE
	//qDebug() << "GLVideoFileDrawable::setVolume(): "<<(QObject*)this<<" source:"<<m_qtSource;
	if(m_qtSource)
	{
		//qDebug() << "GLVideoFileDrawable::setVolume(): "<<(QObject*)this<<" New volume:"<<v;
		m_qtSource->player()->setVolume(v);
	}
	else
	{
		//qDebug() << "GLVideoFileDrawable::setVolume(): "<<(QObject*)this<<" Cannot set volume, v:"<<v;
	}
#endif
}

int GLVideoFileDrawable::volume()
{ 
#ifdef HAS_QT_VIDEO_SOURCE
	if(m_qtSource)
		return m_qtSource->player()->volume();
	return 100;
#endif
}

void GLVideoFileDrawable::setMuted(bool flag)
{ 
#ifdef HAS_QT_VIDEO_SOURCE
	if(m_qtSource)
		m_qtSource->player()->setMuted(flag);
#endif
}

bool GLVideoFileDrawable::isMuted()
{
#ifdef HAS_QT_VIDEO_SOURCE
	if(m_qtSource)
		return m_qtSource->player()->isMuted();
	return false;
#endif

}

void GLVideoFileDrawable::setStatus(int status)
{ 
#ifdef HAS_QT_VIDEO_SOURCE
	if(m_qtSource)
	{
		switch(status)
		{
			case 0:
				m_qtSource->player()->stop();
				break;
			case 1:
				m_qtSource->player()->play();
				break;
			case 2:
				m_qtSource->player()->pause();
				break;
			default:
				break;
		}
	}
			
#endif
}

int GLVideoFileDrawable::status()
{
#ifdef HAS_QT_VIDEO_SOURCE
	if(m_qtSource)
		return (int)m_qtSource->player()->state();
	return 0;
#endif

}

void GLVideoFileDrawable::setPosition(qint64 pos)
{ 
#ifdef HAS_QT_VIDEO_SOURCE
	if(m_qtSource)
		m_qtSource->player()->setPosition(pos);
#endif
}

quint64 GLVideoFileDrawable::position()
{ 
#ifdef HAS_QT_VIDEO_SOURCE
	if(m_qtSource)
		return m_qtSource->player()->position();
	return 0;
#endif
}

void GLVideoFileDrawable::setLiveStatus(bool flag)
{
	GLVideoDrawable::setLiveStatus(flag);
#ifdef HAS_QT_VIDEO_SOURCE	

	qDebug() << "GLVideoFileDrawable::setLiveStatus: new status:"<<flag;
	
	if(flag)
	{
		GLEditorGraphicsScene *scenePtr = dynamic_cast<GLEditorGraphicsScene*>(scene());
		if(!scenePtr)
		{
			if(m_qtSource)
			{
				m_qtSource->player()->play();
				
				QPropertyAnimation *anim = new QPropertyAnimation(this, "volume");
				anim->setEndValue(100);
				anim->setDuration(xfadeLength());
				anim->start();
			}
			else
			{
				qDebug() << "GLVideoFileDrawable::setLiveStatus: Can't fade in volume - no video source.";
			}
		}
		else
		{
			qDebug() << "GLVideoFileDrawable::setLiveStatus: Not going live due to the fact we're in the editor scene. Also, limiting frame rate to 10fps for CPU usage's sake.";
			setFpsLimit(10.);
		}
	}
	else
	{
		if(m_qtSource)
		{
			QPropertyAnimation *anim = new QPropertyAnimation(this, "volume");
			anim->setEndValue(0);
			anim->setDuration(xfadeLength());
			anim->start();
			
			QTimer::singleShot(xfadeLength(), m_qtSource->player(), SLOT(pause())); 
		}
		else
		{
			qDebug() << "GLVideoFileDrawable::setLiveStatus: Can't fade out volume - no video source.";
		}
	}
#endif
}
