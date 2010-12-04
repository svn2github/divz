#include "GLSceneGroup.h"

#include "GLDrawable.h"
#include "GLWidget.h"

#include "MetaObjectUtil.h"

#include "GLImageDrawable.h"
#include "GLTextDrawable.h"
#include "GLVideoLoopDrawable.h"
#include "GLVideoInputDrawable.h"
#include "GLVideoFileDrawable.h"
#include "GLVideoReceiverDrawable.h"
#include "GLVideoMjpegDrawable.h"


// ****************************
// GLSceneLayoutItem
// ****************************

GLSceneLayoutItem::GLSceneLayoutItem(int id, const QVariantMap& props)
	: m_drawableId(id)
	, m_props(props)
{
}
GLSceneLayoutItem::GLSceneLayoutItem(GLDrawable *drawable, const QVariantMap& props)
	: m_drawableId(drawable ? drawable->id() : -1)
	, m_props(props) 
{
}
GLSceneLayoutItem::GLSceneLayoutItem(QByteArray& ba)
	: m_drawableId(-1)
{
	fromByteArray(ba);
}

GLSceneLayoutItem::~GLSceneLayoutItem()
{
}
	
QByteArray GLSceneLayoutItem::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	
	QVariantMap map;
	map["drawableId"]	= m_drawableId;
	map["props"] 		= m_props;
	
	stream << map;
	
	return array;
}

void GLSceneLayoutItem::fromByteArray(QByteArray& array)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	if(map.isEmpty())
		return;
	
	m_drawableId	= map["drawableId"].toInt();
	m_props		= map["props"].toMap();
}
	
void GLSceneLayoutItem::setDrawable(GLDrawable* d)
{
	setDrawableId(d ? d->id() : -1);
}

void GLSceneLayoutItem::setDrawableId(int d)
{
	m_drawableId = d;
}

void GLSceneLayoutItem::setPropertyHash(const QVariantMap& props)
{
	m_props = props;
}
	

// ****************************
// GLSceneLayoutItem
// ****************************

GLSceneLayout::GLSceneLayout(GLScene *s)
	: m_scene(s)
{
}

GLSceneLayout::GLSceneLayout(QByteArray& ba, GLScene* s)
	: m_scene(s)
{
	fromByteArray(ba);
}

GLSceneLayout::~GLSceneLayout()
{
}
	
QByteArray GLSceneLayout::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	
	QVariantMap map;
	map["layoutId"]		= layoutId();
	map["layoutName"] 	= m_layoutName;
	
	QVariantList items;
	foreach(GLSceneLayoutItem *item, m_items)
		items << item->toByteArray();
	
	map["items"] = items;
	
	stream << map;
	
	return array;
}

void GLSceneLayout::fromByteArray(QByteArray& array)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	if(map.isEmpty())
		return;
	
	m_layoutId	= map["layoutId"].toInt();
	m_layoutName	= map["layoutName"].toString();
	
	m_items.clear();
	QVariantList items = map["items"].toList();
	foreach(QVariant var, items)
	{
		QByteArray data = var.toByteArray();
		m_items << new GLSceneLayoutItem(data);
	}
}
	
void GLSceneLayout::setLayoutName(const QString& name)
{
	emit layoutNameChanged(name);
}

/*private:
	GLScene *m_scene;
	int m_layoutId;
	QString m_layoutName;
	QList<GLSceneLayoutItem*> m_items;*/


// ****************************
// GLScene
// ****************************

GLScene::GLScene(QObject *parent)
	: QObject(parent)
	, m_sceneId(-1)
	, m_glWidget(0)
{

}

GLScene::GLScene(QByteArray& ba, QObject *parent)
	: QObject(parent)
	, m_sceneId(-1)
	, m_glWidget(0)
{
	fromByteArray(ba);
}

GLScene::~GLScene() {}
	
int GLScene::sceneId()
{
	if(m_sceneId<0)
	{
		QSettings s;
		m_sceneId = s.value("glscene/scene-id-counter",100).toInt() + 1;
		s.setValue("glscene/scene-id-counter", m_sceneId);
	}
	return m_sceneId;
}

