
#include "LiveScene.h"
#include "LiveLayer.h"
#include "../glvidtex/GLDrawable.h"
#include "../glvidtex/GLWidget.h"


///////////////////////
LiveScene::LiveScene(QObject *parent)
	: QObject(parent)
	, m_glWidget(0)
{}

LiveScene::~LiveScene()
{}

QList<LayerControlWidget*> LiveScene::controlWidgets()
{
	QList<LayerControlWidget*> list;
// 	foreach(LiveLayer *layer, m_layers)
// 		if(layer->controlWidget())
// 			list.append(layer->controlWidget());
	return list;
}

void LiveScene::addLayer(LiveLayer *layer)
{
	if(!layer)
		return;
	if(!m_layers.contains(layer))
	{
		m_layers.append(layer);
		if(m_glWidget)
			m_glWidget->addDrawable(layer->drawable(m_glWidget));

		emit layerAdded(layer);
	}
}

void LiveScene::removeLayer(LiveLayer *layer)
{
	if(!layer)
		return;
	if(m_layers.contains(layer))
	{
		m_layers.removeAll(layer);
		if(m_glWidget)
			m_glWidget->removeDrawable(layer->drawable(m_glWidget));

		emit layerRemoved(layer);
	}
}

void LiveScene::attachGLWidget(GLWidget *glw)
{
	if(m_glWidget)
		detachGLWidget();

	m_glWidget = glw;

	foreach(LiveLayer *layer, m_layers)
		m_glWidget->addDrawable(layer->drawable(m_glWidget));
}

void LiveScene::detachGLWidget(bool hideFirst)
{
	if(!m_glWidget)
		return;

	if(hideFirst)
	{
		bool foundVisible = false;
		foreach(LiveLayer *layer, m_layers)
		{
			if(layer->drawable(m_glWidget)->isVisible())
			{
				foundVisible = true;
				connect(layer->drawable(m_glWidget), SIGNAL(isVisible(bool)), this, SLOT(layerVisibilityChanged(bool)));
				layer->drawable(m_glWidget)->hide();
			}
		}

		if(foundVisible)
			return;
	}

	foreach(LiveLayer *layer, m_layers)
		m_glWidget->removeDrawable(layer->drawable(m_glWidget));

	m_glWidget = 0;
}

void LiveScene::layerVisibilityChanged(bool flag)
{
	if(flag)
		return;
	
	if(!m_glWidget)
		return;
		
	bool foundVisible = false;
	foreach(LiveLayer *layer, m_layers)
		if(layer->drawable(m_glWidget)->isVisible())
		{
			foundVisible = true;
			break;
		}

	if(!foundVisible)
		detachGLWidget(false);

}
