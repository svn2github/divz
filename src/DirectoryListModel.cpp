#include "DirectoryListModel.h"

// #include <QPixmap>
// #include <QTimer>
// #include <QAbstractListModel>
// #include <QFileIconProvider>
// #include <QFileInfo>

#include <QFSFileEngine>

#include "AppSettings.h"

#include "3rdparty/md5/qtmd5.h"

#include <QPainter>
#include <QPixmapCache>
#include <QDebug>

// Just so we can access MainWindow::isTransitionActive() through the MainWindow::mw() pointer
#include "MainWindow.h"

#define CACHE_DIR "dviz-directorylist"

#define NEED_PIXMAP_TIMEOUT 100
#define NEED_PIXMAP_TIMEOUT_FAST 50

QPixmap * DirectoryListModel::m_blankPixmap = 0;
int DirectoryListModel::m_blankPixmapRefCount = 0;

DirectoryListModel::DirectoryListModel(QObject *parent)
	: QAbstractListModel(parent)
	, m_iconProvider(0)
	, m_lockRowcount(false)
	, m_iconSize(32,32)
	, m_nameLengthMax(16)
	, m_isMyComputer(false)
{
	if(!m_blankPixmap)
	{
		//double ratio = AppSettings::liveAspectRatio();
		//double ratio = 4/3;
		m_blankPixmap = new QPixmap(m_iconSize); //48,(int)(48 * (1/ratio)));
		m_blankPixmap->fill(Qt::lightGray);
		QPainter painter(m_blankPixmap);
		painter.setPen(QPen(Qt::black,1,Qt::DotLine));
		painter.drawRect(m_blankPixmap->rect().adjusted(0,0,-1,-1));
		painter.end();
	}
	
	m_blankPixmapRefCount ++;
	
	m_iconTypeCache[QFileIconProvider::Folder] = iconProvider()->icon(QFileIconProvider::Folder);
	m_iconTypeCache[QFileIconProvider::Drive]  = iconProvider()->icon(QFileIconProvider::Drive);
	m_iconTypeCache[QFileIconProvider::File]   = iconProvider()->icon(QFileIconProvider::File);
	
	
	connect(&m_needPixmapTimer, SIGNAL(timeout()), this, SLOT(makePixmaps()));
}

DirectoryListModel::~DirectoryListModel()
{
	m_blankPixmapRefCount --;
	if(m_blankPixmapRefCount <= 0)
	{
		delete m_blankPixmap;
		m_blankPixmap = 0;
	}

}
	
// QAbstractListModel::
int DirectoryListModel::rowCount ( const QModelIndex & /*parent */) const
{
	return m_entryList.size();
}

QString DirectoryListModel::cacheFile(const QString& file) const
{
	QPixmap cache;
	QDir path(QString("%1/%2").arg(AppSettings::cachePath()).arg(CACHE_DIR));
	if(!path.exists())
		QDir(AppSettings::cachePath()).mkdir(CACHE_DIR);
	
	return QString("%1/%2/%3.gif").arg(AppSettings::cachePath()).arg(CACHE_DIR).arg(MD5::md5sum(file));
}

QVariant DirectoryListModel::data ( const QModelIndex & index, int role ) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= rowCount())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		QFileInfo info = fileInfo(index);
		if(m_isMyComputer)
			return info.absoluteFilePath();

		QString file = info.fileName();
		return file;
		//return file.left(m_nameLengthMax) + (file.length() > m_nameLengthMax ? "..." : "");
	}
	else
	if (role == Qt::ToolTipRole)
	{
		return fileInfo(index).fileName();
	}
	else if(Qt::DecorationRole == role)
	{
		QFileInfo info = fileInfo(index);
		QString   key  = cacheKey(info);
		
		QPixmap icon;
		if(!QPixmapCache::find(key,icon))
		{
			DirectoryListModel * self = const_cast<DirectoryListModel*>(this);
 			if(info.isFile())
 			{
 				QString file = info.canonicalFilePath();
				if(m_pixmapCache.contains(file))
				{
					qDebug() << "Pixmaps pre-cached for "<<file;
					return m_pixmapCache[file];
				}

				self->needPixmap(file);
				//return *m_blankPixmap;
				QIcon iconObject = m_iconTypeCache[QFileIconProvider::File];
				return iconObject.pixmap(m_iconSize);
 			}
 			else
 			{
 				QFileIconProvider::IconType type = 
					    m_isMyComputer || info.isRoot() ? QFileIconProvider::Drive :
					    info.isDir() ?  QFileIconProvider::Folder:
					    info.isFile() ? QFileIconProvider::File  :
					    QFileIconProvider::File;
				
				QIcon iconObject = m_iconTypeCache[type];
				icon = iconObject.pixmap(m_iconSize);
 				
 				QPixmapCache::insert(key,icon);
 			}
		}
		
		return icon;
	}
	else
		return QVariant();
}