QByteArray GLScene::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	
	QVariantMap map;
	map["sceneId"]		= sceneId();
	map["sceneName"] 	= m_sceneName;
	
	QVariantList layouts;
	foreach(GLSceneLayout *layout, m_layouts)
		layouts << layout->toByteArray();
	
	map["layouts"] = layouts;
	
	QVariantList drawables;
	foreach(GLDrawable *d, m_itemList)
	{
		QVariantMap drawableData;
		drawableData["class"] = d->metaObject()->className();
		drawableData["bytes"] = d->toByteArray();
		drawables << drawableData;
	}
	
	map["drawables"] = drawables;
	
	stream << map;
	
	return array;
}

void GLScene::fromByteArray(QByteArray& array)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	if(map.isEmpty())
		return;
	
	m_sceneId	= map["sceneId"].toInt();
	m_sceneName	= map["sceneName"].toString();
	
	m_layouts.clear();
	QVariantList layouts = map["layouts"].toList();
	foreach(QVariant var, layouts)
	{
		QByteArray data = var.toByteArray();
		addLayout(new GLSceneLayout(data, this));
	}
		
	m_itemList.clear();
	QVariantList drawables = map["drawables"].toList();
	foreach(QVariant var, drawables)
	{
		QVariantMap meta = var.toMap();
		QString className = meta["class"].toString();
// 		const QMetaObject *metaObject = MetaObjectUtil::metaObjectForClass(className);
// 		if(!metaObject)
// 		{
// 			qDebug() << "Error: GLScene::fromByteArray(): Unable to create drawable of type:"<<className;
// 			continue;
// 		}
// 		
// 		//qDebug() << "LiveScene::fromByteArray(): Debug: metaObject classname: "<<metaObject->className()<<", asked for:"<<className;
// 		
// 		QObject *instance = metaObject->newInstance(Q_ARG(QObject*,0));
// 		if(!instance)
// 		{
// 			qDebug() << "Error: GLScene::fromByteArray(): Creation of "<<className<<" failed.";
// 			continue;
// 		}

		GLDrawable *drawable = 0;
		
		#define IF_CLASS(x) if(className == #x) { drawable = new x(); }
		
		IF_CLASS(GLImageDrawable);
		IF_CLASS(GLTextDrawable);
		IF_CLASS(GLVideoFileDrawable);
		IF_CLASS(GLVideoInputDrawable);
		IF_CLASS(GLVideoLoopDrawable);
		IF_CLASS(GLVideoReceiverDrawable);
		IF_CLASS(GLVideoMjpegDrawable);
		
		#undef IF_CLASS
		
		
		//GLDrawable *drawable = dynamic_cast<GLDrawable*>(instance);
		if(!drawable)
		{
//			qDebug() << "Error: GLScene::fromByteArray(): Object created for type "<<className<<" does not inherit from GLDrawable.";
 			qDebug() << "Error: GLScene::fromByteArray(): Creation of "<<className<<" failed.";
			continue;
		}
		
		QByteArray bytes = meta["bytes"].toByteArray();
		drawable->fromByteArray(bytes);
		
		addDrawable(drawable);
	}
}
	
void GLScene::addDrawable(GLDrawable *d)
{
	if(!d)
		return;
	m_itemList << d;
	m_drawableIdLookup[d->id()] = d;
	emit drawableAdded(d);
	
	if(m_glWidget)
		m_glWidget->addDrawable(d);
}

void GLScene::removeDrawable(GLDrawable *d)
{
	if(!d)
		return;
	m_itemList.removeAll(d);
	m_drawableIdLookup.remove(d->id());
	emit drawableRemoved(d);
	
	if(m_glWidget)
		m_glWidget->removeDrawable(d);
}

GLDrawable * GLScene::lookupDrawable(int id)
{
	return m_drawableIdLookup[id];
}

// This is the 'crossover' method which
// connects the drawables in this scene to an actual display widget.
// Only one widget can be set at a time - if the widget is changed,
// the drawables are removed from the old and added to the new
void GLScene::setGLWidget(GLWidget *glw, int /*zIndexOffset*/)
{
	if(m_glWidget)
		detachGLWidget();
		
	m_glWidget = glw;
	
	foreach(GLDrawable *d, m_itemList)
		m_glWidget->addDrawable(d);
}

void GLScene::detachGLWidget()
{
	if(!m_glWidget)
		return;
		
	foreach(GLDrawable *d, m_itemList)
		m_glWidget->removeDrawable(d);
	
	m_glWidget = 0;
}

