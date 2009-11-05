#ifndef DirectoryListModel_H
#define DirectoryListModel_H

#include <QPixmap>
#include <QTimer>
#include <QAbstractListModel>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QDir>

class DirectoryListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	DirectoryListModel(QObject *parent=0);
	~DirectoryListModel();
	
	// QAbstractListModel::
	int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

	// DirectoryListModel::
	void setDirectory(const QDir&);
	void setDirectory(const QString&);
	QDir currentDirectory() const { return m_dir; }
	QString currentPath() const { return m_dir.absolutePath(); }
	
	QModelIndex indexForFile(const QString&);
	QModelIndex indexForFile(const QFileInfo&);
	QModelIndex indexForRow(int row);
	
	QFileInfo fileInfo(const QModelIndex&) const;
	QFileInfo fileInfo(int row) const;
	
	QFileIconProvider * iconProvider() ;
	void setIconProvider(QFileIconProvider*);
	
	void setFilters(const QStringList &);
	QStringList filters() const { return m_filters; }
	
	void setIconSize(const QSize&);
	QSize iconSize() { return m_iconSize; }
	
private slots:
	void makePixmaps();
	
	// for QFileSystemWatcher
// 	void directoryChanged ( const QString & path );
// 	void fileChanged ( const QString & path );
	
private:
	QPixmap generatePixmap(const QFileInfo&);
	void needPixmap(const QString&);
	
	QString cacheKey(const QFileInfo&) const;
	
	void loadEntryList();
	
// members:
	QDir m_dir;
	QStringList m_filters;
	
	QStringList m_entryList;
	bool m_listLoaded;
	
	
	QStringList m_needPixmaps;

	QTimer m_needPixmapTimer;
	
	QSize m_iconSize;
	
	QFileIconProvider * m_iconProvider;
	QFileIconProvider m_defaultIconProvider;
	
	static QPixmap * m_blankPixmap;
	
	bool m_lockRowcount;
	
	
// 	QFileSystemWatcher * m_watcher;

};

#endif

