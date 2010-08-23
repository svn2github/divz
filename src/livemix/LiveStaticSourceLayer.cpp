#include "LiveStaticSourceLayer.h"

#include "../glvidtex/GLWidget.h"
#include "../glvidtex/GLVideoDrawable.h"
#include "../glvidtex/StaticVideoSource.h"

#include <QFileInfo>

///////////////////////
LiveStaticSourceLayer::LiveStaticSourceLayer(QObject *parent)
	: LiveLayer(parent)
{
// 	m_controlWidget = new LayerControlWidget(this);
}

LiveStaticSourceLayer::~LiveStaticSourceLayer()
{
}

GLDrawable* LiveStaticSourceLayer::createDrawable(GLWidget *context)
{
	// add secondary frame
	GLVideoDrawable *drawable = new GLVideoDrawable();

// 	qDebug() << "LiveStaticSourceLayer::createDrawable: context viewport:"<<context->viewport();

	m_staticSource = new StaticVideoSource();
	m_staticSource->setImage(QImage("squares2.png"));
	m_staticSource->start();
	
	drawable->setVideoSource(m_staticSource);
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
	//drawable->addShowAnimation(GLDrawable::AnimFade,2500).curve = QEasingCurve::OutElastic;
	//drawable->addShowAnimation(GLDrawable::AnimFade,100);
// 	#endif
//
	drawable->addHideAnimation(GLDrawable::AnimFade);
	drawable->addHideAnimation(GLDrawable::AnimZoom,1000);
//

	
	
// 	qDebug() << "LiveStaticSourceLayer::createDrawable: setup complete, drawable rect:"<<drawable->rect();
	
	return drawable;
}

//void LiveStaticSourceLayer::setupInstanceProperties(GLDrawable *drawable)
void LiveStaticSourceLayer::initDrawable(GLDrawable *newDrawable, bool isFirst)
{
	LiveLayer::initDrawable(newDrawable, isFirst);
	
	if(isFirst)
	{
		//loadLayerPropertiesFromObject(m_textSource, props);
		
		setFile("../data/icon-next-large.png");
		m_props["file"] = LiveLayerProperty("file",file());
	}
	else
	{
		//applyLayerPropertiesToObject(m_textSource, props);
		setFile(file());
	}
	
}

// QList<QtPropertyEditorIdPair> LiveStaticSourceLayer::createPropertyEditors(QtVariantPropertyManager *manager)
// {
// 	QList<QtPropertyEditorIdPair> list = LiveLayer::createPropertyEditors(manager);
// 		
// // 	GLDrawable *gl = drawable();
// // 	if(!gl)
// // 		return list;
// 		
// 	// TODO expose filename property
// 	
// // 	QtVariantProperty *property;
// // 	
// // 	
// // 	VideoSource *source = gl->videoSource();
// // 	
// // 	CameraThread *camera = dynamic_cast<CameraThread*>(source);
// // 	if(camera)
// // 	{
// // 		property = manager->addProperty(QVariant::Bool, tr("Deinterlace"));
// // 		property->setValue(camera->deinterlace());
// // 		list << QtPropertyEditorIdPair("deinterlace", property);
// // 	}
// // 	
// // 	///////////////////////////////////////////
// // 	
// // 	property = manager->addProperty(QVariant::Int, tr("Contrast"));
// // 	property->setAttribute(QLatin1String("minimum"), -100);
// // 	property->setAttribute(QLatin1String("maximum"), 100);
// // 	property->setValue(gl->brightness());
// // 	list << QtPropertyEditorIdPair("contrast", property);
// // 	
// // 	property = manager->addProperty(QVariant::Int, tr("Hue"));
// // 	property->setAttribute(QLatin1String("minimum"), -100);
// // 	property->setAttribute(QLatin1String("maximum"), 100);
// // 	property->setValue(gl->brightness());
// // 	list << QtPropertyEditorIdPair("saturation", property);
// // 	
// // 	property = manager->addProperty(QVariant::Int, tr("Saturation"));
// // 	property->setAttribute(QLatin1String("minimum"), -100);
// // 	property->setAttribute(QLatin1String("maximum"), 100);
// // 	property->setValue(gl->brightness());
// // 	list << QtPropertyEditorIdPair("saturation", property);
// // 	
// // 	
// // 	///////////////////////////////////////////
// // 	
// // 	property = manager->addProperty(QVariant::Bool, tr("Flip Horizontal"));
// // 	property->setValue(gl->flipHorizontal());
// // 	list << QtPropertyEditorIdPair("flipHorizontal", property);
// // 	
// // 	property = manager->addProperty(QVariant::Bool, tr("Flip Vertical"));
// // 	property->setValue(gl->flipHorizontal());
// // 	list << QtPropertyEditorIdPair("flipVertical", property);
// // 	
// // 	///////////////////////////////////////////
// // 		
// // 	property = manager->addProperty(QVariant::PointF, tr("Crop Top-Left"));
// // 	property->setValue(gl->cropTopLeft());
// // 	list << QtPropertyEditorIdPair("cropTopLeft", property);
// // 	
// // 	property = manager->addProperty(QVariant::PointF, tr("Crop Bottom-Right"));
// // 	property->setValue(gl->cropBottomRight());
// // 	list << QtPropertyEditorIdPair("cropBottomRight", property);
// // 	
// // 	///////////////////////////////////////////
// 	
// 	return list;
// }
// 

void LiveStaticSourceLayer::setFile(const QString& file)
{
	QImage image(file);
	if(image.isNull())
		qDebug() << "LiveStaticsourceLayer::setFile: Error loading file:"<<file;
	else
	{
		m_staticSource->setImage(image);
		m_props["file"].value = file;
		setInstanceName(QFileInfo(file).fileName());
	}
}


void LiveStaticSourceLayer::setLayerProperty(const QString& propertyId, const QVariant& value)
{
	// TODO load filename from prop and set image
	
	if(propertyId == "file")
	{
		setFile(value.toString());
	}
	
	LiveLayer::setLayerProperty(propertyId,value);

}
