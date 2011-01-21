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

#include "GLDrawable.h"

class DirectorWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit DirectorWindow(QWidget *parent = 0);
	~DirectorWindow();
	
	GLSceneGroupCollection *collection() { return m_collection; }
	
	GLSceneGroup *currentGroup() { return m_currentGroup; }
	GLScene *currentScene() { return m_currentScene; }
	GLDrawable *currentDrawable() { return m_currentDrawable.data(); }
	GLPlaylistItem *currentItem() { return m_currentItem; }
	
	QString fileName() { return m_fileName; }

signals:
	void closed();
	
public slots:
	void showOpenFileDialog();
	void showSaveAsDialog();
	void saveFile();
	
	void showPlayerSetupDialog();
	
	void showEditWindow();
	void addNewGroup();
	void deleteGroup();
	
	bool readFile(const QString& fileName);
	bool writeFile(const QString& fileName="");
	
	void setCollection(GLSceneGroupCollection *collection);
	void setCurrentGroup(GLSceneGroup*, GLScene *currentScene=0);
	void setCurrentScene(GLScene *);
	void setCurrentDrawable(GLDrawable *drawable=0);
	void setCurrentItem(GLPlaylistItem *);
	void fadeBlack(bool toBlack=true);
	void setFadeSpeedPercent(int);
	void setFadeSpeedTime(double);
	
	void showPlayerLiveMonitor();

private slots:
	
	void groupSelected(const QModelIndex &);
	void currentGroupChanged(const QModelIndex &idx,const QModelIndex &);
	
	void slideSelected(const QModelIndex &);
	void currentSlideChanged(const QModelIndex &idx,const QModelIndex &);
	
	void drawableSelected(const QModelIndex &);
	void currentDrawableChanged(const QModelIndex &idx,const QModelIndex &);
	
	void itemSelected(const QModelIndex &);
	void currentItemChanged(const QModelIndex &idx,const QModelIndex &);
	
	void itemLengthChanged(double);
	void btnHideDrawable();
	void btnSendToPlayer();
	void btnAddToPlaylist();
	void btnRemoveFromPlaylist();
	void btnMoveUp();
	void btnMoveDown();

	void playlistTimeChanged(GLDrawable*, double);
	void playlistItemChanged(GLDrawable*, GLPlaylistItem *);
	void playlistItemDurationChanged();
	
	void pausePlaylist();
	void playPlaylist();
	
	void playerAdded(PlayerConnection *);
	void playerRemoved(PlayerConnection *);
	
	void changeCanvasSize();
	
	void loadVideoInputList(int);
	void videoInputClicked();
	
	void sceneDataChanged();
	
protected:
	void closeEvent(QCloseEvent *event);
	
	void setupUI();
	void readSettings();
	void writeSettings();
	
	void showPlayerLiveMonitor(PlayerConnection *con);

private:
	Ui::DirectorWindow *ui;
	
	QString m_fileName;
	PlayerConnectionList *m_players;
	GLSceneGroupCollection *m_collection;
	
	GLSceneGroup * m_currentGroup;
	GLScene * m_currentScene;
	QPointer<GLDrawable> m_currentDrawable;
	GLPlaylistItem * m_currentItem;
	
	GLEditorGraphicsScene *m_graphicsScene;
	
	FlowLayout *m_videoViewerLayout;
	VideoInputSenderManager *m_vidSendMgr;
	QList<QPointer<VideoReceiver> > m_receivers;
	
	QList<PlayerConnection*> m_videoPlayerList;
};

#endif // DIRECTORWINDOW_H