// DirectoryListModel::
void DirectoryListModel::setDirectory(const QDir& d)
{
	m_needPixmapTimer.stop();
	m_pixmapCache.clear();
	m_needPixmaps.clear();
	m_dir = d;
	m_listLoaded = false;
	loadEntryList();
}

void DirectoryListModel::setDirectory(const QString& str)
{
	if(str == MY_COMPUTER)
		m_isMyComputer = true;
	else
		m_isMyComputer = false;
	setDirectory(QDir(str));
}

QModelIndex DirectoryListModel::indexForFile(const QString& file)
{
	return indexForRow(m_entryList.indexOf(file));
}

QModelIndex DirectoryListModel::indexForFile(const QFileInfo& info)
{
	return indexForFile(info.canonicalFilePath());
}

QModelIndex DirectoryListModel::indexForRow(int row)
{
	return createIndex(row,0);
}


QString DirectoryListModel::cacheKey(const QFileInfo& info) const
{
	return QString("dviz-dlm-%1").arg(info.canonicalFilePath());
}

QFileInfo DirectoryListModel::fileInfo(int row) const
{
// 	if(row < 0)
// 	{
// 		qDebug() << "DirectoryListModel::fileInfo: row less than zero!";
// 	}
	
	return row >= 0 && row < m_entryList.size() ? 
		QFileInfo(m_entryList.at(row)) : 
		QFileInfo();
}

QFileInfo DirectoryListModel::fileInfo(const QModelIndex &idx) const
{
	return fileInfo(idx.row());
}

QFileIconProvider * DirectoryListModel::iconProvider() 
{
	return m_iconProvider ? m_iconProvider : &m_defaultIconProvider;
}

void DirectoryListModel::setIconProvider(QFileIconProvider *provider)
{
	m_iconProvider = provider;
}
	
void DirectoryListModel::setFilters(const QStringList &list)
{
	m_filters = list;
	m_listLoaded = false;
	loadEntryList();
}

void DirectoryListModel::setIconSize(const QSize& size)
{
	m_iconSize = size;
	//foreach(QString info, m_entryList)
	//	needPixmap(info);
}

/** slots **/
void DirectoryListModel::makePixmaps()
{	
	// Avoid generating a pixmap while the transiton is active because
	// the pixmap rendering is a potentially CPU-intensive and time-costly
	// routine which would likely cause the transition to stutter and not
	// render smoothly.
	if(MainWindow::mw() &&
	   MainWindow::mw()->isTransitionActive())
	   return;

	QTime t2;
	t2.start();
	
	if(m_needPixmaps.isEmpty())
	{
		m_needPixmapTimer.stop();
		return;
	}
	
	QFileInfo info = QFileInfo(m_needPixmaps.takeFirst());
	qDebug() << "DirectoryListModel::makePixmaps: file:"<<info.canonicalFilePath()<<", remaining size:"<<m_needPixmaps.size();
	
	QString key = cacheKey(info);
	QPixmapCache::remove(key);
	
	QString file = info.canonicalFilePath();
	QString cacheFilename = cacheFile(file);

	QPixmap icon;

	if(QFile(cacheFilename).exists())
	{
		QTime t;
		t.start();
		icon = QPixmap(cacheFilename);
		//qDebug() << "::makePixmaps, load from disk, elapsed:"<<t.elapsed();
	}
	else
	{
		icon = generatePixmap(info);
		icon.save(cacheFilename,"GIF");
	}

	QPixmapCache::insert(key,icon);
	m_pixmapCache[file] = icon;

	
	m_needPixmapTimer.stop();
	
	if(!m_needPixmaps.isEmpty())
		m_needPixmapTimer.start(NEED_PIXMAP_TIMEOUT_FAST);
	else
	{
		QModelIndex first = indexForFile(m_entryList.first());
		QModelIndex last = indexForFile(m_entryList.last());
		dataChanged(first,last);
	}
	
	//qDebug() << "::makePixmaps, end, elapsed:"<<t2.elapsed();
}
	
	// for QFileSystemWatcher
// 	void directoryChanged ( const QString & path );
// 	void fileChanged ( const QString & path );
	

