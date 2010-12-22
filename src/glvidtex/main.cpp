#include <QApplication>

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include "GLWidget.h"

#include "GLImageDrawable.h"
#include "GLVideoLoopDrawable.h"
#include "GLVideoInputDrawable.h"
#include "GLVideoFileDrawable.h"
#include "GLVideoReceiverDrawable.h"
#include "GLTextDrawable.h"
#include "GLVideoMjpegDrawable.h"

#include "MetaObjectUtil.h"

#include "PlayerWindow.h"

int main(int argc, char *argv[])
{

	QApplication app(argc, argv);
	
	qApp->setApplicationName("GLVidTex");
	qApp->setOrganizationName("Josiah Bryan");
	qApp->setOrganizationDomain("mybryanlife.com");
	
	MetaObjectUtil_Register(GLImageDrawable);
	MetaObjectUtil_Register(GLTextDrawable);
	MetaObjectUtil_Register(GLVideoFileDrawable);
	MetaObjectUtil_Register(GLVideoInputDrawable);
	MetaObjectUtil_Register(GLVideoLoopDrawable);
	MetaObjectUtil_Register(GLVideoReceiverDrawable);

 	PlayerWindow *glw = new PlayerWindow();
/*	GLWidget *glw = new GLWidget();
	glw->resize(640,480);
	glw->setViewport(QRectF(0,0,1000,750));*/
 	
//  	GLVideoInputDrawable *gld = new GLVideoInputDrawable();
//  	gld->setVideoInput("/dev/video0");
// 	gld->setCardInput("S-Video");
// 	//gld->setDeinterlace(true);
// 	gld->setRect(glw->viewport());
// 	gld->setVisible(true);
// 	glw->addDrawable(gld);

// 	GLTextDrawable *gld = new GLTextDrawable("Hello 1");
// 	gld->setRect(glw->viewport());
// 	gld->setVisible(true);
// 	glw->addDrawable(gld);
// 	
// 	gld = new GLTextDrawable("Hello 2");
// 	gld->setRect(glw->viewport().translated(100,100));
// 	gld->setVisible(true);
// 	glw->addDrawable(gld);
// 		
 	glw->show();
		
	int x = app.exec();
	delete glw;
	return x;
}
