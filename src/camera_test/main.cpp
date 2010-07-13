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

// 	CameraTest t;
	CameraViewerWidget t;
	t.setCamera("vfwcap://0");
	t.show();
	t.resize(320,240);

	CameraViewerWidget t2;
	t2.setCamera("vfwcap://0");
	t2.show();
	t2.resize(320,240);

	CameraViewerWidget t3;
	t3.setCamera("vfwcap://0");
	t3.show();
	t3.resize(320,240);

	CameraViewerWidget t4;
	t4.setCamera("vfwcap://0");
	t4.show();
	t4.resize(320,240);

	return app.exec();
}

