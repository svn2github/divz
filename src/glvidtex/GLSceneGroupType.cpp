
#include "GLSceneGroupType.h"
#include "GLSceneGroup.h"
#include "GLDrawable.h"
#include "GLSceneTypes.h"


//============================================================================
//	GLSceneType 

GLSceneType::GLSceneType(QObject *parent)
	: QObject(parent)
	, m_id("")
	, m_factoryFresh(false)
{
	
}

QString GLSceneType::id() 
{
	return m_id;
}

QString GLSceneType::title()
{
	return "Undefined";
}

QString GLSceneType::description()
{
	return "Undefined Scene Type";
}

QList<GLSceneType::AuditError> GLSceneType::auditTemplate(GLScene *scene)
{
	AuditErrorList list;
	if(!scene)
	{
		list << AuditError("Invalid scene pointer");
	}
	else
	{
		GLDrawableList drawables = scene->drawableList();
		QHash<QString,GLDrawable*> hash;
		foreach(GLDrawable *gld, drawables)
			hash[gld->itemName().toLower()] = gld;
			
		foreach(FieldInfo info, m_fieldInfoList)
		{
			if(hash.contains(info.name.toLower()))
			{
				QString type = info.expectedType;
				if(!type.startsWith("GL"))
					type = "GL" + type;
				if(!type.endsWith("Drawable"))
					type += "Drawable";
					
				QString className = QString(hash[info.name]->metaObject()->className());
				if(className != type)
				{
					list << AuditError(QString("Item '%1' has wrong type - expected '%1', found '%2'").arg(info.name).arg(type).arg(className), 0, info, !info.required);
				}
			}
			else
			{
				list << AuditError(QString("Cannot find field '%1'").arg(info.name), 0, info, !info.required);
			}
		}
	}
	
	return list;
}

bool GLSceneType::applyFieldData(QString field)
{
	if(!scene())
	{
		qDebug() << "GLSceneType::applyFieldData("<<field<<"): No scene bound, nothing done.";
		return false;
	}
	else
	{
		GLDrawableList drawables = scene()->drawableList();
		QHash<QString,GLDrawable*> hash;
		foreach(GLDrawable *gld, drawables)
			hash[gld->itemName().toLower()] = gld;
			
		if(!field.isEmpty())
		{
			GLDrawable *gld = hash[field.toLower()];
			if(gld)
			{
				const char *propName = gld->metaObject()->userProperty().name();
				gld->setProperty(propName, m_fields[field]);
				//qDebug() << "GLSceneType::applyFieldData("<<field<<"): Set field:"<<field<<", property:"<<propName<<", data:"<<m_fields[field];
				return true;
			}
			else
			{
				qDebug() << "GLSceneType::applyFieldData("<<field<<"): Unable to find requested field:"<<field;
				return false;
			}
		}
		
		bool ok = true;
		foreach(FieldInfo info, m_fieldInfoList)
		{
			GLDrawable *gld = hash[info.name.toLower()];
			if(gld)
			{
				const char *propName = gld->metaObject()->userProperty().name();
				gld->setProperty(propName, m_fields[field]);
				//qDebug() << "GLSceneType::applyFieldData("<<field<<"): Set field:"<<info.name<<", property:"<<propName<<", data:"<<m_fields[field];
			}
			else
			{
				if(info.required)
				{
					qDebug() << "GLSceneType::applyFieldData("<<field<<"): Unable to find required field:"<<info.name;
					ok = false;
				}
			}
		}
		
		return ok;
	}
}

void GLSceneType::setField(QString field, QVariant data)
{
	m_fields[field] = data;
	qDebug() << "GLSceneType::setField(): field:"<<field<<", data:"<<data; 
	applyFieldData(field);
}

GLScene *GLSceneType::generateScene(GLScene *sceneTemplate)
{
	if(!sceneTemplate)
		return 0;
		
	GLScene *scene = sceneTemplate->clone();
	GLSceneType *type = GLSceneTypeFactory::newInstance(id());
	
	if(!type->attachToScene(scene))
	{
		delete scene;
		return 0;
	}
	
	return scene;
}

bool GLSceneType::attachToScene(GLScene *scene)
{
	if(!scene)
	{
		qDebug() << "GLSceneType::attachToScene(): Given scene pointer is NULL";
		return false;
	}
	
	GLSceneType *type = this;
	
	if(type->m_factoryFresh)
	{
		type = GLSceneTypeFactory::newInstance(type->id());
		type->m_fields = m_fields;
		type->m_params = m_params;
	}
	
	type->m_scene = scene;
	scene->setSceneType(type);
	
	return type->applyFieldData();
}
	
QList<QWidget*> GLSceneType::createEditorWidgets(GLScene*, DirectorWindow */*director*/)
{
	return QList<QWidget*>();
}


QByteArray GLSceneType::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);

	QVariantMap map;
	map["id"]	= id();
	map["fields"]	= m_fields;
	map["params"]	= m_params;
	
	stream << map;

	return array;

}

void GLSceneType::setParam(QString param, QString value)
{
	setParam(param,QVariant(value));
}

void GLSceneType::setParam(QString param, QVariant value)
{
	m_params[param] = value;
}

