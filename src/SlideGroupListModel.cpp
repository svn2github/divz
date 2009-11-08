#include "SlideGroupListModel.h"
#include <QDebug>

#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif

#include <QMimeData>
#include <QPixmap>
#include <QPixmapCache>

#include "MyGraphicsScene.h"
#include "model/SlideGroup.h"
#include "model/Slide.h"
#include "MainWindow.h"
#include "AppSettings.h"

#include "DeepProgressIndicator.h"

#define DEBUG_MARK() qDebug() << "[DEBUG] "<<__FILE__<<":"<<__LINE__

#define POINTER_STRING(ptr) QString().sprintf("%p",static_cast<void*>(ptr))

#define NEED_PIXMAP_TIMEOUT 150
#define NEED_PIXMAP_TIMEOUT_FAST 100
#define DIRTY_TIMEOUT 250
#define QUEUE_STATE_CHANGE_TIME 1000

// blank 4x3 pixmap
QPixmap * SlideGroupListModel::m_blankPixmap = 0;
int SlideGroupListModel::m_blankPixmapRefCount = 0;

SlideGroupListModel::SlideGroupListModel(SlideGroup *g, QObject *parent)
		: QAbstractListModel(parent)
		, m_slideGroup(0)
		, m_scene(0)
		, m_dirtyTimer(0)
		, m_iconSize(192,0)
		, m_sceneRect(0,0,1024,768)
		, m_queuedIconGenerationMode(false)
		
{
		
	m_dirtyTimer = new QTimer(this);
	m_dirtyTimer->setSingleShot(true);
	connect(m_dirtyTimer, SIGNAL(timeout()), this, SLOT(modelDirtyTimeout()));
	
	m_dirtyTimer2 = new QTimer(this);
	m_dirtyTimer2->setSingleShot(true);
	connect(m_dirtyTimer2, SIGNAL(timeout()), this, SLOT(modelDirtyTimeout2()));
	
	if(!m_blankPixmap)
	{
		m_blankPixmap = new QPixmap(192,192 * (1/AppSettings::liveAspectRatio()));
		m_blankPixmap->fill(Qt::lightGray);
		QPainter painter(m_blankPixmap);
		painter.setPen(QPen(Qt::black,1,Qt::DotLine));
		painter.drawRect(m_blankPixmap->rect().adjusted(0,0,-1,-1));
		painter.end();
	}
	
	connect(&m_needPixmapTimer, SIGNAL(timeout()), this, SLOT(makePixmaps()));

	connect(&m_queueStateChangeTimer, SIGNAL(timeout()), this, SLOT(turnOffQueuedIconGeneration()));
	m_queueStateChangeTimer.setSingleShot(true);
	
	if(m_slideGroup)
		setSlideGroup(g);
	
	if(MainWindow::mw())
	{
		m_sceneRect = MainWindow::mw()->standardSceneRect();
		connect(MainWindow::mw(), SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
	}
			
	setSceneRect(m_sceneRect);
}

SlideGroupListModel::~SlideGroupListModel()
{
	m_blankPixmapRefCount --;
	if(m_blankPixmapRefCount <= 0)
	{
		delete m_blankPixmap;
		m_blankPixmap = 0;
	}
	
	if(m_scene)
	{
		delete m_scene;
		m_scene = 0;
	}
}

void SlideGroupListModel::setQueuedIconGenerationMode(bool flag)
{
	m_queuedIconGenerationMode = flag;
}

void SlideGroupListModel::turnOffQueuedIconGeneration()
{
	setQueuedIconGenerationMode(false);
}

void SlideGroupListModel::aspectRatioChanged(double)
{
	setSceneRect(MainWindow::mw()->standardSceneRect());
}


Qt::ItemFlags SlideGroupListModel::flags(const QModelIndex &index) const
{
	if (index.isValid())	
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
}


bool SlideGroupListModel::dropMimeData ( const QMimeData * data, Qt::DropAction /*action*/, int /*row*/, int /*column*/, const QModelIndex & parent )
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
	QList<Slide*> newList;
	for(int i=0;i<parent.row()+1;i++)
		if(!removed.contains(i))
			newList << m_sortedSlides.at(i);
	
	// add in the dropped slides
	QList<Slide*> dropped;
	foreach(int x, removed)
	{
		newList << m_sortedSlides.at(x);
		dropped << m_sortedSlides.at(x);;
	}
	
	// add in the rest of the slides
	for(int i=parent.row()+1;i<m_sortedSlides.size();i++)
		if(!removed.contains(i))
			newList << m_sortedSlides.at(i);
	
	// renumber all the slides
	int nbr = 0;
	foreach(Slide *x, newList)
		x->setSlideNumber(nbr++);
	
	m_sortedSlides = newList;
	
	QModelIndex top    = indexForSlide(m_sortedSlides.first()),
		    bottom = indexForSlide(m_sortedSlides.last());
	
	dataChanged(top,bottom);
	
	emit slidesDropped(dropped);
	
	return true;	
}

