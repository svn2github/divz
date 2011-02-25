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
class SwitcherWindow;
class PropertyEditorWindow;
class DirectorSourceWidget;
class DirectorMdiSubwindow;
#include "GLDrawable.h"

class DirectorWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit DirectorWindow(QWidget *parent = 0);
	~DirectorWindow();
	
	PlayerConnectionList *players() { return m_players; }
	
	QList<QMdiSubWindow*> subwindows(); 

signals:
	void closed();
	
	void subwindowAdded(QMdiSubWindow*);
	void subwindowActivated(QMdiSubWindow*);
	
public slots:
	
	void showPlayerSetupDialog();
	
	
	void fadeBlack(bool toBlack=true);
	void setFadeSpeedPercent(int);
	void setFadeSpeedTime(double);
	
	void showPlayerLiveMonitor();
	
	/// DirectorSourceWidget subclasses can use this to request the property editor be displayed for their instance.
	void showPropertyEditor(DirectorSourceWidget*);

private slots:
	void playerAdded(PlayerConnection *);
	void playerRemoved(PlayerConnection *);
	
	void chooseOutput();
	
	void updateWindowMenu();
	void updateMenus();
	void setActiveSubWindow(QWidget *window);
	
	void videoInputListReceived(const QStringList&);
	
// 	VideoPlayerWidget * addVideoPlayer();
	void addVideoPlayer();
	GroupPlayerWidget * addGroupPlayer();
	OverlayWidget * addOverlay();
	
	void showAllSubwindows();
	void createUserSubwindows();
	void applyTiling();
	
	void showPreviewWin();
	void showPropEditor();
	void showSwitcher();
	
	DirectorMdiSubwindow *addSubwindow(QWidget*);
	
protected:
	void closeEvent(QCloseEvent *event);
	void showEvent(QShowEvent *event);
	
	void setupUI();
	void readSettings();
	void writeSettings();
	
	
	void showPlayerLiveMonitor(PlayerConnection *con);
	
	friend class DirectorMdiSubwindow;
	static QMap<QString,QRect> s_storedSystemWindowsGeometry;
	
	QMdiSubWindow *windowForWidget(QWidget*);
	
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
	
	QPointer<PropertyEditorWindow> m_propWin;
	QPointer<SwitcherWindow> m_switcherWin;
		
	
};

class DirectorSourceWidget : public QWidget
{
public:
	DirectorSourceWidget(DirectorWindow *dir)
		: QWidget(dir)
		, m_dir(dir)
		, m_switcher(0)
	{}
	
	virtual ~DirectorSourceWidget() {}
	
	/// Returns the SwitcherWindow currently in use
	virtual SwitcherWindow *switcher() { return m_switcher; }
	
	/// Subclasses are expected to call SwitcherWindow::notifyIsLive(DirectorSourceWidget*) to notify the switcher when they are spontaneously switched live.
	virtual void setSwitcher(SwitcherWindow* switcher) { m_switcher = switcher; }
	
	/// The SwitcherWindow will call this method to request subclasses to go live.
	/// Subclasses may return false if there is some reason they could not go live.
	virtual bool switchTo() { return false; };
	
	/// If, for some reason, a subclass is not able to be switched to (for example, OverlayWidget), then overload this to return false so the SwitcherWindow can ignore this subclass.
	virtual bool canSwitchTo() { return true; }
	
	/// Subclasses are to use these two methods (saveToMap() and loadFromMap()) to load/save properties between sessions
	virtual QVariantMap saveToMap() { return QVariantMap(); }
	virtual void loadFromMap(const QVariantMap&) {}
	
	/// Subclasses are expected to return the currently "active" scene displayed in the window
	virtual GLScene *scene() { return 0; }

protected:
	DirectorWindow *m_dir;
	SwitcherWindow *m_switcher;
};

class GroupPlayerWidget : public DirectorSourceWidget
{
	Q_OBJECT
public:
	GroupPlayerWidget(DirectorWindow*);
	~GroupPlayerWidget();
	
	QString file();// { return m_collection->fileName(); }
	int currentIndex() { return m_combo->currentIndex(); }
	
