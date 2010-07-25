#include "GLThread.h"
#include <QTime>

#include <QtGui>
#include <QtOpenGL>
#include <QColor>

#include "GLWidget.h"


GLThread::GLThread(GLWidget *gl) 
	: QThread(), glw(gl)
{
	doRendering = true;
	doResize = false;
}

void GLThread::stop()
{
	doRendering = false;
}

void GLThread::resizeViewport(const QSize &size)
{
	w = size.width();
	h = size.height();
	doResize = true;
}    

void GLThread::run()
{
	srand(QTime::currentTime().msec());
	rotAngle = rand() % 360;
	
	glw->makeCurrent();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();        
	glOrtho(-5.0, 5.0, -5.0, 5.0, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, 200, 200);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	while (doRendering) 
	{
		if (doResize) 
		{
			glViewport(0, 0, w, h);
			doResize = false;
		}
		// Rendering code goes here
		
		glRotatef(rotAngle / 16.0, 1.0, 0.0, 0.0);
		glRotatef(rotAngle / 16.0, 0.0, 1.0, 0.0);
    		glRotatef(rotAngle / 16.0, 0.0, 0.0, 1.0);
		
		//qglClearColor(QColor::fromCmykF(0.39, 0.39, 0.0, 0.0).dark());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glBegin(GL_POLYGON);
// 		glTexCoord2f(1.0, 1.0);
		glVertex3f(0.8, 0.8, -2.0);
// 		glTexCoord2f(0.0, 1.0);
		glVertex3f(0.2, 0.8, -100.0);
// 		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.2, 0.2, -100.0);
// 		glTexCoord2f(1.0, 0.0);
		glVertex3f(0.8, 0.2, -2.0);
		glEnd();

		glw->swapBuffers();
		msleep(40);
	}
}


