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
	t.setAttribute(Qt::WA_OpaquePaintEvent );
 	t.show();


//  	QGraphicsScene *scene = new QGraphicsScene;
// 	scene->addText("Hello, world!");
// // 	scene->setBackgroundBrush(Qt::NoBrush);
// 	
// 	QGraphicsView2 * view = new QGraphicsView2;
// 	view->setScene(scene);
//  	view->setAttribute(Qt::WA_OpaquePaintEvent);
// 	view->setViewport(&t);
// // 	view->setBackgroundBrush(QColor(255,0,0,128));
// 	view->show();
	
//  	QBrush brBrush(Qt::NoBrush); //QColor(0,255,0,128));
// 	QPalette p(brBrush,brBrush,brBrush,brBrush,brBrush,brBrush,brBrush,brBrush,brBrush);
// // 	QPalette p;
// // 	p.setBrush(QPalette::Window, QBrush());
// 	view->setPalette(p);
// 	t.setPalette(p);

	//t.setView(view);
	
	return app.exec();
}