void GLSceneType::setParam(QString param, QVariantList value)
{
	setParam(param,QVariant(value));
}

void GLSceneType::setParams(QVariantMap map)
{
	foreach(QString param, map.keys())
		setParam(param,map.value(param));
}

void GLSceneType::setLiveStatus(bool flag)
{
	m_liveStatus = flag;
}

//============================================================================
//	GLSceneGroupType 

GLSceneGroupType::GLSceneGroupType(QObject *parent)
	: QObject(parent)
	, m_id("")
{
	
}

QString GLSceneGroupType::id() 
{
	return m_id; 
}

QString GLSceneGroupType::title()
{
	return "Undefined";
}

QString GLSceneGroupType::description()
{
	return "Undefined Scene Type";
}

GLSceneGroup *GLSceneGroupType::generateGroup(GLSceneGroup */*sceneTemplate*/)
{
	return 0;
}

GLSceneGroupType *GLSceneGroupType::attachToGroup (GLSceneGroup */*sceneTemplate*/, QByteArray)
{
	return 0;
}
	
QList<QWidget*> GLSceneGroupType::createEditorWidgets(GLSceneGroup*, GLScene*, DirectorWindow */*director*/)
{
	return QList<QWidget*>();
}

QByteArray GLSceneGroupType::toByteArray()
{
	return QByteArray();
}

void GLSceneGroupType::setParam(QString param, QString value)
{
	m_params[param] = value;
}

void GLSceneGroupType::setParam(QString param, QVariant value)
{
	m_params[param] = value;
}

void GLSceneGroupType::setParam(QString param, QVariantList value)
{
	m_params[param] = value;
}

void GLSceneGroupType::setParams(QVariantMap map)
{
	foreach(QString param, map.keys())
		m_params[param] = map.value(param);
}

void GLSceneGroupType::setLiveStatus(bool flag)
{
	m_liveStatus = flag;
}

//============================================================================
//	GLSceneTypeFactory 
 
GLSceneTypeFactory *GLSceneTypeFactory::m_inst = 0;

GLSceneTypeFactory::GLSceneTypeFactory()
{
	#define ADD_CLASS(x) addType(new x());
	
	ADD_CLASS(GLSceneTypeCurrentWeather);
	
	#undef ADD_CLASS
}

GLSceneTypeFactory *GLSceneTypeFactory::d()
{
	if(!m_inst)
		m_inst = new GLSceneTypeFactory();
	return m_inst;
}

GLSceneType *GLSceneTypeFactory::lookup(QString id)
{
	GLSceneType *t = d()->m_lookup[id];
	t->m_factoryFresh = true;
	return t;
}

GLSceneType *GLSceneTypeFactory::newInstance(QString id)
{
	GLSceneType *inst = lookup(id);
	if(!inst)
		 return 0;
	
	QString className = QString(inst->metaObject()->className());
	
	GLSceneType *newInst = 0;
	#define IF_CLASS(x) if(className == #x) { newInst = new x(); }
	
	IF_CLASS(GLSceneTypeCurrentWeather);
	
	#undef IF_CLASS
	
	return newInst;
}

GLSceneType *GLSceneTypeFactory::fromByteArray(QByteArray array, GLScene *sceneToAttach)
{
	if(!sceneToAttach)
		return 0;
		
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	QString id = map["id"].toString();
	
	GLSceneType *type = newInstance(id);
	if(!type)
		return 0;
	
	type->m_params = map["params"].toMap();
	type->m_fields = map["fields"].toMap();
	
	type->attachToScene(sceneToAttach);
	
	return type;
}

QList<GLSceneType*> GLSceneTypeFactory::list()
{
	return d()->m_list;
}

void GLSceneTypeFactory::addType(GLSceneType *type)
{
	if(!type)
		return;
	d()->m_list.append(type);
	d()->m_lookup[type->id()] = type;
}

// void GLSceneTypeFactory::removeType(GLSceneType *type)
// {
// 	if(!type)
// 		return;
// 	d()->m_list.removeAll(type);
// 	d()->m_lookup[type->id()] = 0;
// }


//============================================================================
//	GLSceneGroupTypeFactory
 
GLSceneGroupTypeFactory *GLSceneGroupTypeFactory::m_inst = 0;

GLSceneGroupTypeFactory::GLSceneGroupTypeFactory()
{
}

GLSceneGroupTypeFactory *GLSceneGroupTypeFactory::d()
{
	if(!m_inst)
		m_inst = new GLSceneGroupTypeFactory();
	return m_inst;
}

GLSceneGroupType *GLSceneGroupTypeFactory::newInstance(QString id)
{
	return d()->m_lookup[id];
}

QList<GLSceneGroupType*> GLSceneGroupTypeFactory::list()
{
	return d()->m_list;
}

void GLSceneGroupTypeFactory::addType(GLSceneGroupType *type)
{
	if(!type)
		return;
	d()->m_list.append(type);
	d()->m_lookup[type->id()] = type;
}

// void GLSceneGroupTypeFactory::removeType(GLSceneGroupType *type)
// {
// 	if(!type)
// 		return;
// 	d()->m_list.removeAll(type);
// 	d()->m_lookup[type->id()] = 0;
// }

