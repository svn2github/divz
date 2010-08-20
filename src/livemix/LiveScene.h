#ifndef LiveScene_H
#define LiveScene_H

#include <QObject>
#include <QList>

#include "LiveLayer.h"
class LayerControlWidget;
class GLWidget;

class LiveScene : public QObject
{
	Q_OBJECT
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
