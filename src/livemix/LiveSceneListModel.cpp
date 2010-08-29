#include "LiveSceneListModel.h"
#include <QDebug>

// #ifndef QT_NO_OPENGL
// # include <QtOpenGL/QGLWidget>
// #endif

#include <QMimeData>
#include <QPixmap>
#include <QPixmapCache>

/*#include "MyGraphicsScene.h"*/
#include "LiveScene.h"
#include "LiveLayer.h"
// #include "model/SlideGroup.h"
// #include "model/Slide.h"
// #include "MainWindow.h"
// #include "AppSettings.h"

// #include "DeepProgressIndicator.h"

#define DEBUG_MARK() qDebug() << "[DEBUG] "<<__FILE__<<":"<<__LINE__

#define POINTER_STRING(ptr) QString().sprintf("%p",static_cast<void*>(ptr))

// #define NEED_PIXMAP_TIMEOUT 150
// #define NEED_PIXMAP_TIMEOUT_FAST 100
// #define DIRTY_TIMEOUT 250
// #define QUEUE_STATE_CHANGE_TIME 1000

// blank 4x3 pixmap

LiveSceneListModel::LiveSceneListModel(QObject *parent)
	: QAbstractListModel(parent)
	//, m_scene(0)
	, m_scene(0)
// 	, m_dirtyTimer(0)
// 	, m_iconSize(192,0)
// 	, m_sceneRect(0,0,1024,768)
// 	, m_queuedIconGenerationMode(false)
// 	, m_blankPixmap(0)
{
		
// 	m_dirtyTimer = new QTimer(this);
 	m_dirtyTimer.setSingleShot(true);
 	connect(&m_dirtyTimer, SIGNAL(timeout()), this, SLOT(modelDirtyTimeout()));
// 	
// 	m_dirtyTimer2 = new QTimer(this);
// 	m_dirtyTimer2->setSingleShot(true);
// 	connect(m_dirtyTimer2, SIGNAL(timeout()), this, SLOT(modelDirtyTimeout2()));
// 	
// 	if(!m_blankPixmap)
// 	{
// 		regenerateBlankPixmap();
// 	}
// 	
// 	connect(&m_needPixmapTimer, SIGNAL(timeout()), this, SLOT(makePixmaps()));
// 
// 	connect(&m_queueStateChangeTimer, SIGNAL(timeout()), this, SLOT(turnOffQueuedIconGeneration()));
// 	m_queueStateChangeTimer.setSingleShot(true);
	
// 	if(m_scene)
// 		setSlideGroup(g);
// 	
// 	if(MainWindow::mw())
// 	{
// 		m_sceneRect = MainWindow::mw()->standardSceneRect();
// 		connect(MainWindow::mw(), SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
// 	}
// 			
// 	setSceneRect(m_sceneRect);
}

LiveSceneListModel::~LiveSceneListModel()
{
// 	if(m_blankPixmap)
// 	{
// 		delete m_blankPixmap;
// 		m_blankPixmap = 0;
// 	}
// 	
// 	if(m_scene)
// 	{
// 		delete m_scene;
// 		m_scene = 0;
// 	}
}

// void LiveSceneListModel::regenerateBlankPixmap()
// {
// 	if(m_blankPixmap)
// 		delete m_blankPixmap;
// 		
// 	m_blankPixmap = new QPixmap(m_iconSize.width(),m_iconSize.width() * (1/AppSettings::liveAspectRatio()));
// 	m_blankPixmap->fill(Qt::lightGray);
// 	QPainter painter(m_blankPixmap);
// 	painter.setPen(QPen(Qt::black,1,Qt::DotLine));
// 	painter.drawRect(m_blankPixmap->rect().adjusted(0,0,-1,-1));
// 	painter.end();
// }
// 
// void LiveSceneListModel::setQueuedIconGenerationMode(bool flag)
// {
// 	m_queuedIconGenerationMode = flag;
// }
// 
// void LiveSceneListModel::turnOffQueuedIconGeneration()
// {
// 	setQueuedIconGenerationMode(false);
// }
// 
// void LiveSceneListModel::aspectRatioChanged(double)
// {
// 	setSceneRect(MainWindow::mw()->standardSceneRect());
// }
// 

