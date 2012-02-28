#include "DocumentListModel.h"
#include <QDebug>

#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif


#include "model/SlideGroupFactory.h"
#include "model/SlideGroup.h"
#include "model/Slide.h"
#include "model/Document.h"

#include "MyGraphicsScene.h"
#include "MainWindow.h"
#include "DeepProgressIndicator.h"
#include "AppSettings.h"

#include <QPixmap>
#include <QPixmapCache>

#define DEBUG_MARK() qDebug() << "mark: "<<__FILE__<<":"<<__LINE__
#define POINTER_STRING(ptr) QString().sprintf("%p",static_cast<void*>(ptr))

#define NEED_PIXMAP_TIMEOUT 150
#define NEED_PIXMAP_TIMEOUT_FAST 100
#define DIRTY_TIMEOUT 250

// blank 4x3 pixmap
QPixmap * DocumentListModel::m_blankPixmap = 0;
int DocumentListModel::m_blankPixmapRefCount = 0;

bool group_num_compare(SlideGroup *a, SlideGroup *b)
{
	return (a && b) ? a->groupNumber() < b->groupNumber() : true;
}



DocumentListModel::DocumentListModel(Document *d, QObject *parent)
		: QAbstractListModel(parent), 
			m_doc(d),/* m_scene(0), m_view(0),*/ 
			m_iconSize(48,0), 
			m_dirtyTimer(0), 
			m_sceneRect(0,0,1024,768),
			m_queuedIconGenerationMode(false)
{
	if(!m_blankPixmap)
	{
		m_blankPixmap = new QPixmap(48,48 * (1/AppSettings::liveAspectRatio()));
		m_blankPixmap->fill(Qt::lightGray);
		QPainter painter(m_blankPixmap);
		painter.setPen(QPen(Qt::black,1,Qt::DotLine));
		painter.drawRect(m_blankPixmap->rect().adjusted(0,0,-1,-1));
		painter.end();
	}
	m_blankPixmapRefCount ++;
	
	connect(&m_needPixmapTimer, SIGNAL(timeout()), this, SLOT(makePixmaps()));
		
	if(m_doc)
		setDocument(d);
		
	if(MainWindow::mw())
	{
		m_sceneRect = MainWindow::mw()->standardSceneRect();
		connect(MainWindow::mw(), SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
	}
			
	setSceneRect(m_sceneRect);
}

DocumentListModel::~DocumentListModel()
{
// 	if(m_scene)
// 	{
// 		delete m_scene;
// 		m_scene = 0;
// 	}
// 	
// 	if(m_view)
// 	{
// 		delete m_view;
// 		m_view = 0;
// 	}

	m_blankPixmapRefCount --;
	if(m_blankPixmapRefCount <= 0)
	{
		delete m_blankPixmap;
		m_blankPixmap = 0;
	}
}

void DocumentListModel::aspectRatioChanged(double /*x*/)
{
	//qDebug() << "DocumentListModel::aspectRatioChanged: x:"<<x;
	setSceneRect(MainWindow::mw()->standardSceneRect());
}

Qt::ItemFlags DocumentListModel::flags(const QModelIndex &index) const
{
	if (index.isValid())	
		return /*Qt::ItemIsEditable |*/ Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	
	return/* Qt::ItemIsEditable | */Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
}


#include "songdb/SongSlideGroup.h"
#include "songdb/SongRecord.h"

bool DocumentListModel::dropMimeData ( const QMimeData * data, Qt::DropAction action, int /*row*/, int /*column*/, const QModelIndex & parent )
{
	QList<SlideGroup*> newList;
	QList<SlideGroup*> dropped;
	QList<SlideGroup*> oldGroups = m_sortedGroups;
	
	int parentRow = parent.row();
	if(parentRow < 0)
		parentRow = oldGroups.size()-1;
		
	// Check to see if this drag/drop came from an external source
	if(!data->hasFormat(itemMimeType()))
	{
		if(data->hasFormat(SongRecordListModel::itemMimeType()))
		{
			action = Qt::MoveAction;
			
			QByteArray ba = data->data(SongRecordListModel::itemMimeType());
			QStringList list = QString(ba).split(",");
			
			//qDebug() << "DocumentListModel::dropMimeData: Dropped songId list: "<<list;
			
			int count = 0;
			
			QList<SlideGroup*> newGroups;
			
			foreach(QString songIdString, list)
			{
				SongRecord *song = SongRecordListModel::instance()->songAt(songIdString.toInt());
				SongSlideGroup *group = new SongSlideGroup();
				group->setSong(song);
				
				//qDebug() << "DocumentListModel::dropMimeData: Dropped song: "<<song->title();
				
				m_doc->addGroup(group);
				
				newGroups << group;
			}
		
			for(int i=0;i<parentRow+1;i++)
			{
				SlideGroup *x = oldGroups.at(i);
				//qDebug() << "DocumentListModel::dropMimeData(1): i:"<<i<<", count:"<<count<<", title:"<<x->groupTitle();
				newList << x;
				count ++;
			}
			
			foreach(SlideGroup *group, newGroups)
			{
				//qDebug() << "DocumentListModel::dropMimeData(n): i:(new), count:"<<count<<", title:"<<group->groupTitle();
				newList << group;
				dropped << group;
				count++;
			}
			
			// add in the rest of the slides
			for(int i=parentRow+1;i<oldGroups.size();i++)
			{
				SlideGroup *x = oldGroups.at(i);
				//qDebug() << "DocumentListModel::dropMimeData(2): i:"<<i<<", count:"<<count<<", title:"<<x->groupTitle();
				newList << x;
				count ++;
			}
		}
		else
		{
			qDebug() << "DocumentListModel::dropMimeData: Rejecting external data";
			return false;
		}
	}
	
	
	if(newList.isEmpty())
	{
		//qDebug() << "DocumentListModel::dropMimeData: Processing INTERNAL drop";
		QByteArray ba = data->data(itemMimeType());
		QStringList list = QString(ba).split(",");
		
		// Invert the actions since I cant figure out how to change the action modifier keys
		action = action == Qt::MoveAction ? Qt::CopyAction : Qt::MoveAction; 
		
		// convert csv list to integer list of slide numbers
		QList<int> removed;
		for(int i=0;i<list.size();i++)
		{
			int x = list.at(i).toInt();
			removed << x;
		}
		
		//qDebug() << "dropMimeData: def[copy="<<Qt::CopyAction<<",move="<<Qt::MoveAction<<"], action:"<<action<<", start size: "<<m_sortedGroups.size();
		
		//qDebug() << "Internal drop, parent: "<<parent.row();
		
	// 	if(action == Qt::CopyAction)
	// 		beginInsertRows(QModelIndex(),parent.row()+1,list.size());
		
		// add the slides from start to parent row
		for(int i=0;i<parentRow+1;i++)
			if(!removed.contains(i) || action == Qt::CopyAction)
				newList << m_sortedGroups.at(i);
		
		// add in the dropped slides
		foreach(int x, removed)
		{
			SlideGroup * group = m_sortedGroups.at(x);
			if(action == Qt::CopyAction)
			{
				group = group->clone();
				m_doc->addGroup(group);
			}
			newList << group;
			dropped << group;
		}
		
		// add in the rest of the slides
		for(int i=parentRow+1;i<m_sortedGroups.size();i++)
			if(!removed.contains(i) || action == Qt::CopyAction)
				newList << m_sortedGroups.at(i);
	}
	
	// renumber all the slides
	int nbr = 1;
	foreach(SlideGroup *x, newList)
	{
		//qDebug() << "DocumentListModel::dropMimeData: nbr:"<<nbr<<", title:"<<x->groupTitle();
		x->setGroupNumber(nbr++);
	}
	
	m_sortedGroups = newList;
	
 	if(action == Qt::CopyAction)
 		internalSetup();
 	else
 	{
 		//endInsertRows();
		
		//qDebug() << "dropMimeData: end size: "<<m_sortedGroups.size();
		
		//qSort(m_sortedGroups.begin(), m_sortedGroups.end(), group_num_compare);
		
		QModelIndex top    = indexForGroup(m_sortedGroups.first()),
			    bottom = indexForGroup(m_sortedGroups.last());
		
		dataChanged(top,bottom);
	}
	
	emit groupsDropped(dropped);
	
	return true;	
}

QMimeData * DocumentListModel::mimeData(const QModelIndexList & list) const
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

bool DocumentListModel::setData(const QModelIndex & index, const QVariant & value, int /*role*/)
{
	QString name = value.toString();
	SlideGroup * s = groupFromIndex(index);
	s->setGroupTitle(name);
	
	emit dataChanged(index,index);
	return true;
}


void DocumentListModel::releaseDocument()
{
	int sz = m_doc->groupList().size();
	beginRemoveRows(QModelIndex(),0,sz); // hack - yes, I know
	
	disconnect(m_doc,0,this,0);
	m_doc = 0;
	m_sortedGroups.clear();
	
	endRemoveRows();
}

void DocumentListModel::setDocument(Document *doc)
{
// 	printf("DocumentListModel::setDocument: old doc ptr: %p\n",m_doc);
// 	printf("DocumentListModel::setDocument: new doc ptr: %p\n",doc);
	//qDebug() << "DocumentListModel::setDocument: setting slide group:"<<g->groupNumber();
	if(m_doc && m_doc != doc)
		releaseDocument();
	
	connect(doc,SIGNAL(slideGroupChanged(SlideGroup *, QString, Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideGroupChanged(SlideGroup *, QString, Slide *, QString, AbstractItem *, QString, QString, QVariant)));
	
	m_doc = doc;
	
	int sz = m_doc->groupList().size();
	
	m_queuedIconGenerationMode = true;
	
	beginInsertRows(QModelIndex(),0,sz);
	
	internalSetup();
	
	endInsertRows();
	
	m_queuedIconGenerationMode = false;
}

void DocumentListModel::internalSetup()
{
	QList<SlideGroup*> glist = m_doc->groupList();
	qSort(glist.begin(), glist.end(), group_num_compare);
	m_sortedGroups = glist;
	
	if(m_sortedGroups.isEmpty())
		return;
	
	QModelIndex top    = indexForGroup(m_sortedGroups.first()),
		    bottom = indexForGroup(m_sortedGroups.last());
	
	dataChanged(top,bottom);
}

void DocumentListModel::slideGroupChanged(SlideGroup *g, QString groupOperation, Slide */*slide*/, QString /*slideOperation*/, AbstractItem */*item*/, QString /*operation*/, QString /*fieldName*/, QVariant /*value*/)
{
	if(!m_dirtyTimer)
	{
		m_dirtyTimer = new QTimer(this);
		connect(m_dirtyTimer, SIGNAL(timeout()), this, SLOT(modelDirtyTimeout()));
		
		m_dirtyTimer->setSingleShot(true);
	
	}
	//qDebug() << "slideGroupChanged:"<<groupOperation;
	if(groupOperation == "remove" || groupOperation == "add")
	{
		int sz = m_doc->groupList().size();
		if(groupOperation == "add")
			beginInsertRows(QModelIndex(),sz-1,sz);
		else
			beginRemoveRows(QModelIndex(),0,sz+1); // hack - yes, I know
		
		internalSetup();
		
		if(groupOperation == "add")
			endInsertRows();
		else
			endRemoveRows();
	}
	else
	{
		if(m_dirtyTimer->isActive())
			m_dirtyTimer->stop();

		QPixmapCache::remove(POINTER_STRING(g));
			
		m_dirtyTimer->start(DIRTY_TIMEOUT);
		if(!m_dirtyGroups.contains(g))
			m_dirtyGroups << g;
	}
}

void DocumentListModel::modelDirtyTimeout()
{
	//emit modelChanged();
	qSort(m_dirtyGroups.begin(),
	      m_dirtyGroups.end(), 
	      group_num_compare);
	
	QModelIndex top    = indexForGroup(m_dirtyGroups.first()), 
	            bottom = indexForGroup(m_dirtyGroups.last());
        //qDebug() << "DocumentListModel::modelDirtyTimeout: top:"<<top<<", bottom:"<<bottom;
	m_dirtyGroups.clear();
	
	dataChanged(top,bottom);
}
	
int DocumentListModel::rowCount(const QModelIndex &/*parent*/) const
{
	int rc = m_doc ? m_doc->numGroups() : 0;
	
	//qDebug() << "DocumentListModel::rowCount: rc:"<<rc;
	return rc;
}

SlideGroup * DocumentListModel::groupFromIndex(const QModelIndex &index)
{
	return m_sortedGroups.at(index.row());
}

SlideGroup * DocumentListModel::groupAt(int row)
{
	return m_sortedGroups.at(row);
}

QModelIndex DocumentListModel::indexForGroup(SlideGroup *g) const
{
	return createIndex(m_sortedGroups.indexOf(g),0);
}

QModelIndex DocumentListModel::indexForRow(int row) const
{
	return createIndex(row,0);
}

QVariant DocumentListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= m_sortedGroups.size())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		SlideGroup *g = m_sortedGroups.at(index.row());
		if(!g->groupTitle().isEmpty())
		{
			return g->groupTitle();
		}
		else
		{
			return QString("Group %1").arg(g->groupNumber());
		}
	}
	else if(Qt::DecorationRole == role)
	{
		SlideGroup *g = m_sortedGroups.at(index.row());
		QString cacheKey = POINTER_STRING(g);
		
		QPixmap icon;
		if(!QPixmapCache::find(cacheKey,icon))
		{
			DocumentListModel * self = const_cast<DocumentListModel*>(this);
			if(m_queuedIconGenerationMode)
			{
				self->needPixmap(g);
				return *m_blankPixmap;
			}
			else
			{
				icon = self->generatePixmap(g);
				QPixmapCache::insert(cacheKey,icon);
			}
		}
		
		return icon;
	}
	else
		return QVariant();
}