QMimeData * SlideGroupListModel::mimeData(const QModelIndexList & list) const
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


bool slide_num_compare(Slide *a, Slide *b)
{
	return (a && b) ? a->slideNumber() < b->slideNumber() : true;
}

void SlideGroupListModel::setSlideGroup(SlideGroup *g)
{
	if(!g)
		return;
		
	if(m_queueStateChangeTimer.isActive())
		m_queueStateChangeTimer.stop();
		
	m_queuedIconGenerationMode = true;
	
	m_needPixmaps.clear();
	
	if(m_slideGroup)// && m_slideGroup != g)
	{
		disconnect(m_slideGroup,0,this,0);
	}
	
	
	if(g) // != g)
	{
		connect(g,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));
		connect(g,SIGNAL(destroyed(QObject*)), this, SLOT(releaseSlideGroup()));
	}
	
	m_slideGroup = g;
	
	int sz = 0;
	if(m_slideGroup)
		sz = m_slideGroup->numSlides();
	
	//beginInsertRows(QModelIndex(),0,sz-1);
	
	internalSetup();
	
	//endInsertRows();
	
	m_queueStateChangeTimer.start(QUEUE_STATE_CHANGE_TIME);
}

void SlideGroupListModel::releaseSlideGroup()
{
	if(!m_slideGroup)
		return;
		
	disconnect(m_slideGroup,0,this,0);
	int sz = m_slideGroup->slideList().size();
	beginRemoveRows(QModelIndex(),0,sz);
	m_slideGroup = 0;
	endRemoveRows();
}


void SlideGroupListModel::internalSetup()
{
	if(!m_slideGroup)
		return;
	
	QList<Slide*> slist = m_slideGroup->slideList();
	
	if(slist.size() <= 0)
		return;
		
	qSort(slist.begin(), slist.end(), slide_num_compare);
	m_sortedSlides = slist;

	QModelIndex top    = indexForSlide(m_sortedSlides.first()),
		    bottom = indexForSlide(m_sortedSlides.last());

	dataChanged(top,bottom);
	
}

void SlideGroupListModel::slideChanged(Slide *slide, QString slideOperation, AbstractItem */*item*/, QString /*operation*/, QString /*fieldName*/, QVariant /*value*/)
{
	if(!m_slideGroup)
		return;
	
	
// 	if(slideOperation == "change" && !m_sortedSlides.contains(slide))
// 	{
// 		slideOperation = "add";
// 	}
	//qDebug() << "SlideGroupListModel::slideChanged: operation:"<<slideOperation;
	
	if(slideOperation == "remove" || slideOperation == "add")
	{
		int sz = m_slideGroup->slideList().size();
		if(slideOperation == "add")
			beginInsertRows(QModelIndex(),sz-1,sz);
		else
			beginRemoveRows(QModelIndex(),0,sz+1); // hack - yes, I know
		
		//qDebug("slide added");
		internalSetup();
		
		if(slideOperation == "add")
			endInsertRows();
		else
			endRemoveRows();
	}
	else
	{
		markSlideDirty(slide);
	}
}

void SlideGroupListModel::markSlideDirty(Slide *slide)
{
	if(m_dirtyTimer->isActive())
		m_dirtyTimer->stop();

	m_dirtyTimer->start(250);
	
	if(!m_dirtySlides.contains(slide))
		m_dirtySlides << slide;
}

void SlideGroupListModel::modelDirtyTimeout()
{
	
	qSort(m_dirtySlides.begin(),
	      m_dirtySlides.end(), 
	      slide_num_compare);
	
	if(m_dirtySlides.isEmpty())
		return;
	
	foreach(Slide *slide, m_dirtySlides)
		QPixmapCache::remove(POINTER_STRING(slide));
	
	QModelIndex top    = indexForSlide(m_dirtySlides.first()), 
	            bottom = indexForSlide(m_dirtySlides.last());
	
	//qDebug() << "SlideGroupListModel::modelDirtyTimeout: top:"<<top<<", bottom:"<<bottom;
	
	m_dirtySlides.clear();
	
	dataChanged(top,bottom);
}


