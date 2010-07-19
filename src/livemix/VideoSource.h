#ifndef VideoSource_H
#define VideoSource_H

#include <QThread>
#include <QImage>

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

	virtual QImage frame() = 0;
	
signals:
	void frameReady(int frameHoldTime);

protected:
	virtual void run() = 0;
	
	bool m_killed;

private:
	int m_refCount;
	QList<VideoWidget*> m_consumerList;
};



#endif