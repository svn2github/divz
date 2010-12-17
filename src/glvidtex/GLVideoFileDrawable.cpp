#include "GLVideoFileDrawable.h"

#ifdef HAS_QT_VIDEO_SOURCE
#include "QtVideoSource.h"
#endif

GLVideoFileDrawable::GLVideoFileDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
	, m_videoLength(-1)
{
	if(!file.isEmpty())
		setVideoFile(file);
	
	//QTimer::singleShot(1500, this, SLOT(testXfade()));
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
		
		QtVideoSource *source = new QtVideoSource();
		source->setFile(file);
		source->start();
		
		// Duration is in milleseconds, we store length in seconds
		m_videoLength = source->player()->duration() / 1000.;
		qDebug() << "GLVideoFileDrawable::setvideoFile: "<<file<<": Duration: "<<m_videoLength;
		
		setVideoSource(source);
		
	#else
	
		qDebug() << "GLVideoFileDrawable::setVideoFile: "<<file<<": GLVidTex Graphics Engine not compiled with QtMobility support, therefore, unable to play back video files with sound. Use GLVideoLoopDrawable to play videos as loops without QtMobility.";
	
	#endif
	
	return true;
	
}
