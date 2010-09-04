#include "LiveStaticSourceLayer.h"

#include "../glvidtex/GLWidget.h"
#include "../glvidtex/GLVideoDrawable.h"
#include "../glvidtex/StaticVideoSource.h"

#include "ExpandableWidget.h"
#include <QFileInfo>

///////////////////////
LiveStaticSourceLayer::LiveStaticSourceLayer(QObject *parent)
	: LiveLayer(parent)
{
	m_staticSource = new StaticVideoSource();
	m_staticSource->setImage(QImage("squares2.png"));
	m_staticSource->start();
	
	//setFile("../data/icon-next-large.png");
	m_props["file"] = file();
}

LiveStaticSourceLayer::~LiveStaticSourceLayer()
{
}

GLDrawable* LiveStaticSourceLayer::createDrawable(GLWidget *context)
{
	// add secondary frame
	GLVideoDrawable *drawable = new GLVideoDrawable();

 	//qDebug() << "LiveStaticSourceLayer::createDrawable(): context viewport:"<<context->viewport();

	drawable->setVideoSource(m_staticSource);
	drawable->setRect(context->viewport());
	drawable->setZIndex(-1);
	drawable->setObjectName("Static");
	
	drawable->addShowAnimation(GLDrawable::AnimFade);
	drawable->addHideAnimation(GLDrawable::AnimFade);
	drawable->addHideAnimation(GLDrawable::AnimZoom,1000);
	
 	//qDebug() << "LiveStaticSourceLayer::createDrawable(): "<<drawable<<": setup complete, drawable rect:"<<drawable->rect();
	
	return drawable;
}

void LiveStaticSourceLayer::initDrawable(GLDrawable *newDrawable, bool isFirst)
{
	LiveLayer::initDrawable(newDrawable, isFirst);

	//setFile(file());
}

QWidget * LiveStaticSourceLayer::createLayerPropertyEditors()
{
	QWidget * base = new QWidget();
	QVBoxLayout *blay = new QVBoxLayout(base);
	blay->setContentsMargins(0,0,0,0);

	ExpandableWidget *groupContent = new ExpandableWidget("Image",base);
	blay->addWidget(groupContent);

	QWidget *groupContentContainer = new QWidget;
	QFormLayout *formLayout = new QFormLayout(groupContentContainer);
	formLayout->setContentsMargins(3,3,3,3);

	groupContent->setWidget(groupContentContainer);

	PropertyEditorOptions opts;
	opts.stringIsFile = true;
	opts.fileTypeFilter = tr("Image Files (*.png *.jpg *.bmp *.svg *.xpm *.gif);;Any File (*.*)");
	formLayout->addRow(tr("&Image:"), generatePropertyEditor(this, "file", SLOT(setFile(const QString&)), opts));

	groupContent->setExpandedIfNoDefault(true);
	
	/////////////////////////////////////////
	
	QWidget *basics =  LiveLayer::createLayerPropertyEditors();
	blay->addWidget(basics);
	
	return base;
}

void LiveStaticSourceLayer::setFile(const QString& file)
{
	QImage image(file);
	if(image.isNull())
		qDebug() << "LiveStaticsourceLayer::setFile: Error loading file:"<<file;
	else
	{
		m_staticSource->setImage(image);
		m_props["file"] = file;
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
