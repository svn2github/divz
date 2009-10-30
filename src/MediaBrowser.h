#ifndef MEDIABROWSER_H
#define MEDIABROWSER_H

#include <QWidget>

class QFileSystemModel;
class QListView;
class QFileInfo;
class QModelIndex;
class QPushButton;
class QLineEdit;
class QComboBox;
class QSplitter;
class SlideGroupViewer;
#include <QStringList>
#include <QRegExp>

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
	MediaBrowser(QWidget *parent=0);
	~MediaBrowser();
	
	QByteArray saveState();
	bool restoreState (const QByteArray & state);
	
signals:
	void fileSelected(const QFileInfo&);
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

protected:
	void setupUI();
	
	void pushBackward(const QString&);
	QString popBackward();
	
	void unshiftForward(const QString&);
	QString shiftForward();
	
	void clearForward();
	//void clearBackward();
	
	bool checkCanGoUp();
	
	QStringList makeModelFilterList(const QString &userFilter);

	QWidget		* m_searchBase;
	QPushButton	* m_clearSearchBtn;
	QLineEdit	* m_searchBox;
	
	QPushButton	* m_btnBack;
	QPushButton	* m_btnForward;
	QPushButton	* m_btnUp;
	
	QStringList	m_pathsBackward;
	QStringList	m_pathsForward;
	
	QListView 	* m_listView;
	QFileSystemModel * m_fsModel;
	
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
	
};


#endif
