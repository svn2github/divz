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

#include "../livemix/VideoWidget.h"
#include "../livemix/CameraThread.h"

#include "VideoSender.h"
#include "VideoReceiver.h"

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
	
// 	VideoWidget *glw = new VideoWidget();
//  	//GLWidget *glw = new GLWidget();
//  	glw->resize(640,480);
//  	//glw->setViewport(QRectF(0,0,1000,750));
//  	
//  	CameraThread *source = CameraThread::threadForCamera("/dev/video0");
//  	source->setInput("S-Video");
//  	source->setFps(30);
//  	//source->setDeinterlace(true);
// 	source->registerConsumer(glw);
// 	source->enableRawFrames(true);
// 	
// 	//glw->setVideoSource(source);
// 	
// 	VideoSender *sender = new VideoSender();
// 	sender->setVideoSource(source);
// 	if(!sender->listen(QHostAddress::Any,8899))
// 	{
// 		qDebug() << "Unable to setup server";
// 		return -1;
// 	}
// 	
// 	VideoReceiver *rx = VideoReceiver::getReceiver("localhost",8899);
// 	glw->setVideoSource(rx);
	
 	
//  	GLVideoInputDrawable *gld = new GLVideoInputDrawable();
//  	gld->setVideoInput("/dev/video0");
// 	gld->setCardInput("S-Video");
// 	//gld->setDeinterlace(true);
// 	gld->setRect(glw->viewport());
// 	gld->setVisible(true);
// 	glw->addDrawable(gld);

//  	GLImageDrawable *gld = new GLImageDrawable("Pm5544.jpg");
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
