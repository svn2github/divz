
#include "LiveLayer.h"

#include "../glvidtex/GLDrawable.h"
#include "../glvidtex/GLWidget.h"


///////////////////////

LiveLayer::LiveLayer(QObject *parent)
	: QObject(parent)
{
}

LiveLayer::~LiveLayer()
{}

GLDrawable* LiveLayer::drawable(GLWidget *widget)
{
	if(m_drawables.contains(widget))
	{
// 		qDebug() << "LiveLayer::drawable: widget:"<<widget<<", cache hit";
		return m_drawables[widget];
	}
	else
	{
		GLDrawable *drawable = createDrawable(widget);
		if(m_drawables.isEmpty())
		{
// 			qDebug() << "LiveLayer::drawable: widget:"<<widget<<", cache miss, first drawable";
			initDrawable(drawable);
		}
		else
		{
// 			qDebug() << "LiveLayer::drawable: widget:"<<widget<<", cache miss, copy from first";
			initDrawable(drawable, m_drawables[m_drawables.keys().first()]);
		}
		
		m_drawables[widget] = drawable;
		
		if(widget->property("isEditorWidget").toInt())
			drawable->show();
		else
			connect(this, SIGNAL(isVisible(bool)), drawable, SLOT(setVisible(bool)));
		
		return drawable;
	}
}

void LiveLayer::setIsVisible(bool flag)
{
// 	qDebug() << "LiveLayer::setVisible";
// 	foreach(GLWidget *widget, m_drawables.keys())
// 		m_drawables[widget]->setVisible(flag);
	
	emit isVisible(flag);
}

void LiveLayer::changeInstanceName(const QString& name)
{
	m_instanceName = name;
	//emit instanceNameChanged(name);
}

GLDrawable *LiveLayer::createDrawable(GLWidget */*widget*/)
{
 	qDebug() << "LiveLayer::createDrawable: Nothing created.";
	return 0;
}

void LiveLayer::initDrawable(GLDrawable *drawable, GLDrawable *copyFrom)
{
// 	qDebug() << "LiveLayer::initDrawable: drawable:"<<drawable<<", copyFrom:"<<copyFrom;
	if(copyFrom)
	{
		drawable->setRect(copyFrom->rect());
		drawable->setZIndex(copyFrom->zIndex());
		drawable->setOpacity(copyFrom->opacity());
	}
}
