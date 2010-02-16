#include "SlideItemListModel.h"
#include <QDebug>

#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif

#include <QMimeData>

#include "MyGraphicsScene.h"
#include "model/AbstractItem.h"
#include "model/SlideGroup.h"
#include "model/Slide.h"
#include "MainWindow.h"

#define DEBUG_MARK() qDebug() << "[DEBUG] "<<__FILE__<<":"<<__LINE__



bool SlideItemListModel_item_zval_compare(AbstractItem *a, AbstractItem *b)
{
	AbstractVisualItem * va = dynamic_cast<AbstractVisualItem*>(a);
	AbstractVisualItem * vb = dynamic_cast<AbstractVisualItem*>(b);
	if(va && vb) 
		return va->zValue() > vb->zValue();
	if(!va && vb)
		return true;
	if(va && !vb)
		return false;
	if(!a || !b)
		return false;
	return a->itemId() < b->itemId();
}

SlideItemListModel::SlideItemListModel(Slide *g, QObject *parent)
		: QAbstractListModel(parent), m_slide(0), /*m_scene(0), */m_dirtyTimer(0), m_iconSize(192,0)/*, m_sceneRect(0,0,1024,768)*/
{
	if(m_slide)
		setSlide(g);
	
// 	if(MainWindow::mw())
// 	{
// 		m_sceneRect = MainWindow::mw()->standardSceneRect();
// 		connect(MainWindow::mw(), SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
// 	}
			
//	setSceneRect(m_sceneRect);
}

SlideItemListModel::~SlideItemListModel()
{
// 	if(m_scene)
// 	{
// 		delete m_scene;
// 		m_scene = 0;
// 	}
}

// void SlideItemListModel::aspectRatioChanged(double)
// {
// 	setSceneRect(MainWindow::mw()->standardSceneRect());
// }


Qt::ItemFlags SlideItemListModel::flags(const QModelIndex &index) const
{
	if (index.isValid())	
	{
		if(index.row() < m_sortedItems.size())
		{
			AbstractItem * item = m_sortedItems.at(index.row());
			
			AbstractVisualItem * vis = dynamic_cast<AbstractVisualItem*>(item);
			if(vis)
			{
				return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
			}
		}
	}
	
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
}


bool SlideItemListModel::dropMimeData ( const QMimeData * data, Qt::DropAction /*action*/, int /*row*/, int /*column*/, const QModelIndex & parent )
{
	QByteArray ba = data->data(itemMimeType());
	QStringList list = QString(ba).split(",");
	
	// convert csv list to integer list of item  numbers
	QList<int> removed;
	for(int i=0;i<list.size();i++)
	{
		int x = list.at(i).toInt();
		removed << x;
	}
	
	// add the items from start to parent row
	QList<AbstractItem*> newList;
	for(int i=0;i<parent.row()+1;i++)
		if(!removed.contains(i))
			newList << m_sortedItems.at(i);
	
	// add in the dropped slides
	QList<AbstractItem*> dropped;
	foreach(int x, removed)
	{
		newList << m_sortedItems.at(x);
		dropped << m_sortedItems.at(x);;
	}
	
	// add in the rest of the slides
	for(int i=parent.row()+1;i<m_sortedItems.size();i++)
		if(!removed.contains(i))
			newList << m_sortedItems.at(i);
	
	// renumber all the slides
	int nbr = newList.size();
	foreach(AbstractItem *x, newList)
	{
		AbstractVisualItem * vis = dynamic_cast<AbstractVisualItem*>(x);
		if(vis)
			vis->setZValue(nbr--);
	}
	
	//m_sortedItems = newList;
	
	QList<AbstractItem*> slist = m_slide->itemList();
	qSort(slist.begin(), slist.end(), SlideItemListModel_item_zval_compare);
	m_sortedItems = slist;
	
	
	m_pixmaps.clear();
	
	QModelIndex top    = indexForItem(m_sortedItems.first()),
		    bottom = indexForItem(m_sortedItems.last());
	
	dataChanged(top,bottom);
	
	emit itemsDropped(dropped);
	
	return true;	
}

QMimeData * SlideItemListModel::mimeData(const QModelIndexList & list) const
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

