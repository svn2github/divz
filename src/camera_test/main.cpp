#include <QtGui/QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextCursor>

//#include "CameraTest.h"
#include "CameraViewerWidget.h"

int main(int argc, char **argv)
{
	//QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
	QApplication app(argc, argv);

//  	CameraTest t;
//  	t.resize(320,240);
//  	t.show();
	
	
	CameraViewerWidget t;
	t.setSourceRectAdjust(11,0,-6,-3);
	t.setObjectName("v1");
	t.setWindowTitle("v1");
	//t.setCamera("vfwcap://0");
	t.setCamera("/dev/video0",30);
	t.resize(320,240);
	t.setOverlayText("/dev/video0");
	t.show();
	
	
	
	CameraViewerWidget t2;
	t2.setObjectName("v2");
	t2.setWindowTitle("v2");
	t2.setCamera("/dev/video0",10);
	t2.resize(320,240);
	t2.show();
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

