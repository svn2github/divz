#ifndef GLSceneGroupType_H
#define GLSceneGroupType_H

#include <QtGui>

class GLSceneType : public QObject
{
	Q_OBJECT
public:
	GLSceneType(QObject *parent=0);
	
	virtual int id();
	
private:
	int m_id;
};

class GLSceneGroupType : public QObject
{
	Q_OBJECT
public:
	GLSceneGroupType(QObject *parent=0);
	
	virtual int id();
	
private:
	int m_id;

};


#endif
