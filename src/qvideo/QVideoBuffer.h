#ifndef QVIDEOBUFFER_H
#define QVIDEOBUFFER_H

#include <QtCore>
#include <QtGui>
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

class QVideo;

class QFFMpegVideoFrame
{
public:
	QFFMpegVideoFrame()
		: frame(0)
		, pts(0)
		, previous_pts(0)
			       {}
	QImage * frame;
	double pts;

	double previous_pts;
};

class QVideoBuffer : public QObject
{
	Q_OBJECT

public:
	QVideoBuffer(QObject * parent = 0);
	~QVideoBuffer();

	bool isBuffering()	{ return m_buffering; }
	bool isEmpty()		{ return m_buffer_queue.isEmpty(); }

	bool needsFrame()	{ return m_needs_frame; }

	int count()		{ return m_buffer_queue.count(); }

	void flush();

signals:
	void nowEmpty();

public slots:
	QFFMpegVideoFrame getNextFrame();
	void addFrame(QFFMpegVideoFrame frame);

private:
	QQueue<QFFMpegVideoFrame> m_buffer_queue;

	QMutex m_buffer_mutex;

	bool m_buffering;

	bool m_needs_frame;
};

#endif // QVIDEOBUFFER_H
