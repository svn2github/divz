#include <QtGui/QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QtOpenGL/QGLWidget>
#include <QDebug>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextCursor>

#include "VideoTest.h"

int main(int argc, char **argv)
{
	//QGL::setPreferredPaintEngine(QPaintEngine::OpenGL);
	QApplication app(argc, argv);

	VideoTest t;
	t.show();

	return app.exec();
}

