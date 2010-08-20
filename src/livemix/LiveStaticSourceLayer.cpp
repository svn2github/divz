#include "LiveStaticSourceLayer.h"

#include "../glvidtex/GLWidget.h"
#include "../glvidtex/GLVideoDrawable.h"
#include "../glvidtex/StaticVideoSource.h"

///////////////////////
LiveStaticSourceLayer::LiveStaticSourceLayer(QObject *parent)
	: LiveLayer(parent)
{
	m_controlWidget = new LayerControlWidget(this);
}

LiveStaticSourceLayer::~LiveStaticSourceLayer()
{
}

GLDrawable* LiveStaticSourceLayer::createDrawable(GLWidget *context)
{
	// add secondary frame
	GLVideoDrawable *drawable = new GLVideoDrawable();


	StaticVideoSource *source = new StaticVideoSource();
	//source->setImage(QImage("me2.jpg"));
	QImage img("dsc_6645-1.jpg");
	if(img.isNull())
		source->setImage(QImage("../glvidtex/me2.jpg"));
	else
		source->setImage(img);
	//source->setImage(QImage("/opt/qtsdk-2010.02/qt/examples/opengl/pbuffers/cubelogo.png"));

	source->start();
	drawable->setVideoSource(source);
	drawable->setRect(context->viewport());
	drawable->setZIndex(-1);
	drawable->setObjectName("Static");
	//drawable->setOpacity(0.5);
	//drawable->show();


// 	#ifdef HAS_QT_VIDEO_SOURCE
// 	// just change enterance anim to match effects
// 	drawable->addShowAnimation(GLDrawable::AnimFade);
 	//drawable->addShowAnimation(GLDrawable::AnimZoom);
// 	#else
	drawable->addShowAnimation(GLDrawable::AnimZoom,2500).curve = QEasingCurve::OutElastic;
// 	#endif
//
 	drawable->addHideAnimation(GLDrawable::AnimFade);
 	drawable->addHideAnimation(GLDrawable::AnimZoom,1000);
//

	changeInstanceName("File 'me2.jpg'");
	
	return drawable;
}


void LiveStaticSourceLayer::setupInstanceProperties(GLDrawable *drawable)
{
	LiveLayer::setupInstanceProperties(drawable);
	
	// TODO expose a 'filename' property
	
	/*
	GLVideoDrawable *vid = dynamic_cast<GLVideoDrawable*>(drawable);
	if(!vid)
		return;
	
	QStringList props
		<< "brightness"
		<< "contrast"
		<< "hue"
		<< "saturation"
		<< "flipHorizontal"
		<< "flipVertical"
		<< "cropTopLeft"
		<< "cropBottomRight";
		
	foreach(QString prop, props)
	{
		m_props[prop] = vid->property(qPrintable(prop));
	}*/
}

QList<QtPropertyEditorIdPair> LiveStaticSourceLayer::createPropertyEditors(QtVariantPropertyManager *manager)
{
	QList<QtPropertyEditorIdPair> list = LiveLayer::createPropertyEditors(manager);
		
// 	GLDrawable *gl = drawable();
// 	if(!gl)
// 		return list;
		
	// TODO expose filename property
	
// 	QtVariantProperty *property;
// 	
// 	
// 	VideoSource *source = gl->videoSource();
// 	
// 	CameraThread *camera = dynamic_cast<CameraThread*>(source);
// 	if(camera)
// 	{
// 		property = manager->addProperty(QVariant::Bool, tr("Deinterlace"));
// 		property->setValue(camera->deinterlace());
// 		list << QtPropertyEditorIdPair("deinterlace", property);
// 	}
// 	
// 	///////////////////////////////////////////
// 	
// 	property = manager->addProperty(QVariant::Int, tr("Contrast"));
// 	property->setAttribute(QLatin1String("minimum"), -100);
// 	property->setAttribute(QLatin1String("maximum"), 100);
// 	property->setValue(gl->brightness());
// 	list << QtPropertyEditorIdPair("contrast", property);
// 	
// 	property = manager->addProperty(QVariant::Int, tr("Hue"));
// 	property->setAttribute(QLatin1String("minimum"), -100);
// 	property->setAttribute(QLatin1String("maximum"), 100);
// 	property->setValue(gl->brightness());
// 	list << QtPropertyEditorIdPair("saturation", property);
// 	
// 	property = manager->addProperty(QVariant::Int, tr("Saturation"));
// 	property->setAttribute(QLatin1String("minimum"), -100);
// 	property->setAttribute(QLatin1String("maximum"), 100);
// 	property->setValue(gl->brightness());
// 	list << QtPropertyEditorIdPair("saturation", property);
// 	
// 	
// 	///////////////////////////////////////////
// 	
// 	property = manager->addProperty(QVariant::Bool, tr("Flip Horizontal"));
// 	property->setValue(gl->flipHorizontal());
// 	list << QtPropertyEditorIdPair("flipHorizontal", property);
// 	
// 	property = manager->addProperty(QVariant::Bool, tr("Flip Vertical"));
// 	property->setValue(gl->flipHorizontal());
// 	list << QtPropertyEditorIdPair("flipVertical", property);
// 	
// 	///////////////////////////////////////////
// 		
// 	property = manager->addProperty(QVariant::PointF, tr("Crop Top-Left"));
// 	property->setValue(gl->cropTopLeft());
// 	list << QtPropertyEditorIdPair("cropTopLeft", property);
// 	
// 	property = manager->addProperty(QVariant::PointF, tr("Crop Bottom-Right"));
// 	property->setValue(gl->cropBottomRight());
// 	list << QtPropertyEditorIdPair("cropBottomRight", property);
// 	
// 	///////////////////////////////////////////
	
	return list;
}

void LiveStaticSourceLayer::setInstanceProperty(const QString& key, const QVariant& value)
{
	// TODO load filename from prop and set image
	
// 	if(key == "deinterlace")
// 	{
// 		VideoSource *source = gl->videoSource();
// 		
// 		CameraThread *camera = dynamic_cast<CameraThread*>(source);
// 		if(camera)
// 		{
// 			camera->setDeinterlace((bool)value.toInt());
// 		}
// 	}
	
	LiveLayer::setInstanceProperty(key,value);

}
