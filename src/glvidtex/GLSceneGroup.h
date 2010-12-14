#ifndef GLSceneGroup_H
#define GLSceneGroup_H

#include <QtGui>

class GLScene;
class GLWidget;
class GLDrawable;

typedef QList<GLDrawable*> GLDrawableList;


class GLSceneLayoutItem : public QObject
{
	Q_OBJECT
public:
	GLSceneLayoutItem(int id=0, const QVariantMap& props = QVariantMap());
	GLSceneLayoutItem(GLDrawable *drawable=0, const QVariantMap& props = QVariantMap());
	GLSceneLayoutItem(QByteArray&);
	virtual ~GLSceneLayoutItem();
	
	int drawableId() { return m_drawableId; }
	QVariantMap propertyHash() { return m_props; }
	
	QByteArray toByteArray();
	void fromByteArray(QByteArray&);
	
public slots:
	void setDrawable(GLDrawable*);
	void setDrawableId(int);
	void setPropertyHash(const QVariantMap&); 
	
private:
	int m_drawableId;
	QVariantMap m_props;
};

class GLSceneLayout : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int layoutId READ layoutId);
	Q_PROPERTY(QString layoutName READ layoutName WRITE setLayoutName);
	
public:
	GLSceneLayout(GLScene *);
	GLSceneLayout(QByteArray&, GLScene*);
	virtual ~GLSceneLayout();
	
	int layoutId() { return m_layoutId; }
	QString layoutName() { return m_layoutName; }
	QPixmap pixmap() { return m_pixmap; }
	
	
	QList<GLSceneLayoutItem*> layoutItems() { return m_items; }
	void addLayoutItem(GLSceneLayoutItem* x) { m_items << x; }
	void removeLayoutItem(GLSceneLayoutItem* x) { m_items.removeAll(x); }
	
	QByteArray toByteArray();
	void fromByteArray(QByteArray&);
	
public slots:
	void setLayoutName(const QString& name);
	void setPixmap(const QPixmap& pix);

signals:
	void layoutNameChanged(const QString& name);
	void pixmapChanged(const QPixmap& pixmap);

private:
	GLScene *m_scene;
	int m_layoutId;
	QString m_layoutName;
	QPixmap m_pixmap;
	QList<GLSceneLayoutItem*> m_items;
};

class GLScene;
class GLSceneLayoutListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	GLSceneLayoutListModel(GLScene*);
	~GLSceneLayoutListModel();
	
	int rowCount(const QModelIndex &/*parent*/) const;
	QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	
private slots:
	void layoutAdded(GLSceneLayout*);
	void layoutRemoved(GLSceneLayout*);
	
private:
	GLScene *m_scene;

};

class GLScene : public QAbstractListModel
{
	Q_OBJECT
	Q_PROPERTY(int sceneId READ sceneId);
	Q_PROPERTY(QString sceneName READ sceneName WRITE setSceneName);
public:
	GLScene(QObject *parent=0);
	GLScene(QByteArray&, QObject *parent=0);
	~GLScene();
	
	int sceneId();
	QString sceneName() { return m_sceneName; }
	
	QByteArray toByteArray();
	void fromByteArray(QByteArray&);
	
	GLDrawableList drawableList() { return m_itemList; }
	void addDrawable(GLDrawable*);
	void removeDrawable(GLDrawable*);
	
	GLDrawable * lookupDrawable(int id);
	
	int size() const;
	GLDrawable * at(int idx);
	
	// QAbstractListModel
	virtual int rowCount(const QModelIndex &/*parent*/) const;
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	
	// This is the 'crossover' method which
	// connects the drawables in this scene to an actual display widget.
	// Only one widget can be set at a time - if the widget is changed,
	// the drawables are removed from the old and added to the new
	void setGLWidget(GLWidget*, int zIndexOffset=0);
	void detachGLWidget();
	
	// Layouts specify properties of the drawbles in the scene
	QList<GLSceneLayout*> layouts() { return m_layouts; }
	void addLayout(GLSceneLayout*);
	void removeLayout(GLSceneLayout*);
	
	GLSceneLayoutListModel *layoutListModel();
	
	GLSceneLayout * lookupLayout(int id);
	
	bool listOnlyUserItems() { return m_listOnlyUserItems; }

public slots:
	void setSceneName(const QString& name);
	void setListOnlyUserItems(bool);
	
signals:
	void drawableAdded(GLDrawable*);
	void drawableRemoved(GLDrawable*);
	void layoutAdded(GLSceneLayout*);
	void layoutRemoved(GLSceneLayout*);
	
	void sceneNameChanged(const QString&);
	
private slots:
	void drawableDestroyed();
	
protected:
	friend class GLSceneLayoutListModel;
	
