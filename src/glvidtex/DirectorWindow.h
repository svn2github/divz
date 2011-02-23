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
	
protected:
	void closeEvent(QCloseEvent *event);
	
	void setupUI();
	void readSettings();
	void writeSettings();
	
	void showPlayerLiveMonitor(PlayerConnection *con);

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
		
	
};

#endif // DIRECTORWINDOW_H
