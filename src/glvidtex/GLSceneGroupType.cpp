
#include "GLSceneGroupType.h"


/// GLSceneType

GLSceneType::GLSceneType(QObject *parent)
	: QObject(parent)
	, m_id("")
{
	
}

QString GLSceneType::id() const
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

QList<GLSceneType::ParameterInfo> GLSceneType::parameters()
{
	return QList<GLSceneType::ParameterInfo>();
}

QList<GLSceneType::FieldInfo> GLSceneType::fields()
{
	return QList<GLSceneType::FieldInfo>();
}

GLSceneType::AuditError::AuditError()
	: item(0)
{}

QList<GLSceneType::AuditError> GLSceneType::auditTemplate(GLScene*)
{
	return QList<GLSceneType::AuditError>();
}

GLScene *GLSceneType::generateScene(GLScene */*sceneTemplate*/)
{
	return 0;
}

GLSceneType *GLSceneType::attachToScene(GLScene */*sceneTemplate*/)
{
	return 0;
}
	
QList<QWidget*> GLSceneType::createEditorWidgets(GLScene*, DirectorWindow */*director*/)
{
	return QList<QWidget*>();
}


GLSceneType *GLSceneType::fromByteArray(GLScene */*sceneTemplate*/, QByteArray /*ba*/)
{
	return 0;
}

QByteArray GLSceneType::toByteArray()
{
	return QByteArray();
}

void GLSceneType::setParam(QString param, QString value)
{
	m_params[param] = value;
}

void GLSceneType::setParam(QString param, QVariant value)
{
	m_params[param] = value;
}

void GLSceneType::setParam(QString param, QVariantList value)
{
	m_params[param] = value;
}

void GLSceneType::setParams(QVariantMap map)
{
	foreach(QString param, map.keys())
		m_params[param] = map.value(param);
}

void GLSceneType::setLiveStatus(bool flag)
{
	m_liveStatus = flag;
}

/// GLSceneGroupType

GLSceneGroupType::GLSceneGroupType(QObject *parent)
	: QObject(parent)
	, m_id("")
{
	
}

QString GLSceneGroupType::id() const
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

QList<GLSceneType::ParameterInfo> GLSceneGroupType::parameters()
{
	return QList<GLSceneType::ParameterInfo>();
}

QList<GLSceneType*> GLSceneGroupType::sceneTypes()
{
	return QList<GLSceneType*>();
}

GLSceneGroup *GLSceneGroupType::generateGroup(GLSceneGroup */*sceneTemplate*/)
{
	return 0;
}

GLSceneGroupType *GLSceneGroupType::attachToGroup (GLSceneGroup */*sceneTemplate*/)
{
	return 0;
}
	
QList<QWidget*> GLSceneGroupType::createEditorWidgets(GLSceneGroup*, GLScene*, DirectorWindow */*director*/)
{
	return QList<QWidget*>();
}

GLSceneGroupType *GLSceneGroupType::fromByteArray(GLSceneGroup */*sceneTemplate*/, QByteArray /*ba*/)
{
	return 0;
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

/// GLSceneTypeFactory
GLSceneTypeFactory *GLSceneTypeFactory::m_inst = 0;

GLSceneTypeFactory::GLSceneTypeFactory()
{
}

GLSceneTypeFactory *GLSceneTypeFactory::d()
{
	if(!m_inst)
		m_inst = new GLSceneTypeFactory();
	return m_inst;
}

GLSceneType *GLSceneTypeFactory::lookup(QString id)
{
	return d()->m_lookup[id];
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

void GLSceneTypeFactory::removeType(GLSceneType *type)
{
	if(!type)
		return;
	d()->m_list.removeAll(type);
	d()->m_lookup[type->id()] = 0;
}


/// GLSceneGroupTypeFactory
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

GLSceneGroupType *GLSceneGroupTypeFactory::lookup(QString id)
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

void GLSceneGroupTypeFactory::removeType(GLSceneGroupType *type)
{
	if(!type)
		return;
	d()->m_list.removeAll(type);
	d()->m_lookup[type->id()] = 0;
}

