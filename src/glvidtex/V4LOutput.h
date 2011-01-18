#ifndef V4LOutput_H
#define V4LOutput_H

#include <QtGui>

#include "../livemix/VideoFrame.h"
#include "VideoConsumer.h"

class V4LOutput : public QObject, 
		  public VideoConsumer
{
	Q_OBJECT
public:
	V4LOutput(QString output = "/dev/video2", QObject *parent=0);
	~V4LOutput();
	
	void setVideoSource(VideoSource *source);

public slots:
	void disconnectVideoSource();

private slots:
	void frameReady();
	void processFrame();
	
private:
	void setupOutput();
	int startPipe(int dev, int width, int height);
	
	int m_v4lOutputDev;
	QString m_v4lOutputName;
	
	QSize m_outputSize;
	
	VideoFramePtr m_frame;
};


#endif
