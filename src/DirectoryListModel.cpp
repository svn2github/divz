#include "DirectoryListModel.h"

// #include <QPixmap>
// #include <QTimer>
// #include <QAbstractListModel>
// #include <QFileIconProvider>
// #include <QFileInfo>

#include <QPainter>
#include <QPixmapCache>
#include <QDebug>


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

QVariant DirectoryListModel::data ( const QModelIndex & index, int role ) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= rowCount())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		QString file = fileInfo(index).fileName();
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
				self->needPixmap(info.canonicalFilePath());
				return *m_blankPixmap;
 			}
 			else
 			{
 				QFileIconProvider::IconType type = 
					    info.isDir() ?  QFileIconProvider::Folder:
					    info.isFile() ? QFileIconProvider::File  :
					    info.isRoot() ? QFileIconProvider::Drive :
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
	m_needPixmaps.clear();
	m_dir = d;
	m_listLoaded = false;
	loadEntryList();
}

void DirectoryListModel::setDirectory(const QString& str)
{
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
	return row < m_entryList.size() ? 
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
	if(m_needPixmaps.isEmpty())
	{
		m_needPixmapTimer.stop();
		return;
	}
	
	QFileInfo info = QFileInfo(m_needPixmaps.takeFirst());
	qDebug() << "DirectoryListModel::makePixmaps: file:"<<info.canonicalFilePath()<<", remaining size:"<<m_needPixmaps.size();
	
	QString key = cacheKey(info);
	QPixmapCache::remove(key);
		
	QPixmap icon = generatePixmap(info);
	QPixmapCache::insert(key,icon);
	
	m_needPixmapTimer.stop();
	
	QModelIndex idx = indexForFile(info);
	dataChanged(idx,idx);
	
	if(!m_needPixmaps.isEmpty())
		m_needPixmapTimer.start(NEED_PIXMAP_TIMEOUT_FAST);
}
	
	// for QFileSystemWatcher
// 	void directoryChanged ( const QString & path );
// 	void fileChanged ( const QString & path );
	

/** private **/
QPixmap DirectoryListModel::generatePixmap(const QFileInfo& info)
{
	qDebug() << "DirectoryListModel::generatePixmap: file:"<<info.canonicalFilePath();
	
	QIcon icon = iconProvider()->icon(info);
	if(icon.isNull())
	{
		QFileIconProvider::IconType type = 
					    info.isDir() ? QFileIconProvider::Folder :
					    info.isFile() ? QFileIconProvider::File  :
					    info.isRoot() ? QFileIconProvider::Drive :
					    QFileIconProvider::File;
		icon = iconProvider()->icon(type);
		//qDebug() << "DirectoryListModel::generatePixmap: file:"<<info.canonicalFilePath()<<", type:"<<type<<", null?"<<icon.isNull();
	}
	
	if(icon.isNull())
		return *m_blankPixmap;
		
	QPixmap pixmap = icon.pixmap(m_iconSize);
	if(pixmap.isNull())
		return *m_blankPixmap;
	
	return pixmap;
}

void DirectoryListModel::needPixmap(const QString& file)
{
	qDebug() << "DirectoryListModel::needPixmap: file:"<<file;
	if(!m_needPixmaps.contains(file))
		m_needPixmaps.append(file);
	if(!m_needPixmapTimer.isActive())
		m_needPixmapTimer.start(NEED_PIXMAP_TIMEOUT);
}

bool DirectoryListModel_sortFileList(QString a, QString b)
{
	QFileInfo ia(a), ib(b);
	if((ia.isDir() && ib.isDir()) ||
	   (ia.isFile() && ib.isFile()))
		return a.toLower() < b.toLower();
	else
	if(ia.isDir() && ib.isFile())
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
	
	QStringList entries = m_dir.entryList(m_filters, 
		QDir::AllDirs | 
		QDir::Drives  | 
		QDir::NoDotAndDotDot |
		QDir::Files
		);
	QString path = m_dir.absolutePath();
	
	beginInsertRows(QModelIndex(),0,entries.size());
	foreach(QString file, entries)
	{
		m_entryList << QFileInfo(QString("%1/%2").arg(path,file)).canonicalFilePath();
		//needPixmap(file);
	}
	qSort(m_entryList.begin(),m_entryList.end(),DirectoryListModel_sortFileList);
	endInsertRows();
}

