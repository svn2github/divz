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
		loadLayerPropertiesFromObject(drawable, props);
		
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
			
			m_props["deinterlace"].value = source->deinterlace();
		}
	}
	else
	{
		applyLayerPropertiesToObject(drawable, props);
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


#include "ExpandableWidget.h"
	
QWidget * LiveVideoInputLayer::createLayerPropertyEditors()
{
	QWidget * base = new QWidget();
	QVBoxLayout *blay = new QVBoxLayout(base);
	blay->setContentsMargins(0,0,0,0);
	
	ExpandableWidget *groupContent = new ExpandableWidget("Video Input",base);
	blay->addWidget(groupContent);
	
	QWidget *groupContentContainer = new QWidget;
	QGridLayout *gridLayout = new QGridLayout(groupContentContainer);
	gridLayout->setContentsMargins(3,3,3,3);
	
	groupContent->setWidget(groupContentContainer);
	
	//formLayout->addRow(tr("&Text:"), generatePropertyEditor(this, "text", SLOT(setText(const QString&))));
	int row = 0;
	gridLayout->addWidget(generatePropertyEditor(this, "deinterlace", SLOT(setDeinterlace(bool))), row, 0, 1, 2);

// 	QFormLayout *formLayout = new QFormLayout(groupContentContainer);
// 	formLayout->setContentsMargins(3,3,3,3);
// 	
// 	groupContent->setWidget(groupContentContainer);
// 	
// 	formLayout->addRow("", generatePropertyEditor(this, "deinterlace", SLOT(setDeinterlace(bool))));
// 	
 	groupContent->setExpanded(true);
	
	/////////////////////////////////////////
	
	QWidget *basics =  LiveLayer::createLayerPropertyEditors();
	blay->addWidget(basics);
	
	return base;
}

void LiveVideoInputLayer::setDeinterlace(bool flag)
{
	if(m_camera)
		m_camera->setDeinterlace(flag);
}

void LiveVideoInputLayer::setLayerProperty(const QString& key, const QVariant& value)
{
	if(key == "deinterlace")
	{
		setDeinterlace(value.toBool());
	}
	else
	if(m_camera)
	{
		const char *keyStr = qPrintable(key);
		if(m_camera->metaObject()->indexOfProperty(keyStr)>=0)
		{
			m_camera->setProperty(keyStr, value);
		}
	}
	
	LiveLayer::setLayerProperty(key,value);

}