void DocumentListModel::setQueuedIconGenerationMode(bool flag)
{
	m_queuedIconGenerationMode = flag;
}

void DocumentListModel::setSceneRect(QRect r)
{
	m_sceneRect = r;
	adjustIconAspectRatio();
	
	if(!m_sortedGroups.isEmpty())
	{
		QModelIndex top    = indexForGroup(m_sortedGroups.first()), 
			    bottom = indexForGroup(m_sortedGroups.last());

		foreach(SlideGroup *g, m_sortedGroups)
			QPixmapCache::remove(POINTER_STRING(g));

		dataChanged(top,bottom);
	}
}

void DocumentListModel::adjustIconAspectRatio()
{
	QRect r = sceneRect();
	qreal a = (qreal)r.height() / (qreal)r.width();
	
	//qDebug() << "DocumentListModel::adjustIconAspectRatio(): r:"<<r<<", a:"<<a;
	
	m_iconSize.setHeight((int)(m_iconSize.width() * a));
}

void DocumentListModel::setIconSize(QSize sz)
{
	m_iconSize = sz;
	adjustIconAspectRatio();
}

QPixmap DocumentListModel::generatePixmap(SlideGroup *g)
{
	DeepProgressIndicator * d = DeepProgressIndicator::indicatorForObject(this);
 	if(d)
 		d->step();
 		
	QPixmap icon;
	
	SlideGroupFactory *factory = SlideGroupFactory::factoryForType(g->groupType());
	if(!factory)
		factory = SlideGroupFactory::factoryForType(SlideGroup::GroupType);
	
	if(factory)
		icon = factory->generatePreviewPixmap(g,m_iconSize,m_sceneRect);
	
		
	return icon;
	
}

