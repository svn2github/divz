#ifndef GLThread_H
#define GLThread_H
 
#include <QThread>
#include <QSize>

class GLWidget;

class GLThread : public QThread
{
public:
	GLThread(GLWidget *glWidget);
	void resizeViewport(const QSize &size);
	void run();
	void stop();

private:
	bool doRendering;
	bool doResize;
	int w;
	int h;
	int rotAngle;
	GLWidget *glw;
};

#endif
