#include "ArrangementListModel.h"
#include <QDebug>

#include <QPixmap>
#include <QPixmapCache>

#define DEBUG_MARK() qDebug() << "mark: "<<__FILE__<<":"<<__LINE__
#define POINTER_STRING(ptr) QString().sprintf("%p",static_cast<void*>(ptr))



ArrangementListModel::ArrangementListModel(QObject *parent)
		: QAbstractListModel(parent)
{
}

ArrangementListModel::~ArrangementListModel()
{
}

Qt::ItemFlags ArrangementListModel::flags(const QModelIndex &index) const
{
	if (index.isValid())	
		return /*Qt::ItemIsEditable |*/ Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	
	return/* Qt::ItemIsEditable | */Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
}


bool ArrangementListModel::dropMimeData ( const QMimeData * data, Qt::DropAction action, int /*row*/, int /*column*/, const QModelIndex & parent )
{
	QStringList newList;
	QStringList dropped;
	QStringList oldGroups = m_blocks;
	
	int parentRow = parent.row();
	if(parentRow < 0)
		parentRow = oldGroups.size()-1;
		
	// Check to see if this drag/drop came from an external source
	if(!data->hasFormat(itemMimeType()))
	{
		if(data->hasFormat(interListMimeType()))
		{
			QByteArray ba = data->data(interListMimeType());
			QStringList list = QString(ba).split(",");
			
			int count = 0;
			
			QStringList newGroups = list;
		
			for(int i=0;i<parentRow+1;i++)
			{
				newList << oldGroups.at(i);
				count ++;
			}
			
			foreach(QString group, newGroups)
			{
				//qDebug() << "DocumentListModel::dropMimeData(n): i:(new), count:"<<count<<", title:"<<group->groupTitle();
				newList << group;
				dropped << group;
				count++;
			}
			
			// add in the rest of the slides
			for(int i=parentRow+1;i<oldGroups.size();i++)
			{
				//qDebug() << "DocumentListModel::dropMimeData(2): i:"<<i<<", count:"<<count<<", title:"<<x->groupTitle();
				newList << oldGroups.at(i);;
				count ++;
			}
		}
		else
		{
			qDebug() << "ArrangementListModel::dropMimeData: Rejecting external data";
			return false;
		}
	}
	
	if(newList.isEmpty())
	{
		//qDebug() << "ArrangementListModel::dropMimeData: Processing INTERNAL drop";
		QByteArray ba = data->data(itemMimeType());
		QStringList list = QString(ba).split(",");
		
		// Invert the actions since I cant figure out how to change the action modifier keys
		action = action == Qt::MoveAction ? Qt::CopyAction : Qt::MoveAction; 
		
		// convert csv list to integer list of block indexes
		QList<int> removed;
		for(int i=0;i<list.size();i++)
		{
			int x = list.at(i).toInt();
			removed << x;
		}
		
		//qDebug() << "dropMimeData: def[copy="<<Qt::CopyAction<<",move="<<Qt::MoveAction<<"], action:"<<action<<", start size: "<<m_blocks.size();
		
		//qDebug() << "Internal drop, parent: "<<parent.row();
		
	// 	if(action == Qt::CopyAction)
	// 		beginInsertRows(QModelIndex(),parent.row()+1,list.size());
		
		// add the slides from start to parent row
		for(int i=0;i<parentRow+1;i++)
			if(!removed.contains(i) || action == Qt::CopyAction)
				newList << m_blocks.at(i);
		
		// add in the dropped slides
		foreach(int x, removed)
		{
			QString block = m_blocks.at(x);
			newList << block;
			dropped << block;
		}
		
		// add in the rest of the slides
		for(int i=parentRow+1;i<m_blocks.size();i++)
			if(!removed.contains(i) || action == Qt::CopyAction)
				newList << m_blocks.at(i);
	}
	
	m_blocks = newList;
	
 	if(action == Qt::CopyAction)
 		internalSetup();
 	else
 	{
 		//endInsertRows();
		
		//qDebug() << "dropMimeData: end size: "<<m_blocks.size();
		
		//qSort(m_blocks.begin(), m_blocks.end(), group_num_compare);
		
		QModelIndex top    = createIndex(0,0),
			    bottom = createIndex(m_blocks.size()-1,0);
		
		dataChanged(top,bottom);
	}
	
	emit blocksDropped(dropped);
	emit blockListChanged(m_blocks);
	
	return true;	
}

