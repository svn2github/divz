#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>

#include "../glvidtex/GLDrawable.h"
#include "../glvidtex/GLVideoDrawable.h"
#include "../glvidtex/GLWidget.h"
#include "CameraThread.h"

class LiveLayer;
class LayerControlWidget : public QWidget 
{
	Q_OBJECT
public:
	LayerControlWidget(LiveLayer*);
	virtual ~LayerControlWidget();
	
	LiveLayer *layer() { return m_layer; }
	
protected:
	virtual void setupUI();
	
private:
	LiveLayer *m_layer;
};

class LiveLayer : public QObject
{
	Q_OBJECT
public:
	LiveLayer(QObject *parent=0);
	virtual ~LiveLayer();
	
	GLDrawable * drawable();
	
	LayerControlWidget *controlWidget() { return m_controlWidget; }
	
protected:
	friend class LiveScene;
	virtual void setupDrawable();
	
	GLDrawable * m_drawable;
	LayerControlWidget *m_controlWidget; 
};

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
	void detachGLWidget();
	GLWidget *currentGLWidget() { return m_glWidget; }
	
signals:
	void layerAdded(LiveLayer*);
	void layerRemoved(LiveLayer*);

private:
	GLWidget *m_glWidget;
	QList<LiveLayer*> m_layers;
	
};

///////////////////////

class LiveVideoInputLayer;
class VideoInputControlWidget : public LayerControlWidget
{
	Q_OBJECT
public:
	VideoInputControlWidget(LiveVideoInputLayer *);
	~VideoInputControlWidget();
	
	bool deinterlace() { return m_deinterlace; }
	
public slots:
	void setDeinterlace(bool flag);

protected slots:
	void deviceBoxChanged(int);

protected:
	virtual void setupUI();
	
private:
	LiveVideoInputLayer *m_videoLayer;
	QComboBox *m_deviceBox;
	QStringList m_cameras;
	bool m_deinterlace;
};

class LiveVideoInputLayer : public LiveLayer
{
	Q_OBJECT
public:
	LiveVideoInputLayer(QObject *parent=0);
	~LiveVideoInputLayer();
	
protected:
	virtual void setupDrawable();
	
	friend class VideoInputControlWidget;
	
	void setCamera(CameraThread*);
	CameraThread *camera() { return m_camera; }
	GLVideoDrawable *videoDrawable() { return m_videoDrawable; }
	
private:
	
	GLVideoDrawable *m_videoDrawable;
	CameraThread *m_camera;
};

///////////////////////

class QAction;
class QMenu;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();


protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void about();
	

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void readSettings();
	void writeSettings();
	
	void createLeftPanel();
	void createCenterPanel();
	void createRightPanel();
	
	void setupSampleScene();
	
	
	QMenu *m_fileMenu;
	QMenu *m_editMenu;
	QMenu *m_helpMenu;
	QToolBar *m_fileToolBar;
	QToolBar *m_editToolBar;
	QAction *m_exitAct;
	QAction *m_aboutAct;
	QAction *m_aboutQtAct;
	
	
	QSplitter *m_mainSplitter;
	QSplitter *m_leftSplitter;
	QWidget *m_leftPanel;
	QWidget *m_centerPanel;
	QWidget *m_rightPanel;
	
	
	QList<LiveScene*> m_scenes;
	GLWidget *m_mainViewer;
	GLWidget *m_layerViewer;
	GLWidget *m_mainOutput;
	
	QScrollArea *m_controlArea;	
	QWidget *m_controlBase;
	
	QScrollArea *m_layerArea;
	QWidget *m_layerBase;
	
	
};

#endif
