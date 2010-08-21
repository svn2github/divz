#include <QtGui/QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextCursor>

//#include "CameraTest.h"
// #include "VideoWidget.h"
 #include "CameraThread.h"
// #include "VideoThread.h"
// #include "MjpegThread.h"

#include "LiveVideoInputLayer.h"
#include "LiveStaticSourceLayer.h"
#include "LiveScene.h"
#include "../glvidtex/GLWidget.h"
#include "../glvidtex/GLVideoDrawable.h"
#include "../glvidtex/StaticVideoSource.h"

//#include "MainWindow.h"

//#include "MdiCamera.h"

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif



int main(int argc, char **argv)
{
/*	#ifdef Q_WS_X11
		qDebug() << "Setting up multithreaded X11 library calls";
		XInitThreads();
	#endif*/
	
	//QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
	QApplication app(argc, argv);
	qApp->setApplicationName("LiveMix");
	qApp->setOrganizationName("Josiah Bryan");
	qApp->setOrganizationDomain("mybryanlife.com");

	
// 	GLWidget widget(0);
// 	widget.setWindowTitle("Thread0");
// 	widget.show();
//  	widget.startRendering();
// 	
	
//  	CameraTest t;
//  	t.resize(320,240);
//  	t.show();


// 	MainWindow mw;
// 	mw.show();

	GLWidget *glOutputWin = new GLWidget();
	{
		glOutputWin->setWindowTitle("Live");
		glOutputWin->resize(1000,750);
		// debugging...
		//glOutputWin->setProperty("isEditorWidget",true);
	
	}
	
	GLWidget *glEditorWin = new GLWidget();
	{
		glEditorWin->setWindowTitle("Editor");
		glEditorWin->resize(400,300);
		glEditorWin->setProperty("isEditorWidget",true);
	}
	
	// Setup Editor Background
	{
		QSize size = glEditorWin->viewport().size().toSize();
		size /= 4;
		//qDebug() << "MainWindow::createLeftPanel(): size:"<<size;
		QImage bgImage(size, QImage::Format_ARGB32_Premultiplied);
		QBrush bgTexture(QPixmap("squares2.png"));
		QPainter bgPainter(&bgImage);
		bgPainter.fillRect(bgImage.rect(), bgTexture);
		bgPainter.end();
		
		StaticVideoSource *source = new StaticVideoSource();
		source->setImage(bgImage);
		//source->setImage(QImage("squares2.png"));
		source->start();
		
		GLVideoDrawable *drawable = new GLVideoDrawable(glEditorWin);
		drawable->setVideoSource(source);
		drawable->setRect(glEditorWin->viewport());
		drawable->setZIndex(-100);
		drawable->setObjectName("StaticBackground");
		drawable->show();
		
		glEditorWin->addDrawable(drawable);
	}
	
	
	
	// setup scene
	LiveScene *scene = new LiveScene();
	{
		//scene->addLayer(new LiveVideoInputLayer());
		scene->addLayer(new LiveStaticSourceLayer());
	}
	
	// add to live output
	{
		//scene->attachGLWidget(glOutputWin);
		scene->layerList().at(0)->drawable(glOutputWin)->setObjectName("Output");
		glOutputWin->addDrawable(scene->layerList().at(0)->drawable(glOutputWin));
		scene->layerList().at(0)->setVisible(true);
	}
	
	// add to editor
	{
		scene->layerList().at(0)->drawable(glEditorWin)->setObjectName("Editor");
		glEditorWin->addDrawable(scene->layerList().at(0)->drawable(glEditorWin));
	}
	
	// show windows
	{
		glOutputWin->show();
		glEditorWin->show();
	}
	
	
	
// 	#ifdef Q_OS_WIN
// 		QString defaultCamera = "vfwcap://0";
// 	#else
// 		QString defaultCamera = "/dev/video0";
// 	#endif
// 
// 	CameraThread *source = CameraThread::threadForCamera(defaultCamera);
// 	if(source)
// 	{
// 		source->setFps(30);
// 		usleep(750 * 1000); // This causes a race condition to manifist itself reliably, which causes a crash every time instead of intermitently. 
// 		// With the crash reproducable, I can now work to fix it.
// 		source->enableRawFrames(true);
// 		//source->setDeinterlace(true);
// 		
// 		GLVideoDrawable *drawable = new GLVideoDrawable(viewer);
// 		drawable->setVideoSource(source);
// 		drawable->setRect(viewer->viewport());
// 		
// 		//if(camera != "/dev/video1")
// 		//	drawable->setAlphaMask(QImage("alphamask2.png"));
// 		
// 		viewer->addDrawable(drawable);
// 		drawable->addShowAnimation(GLDrawable::AnimFade);
// 		drawable->addHideAnimation(GLDrawable::AnimFade);
// 		drawable->show();
// 		drawable->setObjectName(qPrintable(defaultCamera));
// 	}
		
	
	//loadLiveScene(scene);

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