QMimeData * ArrangementListModel::mimeData(const QModelIndexList & list) const
{
	if(list.size() <= 0)
		return 0;
	
	QStringList idList;
	QStringList blockList;
	foreach(QModelIndex idx, list)
	{
		idList << QString::number(idx.row());
		blockList << m_blocks.at(idx.row());
	}
	
	QByteArray idBytes;
	idBytes.append(idList.join(","));
	
	QByteArray blockBytes;
	blockBytes.append(blockList.join(","));
	
	QMimeData *data = new QMimeData();
	data->setData(itemMimeType(), idBytes);
	data->setData(interListMimeType(), blockBytes);
	
	return data;
}

bool ArrangementListModel::setData(const QModelIndex & index, const QVariant & value, int /*role*/)
{
	m_blocks[index.row()] = value.toString();
	
	emit dataChanged(index,index);
	return true;
}



void ArrangementListModel::setBlockList(const QStringList& list)
{
	if(!m_blocks.isEmpty())
	{
		int sz = m_blocks.size();
		beginRemoveRows(QModelIndex(),0,sz); // hack - yes, I know
		
		m_blocks.clear();
		
		endRemoveRows();
	}
	
	m_blocks = list;
	
	beginInsertRows(QModelIndex(),0,list.size()-1);
	
	internalSetup();
	
	endInsertRows();
}

void ArrangementListModel::internalSetup()
{
	if(m_blocks.isEmpty())
		return;
	
	QModelIndex top    = createIndex(0,0),
			bottom = createIndex(m_blocks.size()-1,0);
	
	dataChanged(top,bottom);
}
	
int ArrangementListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_blocks.size();
}

QString ArrangementListModel::blockFromIndex(const QModelIndex &index)
{
	return m_blocks.at(index.row());
}

QString ArrangementListModel::blockAt(int row)
{
	return m_blocks.at(row);
}


QModelIndex ArrangementListModel::indexForRow(int row) const
{
	return createIndex(row,0);
}

QVariant ArrangementListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= m_blocks.size())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		return "";
		//QString block = m_blocks.at(index.row());
		//return block;
	}
	else if(Qt::DecorationRole == role)
	{
		QString block = m_blocks.at(index.row());
		QString cacheKey = tr("block%1").arg(block);
		
		QPixmap icon;
		if(!QPixmapCache::find(cacheKey,icon))
		{
			ArrangementListModel * self = const_cast<ArrangementListModel*>(this);
			icon = self->generatePixmap(block);
			QPixmapCache::insert(cacheKey,icon);
		}
		
		return icon;
	}
	else
		return QVariant();
}

QPixmap ArrangementListModel::generatePixmap(QString block)
{
	QFont font("",9,QFont::Bold); // 9pt default font
	QFontMetrics fm(font);
	QRect rect = fm.boundingRect(block);
	
	QPixmap icon(rect.width()+10,rect.height()+10);
	icon.fill(Qt::transparent);
	
	QPainter p(&icon);
	p.setPen(Qt::blue);
	p.setBrush(Qt::gray);
	p.drawRoundedRect(icon.rect().adjusted(1,1,-1,-1),10.,10.);
	p.setPen(Qt::white);
	p.drawText(9,fm.height()+4,block);
	p.setPen(Qt::black);
	p.drawText(8,fm.height()+3,block);
	p.end();
	
	return icon;
	
}


QVariant ArrangementListModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	return QString("Block %1").arg(section);
}
