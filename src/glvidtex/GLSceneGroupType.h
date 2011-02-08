#ifndef GLSceneGroupType_H
#define GLSceneGroupType_H

#include <QtGui>

class GLDrawable;
class GLScene;
class GLSceneGroup;
class DirectorWindow;

class GLSceneType : public QObject
{
	Q_OBJECT
public:
	GLSceneType(QObject *parent=0);
	
	class AuditError
	{
		AuditError();
		
		GLDrawable *item;
		QString error;
	};
	
	virtual int id();
	virtual QString title();
	virtual QString description();
	virtual QList<AuditError> auditTemplate(GLScene*);

	virtual GLScene *generateScene(GLScene *sceneTemplate);
	
	virtual QList<QWidget*> createEditorWidgets(GLScene*, DirectorWindow *director);

public slots:
	virtual void setParam(QString itemName, QVariant value);
	virtual void setParam(QString itemName, QString value);
	virtual void setParam(QString itemName, QVariantList value);
	virtual void setParams(QVariantMap map);
	
protected:
	int m_id;
	
	QVariantMap m_params;
};

class GLSceneGroupType : public QObject
{
	Q_OBJECT
public:
	GLSceneGroupType(QObject *parent=0);
	
	class AuditError
	{
		AuditError();
		
		GLScene *scene;
		GLDrawable *item;
		QString error;
	};
	
	virtual int id();
	virtual QString title();
	virtual QString description();
	virtual QList<AuditError> auditTemplate(GLSceneGroup*);
	
	virtual GLSceneGroup *generateGroup(GLSceneGroup *groupTemplate);
	
	/// TODO - will this conflict with the scene type method of same name in normal usage?
	virtual QList<QWidget*> createEditorWidgets(GLSceneGroup *, GLScene*, DirectorWindow *director);
	
public slots:
	virtual void setParam(QString itemName, QVariant value);
	virtual void setParam(QString itemName, QString value);
	virtual void setParam(QString itemName, QVariantList value);
	virtual void setParams(QVariantMap map);
	
protected:
	int m_id;
	
	QVariantMap m_params;
};


class GLSceneTypeFactory
{
	GLSceneTypeFactory();
public:
	static GLSceneType *lookup(int id);
	static QList<GLSceneType*> list();
	
	static void addType(GLSceneType*);
	static void removeType(GLSceneType*);

private:
	QList<GLSceneType*> m_list;
	QHash<int,GLSceneType*> m_lookup;

	static GLSceneTypeFactory *d();
	static GLSceneTypeFactory *m_inst;
};

class GLSceneGroupTypeFactory
{
	GLSceneGroupTypeFactory();
public:
	static GLSceneGroupType *lookup(int id);
	static QList<GLSceneGroupType*> list();
	
	static void addType(GLSceneGroupType*);
	static void removeType(GLSceneGroupType*);

private:
	QList<GLSceneGroupType*> m_list;
	QHash<int,GLSceneGroupType*> m_lookup;
	
	static GLSceneGroupTypeFactory *d();
	static GLSceneGroupTypeFactory *m_inst;

};

#endif
