#include <QtGui/QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextCursor>

#include "CameraTest.h"

int main(int argc, char **argv)
{
	//QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
	QApplication app(argc, argv);

	CameraTest t;
	t.show();

	return app.exec();
}

