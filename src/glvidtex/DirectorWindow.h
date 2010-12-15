#ifndef DIRECTORWINDOW_H
#define DIRECTORWINDOW_H

#include <QMainWindow>
#include <QtGui>

namespace Ui {
	class DirectorWindow;
}

class GLSceneGroupCollection;
class GLSceneGroup;
class GLScene;
class GLDrawable;
class GLPlaylistItem;
class PlayerConnectionList;

class DirectorWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit DirectorWindow(QWidget *parent = 0);
	~DirectorWindow();
	
	GLSceneGroupCollection *collection() { return m_collection; }
	
	GLSceneGroup *currentGroup() { return m_currentGroup; }
	GLScene *currentScene() { return m_currentScene; }
	GLDrawable *currentDrawable() { return m_currentDrawable; }
	GLPlaylistItem *currentItem() { return m_currentItem; }
	
	QString fileName() { return m_fileName; }

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
	void setGroup(GLSceneGroup*, GLScene *currentScene=0);
	void setCurrentScene(GLScene *);
	void setCurrentDrawable(GLDrawable *);
	void setCurrentItem(GLPlaylistItem *);

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

	void playlistTimeChanged(double);
	void playlistItemChanged(GLPlaylistItem *);
	
	
protected:
	void closeEvent(QCloseEvent *event);
	
	void setupUI();
	void readSettings();
	void writeSettings();
	

private:
	Ui::DirectorWindow *ui;
	
	QString m_fileName;
	PlayerConnectionList *m_players;
	GLSceneGroupCollection *m_collection;
	
	GLSceneGroup *m_currentGroup;
	GLScene *m_currentScene;
	GLDrawable *m_currentDrawable;
	GLPlaylistItem *m_currentItem;
	
	QGraphicsScene *m_graphicsScene;
};

#endif // DIRECTORWINDOW_H
