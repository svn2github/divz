#include "LiveTextLayer.h"

#include "../glvidtex/GLWidget.h"
#include "../glvidtex/GLVideoDrawable.h"
#include "../glvidtex/TextVideoSource.h"
#include "../glvidtex/StaticVideoSource.h"

#include "ExpandableWidget.h"
//#include <qtcolorpicker.h>

///////////////////////

LiveTextLayer::LiveTextLayer(QObject *parent)
	: LiveLayer(parent)
	, m_textSource(0)
{
	// Text generator
	m_textSource = new TextVideoSource();
	m_textSource->start();
	//m_textSource->moveToThread(m_textSource);
	m_textSource->setHtml("?");
	m_textSource->changeFontSize(40);
	m_textSource->setObjectName("Primary");
	
	QSizeF size = m_textSource->findNaturalSize();
	m_textSource->setTextWidth((int)size.width());
	
	// For cross fading
	m_secondaryTextSource = new TextVideoSource();
	m_secondaryTextSource->start();
	//m_textSource->moveToThread(m_textSource);
	m_secondaryTextSource->setHtml("?");
	m_secondaryTextSource->changeFontSize(40);
	m_secondaryTextSource->setTextWidth((int)size.width());
	m_secondaryTextSource->setObjectName("Secondary");
	
	setText("<b>Hello, World</b>");
	
// 	setOutlineEnabled(true);
// 	setOutlinePen(QPen(1.5,Qt::black));
// 	setFillEnabled(true);
// 	setFullBrush(Qt::black);
// 	setShadowEnabled(true);
// 	setShadowBlurRadius(16);
// 	setShadowOffsetX(0);
// 	setShadowOffsetY(0);
}

LiveTextLayer::~LiveTextLayer()
{
}

GLDrawable* LiveTextLayer::createDrawable(GLWidget *context, bool isSecondary)
{
	// add secondary frame
	// add text overlay frame
	GLVideoDrawable *drawable = new GLVideoDrawable(context);
	
	drawable->setVideoSource(isSecondary ?  m_secondaryTextSource : m_textSource);
	if(isSecondary)
	{
		GLDrawable *primary = LiveLayer::drawable(context);
		if(primary)
		{
			connect(primary, SIGNAL(isVisible(bool)), drawable, SLOT(setHidden(bool)));
			connect(drawable, SIGNAL(isVisible(bool)), primary, SLOT(setHidden(bool)));
		}
	}
	
	drawable->setZIndex(1);
	//drawable->setObjectName("Text");
	drawable->setObjectName(isSecondary ? "Text-Secondary" : "Text-Primary");


	drawable->addShowAnimation(GLDrawable::AnimFade);
	drawable->addHideAnimation(GLDrawable::AnimFade);
 
	return drawable;
}



void LiveTextLayer::initDrawable(GLDrawable *drawable, bool isFirstDrawable)
{
	LiveLayer::initDrawable(drawable, isFirstDrawable);

}

void LiveTextLayer::setText(const QString& text)
{
// 	qDebug() << "LiveTextLayer::setText(): text:"<<text;
	TextVideoSource *textSource = m_secondarySourceActive ? m_textSource : m_secondaryTextSource; 
	//qDebug() << "LiveTextLayer::setText: "<<text<<", secondary flag:"<<m_secondarySourceActive<<", using source: "<<textSource;
	
	textSource->lockUpdates(true);
	
	textSource->setHtml(text);
	
// 	qDebug() << "LiveTextLayer::setText(): changeFontSize(40)";
	
	// TODO make font size configurable
	textSource->changeFontSize(40);
	
	QSize size = textSource->findNaturalSize();
	
// 	qDebug() << "LiveTextLayer::setText(): natural size: "<<size;
	textSource->setTextWidth(size.width());

	m_props["text"] = text;
	
	setInstanceName(text);
	
// 	qDebug() << "LiveTextLayer::setText(): updating alignment";
// 	setAlignment(alignment()); // force recalc of layout

	textSource->lockUpdates(false);
	
	
	// We only set the drawable visible that we want to be visible - inotherwords, we ignore the secondary ("other") drawable (the one we want hidden)
	// because the sig/slot connection in createDrawable() handles the visibility inversion for us.
	QList<GLDrawable*> drawables = m_secondarySourceActive ? m_drawables.values() : m_secondaryDrawables.values();
	foreach(GLDrawable *item, drawables)
	{
		GLVideoDrawable *videoDrawable = dynamic_cast<GLVideoDrawable*>(item);
		if(videoDrawable)
			videoDrawable->setVisible(isVisible(),true); // true = wait for next frame before becoming visible
	}
	
	m_secondarySourceActive = !m_secondarySourceActive;
}


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
			m_secondaryTextSource->setOutlinePen(QPen(color));
		}
		else
		if(key == "fillColor")
		{
			m_textSource->setFillBrush(QBrush(color));
			m_secondaryTextSource->setFillBrush(QBrush(color));
		}
		else
		if(key == "shadowColor")
		{
			m_textSource->setShadowBrush(QBrush(color));
			m_secondaryTextSource->setShadowBrush(QBrush(color));
		}
	}
	else
	{
		const char *keyStr = qPrintable(key);
		if(m_textSource->metaObject()->indexOfProperty(keyStr)>=0)
		{
			m_textSource->setProperty(keyStr, value);
			m_secondaryTextSource->setProperty(keyStr, value);
		}
	}

	LiveLayer::setLayerProperty(key,value);
}

