#ifndef GLThread_H
#define GLThread_H
 
#include <QThread>
#include <QSize>
#include <QtGui>
#include <QtOpenGL>


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
	
	GLuint	texture[1]; // Storage For One Texture
	
	float xrot, yrot, zrot;
	float xscale, yscale, zscale;
	float xscaleInc, yscaleInc, zscaleInc;
// 	GLfloat	xrot;				// X Rotation ( NEW )
// 	GLfloat	yrot;				// Y Rotation ( NEW )
// 	GLfloat	zrot;				// Z Rotation ( NEW )

};

#endif
