#include "GLVideoFileDrawable.h"

#ifdef HAS_QT_VIDEO_SOURCE
#include "QtVideoSource.h"
#endif

GLVideoFileDrawable::GLVideoFileDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
	, m_videoLength(-1)
	, m_qtSource(0)
{
	if(!file.isEmpty())
		setVideoFile(file);
	
	connect(this, SIGNAL(sourceDiscarded(VideoSource*)), this, SLOT(deleteSource(VideoSource*)));
}
	
void GLVideoFileDrawable::testXfade()
{
	qDebug() << "GLVideoFileDrawable::testXfade(): loading file #2";
	setVideoFile("/data/appcluster/dviz-old/dviz-r62-b2/src/data/Seasons_Loop_2_SD.mpg");
}
	
bool GLVideoFileDrawable::setVideoFile(const QString& file)
{
	qDebug() << "GLVideoFileDrawable::setVideoFile(): file:"<<file;
	
	QFileInfo fileInfo(file);
	if(!fileInfo.exists())
	{
		qDebug() << "GLVideoFileDrawable::setVideoFile: "<<file<<" does not exist!";
		return false;
	}
	
	m_videoFile = file;
	
	#ifdef HAS_QT_VIDEO_SOURCE
		
		m_qtSource = new QtVideoSource();
		m_qtSource->setFile(file);
		m_qtSource->start();
		
		// Reset length for next query to videoLength(), below 
		m_videoLength = -1;
		
		setVideoSource(m_qtSource);
		
	#else
	
		qDebug() << "GLVideoFileDrawable::setVideoFile: "<<file<<": GLVidTex Graphics Engine not compiled with QtMobility support, therefore, unable to play back video files with sound. Use GLVideoLoopDrawable to play videos as loops without QtMobility.";
	
	#endif
	
	emit videoFileChanged(file);
	
	return true;
	
}

double GLVideoFileDrawable::videoLength()
{
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

