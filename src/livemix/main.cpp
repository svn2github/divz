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
#include "MjpegThread.h"

#include "MainWindow.h"

//#include "MdiCamera.h"


int main(int argc, char **argv)
{
	//QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
	QApplication app(argc, argv);
	qApp->setApplicationName("LiveMix");
	qApp->setOrganizationName("Josiah Bryan");
	qApp->setOrganizationDomain("mybryanlife.com");


//  	CameraTest t;
//  	t.resize(320,240);
//  	t.show();


	MainWindow mw;
	mw.show();

/*
	MdiCamera a;
	MdiCamera b;
	a.show();
	b.show();*/
/*

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
//	viewer1.setOverlayText("Camera 01");
	viewer1.show();
*/
/*

// 	VideoThread *videoSource = new VideoThread();
// 	videoSource->setVideo("../data/Seasons_Loop_3_SD.mpg");
// 	videoSource->start();

	CameraThread *cameraSource2 = CameraThread::threadForCamera(defaultCamera);
	if(cameraSource2)
		cameraSource2->setFps(30);

	VideoWidget viewer2;
	viewer2.setVideoSource(cameraSource2);
	viewer2.setWindowTitle("Video");
	viewer2.resize(320,240);
	viewer2.setOverlayText("Video loop");
	viewer2.setFps(-1);
	viewer2.show();*/

/*
	MjpegThread * mjpeg = new MjpegThread();
	//mjpeg->connectTo("cameras",8082);
	mjpeg->connectTo("192.168.0.44",80,"/videostream.cgi","admin","lugubrious");
	mjpeg->start();

	VideoWidget viewer3;
	//viewer2.setVideoSource(videoSource);
	viewer3.setVideoSource(mjpeg);
	viewer3.setWindowTitle("Video");
	viewer3.resize(320,240);
	viewer3.setOverlayText("Webcam");
	viewer3.setFps(5);
	viewer3.show();*/


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