QWidget * LiveTextLayer::createLayerPropertyEditors()
{
	QWidget * base = new QWidget();
	QVBoxLayout *blay = new QVBoxLayout(base);
	blay->setContentsMargins(0,0,0,0);

	ExpandableWidget *groupContent = new ExpandableWidget("Text",base);
	blay->addWidget(groupContent);

	QWidget *groupContentContainer = new QWidget;
	QFormLayout *formLayout = new QFormLayout(groupContentContainer);
	formLayout->setContentsMargins(3,3,3,3);

	groupContent->setWidget(groupContentContainer);

	formLayout->addRow(tr("&Text:"), generatePropertyEditor(this, "text", SLOT(setText(const QString&))));

	groupContent->setExpandedIfNoDefault(true);

// 	/////////////////////////////////////////
// 
// 	ExpandableWidget *groupDisplay = new ExpandableWidget("Text Display",base);
// 	blay->addWidget(groupDisplay);
// 
// 	QWidget *groupDisplayContainer = new QWidget;
// 	QFormLayout *dispLayout = new QFormLayout(groupDisplayContainer);
// 	dispLayout->setContentsMargins(3,3,3,3);
// 
// 	groupDisplay->setWidget(groupDisplayContainer);
// 	
// 	
// 	QWidget *boxBase;
// 	QHBoxLayout *hbox;
// 	
// 	{
// 		PropertyEditorOptions opts;
// 		
// 		boxBase = new QWidget();
// 		hbox = new QHBoxLayout(boxBase);
// 		
// 		hbox->addWidget(generatePropertyEditor(m_textSource, "outlineEnabled", SLOT(setOutlineEnabled(bool))));
// 		
// 		opts.value = m_textSource->outlinePen().widthF();
// 		opts.suffix = " px";
// 		opts.noSlider = true;
// 		opts.min = 0.1;
// 		opts.max = 10.;
// 		opts.defaultValue = 1.5;
// 		hbox->addWidget(generatePropertyEditor(m_textSource, "-", SLOT(setOutlineWidth(double)), opts));
// 		
// 		QtColorPicker * colorPicker = new QtColorPicker;
// 		colorPicker->setStandardColors();
// 		colorPicker->setCurrentColor(m_textSource->fillBrush().color());
// 		connect(colorPicker, SIGNAL(colorChanged(const QColor &)), m_textSource, SLOT(setOutlineColor(const QColor &)));
// 		hbox->addWidget(colorPicker);
// 		
// 		dispLayout->addRow(tr("&Outline:"), boxBase);
// 	}
// 	
// 	{
// 		boxBase = new QWidget();
// 		hbox = new QHBoxLayout(boxBase);
// 		
// 		hbox->addWidget(generatePropertyEditor(m_textSource, "fillEnabled", SLOT(setFillEnabled(bool))));
// 		
// 		QtColorPicker * fillColorPicker = new QtColorPicker;
// 		fillColorPicker->setStandardColors();
// 		fillColorPicker->setCurrentColor(m_textSource->fillBrush().color());
// 		connect(fillColorPicker, SIGNAL(colorChanged(const QColor &)), m_textSource, SLOT(setFillBrush(const QColor &)));
// 		hbox->addWidget(fillColorPicker);
// 		
// 		dispLayout->addRow(tr("&Fill Color:"), boxBase);
// 	}
// 	
// 	{
// 		PropertyEditorOptions opts;
// 		
// 		boxBase = new QWidget();
// 		hbox = new QHBoxLayout(boxBase);
// 		
// 		hbox->addWidget(generatePropertyEditor(m_textSource, "shadowEnabled", SLOT(setOutlineEnabled(bool))));
// 		
// 		opts.suffix = " px";
// 		opts.noSlider = true;
// 		opts.min = 0.1;
// 		opts.max = 50.;
// 		opts.defaultValue = 16.;
// 		hbox->addWidget(generatePropertyEditor(m_textSource, "shadowBlurRadius", SLOT(setShadowBlurRadius(double)), opts));
// 		
// 		QtColorPicker * colorPicker = new QtColorPicker;
// 		colorPicker->setStandardColors();
// 		colorPicker->setCurrentColor(m_textSource->shadowBrush().color());
// 		connect(colorPicker, SIGNAL(colorChanged(const QColor &)), m_textSource, SLOT(setShadowColor(const QColor &)));
// 		hbox->addWidget(colorPicker);
// 		
// 		dispLayout->addRow(tr("&Shadow:"), boxBase);
// 	}
// 	
// 	PropertyEditorOptions opts;
// 	opts.suffix = " px";
// 	opts.noSlider = true;
// 	opts.min = -50.;
// 	opts.max = 50.;
// 	opts.defaultValue = 5.;
// 	dispLayout->addRow(tr("&Offset X:"), generatePropertyEditor(m_textSource, "shadowOffsetX", SLOT(setShadowOffsetX(double)), opts));
// 	dispLayout->addRow(tr("&Offset Y:"), generatePropertyEditor(m_textSource, "shadowOffsetY", SLOT(setShadowOffsetY(double)), opts));
// 
// 	groupDisplay->setExpandedIfNoDefault(true);
// 
// 

	/////////////////////////////////////////

	QWidget *basics =  LiveLayer::createLayerPropertyEditors();
	blay->addWidget(basics);

	return base;
}

///////////////////////
