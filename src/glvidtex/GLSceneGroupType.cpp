
#include "GLSceneGroupType.h"


/// GLSceneType

GLSceneType::GLSceneType(QObject *parent)
	: QObject(parent)
	, m_id(-1)
{
	
}

int GLSceneType::id()
{
	return m_id; /// TODO 
}

QString GLSceneType::title()
{
	return "Undefined";
}

QString GLSceneType::description()
{
	return "Undefined Scene Type";
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
	
QList<QWidget*> GLSceneType::createEditorWidgets(GLScene*, DirectorWindow */*director*/)
{
	return QList<QWidget*>();
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


/// GLSceneGroupType

GLSceneGroupType::GLSceneGroupType(QObject *parent)
	: QObject(parent)
	, m_id(-1)
{
	
}

int GLSceneGroupType::id()
{
	return m_id; /// TODO 
}

QString GLSceneGroupType::title()
{
	return "Undefined";
}

QString GLSceneGroupType::description()
{
	return "Undefined Scene Type";
}

GLSceneGroupType::AuditError::AuditError()
	: scene(0)
	, item(0)
{}

QList<GLSceneGroupType::AuditError> GLSceneGroupType::auditTemplate(GLSceneGroup*)
{
	return QList<GLSceneGroupType::AuditError>();
}

GLSceneGroup *GLSceneGroupType::generateGroup(GLSceneGroup */*sceneTemplate*/)
{
	return 0;
}
	
QList<QWidget*> GLSceneGroupType::createEditorWidgets(GLSceneGroup*, GLScene*, DirectorWindow */*director*/)
{
	return QList<QWidget*>();
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

GLSceneType *GLSceneTypeFactory::lookup(int id)
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

GLSceneGroupType *GLSceneGroupTypeFactory::lookup(int id)
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

