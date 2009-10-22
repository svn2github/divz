#ifndef MEDIABROWSER_H
#define MEDIABROWSER_H

#include <QWidget>

class QFileSystemModel;
class QListView;
class QFileInfo;
class QModelIndex;
class QPushButton;
class QLineEdit;
#include <QStringList>

class MediaBrowser : public QWidget
{
	Q_OBJECT

public:
	MediaBrowser(QWidget *parent=0);
	~MediaBrowser();
	
signals:
	void fileSelected(const QFileInfo&);
	void setBackground(const QFileInfo&, bool waitForNextSlide);
	
public slots:
	void setFilter(const QString&);
	void clearFilter() { setFilter(""); }
	
	void setDirectory(const QString&, bool addToHistory = true);
	
	void goBack();
	void goForward();
	void goUp();
	
protected slots:
	void indexDoubleClicked(const QModelIndex&);
	
	void filterChanged(const QString&);
	void filterReturnPressed();
	
	//void fileTypeChanged(const QString&);

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

	QWidget	      * m_searchBase;
	QPushButton   * m_clearSearchBtn;
	QLineEdit     * m_searchBox;
	
	QPushButton   * m_btnBack;
	QPushButton   * m_btnForward;
	QPushButton   * m_btnUp;
	
	QStringList	m_pathsBackward;
	QStringList	m_pathsForward;
	
	QListView * m_listView;
	QFileSystemModel * m_fsModel;
	
	QString		m_fileTypeFilter;
	
	QString		m_currentDirectory;
};


#endif