void DocumentListModel::needPixmap(SlideGroup *group)
{
	if(!m_needPixmaps.contains(group))
		m_needPixmaps.append(group);
	if(!m_needPixmapTimer.isActive())
		m_needPixmapTimer.start(NEED_PIXMAP_TIMEOUT);
}

void DocumentListModel::makePixmaps()
{
	// Avoid generating a pixmap while the transiton is active because
	// the pixmap rendering is a potentially CPU-intensive and time-costly
	// routine which would likely cause the transition to stutter and not
	// render smoothly.
	if(MainWindow::mw() &&
	   MainWindow::mw()->isTransitionActive())
	   return;
	
	if(m_needPixmaps.isEmpty())
	{
		m_needPixmapTimer.stop();
		return;
	}
	
	SlideGroup *group = m_needPixmaps.takeFirst();
	
	QString cacheKey = POINTER_STRING(group);
	QPixmapCache::remove(cacheKey);
		
	QPixmap icon = generatePixmap(group);
	QPixmapCache::insert(cacheKey,icon);
	
	m_needPixmapTimer.stop();
	
	QModelIndex idx = indexForGroup(group);
	dataChanged(idx,idx);
	
	if(!m_needPixmaps.isEmpty())
		m_needPixmapTimer.start(NEED_PIXMAP_TIMEOUT_FAST);
}

QVariant DocumentListModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	return QString("Group %1").arg(section);
}
