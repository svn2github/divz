#ifndef QVIDEOTEST_H
#define QVIDEOTEST_H

#include <QtGui>
#include <QtCore>
#include "QVideo.h"
#include "QVideoBuffer.h"


class QVideo;

class QVideoTest : public QWidget
{
	Q_OBJECT

public:
	QVideoTest(QString file,QWidget * parent = 0);
	~QVideoTest();

private slots:
	void newFrame(QFFMpegVideoFrame);
	//void newVideoImage(QImage);
	void jump();

private:
	QVideo * m_video;
	QLabel * m_screen;
	QTime m_elapsed_time;
};

#endif // QVIDEOTEST_H
