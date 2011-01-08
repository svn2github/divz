
#include "GLSceneGroupType.h"


GLSceneType::GLSceneType(QObject *parent)
	: QObject(parent)
	, m_id(-1)
{
	
}

int GLSceneType::id()
{
	return m_id; /// TODO 
}

GLSceneGroupType::GLSceneGroupType(QObject *parent)
	: QObject(parent)
	, m_id(-1)
{
	
}

int GLSceneGroupType::id()
{
	return m_id; /// TODO 
}
