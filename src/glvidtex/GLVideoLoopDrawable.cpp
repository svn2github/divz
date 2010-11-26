#include "GLVideoLoopDrawable.h"

#include "../livemix/VideoThread.h"

GLVideoLoopDrawable::GLVideoLoopDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
{
	if(!file.isEmpty())
		setVideoFile(file);
	
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
	
	m_videoFile = file;
	
	VideoThread * source = new VideoThread();
	source->setVideo(file);
	//source->setVideo("../samples/BlueFish/EssentialsVol05_Abstract_Media/HD/Countdowns/Abstract_Countdown_3_HD.mp4");
	//source->setVideo("../samples/BlueFish/EssentialsVol05_Abstract_Media/SD/Countdowns/Abstract_Countdown_3_SD.mpg");
	
	source->start();
	
	setVideoSource(source);
	
	return true;
	
}