/** private **/
QPixmap DirectoryListModel::generatePixmap(const QFileInfo& info)
{
	qDebug() << "DirectoryListModel::generatePixmap: file:"<<info.canonicalFilePath();
	QTime timer;
	timer.start();
	
	QIcon icon = iconProvider()->icon(info);
	if(icon.isNull())
	{
		QFileIconProvider::IconType type = 
					    info.isRoot() ? QFileIconProvider::Drive :
					    info.isDir() ? QFileIconProvider::Folder :
					    info.isFile() ? QFileIconProvider::File  :
					    QFileIconProvider::File;
		icon = iconProvider()->icon(type);
		//qDebug() << "DirectoryListModel::generatePixmap: file:"<<info.canonicalFilePath()<<", type:"<<type<<", null?"<<icon.isNull();
	}
	
	if(icon.isNull())
	{
		qDebug() << "DirectoryList::generatePixmap: icon is null, time:"<<timer.elapsed();
		return *m_blankPixmap;
	}
		
	QPixmap pixmap = icon.pixmap(m_iconSize);
	if(pixmap.isNull())
	{
		qDebug() << "DirectoryList::generatePixmap: pixmap is null, time:"<<timer.elapsed();
		return *m_blankPixmap;
	}

	qDebug() << "DirectoryList::generatePixmap: finished, time:"<<timer.elapsed();
	
	return pixmap;
}

void DirectoryListModel::needPixmap(const QString& file)
{
	//qDebug() << "DirectoryListModel::needPixmap: file:"<<file;
	if(!m_needPixmaps.contains(file))
		m_needPixmaps.append(file);
	if(!m_needPixmapTimer.isActive())
		m_needPixmapTimer.start(NEED_PIXMAP_TIMEOUT);
}

bool DirectoryListModel_sortFileList(QString a, QString b)
{
	//QFileInfo ia(a), ib(b);
	QFileInfo ia(a), ib(b);
	if((ia.isDir() && ib.isDir()) ||
	   (ia.isFile() && ib.isFile()))
		return a.toLower() < b.toLower();
	else
	if(ia.isDir() && ib.isFile())
		return true;
	else
		return false;


	
/*	if((m_isDir[a] && m_isDir[b]) ||
	   (!m_isDir[a] && !m_isDir[b]))
		return a.toLower() < b.toLower();
	else
	if(m_isDir[a] && !m_isDir[b])
		return true;
	else
		return false;*/
}


typedef struct FileFlag
{
	QString file;
	bool isDir;
};

bool DirectoryListModel_sortFileFlags(FileFlag a, FileFlag b)
{
	if((a.isDir && b.isDir) ||
	   (!a.isDir && !b.isDir))
		return a.file.toLower() < b.file.toLower();
	else
	if(a.isDir && !b.isDir)
		return true;
	else
		return false;
}


void DirectoryListModel::loadEntryList()
{
	if(m_listLoaded)
		return;
	m_listLoaded = true;
	
	if(!m_entryList.isEmpty())
	{
		beginRemoveRows(QModelIndex(),0,m_entryList.size()); // hack - yes, I know
		m_entryList.clear();
		endRemoveRows();
	}
	
	QStringList entries;
	QString path = "";

	if(m_isMyComputer)
	{
		QFileInfoList drives = QFSFileEngine::drives();
		foreach(QFileInfo info, drives)
		{
			QString str = info.absoluteFilePath();
			str = str.left(str.length() - 1);
			entries << str;
			qDebug()<<"Drive:"<<str;
		}
	}
	else
	{
		entries = m_dir.entryList(m_filters,
			QDir::AllDirs |
			QDir::Drives  |
			QDir::NoDotAndDotDot |
			QDir::Files
			);
		path = m_dir.absolutePath();
	}
	
	beginInsertRows(QModelIndex(),0,entries.size());

	QList<FileFlag> files;
	foreach(QString file, entries)
	{
		 
		if(m_isMyComputer)
		{
			if(QFileInfo(file).isDir())
			{
				//m_isDir[file] = true;
				//m_entryList << file;

				FileFlag flag;
				flag.file = file;
				flag.isDir = true;
				files << flag;
			}
		}
		else
		{
			//QFileInfo info(file);
			QFileInfo info(QString("%1/%2").arg(path,file));
			QString entry = info.canonicalFilePath();
			//m_isDir[entry] = info.isDir();
			//m_entryList << entry;
			FileFlag flag;
			flag.file = entry;
			flag.isDir = info.isDir();
			files << flag;
		}
		//needPixmap(file);
	}
	QTime t2;
	t2.start();
	
	//qSort(m_entryList.begin(),m_entryList.end(),DirectoryListModel_sortFileList);
	qSort(files.begin(),files.end(),DirectoryListModel_sortFileFlags);

	foreach(FileFlag data, files)
	    m_entryList << data.file;
	
	//qDebug() << "qSort entry list elapsed:"<<t2.elapsed();
	
	endInsertRows();
	
	
}

