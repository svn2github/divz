#ifndef MEDIABROWSER_H
#define MEDIABROWSER_H

#include <QWidget>

class QFileSystemModel;
class QFileInfo;
class QModelIndex;
class QPushButton;
class QLineEdit;
class QComboBox;
class QSplitter;
class SlideGroupViewer;
class QVBoxLayout;
class QCheckBox;
#include <QHash>
#include <QStringList>
#include <QRegExp>
#include <QListView>

class DirectoryListModel;

#define MEDIABROWSER_LIST_ICON_SIZE QSize(48,48)

class MediaBrowser : public QWidget
{
	Q_OBJECT
public /*static*/:
	static QRegExp videoRegexp;
	static QRegExp imageRegexp;
	
	static bool isVideo(const QString &extension);
	static bool isImage(const QString &extension);
	
	static QPixmap iconForImage(const QString & file, const QSize & size);

public:
	MediaBrowser(const QString &directory="", QWidget *parent=0);
	~MediaBrowser();
	
	QByteArray saveState();
	bool restoreState (const QByteArray & state);
	
	void setPreviousPathKey(const QString&);
	QString previousPathKey() { return m_prevPathKey; }
	
	void setBackgroundActionsEnabled(bool);
	bool backgroundActionsEnabled() { return m_backgroundActionsEnabled; }
	
	void setViewMode(QListView::ViewMode);
	QListView::ViewMode viewMode();
	
	void setSplitterOrientation(Qt::Orientation);
	Qt::Orientation splitterOrientation();
	
	void setIconSize(const QSize &);
	QSize iconSize() { return m_iconSize; }
	
	bool autoPlayVideo() { return m_autoPlayVideo; }
	
signals:
	void fileSelected(const QFileInfo&);
	void fileDoubleClicked(const QFileInfo&);
	void setSelectedBackground(const QFileInfo&);
	void setLiveBackground(const QFileInfo&, bool waitForNextSlide);
	
public slots:
	void setFilter(const QString&);
	void clearFilter() { setFilter(""); }
	
	void setDirectory(const QString&, bool addToHistory = true);
	
	void goBack();
	void goForward();
	void goUp();
	
	void setFileTypeFilterList(QStringList);
	
	void setAutoPlayVideo(bool);
	
protected slots:
	void indexDoubleClicked(const QModelIndex&);
	void indexSingleClicked(const QModelIndex&);
	
	void filterChanged(const QString&);
	void filterReturnPressed();
	
	void dirBoxReturnPressed();
	void fileTypeChanged(int);
	
	void slotAddToSchedule();
	void slotSetAsBgCurrent();
	void slotSetAsBgLater();
	void slotSetAsBgLive();
	
	void slotBookmarkFolder();
	void slotDelBookmark();
	void loadBookmarkIndex(int);
	void loadBookmarks();
	void saveBookmarks();
	

protected:
	void setupUI();
	
	void pushBackward(const QString&);
	QString popBackward();
	
	void unshiftForward(const QString&);
	QString shiftForward();
	
	void clearForward();
	//void clearBackward();
	
	bool checkCanGoUp();
		
	void matchCurrentDirToBookmark();
		
	QStringList makeModelFilterList(const QString &userFilter);

	QWidget		* m_searchBase;
	QPushButton	* m_clearSearchBtn;
	QLineEdit	* m_searchBox;
	
	QPushButton	* m_btnBack;
	QPushButton	* m_btnForward;
	QPushButton	* m_btnUp;
	
	QStringList	m_pathsBackward;
	QStringList	m_pathsForward;
	
	QHash<QString,QModelIndex> m_lastIndexForPath;
	
	QListView 	* m_listView;
	DirectoryListModel * m_fsModel;
	
	QStringList	m_currentTypeFilterList;
	
	QString		m_currentDirectory;
	
	QWidget		* m_folderBoxBase;
	QLineEdit	* m_dirBox;
	QComboBox	* m_filterBox;
	
	QStringList	m_filteTypeFilterList;
	
	QWidget		* m_btnBase;
	QPushButton	* m_btnAddToSchedue;
	QPushButton	* m_btnSetAsBgLive;
	QPushButton	* m_btnSetAsBgLater;
	QPushButton	* m_btnSetAsBgCurrent;
	
	SlideGroupViewer * m_viewer;
	QSplitter	* m_splitter;
	
	QString		m_prevPathKey;
	
	bool		m_backgroundActionsEnabled;
	
	QSize		m_iconSize;
	
	QWidget 	* m_viewerBase;
	QVBoxLayout	* m_viewerLayout;
	//QList<QWidget*>	m_controlWidgets;
	QWidget		* m_controlWidget;
	bool		m_autoPlayVideo;
	QCheckBox	* m_autoPlayCheckbox;
	
	QWidget 	* m_bookmarkBase;
	QComboBox	* m_bookmarkBox;
	QPushButton	* m_btnBookmark;
	QPushButton	* m_btnDelBookmark;
	bool		m_ignoreBookmarkIdxChange;
	
	QList<QPair<QString,QString> > m_bookmarkList;
	
	
	
};


#endif
