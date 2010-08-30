#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>

#include "LiveScene.h"
#include "LiveTextLayer.h"
#include "LiveVideoInputLayer.h"
#include "LiveStaticSourceLayer.h"

class QAction;
class QMenu;

class QtVariantProperty;
class QtProperty;

class QtBrowserIndex;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();


protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void about();
	void updateLayerList();

	void setCurrentLayer(LiveLayer *layer);
	
	void newFile();
	void open();
	void loadFile(const QString&);
	void save(const QString& file="");
	void saveAs();
	
	void newCameraLayer();
	void newVideoLayer();
	void newTextLayer();
	void newImageLayer();
	
	void addLayer(LiveLayer *);
	
	void layerSelected(const QModelIndex &);
	void currentChanged(const QModelIndex &idx,const QModelIndex &);
	void repaintLayerList();
	void layersDropped(QList<LiveLayer*>);

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
	
	void loadLiveScene(LiveScene*);
	void loadLayerProperties(LiveLayer*);
	void removeCurrentScene();
	
	
	QMenu *m_fileMenu;
	QMenu *m_editMenu;
	QMenu *m_helpMenu;
	QToolBar *m_fileToolBar;
	QToolBar *m_editToolBar;
	QAction *m_exitAct;
	QAction *m_aboutAct;
	QAction *m_aboutQtAct;
	
	QAction *m_openAct;
	QAction *m_newAct;
	QAction *m_saveAct;
	QAction *m_saveAsAct;
	
	QAction *m_newCameraLayerAct;
	QAction *m_newVideoLayerAct;
	QAction *m_newTextLayerAct;
	QAction *m_newImageLayerAct;
	
	
	QSplitter *m_mainSplitter;
	QSplitter *m_editSplitter;
	QSplitter *m_previewSplitter;
	QWidget *m_leftPanel;
	QWidget *m_centerPanel;
	QWidget *m_rightPanel;
	
	
	QList<LiveScene*> m_scenes;
	GLWidget *m_mainViewer;
	GLWidget *m_layerViewer;
	GLWidget *m_mainOutput;
	
	QScrollArea *m_controlArea;	
	QWidget *m_controlBase;
	QWidget *m_currentLayerPropsEditor;
	
	QScrollArea *m_layerArea;
	QWidget *m_layerBase; /// TODO make this a drag widget like fridgemagnets example
	
	LiveScene *m_currentScene;
	
	QHash<LiveLayer*, LayerControlWidget*> m_controlWidgetMap;
	LiveLayer *m_currentLayer;
	
	QString m_currentFile;
	
	QListView * m_layerListView;
	class LiveSceneListModel *m_sceneModel;
	

};

#endif
