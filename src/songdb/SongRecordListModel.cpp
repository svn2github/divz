#include "SongRecordListModel.h"
#include "SongRecord.h"

SongRecordListModel * SongRecordListModel::static_instance = 0;

/* Static Accessor */
SongRecordListModel * SongRecordListModel::instance()
{
	if(!static_instance)
		static_instance = new SongRecordListModel();
	return static_instance;
}

/// ** PRIVATE **

SongRecordListModel::SongRecordListModel()
	: QAbstractListModel()
	, m_filter("")
	, m_dirtyTimer(0)
	, m_populating(false)
{
	m_dirtyTimer = new QTimer(this);
	connect(m_dirtyTimer, SIGNAL(timeout()), this, SLOT(modelDirtyTimeout()));
	
	m_dirtyTimer->setSingleShot(true);
	
	populateSongList();
}

void SongRecordListModel::populateSongList()
{
	// prep db
	(void)SongRecord::db();
	
	m_populating = true;
	
	if(m_songList.size() > 0)
	{
		beginRemoveRows(QModelIndex(),0,m_songList.size());
	
		foreach(SongRecord *song, m_songList)
			removeSong(song);
	
		endRemoveRows();
	}
	
	QString clause = "";
	if(!m_filter.trimmed().isEmpty())
		clause = "WHERE title like ? OR text like ?";
	
	QSqlQuery query;
	QString sql = QString("SELECT * FROM %1 %2 ORDER BY title").arg(SONG_TABLE).arg(clause);
	//qDebug() << "SongRecordListModel::populateSongList(): sql:"<<sql;
	
	query.prepare(sql); 
	if(!m_filter.trimmed().isEmpty())
	{
		QString filter = QString("%%1%").arg(m_filter.trimmed());
		query.addBindValue(filter);
		
		filter = QString("%%1%").arg(m_filter.trimmed().replace(QRegExp("\\s{2,}")," "));
		query.addBindValue(filter);
		//qDebug() << "SongRecordListModel::populateSongList(): filter:"<<filter;
	}
	
	query.exec();
	
	if (query.lastError().isValid())
	{
		qDebug() << "SongRecordListModel::populateSongList(): Error loading songs from database:"<<query.lastError();
		return;
	}
	else
	//if(query.size() > 0)
	{
		int sz = query.size();
		//beginInsertRows(QModelIndex(),0,sz);
		
		// hack hack hack
		beginInsertRows(QModelIndex(),0,100);
		
		while(query.next())
			addSong(SongRecord::fromQuery(query));
		
		
		endInsertRows();
		
		//qDebug() << "SongRecordListModel::populateSongList(): query.size():"<<sz<<", m_songList.size():"<<m_songList.size();
	}
// 	else
// 	{
// 		qDebug() << "SongRecordListModel::populateSongList(): 
// 	}
	
	m_populating = false;
}

void SongRecordListModel::songChanged(SongRecord *song, QString /*field*/, QVariant /*value*/)
{
	if(m_dirtyTimer->isActive())
		m_dirtyTimer->stop();

	m_dirtyTimer->start(250);
	
	if(!m_dirtySongs.contains(song))
		m_dirtySongs << song;
}

namespace SongRecordListModelSort
{
	bool song_title_compare(SongRecord *a, SongRecord *b)
	{
		return (a && b) ? a->title() < b->title() : true;
	}
};

void SongRecordListModel::modelDirtyTimeout()
{
	
	qSort(m_dirtySongs.begin(),
	      m_dirtySongs.end(), 
	      SongRecordListModelSort::song_title_compare);
	
	QModelIndex top    = indexForSong(m_dirtySongs.first()), 
	            bottom = indexForSong(m_dirtySongs.last());
	
	m_dirtySongs.clear();
	
	dataChanged(top,bottom);
}

void SongRecordListModel::sortList()
{
	qSort(m_songList.begin(),
	      m_songList.end(), 
	      SongRecordListModelSort::song_title_compare);
}
	
	
/// ** END PRIVATE **

/// ** PROTECTED **
void SongRecordListModel::addSong(SongRecord *song)
{
	if(!m_populating)
	{
		// dirty all rows because sorting may change
		beginInsertRows(QModelIndex(),0,m_songList.size()+1);
	}
	
	m_songList << song;
	connect(song, SIGNAL(songChanged(SongRecord*, QString, QVariant)), this, SLOT(songChanged(SongRecord*, QString, QVariant)));
	
	//qDebug() << "SongRecordListModel::addSong(): Added songid"<<song->songId()<<", title:"<<song->title();
	if(!m_populating)
	{
		sortList();
		endInsertRows();
	}
}

void SongRecordListModel::removeSong(SongRecord *song)
{
	if(!m_populating)
	{
		// dirty all rows because sorting may change
		beginRemoveRows(QModelIndex(),0,m_songList.size()-1);
	}
	
	m_songList.removeAll(song);
	disconnect(song, 0, this, 0);
	
	if(!m_populating)
	{
		sortList();
		endRemoveRows();
	}
}

/// ** END PROTECTED **

/// ** PUBLIC **
	
int SongRecordListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_songList.size();
}

SongRecord * SongRecordListModel::songFromIndex(const QModelIndex &index)
{
	if(!index.isValid())
		return 0;
	return songAt(index.row());
}

SongRecord * SongRecordListModel::songAt(int row)
{
	if(row < 0 || row >= m_songList.size())
		return 0;
			
	return m_songList.at(row);
}

static quint32 SongRecordListModel_uidCounter = 0;
	
QModelIndex SongRecordListModel::indexForSong(SongRecord *song) const
{
	SongRecordListModel_uidCounter++;
	return createIndex(m_songList.indexOf(song),0,SongRecordListModel_uidCounter);
}

QModelIndex SongRecordListModel::indexForRow(int row) const
{
	SongRecordListModel_uidCounter++;
	return createIndex(row,0,SongRecordListModel_uidCounter);
}

QVariant SongRecordListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= m_songList.size())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		//qDebug() << "SongRecordListModel::data: VALID:"<<index.row();
		
		SongRecord * song = m_songList.at(index.row());
		return song->title();
	}
	else
		return QVariant();
}


QVariant SongRecordListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	return QString("%1").arg(section);	
}


void SongRecordListModel::filter(const QString &filter)
{
	if(filter != m_filter)
	{
		m_filter = filter;
		populateSongList();
	}
}

/* Drag/drop support */
Qt::ItemFlags SongRecordListModel::flags(const QModelIndex &index) const
{
	if (index.isValid())	
		return /*Qt::ItemIsEditable |*/ Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
	
	return/* Qt::ItemIsEditable | */Qt::ItemIsEnabled | Qt::ItemIsSelectable/* | Qt::ItemIsDropEnabled*/;
}




QMimeData * SongRecordListModel::mimeData(const QModelIndexList & list) const
{
	if(list.size() <= 0)
		return 0;
		
	QStringList x;
	foreach(QModelIndex idx, list)
		x << QString::number(idx.row());
	
	QByteArray ba;
	ba.append(x.join(","));
	
	QMimeData *data = new QMimeData();
	data->setData(itemMimeType(), ba);
	
	return data;
}

