#include "GLVideoLoopDrawable.h"


#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include "../livemix/VideoThread.h"

GLVideoLoopDrawable::GLVideoLoopDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
	, m_videoLength(-1)
{
	if(!file.isEmpty())
		setVideoFile(file);
	
	connect(this, SIGNAL(sourceDiscarded(VideoSource*)), this, SLOT(deleteSource(VideoSource*)));
	
	//QTimer::singleShot(1500, this, SLOT(testXfade()));
}
	
void GLVideoLoopDrawable::testXfade()
{
	qDebug() << "GLVideoLoopDrawable::testXfade(): loading file #2";
	setVideoFile("/data/appcluster/dviz-old/dviz-r62-b2/src/data/Seasons_Loop_2_SD.mpg");
}
	
bool GLVideoLoopDrawable::setVideoFile(const QString& file)
{
	qDebug() << "GLVideoLoopDrawable::setVideoFile(): file:"<<file;
	
	QFileInfo fileInfo(file);
	if(!fileInfo.exists())
	{
		qDebug() << "GLVideoLoopDrawable::setVideoFile: "<<file<<" does not exist!";
		return false;
	}
	
	if(m_videoFile == file)
		return true;
	
	m_videoFile = file;
	
	VideoThread * source = new VideoThread();
	source->setVideo(file);
	
	// Assuming duration in seconds
	m_videoLength = source->duration(); // / 1000.;
		
	//source->setVideo("../samples/BlueFish/EssentialsVol05_Abstract_Media/HD/Countdowns/Abstract_Countdown_3_HD.mp4");
	//source->setVideo("../samples/BlueFish/EssentialsVol05_Abstract_Media/SD/Countdowns/Abstract_Countdown_3_SD.mpg");
	
	source->start();
	
	setVideoSource(source);
	setObjectName(qPrintable(file));
	
	emit videoFileChanged(file);
	
	//qDebug() << "GLVideoLoopDrawable::setVideoFile: Created video thread:"<<source;
	
	return true;
	
}

void GLVideoLoopDrawable::deleteSource(VideoSource *source)
{
	VideoThread *vt = dynamic_cast<VideoThread*>(source);
	if(vt)
	{
		qDebug() << "GLVideoLoopDrawable::deleteSource: Deleting video thread:" <<vt;
		delete vt;
		vt = 0;
		source = 0;
	}
	else
	{
		qDebug() << "GLVideoLoopDrawable::deleteSource: Source not deleted because its not a 'VideoThread':" <<source;
	}
}