	int m_sceneId;
	QString m_sceneName;
	
	GLDrawableList m_itemList;
	GLDrawableList m_userItemList;
	bool m_listOnlyUserItems;
	QHash<int,GLDrawable*> m_drawableIdLookup;
	
	QList<GLSceneLayout*> m_layouts;
	QHash<int,GLSceneLayout*> m_layoutIdLookup;
		
	GLWidget *m_glWidget;
	GLSceneLayoutListModel *m_layoutListModel;
};
	

class GLSceneGroup : public QAbstractListModel
{
	Q_OBJECT
	
	Q_PROPERTY(int groupId READ groupId);
	Q_PROPERTY(QString groupName READ groupName WRITE setGroupName);
	
public:
	GLSceneGroup(QObject *parent=0);
	GLSceneGroup(QByteArray&, QObject *parent=0);
	~GLSceneGroup();
	
	int groupId();
	QString groupName() { return m_groupName; }
	
	QByteArray toByteArray();
	void fromByteArray(QByteArray&);
	
	// The core of the scene group is a list of scenes.
	// The order is explicit through their index in the QList, though not relevant
	// as the order they are played in is specified by the GLSchedule and GLScheduleItems.
	// Although the scenes are displayed in order in the 'Director' program
	QList<GLScene*> sceneList() { return m_scenes; }
	void addScene(GLScene*);
	void removeScene(GLScene*);
	
	GLScene * lookupScene(int id);
	
	int size() const { return m_scenes.size(); }
	GLScene * at(int idx) { return idx>-1 && idx<size() ? m_scenes[idx] : 0; }
	
	// QAbstractListModel
	virtual int rowCount(const QModelIndex &/*parent*/) const;
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual bool setData(const QModelIndex &index, const QVariant & value, int role) ;

	// Overlay scene, by definition, is a general scene that is to be overlayed on the content of 
	// the other scenes in the list. 
	GLScene * overlayScene() { return m_overlayScene; }
	void setOverlayScene(GLScene*);

public slots:
	void setGroupName(const QString& name);
	
signals:
	void sceneAdded(GLScene*);
	void sceneRemoved(GLScene*);
	
	void groupNameChanged(const QString&);
	void overlaySceneChanged(GLScene*);
	
protected:
	int m_groupId;
	QString m_groupName;
	
	QList<GLScene*> m_scenes;
	QHash<int,GLScene*> m_sceneIdLookup;

	GLScene *m_overlayScene;
};


class GLSceneGroupCollection : public QAbstractListModel
{
	Q_OBJECT
	
	Q_PROPERTY(int collectionId READ collectionId);
	Q_PROPERTY(QString collectionName READ collectionName WRITE setCollectionName);
	
public:
	GLSceneGroupCollection(QObject *parent=0);
	GLSceneGroupCollection(QByteArray&, QObject *parent=0);
	GLSceneGroupCollection(const QString& file, QObject *parent=0);
	~GLSceneGroupCollection();
	
	int collectionId();
	QString collectionName() { return m_collectionName; }
	
	QSizeF canvasSize() { return m_canvasSize; }
	
	QByteArray toByteArray();
	void fromByteArray(QByteArray&);
	
	bool writeFile(const QString& name="");
	bool readFile(const QString& name);
	
	QString fileName() { return m_fileName; }
	
	// The core of the scene group is a list of scenes.
	// The order is explicit through their index in the QList, though not relevant
	// as the order they are played in is specified by the GLSchedule and GLScheduleItems.
	// Although the scenes are displayed in order in the 'Director' program
	QList<GLSceneGroup*> groupList() { return m_groups; }
	void addGroup(GLSceneGroup*);
	void removeGroup(GLSceneGroup*);
	
	GLSceneGroup * lookupGroup(int id);
	
	int size() const { return m_groups.size(); }
	GLSceneGroup * at(int idx) { return idx>-1 && idx<size() ? m_groups[idx] : 0; }
	
	// QAbstractListModel
	virtual int rowCount(const QModelIndex &/*parent*/) const { return size(); }
	virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual bool setData(const QModelIndex &index, const QVariant & value, int role) ;
		
public slots:
	void setCollectionName(const QString& name);
	void setFileName(const QString& name) { m_fileName = name; }
	void setCanvasSize(const QSizeF&);

signals:
	void groupAdded(GLSceneGroup*);
	void groupRemoved(GLSceneGroup*);
	
	void collectionNameChanged(const QString&);
	void canvasSizeChanged(const QSizeF&);

protected:
	int m_collectionId;
	QString m_collectionName;
	QString m_fileName;
	
	QSizeF m_canvasSize;
	
	QList<GLSceneGroup*> m_groups;
	QHash<int,GLSceneGroup*> m_groupIdLookup;
};


#endif
