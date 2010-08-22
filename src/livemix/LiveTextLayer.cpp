#include "LiveTextLayer.h"

#include "../glvidtex/GLWidget.h"
#include "../glvidtex/GLVideoDrawable.h"
#include "../glvidtex/TextVideoSource.h"

///////////////////////

LiveTextLayer::LiveTextLayer(QObject *parent)
	: LiveLayer(parent)
	, m_textSource(0)
{

}

LiveTextLayer::~LiveTextLayer()
{
}

GLDrawable* LiveTextLayer::createDrawable(GLWidget *context)
{
	// add secondary frame
	// add text overlay frame
 	GLVideoDrawable *drawable = new GLVideoDrawable(context);
 	drawable->setObjectName("LiveTextLayer:drawable");
// 
// 
// 	m_textSource = new TextVideoSource();
// 	m_textSource->start();
// 	
// 	drawable->setVideoSource(m_textSource);
// 	
// 	drawable->setZIndex(9999);
// 	drawable->setObjectName("Text");
// 
// 	drawable->addShowAnimation(GLDrawable::AnimFade);
// 	drawable->addShowAnimation(GLDrawable::AnimSlideTop,2500).curve = QEasingCurve::OutElastic;
// 
// 	drawable->addHideAnimation(GLDrawable::AnimFade);
// 	drawable->addHideAnimation(GLDrawable::AnimZoom);

	
	QSizeF size;
	
	if(!m_textSource)
	{
		m_textSource = new TextVideoSource();
		m_textSource->start();
		//source->setHtml("<img src='me2.jpg'><b>TextVideoSource</b>");
		m_textSource->setHtml("?");
		m_textSource->changeFontSize(40);
		
		size = m_textSource->findNaturalSize();
		m_textSource->setTextWidth((int)size.width());
	}
	//qDebug() << "New html: "<<source->html();
	//source->setImage(QImage("/opt/qtsdk-2010.02/qt/examples/opengl/pbuffers/cubelogo.png"));
	
	drawable->setVideoSource(m_textSource);
	//drawable->setRect(glw->viewport());
	//qDebug() << "Text Size: "<<size;
	
	QRectF viewport = context->viewport();

	drawable->setRect(QRectF(
		qMax(viewport.right()  - size.width()  , 0.0),
		qMax(viewport.bottom() - size.height() , 0.0),
		size.width(),
		size.height()));

	drawable->setZIndex(1);
	//drawable->setOpacity(0.5);
	drawable->setObjectName("Text");
	
	drawable->addShowAnimation(GLDrawable::AnimFade);
	//drawable->addShowAnimation(GLDrawable::AnimSlideTop,2500).curve = QEasingCurve::OutElastic;
	//drawable->addShowAnimation(GLDrawable::AnimSlideLeft,2000).curve = QEasingCurve::OutElastic;
 	
 	drawable->addHideAnimation(GLDrawable::AnimFade);
 	//drawable->addHideAnimation(GLDrawable::AnimZoom);
 	

	
	return drawable;	
}



void LiveTextLayer::initDrawable(GLDrawable *drawable, bool isFirstDrawable)
{
	LiveLayer::initDrawable(drawable, isFirstDrawable);
	
	QStringList props = QStringList()
		<< "outlineEnabled"
		<< "outlineColor"
		<< "fillEnabled"
		<< "fillColor"
		<< "shadowEnabled"
		<< "shadowColor"
		<< "shadowBlurRadius"
		<< "shadowOffsetX"
		<< "shadowOffsetY";
		
				
	if(isFirstDrawable)
	{
		loadLayerPropertiesFromObject(drawable, props);
		
		setText("<b>Welcome to LiveMix</b>");
		m_props["text"] = LiveLayerProperty("text",text());
	}
	else
	{
		applyLayerPropertiesToObject(drawable, props);
		
		// Not needed if not first drawable
		//setText(text());
	}
	
	
}

