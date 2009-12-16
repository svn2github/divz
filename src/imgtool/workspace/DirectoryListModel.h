#ifndef DirectoryListModel_H
#define DirectoryListModel_H

#include <QPixmap>
#include <QTimer>
#include <QAbstractListModel>
#include <QFileIconProvider>
#include <QFileInfo>
#include <QDir>
#include <QHash>
#include <QRunnable>
#include <QMutex>

class ThreadedIconProvider;
class LoadImageTask : public QObject, public QRunnable
{
	Q_OBJECT
public:
	LoadImageTask(const QFileInfo &info, const QSize&, QList<QSize> standardSizesNeeded, ThreadedIconProvider*);
	void run();
	
signals:
	void imageLoaded(const QFileInfo&, const QSize&, const QImage&, bool explicitRequest);
	void taskFinished(const QFileInfo&);
	
private:	
	QFileInfo m_info;
	QSize m_size;
	QList<QSize> m_need; 
	ThreadedIconProvider * m_provider;
};

class ThreadedIconProvider : public QObject
{
	Q_OBJECT
public:
	ThreadedIconProvider();
	bool requestIcon(const QFileInfo&, const QSize&);
	
	int queueSize();
	
	void clearQueue();
	
	void prioritize(const QFileInfo&);
	
// 	typedef enum StandardSize
// 	{
// 		_160x120,
// 		_320x240,
// 		_640x480,
// 		_1024x768,
// 		_1600x1200
// 	};

signals:
	void iconReady(const QFileInfo&, const QPixmap&);
	
	void queueSizeChanged(int);
	void queueEmpty();
	
protected:
	friend class LoadImageTask;
	
	bool isSizeCached(const QFileInfo&, const QSize&);
	QList<QSize> standardSizes(); 
	
	QSize nextLargestStandardSize(const QSize&);
	
	QString cacheKey(const QFileInfo&, const QSize&);
	QString cacheFile(const QString&);
	
private slots:
	void imageLoaded(const QFileInfo&, const QSize&, const QImage&, bool explicitRequest);
	void taskFinished(const QFileInfo&);
	
	void processFirstItem();
	
private:
	int m_queueSize;
	QMutex m_queueSizeMutex;
	QList<QSize> m_standardSizes;
	QList<QString> m_inProcess;
	QHash<QString,QSize> m_sizesInProcess;
	
};

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
	
	void setEntryList(const QStringList & list);
	QStringList entryList() { return m_entryList; }
	
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
	
	
public slots:
	void prioritize(const QFileInfo&);
	void prioritize(const QModelIndex& idx) { prioritize(fileInfo(idx)); }
	
	
private slots:
	void makePixmaps();
	void iconReady(const QFileInfo&, const QPixmap&);
	
	// for QFileSystemWatcher
// 	void directoryChanged ( const QString & path );
// 	void fileChanged ( const QString & path );
	
protected:
	virtual QPixmap generatePixmap(const QFileInfo&);
	void needPixmap(const QString&);
	
	QString cacheKey(const QFileInfo&) const;
	
	void loadEntryList();
	
// members:
	QHash<int,QIcon> m_iconTypeCache;
	
	bool m_dirNull;
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
	
	ThreadedIconProvider * m_threadedIconProvider;
	
	
// 	QFileSystemWatcher * m_watcher;

};

#endif

