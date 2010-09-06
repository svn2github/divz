
#include "LiveScene.h"
#include "LiveLayer.h"
#include "../glvidtex/GLDrawable.h"
#include "../glvidtex/GLWidget.h"

QHash<QString, const QMetaObject*> LiveScene::s_metaObjects;

///////////////////////
LiveScene::LiveScene(QObject *parent)
	: QObject(parent)
{}

LiveScene::~LiveScene()
{
	//qDebug() << "LiveScene::~LiveScene() - start";
	while(!m_glWidgets.isEmpty())
	{
		// Yes, I know detachGLWidget() removes the widget from the list -
		// But, on the *rare* chance that it doesnt, I don't want this
		// list to run forever - so the takeFirst() call guarantees that
		// eventually the list will be empty, terminating the loop./
		detachGLWidget(m_glWidgets.takeFirst());
	}
	
	//qDebug() << "LiveScene::~LiveScene() - end";
	
	qDeleteAll(m_layers);
	m_layers.clear();
}

void LiveScene::addLayer(LiveLayer *layer)
{
	if(!layer)
		return;
	if(!m_layers.contains(layer))
	{
		m_layers.append(layer);
		foreach(GLWidget *glw, m_glWidgets)
			glw->addDrawable(layer->drawable(glw));

		layer->setScene(this);
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
		foreach(GLWidget *glw, m_glWidgets)
			glw->removeDrawable(layer->drawable(glw));

		layer->setScene(0);
		emit layerRemoved(layer);
	}
}

void LiveScene::attachGLWidget(GLWidget *glw)
{
	if(!glw)
		return;
		
	m_glWidgets.append(glw);

	foreach(LiveLayer *layer, m_layers)
		glw->addDrawable(layer->drawable(glw));
}

void LiveScene::detachGLWidget(GLWidget *glw)
{
	if(!glw)
		return;

	qDebug() << "LiveScene::detachGLWidget(): glw: "<<glw;
	foreach(LiveLayer *layer, m_layers)
		glw->removeDrawable(layer->drawable(glw));

	m_glWidgets.removeAll(glw);
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
		
		//qDebug() << "LiveScene::fromByteArray(): Debug: metaObject classname: "<<metaObject->className()<<", asked for:"<<className;
		
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
