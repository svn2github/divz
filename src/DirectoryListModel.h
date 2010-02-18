#ifndef DirectoryListModel_H
#define DirectoryListModel_H

#include <QPixmap>
#include <QTimer>
#include <QAbstractListModel>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QDir>
#include <QHash>

#define MY_COMPUTER "My Computer"

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
	
	void setNameLengthMax(int);
	int nameLengthMax(){ return m_nameLengthMax; }
	
	// compat with QFileSystemModel API
	QModelIndex index(int row, int) { return indexForRow(row); }
	QModelIndex index(const QString& file) { return indexForFile(file); }
	void setNameFilters(const QStringList &list) { setFilters(list); } 
	
	
	
private slots:
	void makePixmaps();
	
	// for QFileSystemWatcher
// 	void directoryChanged ( const QString & path );
// 	void fileChanged ( const QString & path );
	
protected:
	virtual QPixmap generatePixmap(const QFileInfo&);
	void needPixmap(const QString&);
	
	QString cacheKey(const QFileInfo&) const;
	
	void loadEntryList();
	
	QString cacheFile(const QString& file) const;
	
	QHash<QString,QPixmap> m_pixmapCache;
	
// members:
	QHash<int,QIcon> m_iconTypeCache;
	
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
	static int m_blankPixmapRefCount;
	
	bool m_lockRowcount;
	
	int m_nameLengthMax;

	bool m_isMyComputer;
	
	
	
// 	QFileSystemWatcher * m_watcher;

};

#endif

