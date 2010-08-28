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
	
	QList<LayerControlWidget*> controlWidgets();
	QList<LiveLayer*> layerList() { return m_layers; }
	void addLayer(LiveLayer*);
	void removeLayer(LiveLayer*);
	
	void attachGLWidget(GLWidget*);
	void detachGLWidget(bool hideFirst=true);
	GLWidget *currentGLWidget() { return m_glWidget; }
	
	QByteArray toByteArray();
	void fromByteArray(QByteArray&);
	
signals:
	void layerAdded(LiveLayer*);
	void layerRemoved(LiveLayer*);

private slots:
	void layerVisibilityChanged(bool);
	
private:
	GLWidget *m_glWidget;
	QList<LiveLayer*> m_layers;
};


#endif