Qt::ItemFlags LiveSceneListModel::flags(const QModelIndex &index) const
{
	if (index.isValid())	
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsUserCheckable;
	
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled | Qt::ItemIsUserCheckable;
}


bool LiveSceneListModel::dropMimeData ( const QMimeData * data, Qt::DropAction action, int /*row*/, int /*column*/, const QModelIndex & parent )
{
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
	
	// add the slides from start to parent row
	QList<LiveLayer*> newList;
	for(int i=0;i<parent.row()+1;i++)
		if(!removed.contains(i) || action == Qt::CopyAction)
			newList << m_sortedLayers.at(i);
	
	// add in the dropped slides
	QList<LiveLayer*> dropped;
	foreach(int x, removed)
	{
		LiveLayer * slide = m_sortedLayers.at(x);
		if(action == Qt::CopyAction)
		{
			QByteArray bytes = slide->toByteArray();
			QString className = slide->metaObject()->className();
			
			const QMetaObject *metaObject = LiveScene::metaObjectForClass(className);
			if(!metaObject)
			{
				qDebug() << "Error: LiveSceneListModel::dropMimeData(): Unable to create layer of type:"<<className;
				continue;
			}
			
			qDebug() << "LiveSceneListModel::dropMimeData(): Debug: metaObject classname: "<<metaObject->className()<<", asked for:"<<className;
			
			QObject *instance = metaObject->newInstance(Q_ARG(QObject*,0));
			if(!instance)
			{
				qDebug() << "Error: LiveSceneListModel::dropMimeData(): Creation of "<<className<<" failed.";
				continue;
			}
			
			
			LiveLayer *layer = dynamic_cast<LiveLayer*>(instance);
			if(!layer)
			{
				qDebug() << "Error: LiveSceneListModel::dropMimeData(): Object created for type "<<className<<" does not inherit from LiveLayer.";
				continue;
			}
			
			layer->fromByteArray(bytes);
			
			m_scene->addLayer(layer);
		}
		
		newList << slide;
		dropped << slide;
	}
	
	// add in the rest of the slides
	for(int i=parent.row()+1;i<m_sortedLayers.size();i++)
		if(!removed.contains(i) || action == Qt::CopyAction)
			newList << m_sortedLayers.at(i);
	
	// renumber all the slides
	int nbr = newList.size();
	foreach(LiveLayer *x, newList)
// 	{
// 		qDebug() << "LiveSceneListModel::dropMimeData: set slide # "<<nbr;
		x->setZIndex(nbr--);
// 	}

	m_sortedLayers = newList;
	
// 	if(action == Qt::CopyAction)
// 		internalSetup();
// 	else
// 	{
		
		QModelIndex top = indexForItem(m_sortedLayers.first()),
			 bottom = indexForItem(m_sortedLayers.last());
		
		dataChanged(top,bottom);
	//}
	
	emit layersDropped(dropped);
	
	return true;	
}

QMimeData * LiveSceneListModel::mimeData(const QModelIndexList & list) const
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


bool livelayer_num_compare(LiveLayer *a, LiveLayer *b)
{
	return (a && b) ? a->zIndex() > b->zIndex() : true;
}