void SlideItemListModel::setSlide(Slide *slide)
{
	if(!slide)
		return;
		
	if(m_slide)
		disconnect(m_slide,0,this,0);
	
	connect(slide,SIGNAL(slideItemChanged(AbstractItem *, QString, QString, QVariant, QVariant)),this,SLOT(itemChanged(AbstractItem *, QString, QString, QVariant)));
	connect(slide,SIGNAL(destroyed(QObject*)), this, SLOT(releaseSlide()));
	
	m_slide = slide;
	
	internalSetup();
}

void SlideItemListModel::releaseSlide()
{
	if(!m_slide)
		return;
		
	m_pixmaps.clear();
	disconnect(m_slide,0,this,0);
	int sz = m_slide->itemList().size();
	beginRemoveRows(QModelIndex(),0,sz);
	m_slide = 0;
	m_sortedItems.clear();
	endRemoveRows();
}


void SlideItemListModel::internalSetup()
{
	if(!m_slide)
		return;
		
	QList<AbstractItem*> slist = m_slide->itemList();
	qSort(slist.begin(), slist.end(), SlideItemListModel_item_zval_compare);
	m_sortedItems = slist;

	QModelIndex top    = indexForItem(m_sortedItems.first()),
		    bottom = indexForItem(m_sortedItems.last());

	m_pixmaps.clear();
	
	dataChanged(top,bottom);
	
// 	qDebug() << "SlideItemListModel::internalSetup(): Dump of m_sortedItems:";
// 	foreach(AbstractItem * item, m_sortedItems)
// 	{
// 		qDebug() << "\t "<<item->itemName(); //<<" | "<<item->zValue();
// 	}
// 	qDebug() << "SlideItemListModel::internalSetup(): Done.";
	
}

void SlideItemListModel::itemChanged(AbstractItem *item, QString operation, QString /*fieldName*/, QVariant /*value*/)
{
	if(!m_slide)
		return;
		
	if(!m_dirtyTimer)
	{
		m_dirtyTimer = new QTimer(this);
		connect(m_dirtyTimer, SIGNAL(timeout()), this, SLOT(modelDirtyTimeout()));
		
		m_dirtyTimer->setSingleShot(true);
	
	}
	
// 	if(slideOperation == "change" && !m_sortedItems.contains(slide))
// 	{
// 		slideOperation = "add";
// 	}
	//qDebug() << "SlideItemListModel::slideChanged: operation:"<<slideOperation;
	
	if(operation == "remove" || operation == "add")
	{
		// if a slide was removed/added, assume all pixmaps are invalid since the order could have changed
		m_pixmaps.clear();
		
		int sz = m_slide->itemList().size();
		if(operation == "add")
			beginInsertRows(QModelIndex(),sz-1,sz);
		else
			beginRemoveRows(QModelIndex(),0,sz+1); // hack - yes, I know
		
		//qDebug("slide added");
		internalSetup();
		
		if(operation == "add")
			endInsertRows();
		else
			endRemoveRows();
	}
	else
	{
		if(m_dirtyTimer->isActive())
			m_dirtyTimer->stop();

		//m_pixmaps.remove(m_sortedItems.indexOf(slide));
		m_dirtyTimer->start(250);
		if(!m_dirtyItems.contains(item))
			m_dirtyItems << item;
	}
}

void SlideItemListModel::modelDirtyTimeout()
{
	
	qSort(m_dirtyItems.begin(),
	      m_dirtyItems.end(), 
	      SlideItemListModel_item_zval_compare);
	      
	
	QModelIndex top    = indexForItem(m_dirtyItems.first()), 
	            bottom = indexForItem(m_dirtyItems.last());
	//qDebug() << "SlideItemListModel::modelDirtyTimeout: top:"<<top<<", bottom:"<<bottom;
	m_dirtyItems.clear();
	
	dataChanged(top,bottom);
}
	
int SlideItemListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_slide ? m_slide->itemCount() : 0;
}

AbstractItem * SlideItemListModel::itemFromIndex(const QModelIndex &index)
{
	if(!index.isValid())
		return 0;
	return itemAt(index.row());
}

