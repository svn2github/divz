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

	void liveLayerClicked();
	
	void valueChanged(QtProperty *property, const QVariant &value);

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
	QWidget *m_layerBase; /// TODO make this a drag widget like fridgemagnets example
	
	LiveScene *m_currentScene;
	
	QHash<LiveLayer*, LayerControlWidget*> m_controlWidgetMap;
	LayerControlWidget * m_currentControlWidget;
	LiveLayer *m_currentLayer;
	

	void addProperty(QtVariantProperty *property, const QString &id);
    	void updateExpandState();
    	
    	class QtVariantPropertyManager *m_variantManager;
	class QtTreePropertyBrowser *m_propertyEditor;
    
	QMap<QtProperty *, QString> m_propertyToId;
	QMap<QString, QtVariantProperty *> m_idToProperty;
	QMap<QString, bool> m_idToExpanded;

};

#endif