void LiveSceneListModel::setLiveScene(LiveScene *scene)
{
	if(!scene)
		return;
	/*	
	if(m_queueStateChangeTimer.isActive())
		m_queueStateChangeTimer.stop();
		
	m_queuedIconGenerationMode = true;
	
	m_needPixmaps.clear();*/
	
	if(m_scene)// && m_scene != g)
	{
		disconnect(m_scene,0,this,0);
	}
	
	
	if(scene) // != g)
	{
		connect(scene, SIGNAL(layerAdded(LiveLayer*)), this, SLOT(layerAdded(LiveLayer*)));
		connect(scene, SIGNAL(layerRemoved(LiveLayer*)), this, SLOT(layerRemoved(LiveLayer*)));
		connect(scene, SIGNAL(destroyed(QObject*)), this, SLOT(releaseLiveScene()));
		
		foreach(LiveLayer *layer, scene->layerList()) 
			connect(layer, SIGNAL(layerPropertyChanged(const QString&, const QVariant&, const QVariant&)),
				this,    SLOT(layerPropertyChanged(const QString&, const QVariant&, const QVariant&)));
		
	}
	
	m_scene = scene;
	
// 	int sz = 0;
// 	if(m_scene)
// 		sz = m_scene->numSlides();
	
	//beginInsertRows(QModelIndex(),0,sz-1);
	
	internalSetup();
	
	//endInsertRows();
	
// 	m_queueStateChangeTimer.start(QUEUE_STATE_CHANGE_TIME);
}

void LiveSceneListModel::releaseLiveScene()
{
	if(!m_scene)
		return;
		
	disconnect(m_scene,0,this,0);
	int sz = m_scene->layerList().size();
	beginRemoveRows(QModelIndex(),0,sz);
	m_scene = 0;
	endRemoveRows();
}


void LiveSceneListModel::internalSetup()
{
	if(!m_scene)
		return;
	
	QList<LiveLayer*> slist = m_scene->layerList();
	
	if(slist.size() <= 0)
		return;
		
	qSort(slist.begin(), slist.end(), livelayer_num_compare);
	m_sortedLayers = slist;

	QModelIndex top    = indexForItem(m_sortedLayers.first()),
		    bottom = indexForItem(m_sortedLayers.last());

	dataChanged(top,bottom);
	
}

void LiveSceneListModel::layerPropertyChanged(const QString& /*propertyId*/, const QVariant& /*value*/, const QVariant& /*oldValue*/)
{
	if(!m_scene)
		return;
	
	markLayerDirty(dynamic_cast<LiveLayer*>(sender()));
}

void LiveSceneListModel::layerRemoved(LiveLayer *layer)
{
	disconnect(layer, 0, this, 0);
	
	int sz = m_scene->layerList().size();
	beginRemoveRows(QModelIndex(),0,sz+1); // hack - yes, I know
	
	internalSetup();
	
	endRemoveRows();
}

void LiveSceneListModel::layerAdded(LiveLayer *layer)
{
	connect(layer, SIGNAL(layerPropertyChanged(const QString&, const QVariant&, const QVariant&)),
		this,    SLOT(layerPropertyChanged(const QString&, const QVariant&, const QVariant&)));
			
	int sz = m_scene->layerList().size();
	beginInsertRows(QModelIndex(),sz-1,sz);
	
	internalSetup();
	
	endInsertRows();
}


void LiveSceneListModel::markLayerDirty(LiveLayer *slide)
{
	if(m_dirtyTimer.isActive())
		m_dirtyTimer.stop();

	m_dirtyTimer.start(250);
	
	if(!m_dirtyLayers.contains(slide))
		m_dirtyLayers << slide;
}

void LiveSceneListModel::modelDirtyTimeout()
{
	
	qSort(m_dirtyLayers.begin(),
	      m_dirtyLayers.end(), 
	      livelayer_num_compare);
	
	if(m_dirtyLayers.isEmpty())
		return;
	
// 	foreach(Slide *slide, m_dirtyLayers)
// 		QPixmapCache::remove(QString("%1-%2").arg(POINTER_STRING(slide)).arg(m_iconSize.width()));
	
	QModelIndex top    = indexForItem(m_dirtyLayers.first()),
	            bottom = indexForItem(m_dirtyLayers.last());
	
	m_dirtyLayers.clear();

	//qDebug() << "LiveSceneListModel::modelDirtyTimeout: top:"<<top<<", bottom:"<<bottom;

	if(!top.isValid())
		top = bottom;
	if(!bottom.isValid())
		bottom = top;
	if((!bottom.isValid() && !top.isValid())
		|| bottom.row()< 0 || top.row() < 0)
	{
		//qDebug() << "LiveSceneListModel::modelDirtyTimeout: Both top:"<<top<<", bottom:"<<bottom<<" are invalid.";
		return;
	}
	
	dataChanged(top,bottom);
}
	
int LiveSceneListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_scene ? m_scene->layerList().size() : 0;
}

LiveLayer * LiveSceneListModel::itemFromIndex(const QModelIndex &index)
{
	if(!index.isValid())
		return 0;
	return itemAt(index.row());
}

LiveLayer * LiveSceneListModel::itemAt(int row)
{
	if(row < 0 || row >= m_sortedLayers.size())
		return 0;
			
	return m_sortedLayers.at(row);
}

static quint32 LiveSceneListModel_uidCounter = 0;
	
QModelIndex LiveSceneListModel::indexForItem(LiveLayer *slide) const
{
	LiveSceneListModel_uidCounter++;
	return createIndex(m_sortedLayers.indexOf(slide),0,LiveSceneListModel_uidCounter);
}

QModelIndex LiveSceneListModel::indexForRow(int row) const
{
	LiveSceneListModel_uidCounter++;
	return createIndex(row,0,LiveSceneListModel_uidCounter);
}

QVariant LiveSceneListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= m_sortedLayers.size())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		//qDebug() << "LiveSceneListModel::data: VALID:"<<index.row();
		
		LiveLayer *layer = m_sortedLayers.at(index.row());
		return QString("%1\n(%2)")
			.arg(layer->instanceName())
			.arg(layer->typeName());
		//QString("Slide %1").arg(slide->slideNumber()+1) +
		//return layer->assumedName() + 
		//	(slide->autoChangeTime() > 0 ? QString("\n%1 secs").arg(slide->autoChangeTime()) : QString());
	}
	else if(Qt::CheckStateRole == role)
	{
		LiveLayer *layer = m_sortedLayers.at(index.row());
		return layer->isVisible() ? 
			Qt::Checked :
			Qt::Unchecked;
	}
// 	else if(Qt::DecorationRole == role)
// 	{
// 		LiveLayer *g = m_sortedLayers.at(index.row());
// 		QString cacheKey = QString("%1-%2").arg(POINTER_STRING(g)).arg(m_iconSize.width());
// 		QPixmap icon;
// 		
// 		//qDebug() << "LiveSceneListModel::data: Decoration for row:"<<index.row();
// 		
// 		if(!QPixmapCache::find(cacheKey,icon))
// 		{
// 			// HACK - Have to remove the const'ness from 'this' so that
// 			// we can cache the pixmap internally. Not sure if this is
// 			// going to ruin anything - but it seems to work just fine
// 			// so far!
// 			LiveSceneListModel * self = const_cast<LiveSceneListModel*>(this);
// 			
// 			if(m_queuedIconGenerationMode)
// 			{
// 				self->needPixmap(g);
// 				return *m_blankPixmap;
// 			}
// 			else
// 			{
// 				icon = self->generatePixmap(g);
// 				QPixmapCache::insert(cacheKey,icon);
// 			}
// 			
// 		}
// 		
// 		return icon;
// 	}
// 	else
		return QVariant();
}

bool LiveSceneListModel::setData(const QModelIndex &index, const QVariant & value, int role) 
{
	if (!index.isValid())
		return false;
	
	if (index.row() >= m_sortedLayers.size())
		return false;
	
	if(role == Qt::CheckStateRole)
	{
		LiveLayer *layer = m_sortedLayers.at(index.row());
		//qDebug() << "LiveSceneListModel::setData: index:"<<index<<", value:"<<value; 
		if(value.isValid() && !value.isNull())
		{
			//slide->setSlideName(value.toString());
			layer->setVisible(value.toBool());
			//LiveSceneListModel * model = const_cast<LiveSceneListModel *>(this);
			//model->
			dataChanged(index,index);
			return true;
		}
	}
	
	return false;
}

 
QVariant LiveSceneListModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	return QString("%1").arg(section);	
}
