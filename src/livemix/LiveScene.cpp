
#include "LiveScene.h"
#include "LiveLayer.h"
#include "../glvidtex/GLDrawable.h"
#include "../glvidtex/GLWidget.h"

QHash<QString, const QMetaObject*> LiveScene::s_metaObjects;

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

void LiveScene::fromByteArray(QByteArray& array)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	//qDebug() << "LiveScene::fromByteArray(): "<<map;
	if(map.isEmpty())
	{
		qDebug() << "Error: LiveScene::fromByteArray(): Map is empty, unable to load scene.";
		return;
	}
	
	qDeleteAll(m_layers);
	m_layers.clear();
	
	QVariantList layers = map["layers"].toList();
	foreach(QVariant layer, layers)
	{
		QVariantMap meta = layer.toMap();
		QString className = meta["class"].toString();
		const QMetaObject *metaObject = LiveScene::metaObjectForClass(className);
		if(!metaObject)
		{
			qDebug() << "Error: LiveScene::fromByteArray(): Unable to create layer of type:"<<className;
			continue;
		}
		
		qDebug() << "LiveScene::fromByteArray(): Debug: metaObject classname: "<<metaObject->className()<<", asked for:"<<className;
		
		QObject *instance = metaObject->newInstance(Q_ARG(QObject*,0));
		if(!instance)
		{
			qDebug() << "Error: LiveScene::fromByteArray(): Creation of "<<className<<" failed.";
			continue;
		}
		
		
		LiveLayer *layer = dynamic_cast<LiveLayer*>(instance);
		if(!layer)
		{
			qDebug() << "Error: LiveScene::fromByteArray(): Object created for type "<<className<<" does not inherit from LiveLayer.";
			continue;
		}
		
		QByteArray bytes = meta["data"].toByteArray();
		layer->fromByteArray(bytes);
		
		addLayer(layer);
	}
}



QByteArray LiveScene::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	QVariantMap map;
	
	QVariantList list;
	if(m_layers.isEmpty())
		return array;
		
	foreach(LiveLayer *layer, m_layers)
	{
		QVariantMap meta;
		meta["class"] = layer->metaObject()->className();
		meta["data"]  = layer->toByteArray();
		
		list.append(meta);
	}
	
	map["layers"] = list;
	
	stream << map;
	
	return array;
}
