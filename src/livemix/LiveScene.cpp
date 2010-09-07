
#include "LiveScene.h"
#include "LiveLayer.h"
#include "../glvidtex/GLDrawable.h"
#include "../glvidtex/GLWidget.h"

QHash<QString, const QMetaObject*> LiveScene::s_metaObjects;

///////////////////////
bool operator==(const LiveScene::KeyFrame& a, const LiveScene::KeyFrame& b) { return a.id == b.id; }

QList<LiveLayer*> LiveScene::KeyFrame::layers()
{
	QList<LiveLayer*> layers;
	foreach(int id, layerProperties.keys())
	{
		layers.append(scene->layerFromId(id));
	}
	
	return layers;
}

QVariantMap LiveScene::KeyFrame::propsForLayer(LiveLayer* layer)
{
	QVariantMap map;
	
	if(layer)
		map = layerProperties[layer->id()];	
	
	return map;
}

QByteArray LiveScene::KeyFrame::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	QVariantMap map;
	
	if(layerProperties.isEmpty())
		return array;
		
	map["frameName"] = frameName;
	map["clockTime"] = clockTime;
	map["playTime"] = playTime;
	map["animParam.length"] = animParam.length;
	map["animParam.type"] = (int)animParam.curve.type();
		
	QVariantList list;
	foreach(int id, layerProperties.keys())
	{
		QVariantMap meta;
		meta["id"] = id;
		meta["map"] = layerProperties[id];
		
		list.append(meta);
	}
	
	map["props"] = list;
	
	stream << map;
	
	return array;
}

void LiveScene::KeyFrame::fromByteArray(QByteArray& array)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	//qDebug() << "LiveScene::fromByteArray(): "<<map;
	if(map.isEmpty())
	{
		qDebug() << "Error: LiveScene::KeyFrame::fromByteArray(): Map is empty, unable to load frame.";
		return;
	}
	
	layerProperties.clear();
	
	frameName = map["frameName"].toString();
	clockTime = map["clockTime"].toTime();
	playTime = map["playTime"].toInt();
	animParam.length = map["animParam.length"].toInt();
	animParam.curve.setType((QEasingCurve::Type)map["animParam.curveType"].toInt());
	
	QVariantList list = map["props"].toList();
	foreach(QVariant var, list)
	{
		QVariantMap meta = var.toMap();
		map.remove("showOnShowLayerId");
		map.remove("hideOnShowLayerId");
		map.remove("aspectRatioMode");
		layerProperties[meta["id"].toInt()] = meta["map"].toMap();
	}
}

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

LiveLayer * LiveScene::layerFromId(int id)
{
	return m_idLookup[id];
}

void LiveScene::addLayer(LiveLayer *layer)
{
	if(!layer)
		return;
	if(!m_layers.contains(layer))
	{
		m_layers.append(layer);
		m_idLookup[layer->id()] = layer;
		foreach(GLWidget *glw, m_glWidgets)
			layer->attachGLWidget(glw);

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
		m_idLookup.remove(layer->id());
		foreach(GLWidget *glw, m_glWidgets)
			layer->detachGLWidget(glw);

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
		layer->attachGLWidget(glw);
}

void LiveScene::detachGLWidget(GLWidget *glw)
{
	if(!glw)
		return;

	//qDebug() << "LiveScene::detachGLWidget(): glw: "<<glw;
	foreach(LiveLayer *layer, m_layers)
		layer->detachGLWidget(glw);

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
		//qDebug() << "LiveScene::fromByteArray(): Debug: metaObject classname: "<<metaObject->className()<<", layer ID:" <<layer->id();
	}
	
	// Basically, let the layers know that loading is complete
	foreach(LiveLayer *layer, m_layers)
		layer->setScene(this);
	
	m_keyFrames.clear();
	QVariantList keys = map["keys"].toList();
	foreach(QVariant key, keys)
	{
		LiveScene::KeyFrame frame(this);
		QByteArray ba = key.toByteArray();
		frame.fromByteArray(ba);
		frame.id = m_keyFrames.size()+1;
		m_keyFrames.append(frame);
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
	
	QVariantList keys;
	foreach(LiveScene::KeyFrame frame, m_keyFrames)
	{
		keys.append(frame.toByteArray());
	}
	map["keys"] = keys;
	
	stream << map;
	
	return array;
}

LiveScene::KeyFrame LiveScene::createKeyFrame()
{
	LiveScene::KeyFrame frame(this);
	
	foreach(LiveLayer *layer, m_layers)
	{
		QVariantMap map = layer->propsToMap();
		// remove props not needed to keyframe
		map.remove("showOnShowLayerId");
		map.remove("hideOnShowLayerId");
		map.remove("aspectRatioMode");
		frame.layerProperties[layer->id()] = map;
	}
	
	return frame;
}

LiveScene::KeyFrame LiveScene::createAndAddKeyFrame()
{
	LiveScene::KeyFrame frame = createKeyFrame();
	addKeyFrame(frame);
	return frame;
}
	
void LiveScene::addKeyFrame(LiveScene::KeyFrame& frame)
{
	frame.id = m_keyFrames.size()+1;
	m_keyFrames.append(frame);
	emit keyFrameAdded(frame);
}

void LiveScene::removeKeyFrame(const LiveScene::KeyFrame& frame)
{
	m_keyFrames.removeAll(frame);
	emit keyFrameRemoved(frame);
}

void LiveScene::removeKeyFrame(int idx)
{
	if(idx < 0 || idx >= m_keyFrames.size())
		return;
		
	removeKeyFrame(m_keyFrames[idx]);
}

void LiveScene::applyKeyFrame(const LiveScene::KeyFrame& frame)
{
	QHash<LiveLayer*,bool> flags;
	foreach(int id, frame.layerProperties.keys())
	{
		LiveLayer * layer = m_idLookup[id];
		if(layer)
		{
			flags[layer] = true;
			// true = apply only if changed
			layer->loadPropsFromMap(frame.layerProperties[id],true);
		}
	}
	
	foreach(LiveLayer *layer, m_layers)
	{
		if(!flags[layer])
			layer->setVisible(false);
	}
}

void LiveScene::setKeyFrameName(int idx, const QString& name)
{
	if(idx < 0 || idx >= m_keyFrames.size())
		return;
	
	LiveScene::KeyFrame frame = m_keyFrames.takeAt(idx);
	frame.frameName = name;
	m_keyFrames.insert(idx,frame);
}

void LiveScene::applyKeyFrame(int idx)
{
	if(idx < 0 || idx >= m_keyFrames.size())
		return;
		
	applyKeyFrame(m_keyFrames[idx]);
}
	
