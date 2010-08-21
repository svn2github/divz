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

	//drawable->show();
	
	//drawable->setObjectName(qPrintable(defaultCamera));


// 	VideoDisplayOptionWidget *opts = new VideoDisplayOptionWidget(drawable);
// 	opts->adjustSize();
// 	opts->show();

	connect(this, SIGNAL(videoSourceChanged(VideoSource*)), drawable, SLOT(setVideoSource(VideoSource*)));
	
	/// TODO
	//setCamera(source);
	return drawable;
}


void LiveVideoInputLayer::initDrawable(GLDrawable *drawable, bool isFirst)
{
	//qDebug() << "LiveVideoInputLayer::setupDrawable: drawable:"<<drawable<<", copyFrom:"<<copyFrom;
	LiveLayer::initDrawable(drawable, isFirst);
	
	GLVideoDrawable *vid = dynamic_cast<GLVideoDrawable*>(drawable);
	if(!vid)
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
			
	if(isFirst)
	{
		loadLayerPropertiesFromDrawable(drawable, props);
		
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
			source->enableRawFrames(true);
			
			setCamera(source);
		}
			
	}
	else
	{
		applyLayerPropertiesToDrawable(drawable, props);
		if(m_camera)
			setCamera(m_camera);
	}
}

void LiveVideoInputLayer::setCamera(CameraThread *camera)
{
	qDebug() << "LiveVideoInputLayer::setCamera: "<<camera;
	emit videoSourceChanged(camera);
	m_camera = camera;
	setInstanceName(camera->inputName());
}


