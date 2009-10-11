#include "DocumentListModel.h"
#include <QDebug>

#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif

#include <QIcon>


#include "model/SlideGroup.h"
#include "model/Slide.h"
#include "model/SlideGroupFactory.h"
#include "MainWindow.h"

#define DEBUG_MARK() qDebug() << "mark: "<<__FILE__<<":"<<__LINE__

DocumentListModel::DocumentListModel(Document *d, QObject *parent)
		: QAbstractListModel(parent), m_doc(d),/* m_scene(0), m_view(0),*/ m_dirtyTimer(0),  m_iconSize(48,0), m_sceneRect(0,0,1024,768)
{
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
}

void DocumentListModel::aspectRatioChanged(double x)
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


bool DocumentListModel::dropMimeData ( const QMimeData * data, Qt::DropAction /*action*/, int /*row*/, int /*column*/, const QModelIndex & parent )
{
	QByteArray ba = data->data(itemMimeType());
	QStringList list = QString(ba).split(",");
	
	// convert csv list to integer list of slide numbers
	QList<int> removed;
	for(int i=0;i<list.size();i++)
	{
		int x = list.at(i).toInt();
		removed << x;
	}
	
	// add the slides from start to parent row
	QList<SlideGroup*> newList;
	for(int i=0;i<parent.row()+1;i++)
		if(!removed.contains(i))
			newList << m_sortedGroups.at(i);
	
	// add in the dropped slides
	QList<SlideGroup*> dropped;
	foreach(int x, removed)
	{
		newList << m_sortedGroups.at(x);
		dropped << m_sortedGroups.at(x);;
	}
	
	// add in the rest of the slides
	for(int i=parent.row()+1;i<m_sortedGroups.size();i++)
		if(!removed.contains(i))
			newList << m_sortedGroups.at(i);
	
	// renumber all the slides
	int nbr = 0;
	foreach(SlideGroup *x, newList)
		x->setGroupNumber(nbr++);
	
	m_sortedGroups = newList;
	
	m_pixmaps.clear();
	
	QModelIndex top    = indexForGroup(m_sortedGroups.first()),
		    bottom = indexForGroup(m_sortedGroups.last());
	
	dataChanged(top,bottom);
	
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


bool group_num_compare(SlideGroup *a, SlideGroup *b)
{
	return (a && b) ? a->groupNumber() < b->groupNumber() : true;
}

void DocumentListModel::setDocument(Document *doc)
{
	//qDebug() << "DocumentListModel::setDocument: setting slide group:"<<g->groupNumber();
	if(m_doc && m_doc != doc)
	{
		disconnect(m_doc,0,this,0);
	}
	
	
	if(m_doc != doc)
		connect(doc,SIGNAL(slideGroupChanged(SlideGroup *, QString, Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideGroupChanged(SlideGroup *, QString, Slide *, QString, AbstractItem *, QString, QString, QVariant)));
	
	m_doc = doc;
	
	internalSetup();
}

void DocumentListModel::internalSetup()
{
	QList<SlideGroup*> glist = m_doc->groupList();
	qSort(glist.begin(), glist.end(), group_num_compare);
	m_sortedGroups = glist;

	QModelIndex top    = indexForGroup(m_sortedGroups.first()),
		    bottom = indexForGroup(m_sortedGroups.last());
	//qDebug() << "DocumentListModel::internalSetup: top:"<<top<<", bottom:"<<bottom;

	m_pixmaps.clear();
	
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
		// if a group was removed/added, assume all pixmaps are invalid since the order could have changed
		m_pixmaps.clear();
		
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

		m_pixmaps.remove(m_sortedGroups.indexOf(g));
		m_dirtyTimer->start(250);
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
	
int DocumentListModel::rowCount(const QModelIndex &parent) const
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

static quint32 DocumentListModel_uidCounter = 0;
	
QModelIndex DocumentListModel::indexForGroup(SlideGroup *g) const
{
	DocumentListModel_uidCounter++;
	return createIndex(m_sortedGroups.indexOf(g),0,DocumentListModel_uidCounter);
}

QModelIndex DocumentListModel::indexForRow(int row) const
{
	DocumentListModel_uidCounter++;
	return createIndex(row,0,DocumentListModel_uidCounter);
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
			return QString("Group %1").arg(g->groupNumber()+1);
		}
	}
	else if(Qt::DecorationRole == role)
	{
		if(!m_pixmaps.contains(index.row()))
		{
			DocumentListModel * self = const_cast<DocumentListModel*>(this);
			self->generatePixmap(index.row());
		}
		
		//return QPixmap(":/images/ok.png");
		return m_pixmaps[index.row()];
// 		SlideGroup *g = m_sortedGroups.at(index.row());
// 		if(!g->iconFile().isEmpty())
// 		{
// 			return QIcon(g->iconFile());
// 		}
// 		else
// 		{
// 			return QVariant();
// 		}
	}
	else
		return QVariant();
}

void DocumentListModel::setSceneRect(QRect r)
{
	m_sceneRect = r;
	adjustIconAspectRatio();
	
	if(!m_sortedGroups.isEmpty())
	{
		QModelIndex top    = indexForGroup(m_sortedGroups.first()), 
			    bottom = indexForGroup(m_sortedGroups.last());

		m_pixmaps.clear();

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

void DocumentListModel::generatePixmap(int row)
{
	//qDebug("generatePixmap: Row#%d: Begin", row);
	SlideGroup *g = m_sortedGroups.at(row);
	
	QPixmap icon;
	
	SlideGroupFactory *factory = SlideGroupFactory::factoryForType(g->groupType());
	if(!factory)
		factory = SlideGroupFactory::factoryForType(SlideGroup::Generic);
	
	if(factory)
		icon = factory->generatePreviewPixmap(g,m_iconSize,m_sceneRect);
	
		
	m_pixmaps[row] = icon;
	
}

QVariant DocumentListModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	return QString("Group %1").arg(section);
}
