#ifndef GLSceneGroup_H
#define GLSceneGroup_H

#include "GLDrawable.h"

typedef QList<GLDrawable*> GLDrawableList;
typedef QHash<QString,QVariant> QStringVariantHash;

class GLSceneLayoutItem
{
	Q_OBJECT
public:
	GLSceneLayoutItem(int id=0, const QStringVariantHash& props = QStringVariantHash());
	GLSceneLayoutItem(GLDrawable *drawable=0, const QStringVariantHash& props = QStringVariantHash());
	GLSceneLayoutItem(const QByteArray&);
	
	int drawableId() { return m_drawableId; }
	QStringVariantHash propertyHash() { return m_props; }
	
	QByteArray toByteArray();
	void fromByteArray(const QByteArray&);
	
public slots:
	void setDrawable(GLDrawable*);
	void setDrawableId(int);
	void setPropertyHash(const QStringVariantHash&); 
	
private:
	int m_drawableId;
	QStringVariantHash m_props;
}

class GLSceneLayout
{
	Q_OBJECT
	Q_PROPERTY(int layoutId READ layoutId);
	Q_PROPERTY(QString layoutName READ layoutName WRITE setLayoutName);
	
public:
	GLSceneLayout(GLScene *);
	GLSceneLayout(const QByteArray&, GLScene*);
	~GLSceneLayout(GLScene *);
	
	int layoutId() { return m_layoutId; }
	QString layoutName() { return m_layoutName; }
	
	QList<GLSceneLayoutItem*> layoutItems();
	
	QByteArray toByteArray();
	void fromByteArray(const QByteArray&);
	
public slots:
	void setLayoutName(const QString&);

signals:
	void layoutNameChanged();

private:
	GLScene *m_scene;
	int m_layoutId;
	QString m_layoutName;
	QList<GLSceneLayoutItem*> m_items;
}


class GLScene : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int sceneId READ sceneId);
	Q_PROPERTY(QString sceneName READ sceneName WRITE setSceneName);
public:
	GLScene(QObject *parent=0);
	GLScene(const QByteArray&, QObject *parent=0);
	~GLScene();
	
	int sceneId();
	QString sceneName() { return m_sceneName; }
	
	QByteArray toByteArray();
	void fromByteArray(const QByteArray&);
	
	GLDrawableList drawableList();
	void addDrawable(GLDrawable*);
	void removeDrawable(GLDrawable*);
	
	GLDrawable * lookupDrawable(int id);
	
	int size();
	GLDrawable * at(int idx);
	
	// This is the 'crossover' method which
	// connects the drawables in this scene to an actual display widget.
	// Only one widget can be set at a time - if the widget is changed,
	// the drawables are removed from the old and added to the new
	void setGLWidget(GLWidget*, int zIndexOffset=0);
	void detachGLWidget();
	
	// Layouts specify properties of the drawbles in the scene
	QList<GLSceneLayout*> layouts();
	void addLayout(GLSceneLayout*);
	void removeLayout(GLSceneLayout*);
	
	GLSceneLayout * lookupLayout(int id);

slots:
	void setSceneName(const QString& name);
	
signals:
	void drawableAdded(GLDrawable*);
	void drawableRemoved(GLDrawable*);
	void layoutAdded(GLSceneLayout*);
	void layoutRemoved(GLSceneLayout*);
	
protected:
	int m_sceneId;
	QString m_sceneName;
	
	GLDrawableList m_itemList;
	QHash<int,GLDrawable*> m_drawableIdLookup;
	
	QList<GLSceneLayout*> m_layouts;
	QHash<int,GLSceneLayout*> m_layoutIdLookup;
		
	GLWidget *m_widget;
};
	

class GLSceneGroup : public QObject
{
	Q_OBJECT
	
	Q_PROPERTY(int groupId READ groupId);
	Q_PROPERTY(QString groupName READ groupName WRITE setGroupName);
	
public:
	SceneGroup(QObject *parent=0);
	SceneGroup(const QByteArray&, QObject *parent=0);
	~SceneGroup();
	
	int groupId();
	QString groupName() { return m_groupName; }
	
	QByteArray toByteArray();
	void fromByteArray(const QByteArray&);
	
	// The core of the scene group is a list of scenes.
	// The order is explicit through their index in the QList, though not relevant
	// as the order they are played in is specified by the GLSchedule and GLScheduleItems.
	// Although the scenes are displayed in order in the 'Director' program
	QList<GLScene*> sceneList();
	void addScene(GLScene*);
	void removeScene(GLScene*);
	
	GLScene * lookupScene(int id);
		
	// Overlay scene, by definition, is a general scene that is to be overlayed on the content of 
	// the other scenes in the list. 
	GLScene * overlayScene() { return m_overlayScene; }
	void setOverlayScene(GLScene*);

slots:
	void setGroupName(const QString& name);
	
signals:
	void sceneAdded(GLScene*);
	void sceneRemoved(GLScene*);
	
	void overlaySceneChanged(GLScene*);
	
protected:
	int m_groupId;
	QString m_groupName;
	
	QList<GLScene*> m_scenes;
	QHash<int,GLScene*> m_sceneIdLookup;

	GLScene *m_overlayScene;
};


#endif