#ifndef DIRECTORWINDOW_H
#define DIRECTORWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QPointer>

namespace Ui {
	class DirectorWindow;
}

class GLSceneGroupCollection;
class GLSceneGroup;
class GLScene;
class GLDrawable;
class GLPlaylistItem;
class PlayerConnection;
class PlayerConnectionList;
class GLEditorGraphicsScene;
class FlowLayout;
class VideoInputSenderManager;
class VideoReceiver;
class VideoWidget;
class GroupPlayerWidget;
class OverlayWidget;
#include "GLDrawable.h"

class DirectorWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit DirectorWindow(QWidget *parent = 0);
	~DirectorWindow();
	
	PlayerConnectionList *players() { return m_players; }

signals:
	void closed();
	
public slots:
	
	void showPlayerSetupDialog();
	
	
	void fadeBlack(bool toBlack=true);
	void setFadeSpeedPercent(int);
	void setFadeSpeedTime(double);
	
	void showPlayerLiveMonitor();

private slots:
	void playerAdded(PlayerConnection *);
	void playerRemoved(PlayerConnection *);
	
	void chooseOutput();
	
	void updateWindowMenu();
	void updateMenus();
	void setActiveSubWindow(QWidget *window);
	
	void videoInputListReceived(const QStringList&);
	void videoInputClicked();
	
	void addVideoPlayer();
	GroupPlayerWidget * addGroupPlayer();
	OverlayWidget * addOverlay();
	void addPreviewWindow();
	
	void showAllSubwindows();
	void createUserSubwindows();
	void applyTiling();
	
protected:
	void closeEvent(QCloseEvent *event);
	void showEvent(QShowEvent *event);
	
	void setupUI();
	void readSettings();
	void writeSettings();
	
	
	void showPlayerLiveMonitor(PlayerConnection *con);
	
	friend class DirectorMdiSubwindow;
	static QMap<QString,QRect> s_storedSystemWindowsGeometry;
	
	
private:
	Ui::DirectorWindow *ui;
	
	PlayerConnectionList *m_players;
	
	GLEditorGraphicsScene *m_graphicsScene;
	
	VideoInputSenderManager *m_vidSendMgr;
	
	QList<QPointer<VideoReceiver> > m_receivers;
	
	bool m_connected;
	
	QSignalMapper *m_windowMapper;
	QAction *m_closeAct;
	QAction *m_closeAllAct;
	QAction *m_tileAct;
	QAction *m_cascadeAct;
	QAction *m_nextAct;
	QAction *m_previousAct;
	QAction *m_separatorAct;
	
	bool m_hasVideoInputsList;	
	GLSceneGroup *m_camSceneGroup;
	
	QVariantList m_storedWindowOptions;
		
	
};

class GroupPlayerWidget : public QWidget
{
	Q_OBJECT
public:
	GroupPlayerWidget(DirectorWindow*);
	~GroupPlayerWidget();
	
	QString file();// { return m_collection->fileName(); }
	int currentIndex() { return m_combo->currentIndex(); }
	
public slots:
	void setCurrentIndex(int x) { m_combo->setCurrentIndex(x); }
 	bool loadFile(QString);
 	
	void saveFile();
	void openEditor();
	void browse();
	void newFile();
 
private slots:
	void clicked();
 	void selectedGroupIndexChanged(int);

private:
	GLWidget *m_glw;
	GLSceneGroup *m_setGroup;
	GLScene *m_scene;
	GLSceneGroupCollection *m_collection;
	DirectorWindow *m_director;
	QComboBox *m_combo;
};

class OverlayWidget : public QWidget
{
	Q_OBJECT
public:
	OverlayWidget(DirectorWindow*);
	~OverlayWidget();
	
	QString file();// { return m_collection->fileName(); }
	int currentIndex() { return m_combo->currentIndex(); }
	
public slots:
	void setCurrentIndex(int x) { m_combo->setCurrentIndex(x); }
	bool loadFile(QString);
	
	void showOverlay();
	void hideOverlay();
	
	void saveFile();
	void openEditor();
	void browse();
	void newFile();
 
private slots:
	//void clicked();
 	void selectedGroupIndexChanged(int);

private:
	GLWidget *m_glw;
	GLSceneGroup *m_setGroup;
	GLScene *m_scene;
	GLSceneGroupCollection *m_collection;
	DirectorWindow *m_director;
	QComboBox *m_combo;
};


class DirectorMdiSubwindow : public QMdiSubWindow
{
	Q_OBJECT
public:
	DirectorMdiSubwindow(QWidget *child=0);
	
	void setWidget(QWidget *);
	
protected slots:
	void applyGeom();

protected:
	void showEvent(QShowEvent *event);
	void moveEvent(QMoveEvent * moveEvent);
	
	QRect m_geom;
	 
};

#endif // DIRECTORWINDOW_H
