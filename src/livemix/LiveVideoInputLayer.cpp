#include "LiveVideoInputLayer.h"


#include "CameraThread.h"
#include "../glvidtex/GLWidget.h"
#include "../glvidtex/GLVideoDrawable.h"


LiveVideoInputLayer::LiveVideoInputLayer(QObject *parent)
	: LiveLayer(parent)
{
}

LiveVideoInputLayer::~LiveVideoInputLayer()
{
	// TODO close camera
}

GLDrawable *LiveVideoInputLayer::createDrawable(GLWidget *widget)
{
	GLVideoDrawable *drawable = new GLVideoDrawable();
	//drawable->setVideoSource(source);
	//drawable->setRect(QRectF(0,0,1000,750));
	drawable->setRect(widget->viewport());

	drawable->addShowAnimation(GLDrawable::AnimFade);
	drawable->addHideAnimation(GLDrawable::AnimFade);

	drawable->show();
	
	//drawable->setObjectName(qPrintable(defaultCamera));


// 	VideoDisplayOptionWidget *opts = new VideoDisplayOptionWidget(drawable);
// 	opts->adjustSize();
// 	opts->show();

	connect(this, SIGNAL(videoSourceChanged(VideoSource*)), drawable, SLOT(setVideoSource(VideoSource*)));
	
	/// TODO
	//setCamera(source);
	return drawable;
}


void LiveVideoInputLayer::initDrawable(GLDrawable *drawable, GLDrawable *copyFrom)
{
	//qDebug() << "LiveVideoInputLayer::setupInstanceProperties: mark1";
	LiveLayer::initDrawable(drawable, copyFrom);
	
	GLVideoDrawable *vid = dynamic_cast<GLVideoDrawable*>(drawable);
	if(!vid)
		return;
		
	if(copyFrom)
	{
		GLVideoDrawable *vid2 = dynamic_cast<GLVideoDrawable*>(copyFrom);
		if(!vid2)
			return;
	
		QStringList props = QStringList()
			<< "brightness"
			<< "contrast"
			<< "hue"
			<< "saturation"
			<< "flipHorizontal"
			<< "flipVertical"
			<< "cropTopLeft"
			<< "cropBottomRight"
			<< "textureOffset";
			
		foreach(QString prop, props)
		{
			vid2->setProperty(qPrintable(prop), vid->property(qPrintable(prop)));
		}
	}
	else
	{
		
		#ifdef Q_OS_WIN
			QString defaultCamera = "vfwcap://0";
		#else
			QString defaultCamera = "/dev/video0";
		#endif
	
		qDebug() << "LiveVideoInputLayer::initDrawable: Using default camera:"<<defaultCamera;
	
		CameraThread *source = CameraThread::threadForCamera(defaultCamera);
		if(source)
		{
			source->setFps(30);
			#ifdef Q_OS_LINUX
				//usleep(750 * 1000); // This causes a race condition to manifist itself reliably, which causes a crash every time instead of intermitently.
				// With the crash reproducable, I can now work to fix it.
			#endif
			source->enableRawFrames(true);
			//source->setDeinterlace(true);
			
			setCamera(source);
		}
	}
	
}

void LiveVideoInputLayer::setCamera(CameraThread *camera)
{
	qDebug() << "LiveVideoInputLayer::setCamera: "<<camera;
	emit videoSourceChanged(camera);
	m_camera = camera;
	changeInstanceName(camera->inputName());
}