void LiveTextLayer::setText(const QString& text)
{
	m_textSource->setHtml(text);
	// TODO make font size configurable
	m_textSource->changeFontSize(40);
	QSize size = m_textSource->findNaturalSize();
	m_textSource->setTextWidth(size.width());
	
	// TODO make anchor configurable
	
	QRectF viewport(0,0,1000,750);

	QRectF newRect(
		qMax(viewport.right()  - size.width()  , 0.0),
		qMax(viewport.bottom() - size.height() , 0.0),
		size.width(),
		size.height());
	
	//qDebug() << "LiveTextLayer::setText:"<<text<<": size:"<<size<<", newRect:"<<newRect;
	
	setLayerProperty("rect",newRect);
	
	m_props["text"].value = text;
// 	
	setInstanceName(text);
}
/*
QList<QtPropertyEditorIdPair> LiveTextLayer::createPropertyEditors(QtVariantPropertyManager *manager)
{
	QList<QtPropertyEditorIdPair> list = LiveLayer::createPropertyEditors(manager);
	
	QtVariantProperty *property;
	
	///////////////////////////////////////////
	
	property = manager->addProperty(QVariant::String, tr("Text"));
	property->setValue(text());
	list << QtPropertyEditorIdPair("text", property);
	
	///////////////////////////////////////////
	
	property = manager->addProperty(QVariant::Bool, tr("Draw Outline?"));
	property->setValue(m_textSource->outlineEnabled());
	list << QtPropertyEditorIdPair("outlineEnabled", property);
	
	property = manager->addProperty(QVariant::Color, tr("Outline Color"));
	property->setValue(m_textSource->outlinePen().color());
	list << QtPropertyEditorIdPair("outlineColor", property);
	
	///////////////////////////////////////////
	
	property = manager->addProperty(QVariant::Bool, tr("Fill Text?"));
	property->setValue(m_textSource->fillEnabled());
	list << QtPropertyEditorIdPair("fillEnabled", property);
	
	property = manager->addProperty(QVariant::Color, tr("Fill Color"));
	property->setValue(m_textSource->fillBrush().color());
	list << QtPropertyEditorIdPair("fillColor", property);
	
	///////////////////////////////////////////
	
	
	property = manager->addProperty(QVariant::Bool, tr("Draw Shadow?"));
	property->setValue(m_textSource->shadowEnabled());
	list << QtPropertyEditorIdPair("shadowEnabled", property);
	
	property = manager->addProperty(QVariant::Color, tr("Shadow Color"));
	property->setValue(m_textSource->shadowBrush().color());
	list << QtPropertyEditorIdPair("shadowColor", property);
	
	property = manager->addProperty(QVariant::Double, tr("Shadow Radius"));
	property->setValue(m_textSource->shadowBlurRadius());
	list << QtPropertyEditorIdPair("shadowBlurRadius", property);
	
	property = manager->addProperty(QVariant::Double, tr("Shadow X"));
	property->setValue(m_textSource->shadowOffsetX());
	list << QtPropertyEditorIdPair("shadowOffsetX", property);
	
	property = manager->addProperty(QVariant::Double, tr("Shadow Y"));
	property->setValue(m_textSource->shadowOffsetY());
	list << QtPropertyEditorIdPair("shadowOffsetY", property);
	
	
	///////////////////////////////////////////
	
	
	
// 	m_props["outlineEnabled"] 	= m_textSource->outlineEnabled();
// 	m_props["outlineColor"] 	= m_textSource->outlinePen().color();
// 	m_props["fillEnabled"] 		= m_textSource->fillEnabled();
// 	m_props["fillColor"] 		= m_textSource->fillBrush().color()
// 	m_props["shadowEnabled"] 	= m_textSource->shadowEnabled();
// 	m_props["shadowColor"] 		= m_textSource->shadowBrush().color();
// 	m_props["shadowBlurRadius"] 	= m_textSource->shadowBlurRadius();
// 	m_props["shadowOffsetX"] 	= m_textSource->shadowOffsetX();
// 	m_props["shadowOffsetY"] 	= m_textSource->shadowOffsetY();
	
	return list;
}*/

void LiveTextLayer::setLayerProperty(const QString& key, const QVariant& value)
{
	if(key == "text")
	{
		setText(value.toString());
	}
	else
	if(key.indexOf("color") > -1)
	{
		QColor color = value.value<QColor>();
		
		if(key == "outlineColor")
		{
			m_textSource->setOutlinePen(QPen(color));
		}
		else
		if(key == "fillColor")
		{
			m_textSource->setFillBrush(QBrush(color));
		}
		else
		if(key == "shadowColor")
		{
			m_textSource->setShadowBrush(QBrush(color));
		}
	}
	else
	{
		const char *keyStr = qPrintable(key);
		if(m_textSource->metaObject()->indexOfProperty(keyStr)>=0)
		{
			m_textSource->setProperty(keyStr, value);
		}
	}
	
	LiveLayer::setLayerProperty(key,value);

}


///////////////////////
