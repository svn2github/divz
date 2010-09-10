#ifndef LiveScene_H
#define LiveScene_H

#include <QObject>
#include <QList>

#include "LiveLayer.h"
class LayerControlWidget;
class GLWidget;

#define LiveScene_Register(a) LiveScene::registerClass(a::staticMetaObject.className(), &a::staticMetaObject);

class LiveScene : public QObject
{
	Q_OBJECT

private:
	static QHash<QString, const QMetaObject*> s_metaObjects;	

public:
	static const QMetaObject* metaObjectForClass(const QString& name)       { return s_metaObjects[name]; }
	static void registerClass(const QString& name, const QMetaObject *meta) { s_metaObjects[name] = meta; }
	
public:
	LiveScene(QObject *parent=0);
	~LiveScene();
	
	QList<LiveLayer*> layerList() { return m_layers; }
	LiveLayer *layerFromId(int);
	
	void addLayer(LiveLayer*);
	void removeLayer(LiveLayer*);
	
	void attachGLWidget(GLWidget*);
	void detachGLWidget(GLWidget*);
	
	QByteArray toByteArray();
	void fromByteArray(QByteArray&);
	
	class KeyFrame
	{
	public:
		KeyFrame(LiveScene *s, int x=-1)
			: scene(s)
			, id(x)
			, playTime(-1)
		{}
		
		LiveScene *scene;
		
		int id;
		
		// int is the id of the layer
		QHash<int, QVariantMap> layerProperties;
		LiveLayer::AnimParam animParam;
		
		// convenience accessors
		QList<LiveLayer*> layers();
		QVariantMap propsForLayer(LiveLayer*);
		
		// playTime : Time on the 'play clock' to show this frame, in seconds
		double playTime;
		// clockTime : if set, overrides playTime - wall time to show this frame
		QTime clockTime;
		
		// Name for displaying in a list, for example, drop down list
		QString frameName;
		
		// Graphic of the way the scene looks with these properties, scaled to 128x128, aspect ratio preserved
		QPixmap pixmap;
		
		// For loading/saving frames
		QByteArray toByteArray();
		void fromByteArray(QByteArray&);
	};
	
	KeyFrame createKeyFrame(QList<LiveLayer*> layers = QList<LiveLayer*>(), const QPixmap& icon=QPixmap());
	KeyFrame createAndAddKeyFrame(QList<LiveLayer*> layers = QList<LiveLayer*>(), const QPixmap& icon=QPixmap());
	
	KeyFrame updateKeyFrame(int, const QPixmap& newIcon=QPixmap());
	
	// not const, because it sets the .id of the frame
	void addKeyFrame(KeyFrame&);
	void removeKeyFrame(const KeyFrame&);
	void removeKeyFrame(int);
	
	void applyKeyFrame(const KeyFrame&);
	void applyKeyFrame(int);
	
	void setKeyFrameName(int, const QString&);
	void setKeyFrameStartTime(int, double);
	void setKeyFrameAnimLength(int, int);
	
	QList<KeyFrame> keyFrames() { return m_keyFrames; }
	
	// time it would take to play ever key frame, include all animations, in sequence
	double sceneLength();
	
signals:
	void layerAdded(LiveLayer*);
	void layerRemoved(LiveLayer*);
	
	void keyFrameAdded(const KeyFrame&);
	void keyFrameRemoved(const KeyFrame&);
	
private:
	void sortKeyFrames();
	
	QList<GLWidget*>  m_glWidgets;
	QList<LiveLayer*> m_layers;
	QHash<int,LiveLayer*> m_idLookup;
	QList<KeyFrame> m_keyFrames;
};

	
bool operator==(const LiveScene::KeyFrame& a, const LiveScene::KeyFrame& b);
	

#endif
