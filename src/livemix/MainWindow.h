#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>

#include "../glvidtex/GLDrawable.h"
#include "../glvidtex/GLVideoDrawable.h"
#include "../glvidtex/GLWidget.h"
#include "../glvidtex/StaticVideoSource.h"
#include "../glvidtex/TextVideoSource.h"
#include "CameraThread.h"

class LiveLayer;
class LayerControlWidget : public QFrame 
{
	Q_OBJECT
public:
	LayerControlWidget(LiveLayer*);
	virtual ~LayerControlWidget();
	
	LiveLayer *layer() { return m_layer; }
	bool isCurrentWidget() { return m_isCurrentWidget; }
	
signals:
	void clicked();
	
public slots:
	void setIsCurrentWidget(bool);
	
protected slots:
	void instanceNameChanged(const QString&);
	void opacitySliderChanged(int);
	void drawableVisibilityChanged(bool);
	
protected:
	virtual void setupUI();
	void mouseReleaseEvent(QMouseEvent*);
	
private:
	LiveLayer *m_layer;
	QLabel *m_nameLabel;
	QSlider *m_opacitySlider;
	QPushButton *m_liveButton;
	QPushButton *m_editButton;
	bool m_isCurrentWidget;
};

class QtVariantProperty;
class QtVariantPropertyManager;

#ifndef QtPropertyEditorIdPair
//typedef QPair<QtVariantProperty,QString> QtPropertyEditorIdPair;
class QtPropertyEditorIdPair
{
public:
	QtPropertyEditorIdPair(QString _id, QtVariantProperty *_value) 
		: id(_id), value(_value) {}
	QString id;
	QtVariantProperty *value;
};

#endif

class QtVariantIdPair
{
public:
	QtVariantIdPair(QString _id, QVariant _value) 
		: id(_id), value(_value) {}
	QString id;
	QVariant value;
};

class LiveLayer : public QObject
{
	Q_OBJECT
public:
	LiveLayer(QObject *parent=0);
	virtual ~LiveLayer();
	
	virtual QString typeName() { return "Generic Layer"; }
	virtual QString instanceName() { return m_instanceName; }
	
	GLDrawable * drawable();
	
	LayerControlWidget *controlWidget() { return m_controlWidget; }
	
	// Used by MainWindow to setup the property editor for this layer
	virtual QList<QtPropertyEditorIdPair> createPropertyEditors(QtVariantPropertyManager *manager);
	
	// Used to save/restore layers 
	QHash<QString,QVariant> instanceProperties();
	
	// Default impl just iterates through all the keys and calls setInstanceProperty()
	virtual void setInstanceProperties(QHash<QString,QVariant> props);
	
	// Query for a specific prop
	QVariant instanceProperty(const QString& id) { return m_props[id]; }
	
signals:
	void instanceNameChanged(const QString&);
	void instancePropertyChanged(const QString&, const QVariant&);
	
public slots:
	// Set a property (emits instancePropertyChanged)
	virtual void setInstanceProperty(const QString&, const QVariant&);
	
protected:
	void changeInstanceName(const QString&);
	virtual void setupInstanceProperties(GLDrawable*);
	
	virtual void setupDrawable();
	
	GLDrawable * m_drawable;
	LayerControlWidget *m_controlWidget; 
	QString m_instanceName;
	
	QHash<QString,QVariant> m_props;
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
	
	virtual QString typeName() { return "Video Input"; }
	
	// Used by MainWindow to setup the property editor for this layer
	virtual QList<QtPropertyEditorIdPair> createPropertyEditors(QtVariantPropertyManager *manager);

public slots:
	// Set a property (emits instancePropertyChanged)
	virtual void setInstanceProperty(const QString&, const QVariant&);
	
protected:
	virtual void setupDrawable();
	virtual void setupInstanceProperties(GLDrawable*);
	
	friend class VideoInputControlWidget;
	
	void setCamera(CameraThread*);
	CameraThread *camera() { return m_camera; }
	GLVideoDrawable *videoDrawable() { return m_videoDrawable; }
	
private:
	GLVideoDrawable *m_videoDrawable;
	CameraThread *m_camera;
};

class LiveStaticSourceLayer : public LiveLayer
{
	Q_OBJECT
public:
	LiveStaticSourceLayer(QObject *parent=0);
	~LiveStaticSourceLayer();
	
	virtual QString typeName() { return "Static Source"; }

	// Used by MainWindow to setup the property editor for this layer
	virtual QList<QtPropertyEditorIdPair> createPropertyEditors(QtVariantPropertyManager *manager);

public slots:
	// Set a property (emits instancePropertyChanged)
	virtual void setInstanceProperty(const QString&, const QVariant&);

protected:
	virtual void setupDrawable();
	virtual void setupInstanceProperties(GLDrawable*);
	
	GLVideoDrawable *videoDrawable() { return m_videoDrawable; }
	
private:
	GLVideoDrawable *m_videoDrawable;
	StaticVideoSource *m_staticSource;
};

class LiveTextLayer  : public LiveLayer
{
	Q_OBJECT
public:
	LiveTextLayer(QObject *parent=0);
	~LiveTextLayer();
	
	virtual QString typeName() { return "Text Layer"; }
	
	QString text() { return m_text; }
	
	// Used by MainWindow to setup the property editor for this layer
	virtual QList<QtPropertyEditorIdPair> createPropertyEditors(QtVariantPropertyManager *manager);

public slots:
	// Calls setInstanceProperty internally
	void setText(const QString& text);
	
	// Set a property (emits instancePropertyChanged)
	virtual void setInstanceProperty(const QString&, const QVariant&);

protected:
	virtual void setupDrawable();
	virtual void setupInstanceProperties(GLDrawable*);
	
	GLVideoDrawable *videoDrawable() { return m_videoDrawable; }
	
private:
	GLVideoDrawable *m_videoDrawable;
	TextVideoSource *m_textSource;
	QString m_text; 
};

///////////////////////

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
