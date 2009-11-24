#include "ImageRecordListModel.h"
#include "ImageRecord.h"

ImageRecordListModel * ImageRecordListModel::static_instance = 0;

/* Static Accessor */
ImageRecordListModel * ImageRecordListModel::instance()
{
	if(!static_instance)
		static_instance = new ImageRecordListModel();
	return static_instance;
}

/// ** PRIVATE **

ImageRecordListModel::ImageRecordListModel()
	: QAbstractListModel()
	, m_filter("")
	, m_dirtyTimer(0)
	, m_populating(false)
{
	m_dirtyTimer = new QTimer(this);
	connect(m_dirtyTimer, SIGNAL(timeout()), this, SLOT(modelDirtyTimeout()));
	
	m_dirtyTimer->setSingleShot(true);
	
	populateList();
}

void ImageRecordListModel::populateList()
{
	// prep db
	(void)ImageRecord::db();
	
	m_populating = true;
	
	if(m_list.size() > 0)
	{
		beginRemoveRows(QModelIndex(),0,m_list.size());
	
		foreach(ImageRecord *record, m_list)
			removeRecord(record);
	
		endRemoveRows();
	}
	
	QString clause = "";
	if(!m_filter.trimmed().isEmpty())
		clause = "WHERE file like ?";
	
	QSqlQuery query;
	QString sql = QString("SELECT * FROM %1 %2 ORDER BY file").arg(IMAGEDB_TABLE).arg(clause);
	//qDebug() << "ImageRecordListModel::populateList(): sql:"<<sql;
	
	query.prepare(sql); 
	if(!m_filter.trimmed().isEmpty())
	{
		QString filter = QString("%%1%").arg(m_filter.trimmed());
		query.addBindValue(filter);
		//qDebug() << "ImageRecordListModel::populateList(): filter:"<<filter;
	}
	
	query.exec();
	
	if (query.lastError().isValid())
	{
		qDebug() << "ImageRecordListModel::populateList(): Error loading records from database:"<<query.lastError();
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
			addRecord(ImageRecord::fromQuery(query));
		
		
		endInsertRows();
		
		//qDebug() << "ImageRecordListModel::populateRecordList(): query.size():"<<sz<<", m_list.size():"<<m_list.size();
	}
// 	else
// 	{
// 		qDebug() << "ImageRecordListModel::populateRecordList(): 
// 	}
	
	m_populating = false;
}

void ImageRecordListModel::recordChanged(ImageRecord *record, QString /*field*/, QVariant /*value*/)
{
	if(m_dirtyTimer->isActive())
		m_dirtyTimer->stop();

	m_dirtyTimer->start(250);
	
	if(!m_dirtyRecords.contains(record))
		m_dirtyRecords << record;
}

namespace ImageRecordListModelSort
{
	bool record_file_compare(ImageRecord *a, ImageRecord *b)
	{
		return (a && b) ? a->file() < b->file() : true;
	}
};

void ImageRecordListModel::modelDirtyTimeout()
{
	
	qSort(m_dirtyRecords.begin(),
	      m_dirtyRecords.end(), 
	      ImageRecordListModelSort::record_file_compare);
	
	QModelIndex top    = indexForRecord(m_dirtyRecords.first()), 
	            bottom = indexForRecord(m_dirtyRecords.last());
	
	m_dirtyRecords.clear();
	
	dataChanged(top,bottom);
}

void ImageRecordListModel::sortList()
{
	qSort(m_list.begin(),
	      m_list.end(), 
	      ImageRecordListModelSort::record_file_compare);
}
	
	
/// ** END PRIVATE **

/// ** PROTECTED **
void ImageRecordListModel::addRecord(ImageRecord *record)
{
	if(!m_populating)
	{
		// dirty all rows because sorting may change
		beginInsertRows(QModelIndex(),0,m_list.size()+1);
	}
	
	m_list << record;
	connect(record, SIGNAL(recordChanged(ImageRecord*, QString, QVariant)), this, SLOT(recordChanged(ImageRecord*, QString, QVariant)));
	
	//qDebug() << "ImageRecordListModel::addRecord(): Added recordid"<<record->recordId()<<", file:"<<record->file();
	if(!m_populating)
	{
		sortList();
		endInsertRows();
	}
}

void ImageRecordListModel::removeRecord(ImageRecord *record)
{
	if(!m_populating)
	{
		// dirty all rows because sorting may change
		beginRemoveRows(QModelIndex(),0,m_list.size()-1);
	}
	
	m_list.removeAll(record);
	disconnect(record, 0, this, 0);
	
	if(!m_populating)
	{
		sortList();
		endRemoveRows();
	}
}

/// ** END PROTECTED **

/// ** PUBLIC **
	
int ImageRecordListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_list.size();
}

ImageRecord * ImageRecordListModel::recordFromIndex(const QModelIndex &index)
{
	if(!index.isValid())
		return 0;
	return recordAt(index.row());
}

ImageRecord * ImageRecordListModel::recordAt(int row)
{
	if(row < 0 || row >= m_list.size())
		return 0;
			
	return m_list.at(row);
}

static quint32 ImageRecordListModel_uidCounter = 0;
	
QModelIndex ImageRecordListModel::indexForRecord(ImageRecord *record) const
{
	ImageRecordListModel_uidCounter++;
	return createIndex(m_list.indexOf(record),0,ImageRecordListModel_uidCounter);
}

QModelIndex ImageRecordListModel::indexForRow(int row) const
{
	ImageRecordListModel_uidCounter++;
	return createIndex(row,0,ImageRecordListModel_uidCounter);
}

QVariant ImageRecordListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= m_list.size())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		//qDebug() << "ImageRecordListModel::data: VALID:"<<index.row();
		
		ImageRecord * record = m_list.at(index.row());
		return record->file();
	}
	else
		return QVariant();
}


QVariant ImageRecordListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	return QString("%1").arg(section);	
}


void ImageRecordListModel::filter(const QString &filter)
{
	if(filter != m_filter)
	{
		m_filter = filter;
		populateList();
	}
}

