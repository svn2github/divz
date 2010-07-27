#ifndef GLThread_H
#define GLThread_H
 
#include <QThread>
#include <QSize>
#include <QtGui>
#include <QtOpenGL>
#include <QMutex>
#include <QMutexLocker>
#include <QTime>

#include "../livemix/VideoFrame.h"
class GLWidget;
class VideoThread;
class CameraThread;

class GLThread : public QThread
{
	Q_OBJECT
public:
	GLThread(GLWidget *glWidget);
	void resizeViewport(const QSize &size);
	void run();
	void stop();

private slots:
	void frameReady();

private:
	bool doRendering;
	bool doResize;
	int w;
	int h;
	int rotAngle;
	GLWidget *glw;
	
	GLuint	texture[1]; // Storage For One Texture
	
	float xrot, yrot, zrot;
	float xscale, yscale, zscale;
	float xscaleInc, yscaleInc, zscaleInc;
	
	CameraThread *videoSource;
	QMutex videoMutex;
	QMutex resizeMutex;
	
	VideoFrame frame;
	int lastFrameTime;
	bool newFrame;
	QTime time;
	
// 	GLfloat	xrot;				// X Rotation ( NEW )
// 	GLfloat	yrot;				// Y Rotation ( NEW )
// 	GLfloat	zrot;				// Z Rotation ( NEW )

};

#endif
