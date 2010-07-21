#include <QtGui/QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextCursor>

//#include "CameraTest.h"
#include "VideoWidget.h"
#include "CameraThread.h"
#include "VideoThread.h"

extern "C" {
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}

int main(int argc, char **argv)
{
	//QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
	QApplication app(argc, argv);

//  	CameraTest t;
//  	t.resize(320,240);
//  	t.show();

	avcodec_init();
	avcodec_register_all();
	avdevice_register_all();
	av_register_all();
	
	#ifdef Q_OS_WIN
	QString defaultCamera = "vfwcap://0";
	#else
	QString defaultCamera = "/dev/video0";
	#endif
	
	
	CameraThread *cameraSource = CameraThread::threadForCamera(defaultCamera);
	if(cameraSource)
		cameraSource->setFps(30);
	
	VideoWidget viewer1;
	viewer1.setVideoSource(cameraSource);
	viewer1.setSourceRectAdjust(11,0,-6,-3);
	viewer1.setWindowTitle("Camera");
	viewer1.resize(320,240);
	viewer1.setOverlayText("Camera 01");
	viewer1.show();
	
	VideoThread *videoSource = new VideoThread();
	videoSource->setVideo("../data/Seasons_Loop_3_SD.mpg");
	videoSource->start();
	
	VideoWidget viewer2;
	viewer2.setVideoSource(videoSource);
	viewer2.setWindowTitle("Video");
	viewer2.resize(320,240);
	viewer2.setOverlayText("Welcome to PCI");
	viewer2.show();
	
	
	/*
	CameraViewerWidget t2;
	t2.setObjectName("v2");
	t2.setWindowTitle("v2");
	#ifdef Q_OS_WIN
	t2.setCamera("vfwcap://0",10);
	#else
	t2.setCamera("/dev/video0",10);
	#endif
	t2.resize(320,240);
	t2.show();*/
	
	
// 	
// 	CameraViewerWidget t3;
// 	t3.setObjectName("v3");
// 	t3.setWindowTitle("v3");
// 	t3.setCamera("/dev/video0",10);
// 	t3.show();
// 	t3.resize(320,240);
// 	

/*
	CameraViewerWidget t3;
	t3.setCamera("vfwcap://0");
	t3.show();
	t3.resize(320,240);

	CameraViewerWidget t4;
	t4.setCamera("vfwcap://0");
	t4.show();
	t4.resize(320,240);*/

	return app.exec();
}