	virtual QVariantMap saveToMap();
	virtual void loadFromMap(const QVariantMap&);
	virtual GLScene *scene() { return m_scene; }

public slots:
	// DirectorSourceWidget::	
	virtual bool switchTo();
	
	void setCurrentIndex(int x) { m_combo->setCurrentIndex(x); }
 	bool loadFile(QString);
 	
	void saveFile();
	void openEditor();
	void browse();
	void newFile();
 
private slots:
	void selectedGroupIndexChanged(int);

private:
	GLWidget *m_glw;
	GLSceneGroup *m_setGroup;
	GLScene *m_scene;
	GLSceneGroupCollection *m_collection;
	DirectorWindow *m_director;
	QComboBox *m_combo;
};

class CameraWidget : public DirectorSourceWidget
{
	Q_OBJECT
public:
	CameraWidget(DirectorWindow*, VideoReceiver*, QString con, GLSceneGroup *camSceneGroup, int index);
	~CameraWidget();
	
	virtual QVariantMap saveToMap();
	virtual void loadFromMap(const QVariantMap&);
	
private slots:
	// DirectorSourceWidget::	
	virtual bool switchTo();
	void setDeinterlace(bool);
	
protected:
	void contextMenuEvent(QContextMenuEvent * event);

private:
	VideoReceiver *m_rx;
	QMenu *m_configMenu;
	QString m_con;
	GLSceneGroup *m_camSceneGroup;
	bool m_deinterlace;
};

class OverlayWidget : public DirectorSourceWidget
{
	Q_OBJECT
public:
	OverlayWidget(DirectorWindow*);
	~OverlayWidget();
	
	QString file();// { return m_collection->fileName(); }
	int currentIndex() { return m_combo->currentIndex(); }
	
	// DirectorSourceWidget::	
	virtual bool canSwitchTo() { return false; } // overlays are added, not switched to
	virtual QVariantMap saveToMap();
	virtual void loadFromMap(const QVariantMap&);
	virtual GLScene *scene() { return m_scene; }
	
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

class SwitcherWindow : public QWidget
{
	Q_OBJECT
public:
	SwitcherWindow(DirectorWindow *);
	
public slots:
	void notifyIsLive(DirectorSourceWidget*);

protected:
	 bool eventFilter(QObject *, QEvent *);
	
private slots:
	void buttonClicked();
	void subwindowAdded(QMdiSubWindow*);
	
private:
	DirectorWindow *m_dir;
};


class PropertyEditorWindow : public QWidget
{
	Q_OBJECT
public:
	PropertyEditorWindow(DirectorWindow *);
	
	void setSourceWidget(DirectorSourceWidget*);
	
private slots:
	void subwindowActivated(QMdiSubWindow*);
	void sourceDestroyed();
	
private:
	QVBoxLayout *m_layout;
	QPointer<DirectorSourceWidget> m_source;
	DirectorWindow *m_dir;
	
};

// class VideoPlayerWidget : public QWidget
// {
// 	Q_OBJECT
// public:
// 	VideoPlayerWidget(DirectorWindow*);
// 	~VideoPlayerWidget();
// 	
// 	QString file() { return m_filename; }
// 	double inPoint() { return m_in; }
// 	double outPoint() { return m_out; }
// 	double position() { return m_pos; }
// 	bool isMuted() { return m_muted; }
// 	int volume() { return m_volume; } 
// 	
// public slots:
// 	void setInPoint(double in);
// 	void setOutPoint(double out);
// 	void setMuted(bool muted);
// 	void setVolume(int volume);
// 	void setPosition(double);
// 	
//  	bool loadFile(QString);
//  	
// 	void browse();
//  
// private slots:
// 	void clicked();
//  	void receivedPosition(int);
// 
// private:
// 	GLWidget *m_glw;
// 	GLSceneGroup *m_setGroup;
// 	GLScene *m_scene;
// 	DirectorWindow *m_director;
// 	
// 	QString m_filename;
// 	double m_in;
// 	double m_out;
// 	double m_pos;
// 	bool m_muted;
// 	int volume;
// 	
// };


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