void SlideGroupListModel::modelDirtyTimeout2()
{
	
	qSort(m_dirtySlides2.begin(),
	      m_dirtySlides2.end(), 
	      slide_num_compare);
	
	if(m_dirtySlides2.isEmpty())
		return;
	
	QModelIndex top    = indexForSlide(m_dirtySlides2.first()), 
	            bottom = indexForSlide(m_dirtySlides2.last());
	
	m_dirtySlides2.clear();
	
	dataChanged(top,bottom);
}
	
int SlideGroupListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_slideGroup ? m_slideGroup->numSlides() : 0;
}

Slide * SlideGroupListModel::slideFromIndex(const QModelIndex &index)
{
	if(!index.isValid())
		return 0;
	return slideAt(index.row());
}

Slide * SlideGroupListModel::slideAt(int row)
{
	if(row < 0 || row >= m_sortedSlides.size())
		return 0;
			
	return m_sortedSlides.at(row);
}

static quint32 SlideGroupListModel_uidCounter = 0;
	
QModelIndex SlideGroupListModel::indexForSlide(Slide *slide) const
{
	SlideGroupListModel_uidCounter++;
	return createIndex(m_sortedSlides.indexOf(slide),0,SlideGroupListModel_uidCounter);
}

QModelIndex SlideGroupListModel::indexForRow(int row) const
{
	SlideGroupListModel_uidCounter++;
	return createIndex(row,0,SlideGroupListModel_uidCounter);
}

