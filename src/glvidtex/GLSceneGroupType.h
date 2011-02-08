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
	
	class ParameterInfo
	{
		QString name;
		QString description;
		QVariant::Type type;
		bool required;
	};
	
	class FieldInfo
	{
		QString name;
		QString description;
		bool required;
	};
	
	virtual QList<ParameterInfo> parameters();
	virtual QList<FieldInfo> fields();
	
	/** A GLSceneType instance is 'valid' only if its been applied/attached to a scene.
		
		Before it's attached to a scene, all it provides is title/description and a
		list of parameters and expected fields. After being attached to the scene,
		the scene type instance can be configured (set the parameters) and
		can update the attached scene at will (say, from a slot connected to a QTimer.)
		
		To attach to a scene, use attachToScene() or generateScene().
		
		The correct GLSceneType is automatically attached to the scene when the scene
		is loaded if a scene type was attached before saving to disk.
		
		\retval true if this instance is attached to scene
		\retval false if this instance is not attached to a scene.
		
		\sa attachToScene()
		\sa generateScene()
	*/ 
	virtual bool isValidInstance() { return m_scene != NULL; }

	/** Duplicates the \a sceneTemplate and generates a new GLScene instance 
		with new ID numbers for each of the drawables. Creates a new
		instance of this scene type and calls GLScene::setSceneType() on the
		new scene instance, and sets scene() to the new scene pointer.
	*/	 
	virtual GLScene *generateScene(GLScene *sceneTemplate);
	
	/** Creates a new instance of this scene type, sets scene() to the \scene
		and calls \a scene->setSceneType() with the new instance's pointer.
		Also sets up any fields to their initial values.
		@param scene The scene to which to attach.
		@return The new instance of this scene type
	*/
	virtual GLSceneType *attachToScene(GLScene *scene);
	
	/** Called by DirectorWindow to create widgets to be used to control the scene.
		For example, for a sports broadcast scene, you may return a widget
		to update the onscreen score. 
	*/
	virtual QList<QWidget*> createEditorWidgets(GLScene *scene, DirectorWindow *director);

	/** Used in the loading of a GLScene from disk to re-attach a GLSceneType to a GLScene.
	    Returns the new GLSceneType instance unique to the scene.
	    Note that the \a generatedScene passed must be the same scene that called 
	    toByteArray() because GLSceneType will store IDs of drawables on the scene
	    into the bytearray for future reference.
		@param scene The scene that for this scene type - will call GLScene::setSceneType
		@param ba The byte array containing the data used to reconstitute the scene type instance
		@return The new instance of this scene type which has been setup for the scene.
	*/
	virtual GLSceneType *fromByteArray(GLScene *scene, QByteArray ba);
	/** Condenses this scene type's settings
		and the field->drawable ID pairings for the scene into a byte array.
		Settings would be things such as, for a weather scene, possibly the zip code or other unique paramters to the type's instance.
		 
		Note that this should be called like:
		\code
		GLScene *scene = ...;
		QByteArray data = scene->sceneType()->toByteArray();
		\endcode
		Of course, make sure sceneType() is valid or you'll segfault.
	*/
	virtual QByteArray toByteArray(QByteArray ba);
	
	virtual GLScene *scene() { return m_scene; }
	
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