// Layouts specify properties of the drawbles in the scene
void GLScene::addLayout(GLSceneLayout *lay)
{
	if(!lay)
		return;
	m_layouts << lay;
	m_layoutIdLookup[lay->layoutId()] = lay;
	emit layoutAdded(lay);
}

void GLScene::removeLayout(GLSceneLayout *lay)
{
	if(!lay)
		return;
	m_layouts.removeAll(lay);
	m_layoutIdLookup.remove(lay->layoutId());
	emit layoutRemoved(lay);
}

GLSceneLayout * GLScene::lookupLayout(int id)
{
	return m_layoutIdLookup[id];
}

void GLScene::setSceneName(const QString& name)
{
	m_sceneName = name;
	emit sceneNameChanged(name);
}

	
/*signals:
	void drawableAdded(GLDrawable*);
	void drawableRemoved(GLDrawable*);
	void layoutAdded(GLSceneLayout*);
	void layoutRemoved(GLSceneLayout*);*/
/*	
protected:
	int m_sceneId;
	QString m_sceneName;
	
	GLDrawableList m_itemList;
	QHash<int,GLDrawable*> m_drawableIdLookup;
	
	QList<GLSceneLayout*> m_layouts;
	QHash<int,GLSceneLayout*> m_layoutIdLookup;
		
	GLWidget *m_widget;*/



// ****************************
// GLSceneGroup
// ****************************

GLSceneGroup::GLSceneGroup(QObject *parent)
	: QObject(parent)
{}

GLSceneGroup::GLSceneGroup(QByteArray& ba, QObject *parent)
	: QObject(parent)
{
	fromByteArray(ba);
}

GLSceneGroup::~GLSceneGroup()
{
}
	
int GLSceneGroup::groupId()
{
	if(m_groupId<0)
	{
		QSettings s;
		m_groupId = s.value("glscenegroup/group-id-counter",100).toInt() + 1;
		s.setValue("glscenegroup/group-id-counter", m_groupId);
	}
	return m_groupId;
}

QByteArray GLSceneGroup::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	
	QVariantMap map;
	map["groupId"]		= groupId();
	map["groupName"] 	= m_groupName;
	
	QVariantList scenes;
	foreach(GLScene *scene, m_scenes)
		scenes << scene->toByteArray();
	
	map["scenes"] = scenes;
	
	stream << map;
	
	return array;
}

void GLSceneGroup::fromByteArray(QByteArray& array)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	if(map.isEmpty())
		return;
	
	m_groupId	= map["groupId"].toInt();
	m_groupName	= map["groupName"].toString();
	
	m_scenes.clear();
	QVariantList scenes = map["scenes"].toList();
	foreach(QVariant var, scenes)
	{
		QByteArray data = var.toByteArray();
		m_scenes << new GLScene(data, this);
	}
}
	
// The core of the scene group is a list of scenes.
// The order is explicit through their index in the QList, though not relevant
// as the order they are played in is specified by the GLSchedule and GLScheduleItems.
// Although the scenes are displayed in order in the 'Director' program
void GLSceneGroup::addScene(GLScene* s)
{
	if(!s)
		return;
	m_scenes << s;
	m_sceneIdLookup[s->sceneId()] = s;
	emit sceneAdded(s);
}

void GLSceneGroup::removeScene(GLScene* s)
{
	if(!s)
		return;
	m_scenes.removeAll(s);
	m_sceneIdLookup.remove(s->sceneId());
	emit sceneRemoved(s);
}

GLScene * GLSceneGroup::lookupScene(int id)
{
	return m_sceneIdLookup[id];
}
	
// Overlay scene, by definition, is a general scene that is to be overlayed on the content of 
// the other scenes in the list. 
void GLSceneGroup::setOverlayScene(GLScene* s)
{
	m_overlayScene = s;
	emit overlaySceneChanged(s);
}

void GLSceneGroup::setGroupName(const QString& name)
{
	m_groupName = name;
	emit groupNameChanged(name);
}

	
// protected:
// 	int m_groupId;
// 	QString m_groupName;
// 	
// 	QList<GLScene*> m_scenes;
// 	QHash<int,GLScene*> m_sceneIdLookup;
// 
// 	GLScene *m_overlayScene;
