#ifndef VideoSource_H
#define VideoSource_H

#include <QThread>
#include <QImage>
#include <QQueue>

#include "VideoFrame.h"

class VideoWidget;
class VideoSource : public QThread
{
	Q_OBJECT

public:
	VideoSource(QObject *parent=0);
	virtual ~VideoSource();

	
	int refCount() { return m_refCount; }
	virtual void registerConsumer(VideoWidget *consumer);
	virtual void release(VideoWidget *consumer=0);

	virtual VideoFrame frame();
	
	bool isBuffered() { return m_isBuffered; }
	void setIsBuffered(bool);
	
signals:
	void frameReady();

protected:
	virtual void run();
	virtual void enqueue(VideoFrame);
	virtual void destroySource();
	
	bool m_killed;
	
	static void initAV();
	static bool isLibAVInit;

	QQueue<VideoFrame> m_frameQueue;
	QList<VideoWidget*> m_consumerList;
	int m_refCount;
	bool m_isBuffered;
	VideoFrame m_singleFrame;
};



#endif