AbstractItem * SlideItemListModel::itemAt(int row)
{
	if(row < 0 || row >= m_sortedItems.size())
		return 0;
			
	return m_sortedItems.at(row);
}

static quint32 SlideItemListModel_uidCounter = 0;
	
QModelIndex SlideItemListModel::indexForItem(AbstractItem *item) const
{
	SlideItemListModel_uidCounter++;
	return createIndex(m_sortedItems.indexOf(item),0,SlideItemListModel_uidCounter);
}

QModelIndex SlideItemListModel::indexForRow(int row) const
{
	SlideItemListModel_uidCounter++;
	return createIndex(row,0,SlideItemListModel_uidCounter);
}

QVariant SlideItemListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= m_sortedItems.size())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		//qDebug() << "SlideItemListModel::data: VALID:"<<index.row();
		
		//Slide *slide = m_sortedItems.at(index.row());
		//return QString("Slide %1").arg(slide->slideNumber()+1) +
		//	(slide->autoChangeTime() > 0 ? QString("\n%1 secs").arg(slide->autoChangeTime()) : QString());
		return AbstractItem::guessTitle(m_sortedItems.at(index.row())->itemName());
	}
	else if(Qt::DecorationRole == role)
	{
// 		if(!m_pixmaps.contains(index.row()))
// 		{
// 			// HACK - Have to remove the const'ness from 'this' so that
// 			// we can cache the pixmap internally. Not sure if this is
// 			// going to ruin anything - but it seems to work just fine
// 			// so far!
// 			SlideItemListModel * self = const_cast<SlideItemListModel*>(this);
// 			self->generatePixmap(index.row());
// 		}
// 		
// 		return m_pixmaps[index.row()];
		return QVariant();
	}
	else
		return QVariant();
}
/*
void SlideItemListModel::setSceneRect(QRect r)
{
	m_sceneRect = r;
	adjustIconAspectRatio();
	
	if(m_scene)
		m_scene->setSceneRect(m_sceneRect);

	if(!m_sortedItems.isEmpty())
	{
		QModelIndex top    = indexForItem(m_sortedItems.first()),
			    bottom = indexForItem(m_sortedItems.last());
		//qDebug() << "DocumentListModel::modelDirtyTimeout: top:"<<top<<", bottom:"<<bottom;
		m_pixmaps.clear();

		dataChanged(top,bottom);
	}
}*/

void SlideItemListModel::adjustIconAspectRatio()
{
// 	QRect r = sceneRect();
// 	qreal a = (qreal)r.height() / (qreal)r.width();
// 	
	qreal a = .75;
 	m_iconSize.setHeight((int)(m_iconSize.width() * a));
}

void SlideItemListModel::setIconSize(QSize sz)
{
	m_iconSize = sz;
	adjustIconAspectRatio();
}


void SlideItemListModel::generatePixmap(int row)
{
 	//return;
// 	Slide * slide = m_sortedItems.at(row);
// 	
// 	int icon_w = m_iconSize.width();
// 	int icon_h = m_iconSize.height();
// 	
// 	if(!m_scene)
// 	{
// 		m_scene = new MyGraphicsScene(MyGraphicsScene::Preview);
// 		m_scene->setSceneRect(m_sceneRect);
// 	}
// 	
// 	//qDebug() << "SlideItemListModel::generatePixmap: Loading slide";
// 	m_scene->setSlide(slide);
// 
// 	QPixmap icon(icon_w,icon_h);
// 	QPainter painter(&icon);
// 	painter.fillRect(0,0,icon_w,icon_h,Qt::white);
// 	
// 	m_scene->render(&painter,QRectF(0,0,icon_w,icon_h),m_sceneRect);
// 	painter.setPen(Qt::black);
// 	painter.setBrush(Qt::NoBrush);
// 	painter.drawRect(0,0,icon_w-1,icon_h-1);
// 	
// 	// clear() so we can free memory, stop videos, etc
// 	m_scene->clear();
// 	//qDebug() << "SlideItemListModel::generatePixmap: Releasing slide\n";
// 	
// 	m_pixmaps[row] = icon;
}
 
QVariant SlideItemListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	return QString("%1").arg(section);	
}
