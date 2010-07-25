#ifndef GLWidget_H
#define GLWidget_H

#include "GLThread.h"
#include <QGLWidget>

class GLWidget : public QGLWidget
{
public:
	GLWidget(QWidget *parent);
	void startRendering();    
	void stopRendering();
	
protected:
	void resizeEvent(QResizeEvent *evt);
	void paintEvent(QPaintEvent *);
	void closeEvent(QCloseEvent *evt);
	
	GLThread glt;
};

#endif