QVariant SlideGroupListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= m_sortedSlides.size())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		//qDebug() << "SlideGroupListModel::data: VALID:"<<index.row();
		
		Slide *slide = m_sortedSlides.at(index.row());
		return QString("Slide %1").arg(slide->slideNumber()+1) +
			(slide->autoChangeTime() > 0 ? QString("\n%1 secs").arg(slide->autoChangeTime()) : QString());
	}
	else if(Qt::DecorationRole == role)
	{
		Slide *g = m_sortedSlides.at(index.row());
		QString cacheKey = POINTER_STRING(g);
		QPixmap icon;
		
		//qDebug() << "SlideGroupListModel::data: Decoration for row:"<<index.row();
		
		if(!QPixmapCache::find(cacheKey,icon))
		{
			// HACK - Have to remove the const'ness from 'this' so that
			// we can cache the pixmap internally. Not sure if this is
			// going to ruin anything - but it seems to work just fine
			// so far!
			SlideGroupListModel * self = const_cast<SlideGroupListModel*>(this);
			
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


void SlideGroupListModel::setSceneRect(QRect r)
{
	m_sceneRect = r;
	adjustIconAspectRatio();
	
	if(m_scene)
		m_scene->setSceneRect(m_sceneRect);

	if(!m_sortedSlides.isEmpty())
	{
		QModelIndex top    = indexForSlide(m_sortedSlides.first()),
			    bottom = indexForSlide(m_sortedSlides.last());
		//qDebug() << "DocumentListModel::modelDirtyTimeout: top:"<<top<<", bottom:"<<bottom;
		
		foreach(Slide *slide, m_sortedSlides)
			QPixmapCache::remove(POINTER_STRING(slide));

		dataChanged(top,bottom);
	}
}

void SlideGroupListModel::adjustIconAspectRatio()
{
	QRect r = sceneRect();
	qreal a = (qreal)r.height() / (qreal)r.width();
	
	m_iconSize.setHeight((int)(m_iconSize.width() * a));
}

void SlideGroupListModel::setIconSize(QSize sz)
{
	m_iconSize = sz;
	adjustIconAspectRatio();
}


QPixmap SlideGroupListModel::generatePixmap(Slide *slide)
{
 	//return QPixmap();
 	//qDebug() << "SlideGroupListModel::generatePixmap: Slide#"<<slide->slideNumber()<<": Mark 0";
 	DeepProgressIndicator * d = DeepProgressIndicator::indicatorForObject(this);
 	if(d)
 		d->step();
 	
 	if(m_dataLoadPending.contains(slide))
 	{
 		if(m_dataLoadPending[slide]->isDataLoadComplete())
 		{
 			//qDebug() << "SlideGroupListModel::generatePixmap: Slide#"<<slide->slideNumber()<<": Mark 1";
 			MyGraphicsScene * scene = m_dataLoadPending[slide];
 			m_dataLoadPending.remove(slide);
 			
 			QPixmap pixmap = renderScene(scene);
 			//delete scene;
 			
 			emit repaintList();
 			
 			return pixmap;
 		}
 		else
 		{
 			MyGraphicsScene * scene = m_dataLoadPending[slide];
			
			int counter = scene->property("_dirty_counter").toInt();
 			scene->setProperty("_dirty_counter",counter+1);
 			
 			if(counter > 1)
 			{
 				//qDebug() << "SlideGroupListModel::generatePixmap: Slide#"<<slide->slideNumber()<<": Mark 1.5\n\n";
 				
 				m_dataLoadPending.remove(slide);
				
				QPixmap pixmap = renderScene(scene);
				//delete scene;
				
				emit repaintList();
				
				//QModelIndex idx = indexForSlide(slide);
				//dataChanged(idx,idx);
				
				if(m_dirtyTimer2->isActive())
					m_dirtyTimer2->stop();
			
				m_dirtyTimer2->start(10);
				
				if(!m_dirtySlides2.contains(slide))
					m_dirtySlides2 << slide;
				
				return pixmap;
 			}
 			else
 			{
				//qDebug() << "SlideGroupListModel::generatePixmap: Slide#"<<slide->slideNumber()<<": Mark 2:"<<counter;
				markSlideDirty(slide);
				return defaultPendingPixmap();
			}
 		}
 	}
 	else
 	{
		if(!m_scene)
			m_scene = new MyGraphicsScene(MyGraphicsScene::Preview);
		if(m_scene->sceneRect() != m_sceneRect)
			m_scene->setSceneRect(m_sceneRect);
		
		m_scene->setMasterSlide(m_slideGroup->masterSlide());
		m_scene->setSlide(slide);
		
		if(m_scene->isDataLoadComplete())
		{
			//qDebug() << "SlideGroupListModel::generatePixmap: Slide#"<<slide->slideNumber()<<": Mark 3";
			return renderScene(m_scene);
		}
		else
		{
			//qDebug() << "SlideGroupListModel::generatePixmap: Slide#"<<slide->slideNumber()<<": Mark 4";
			markSlideDirty(slide);
			
			m_scene->setProperty("_dirty_counter",0);
			m_dataLoadPending[slide] = m_scene;
			m_scene = 0;	
			
			return defaultPendingPixmap();
		}
	}
}

QPixmap SlideGroupListModel::defaultPendingPixmap()
{
	if(m_pendingPixmap.isNull())
	{
		int icon_w = m_iconSize.width();
		int icon_h = m_iconSize.height();
			
		m_pendingPixmap = QPixmap(icon_w,icon_h);
		
		QPainter painter(&m_pendingPixmap);
		painter.fillRect(0,0,icon_w,icon_h,Qt::lightGray);
		
		painter.setPen(Qt::black);
		painter.setBrush(Qt::NoBrush);
		painter.drawRect(0,0,icon_w-1,icon_h-1);
		
		painter.end();
	}
	
	return m_pendingPixmap;
}

QPixmap SlideGroupListModel::renderScene(MyGraphicsScene *scene)
{
	int icon_w = m_iconSize.width();
	int icon_h = m_iconSize.height();
		
	QPixmap icon(icon_w,icon_h);
	QPainter painter(&icon);
	painter.fillRect(0,0,icon_w,icon_h,Qt::white);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::TextAntialiasing, true);
	
	scene->render(&painter,QRectF(0,0,icon_w,icon_h),m_sceneRect);
	painter.setPen(Qt::black);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(0,0,icon_w-1,icon_h-1);
	
	scene->clear();
	
	return icon;
}


void SlideGroupListModel::needPixmap(Slide *group)
{
	if(!m_needPixmaps.contains(group))
		m_needPixmaps.append(group);
	if(!m_needPixmapTimer.isActive())
		m_needPixmapTimer.start(NEED_PIXMAP_TIMEOUT);
}

void SlideGroupListModel::makePixmaps()
{
	if(m_needPixmaps.isEmpty())
	{
		m_needPixmapTimer.stop();
		return;
	}
	
	Slide *group = m_needPixmaps.takeFirst();
	
	QString cacheKey = POINTER_STRING(group);
	QPixmapCache::remove(cacheKey);
		
	QPixmap icon = generatePixmap(group);
	QPixmapCache::insert(cacheKey,icon);
	
	m_needPixmapTimer.stop();
	
	QModelIndex idx = indexForSlide(group);
	dataChanged(idx,idx);
	
	if(!m_needPixmaps.isEmpty())
		m_needPixmapTimer.start(NEED_PIXMAP_TIMEOUT_FAST);
}
 
QVariant SlideGroupListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	return QString("%1").arg(section);	
}
