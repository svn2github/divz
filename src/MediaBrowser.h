#ifndef MEDIABROWSER_H
#define MEDIABROWSER_H

#include <QWidget>

class QFileSystemModel;
class QListView;
class QFileInfo;
class QModelIndex;
class QPushButton;
class QLineEdit;

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
	
	void setDirectory(const QString&);

protected slots:
	void indexDoubleClicked(const QModelIndex&);
	
	void filterChanged(const QString&);
	void filterReturnPressed();

protected:
	void setupUI();

	QWidget	      * m_searchBase;
	QPushButton   * m_clearSearchBtn;
	QLineEdit     * m_searchBox;
	
	QListView * m_listView;
	QFileSystemModel * m_fsModel;
};


#endif
