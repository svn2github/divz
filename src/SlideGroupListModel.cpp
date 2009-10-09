#include "SlideGroupListModel.h"
#include <QDebug>

#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif

#include <QMimeData>

#include "model/SlideGroup.h"
#include "model/Slide.h"
#include "MainWindow.h"

#define DEBUG_MARK() qDebug() << "[DEBUG] "<<__FILE__<<":"<<__LINE__

SlideGroupListModel::SlideGroupListModel(SlideGroup *g, QObject *parent)
		: QAbstractListModel(parent), m_slideGroup(0), m_scene(0), m_dirtyTimer(0), m_iconSize(192,0), m_sceneRect(0,0,1024,768)
{
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
	if(m_scene)
	{
		delete m_scene;
		m_scene = 0;
	}
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
	
	m_pixmaps.clear();
	
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
		
	if(m_slideGroup)// && m_slideGroup != g)
	{
		disconnect(m_slideGroup,0,this,0);
	}
	
	if(m_slideGroup) // != g)
	{
		connect(g,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));
		connect(g,SIGNAL(destroyed(QObject*)), this, SLOT(releaseSlideGroup()));
	}
	
	m_slideGroup = g;
	
	internalSetup();
}

void SlideGroupListModel::releaseSlideGroup()
{
	if(!m_slideGroup)
		return;
		
	m_pixmaps.clear();
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
	qSort(slist.begin(), slist.end(), slide_num_compare);
	m_sortedSlides = slist;

	QModelIndex top    = indexForSlide(m_sortedSlides.first()),
		    bottom = indexForSlide(m_sortedSlides.last());

	m_pixmaps.clear();
	
	dataChanged(top,bottom);
	
}

void SlideGroupListModel::slideChanged(Slide *slide, QString slideOperation, AbstractItem */*item*/, QString /*operation*/, QString /*fieldName*/, QVariant /*value*/)
{
	if(!m_slideGroup)
		return;
		
	if(!m_dirtyTimer)
	{
		m_dirtyTimer = new QTimer(this);
		connect(m_dirtyTimer, SIGNAL(timeout()), this, SLOT(modelDirtyTimeout()));
		
		m_dirtyTimer->setSingleShot(true);
	
	}
	
// 	if(slideOperation == "change" && !m_sortedSlides.contains(slide))
// 	{
// 		slideOperation = "add";
// 	}
	//qDebug() << "slideChanged: operation:"<<slideOperation;
	
	if(slideOperation == "remove" || slideOperation == "add")
	{
		// if a slide was removed/added, assume all pixmaps are invalid since the order could have changed
		m_pixmaps.clear();
		
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
		if(m_dirtyTimer->isActive())
			m_dirtyTimer->stop();

		m_pixmaps.remove(m_sortedSlides.indexOf(slide));
		m_dirtyTimer->start(250);
		if(!m_dirtySlides.contains(slide))
			m_dirtySlides << slide;
	}
}

void SlideGroupListModel::modelDirtyTimeout()
{
	
	qSort(m_dirtySlides.begin(),
	      m_dirtySlides.end(), 
	      slide_num_compare);
	
	QModelIndex top    = indexForSlide(m_dirtySlides.first()), 
	            bottom = indexForSlide(m_dirtySlides.last());
	//qDebug() << "SlideGroupListModel::modelDirtyTimeout: top:"<<top<<", bottom:"<<bottom;
	m_dirtySlides.clear();
	
	dataChanged(top,bottom);
}
	
int SlideGroupListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_slideGroup ? m_slideGroup->numSlides() : 0;
}

Slide * SlideGroupListModel::slideFromIndex(const QModelIndex &index)
{
	return m_sortedSlides.at(index.row());
}

Slide * SlideGroupListModel::slideAt(int row)
{
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
		return QString("Slide %1").arg(m_sortedSlides.at(index.row())->slideNumber()+1);
	}
	else if(Qt::DecorationRole == role)
	{
		if(!m_pixmaps.contains(index.row()))
		{
			// HACK - Have to remove the const'ness from 'this' so that
			// we can cache the pixmap internally. Not sure if this is
			// going to ruin anything - but it seems to work just fine
			// so far!
			SlideGroupListModel * self = const_cast<SlideGroupListModel*>(this);
			self->generatePixmap(index.row());
		}
		
		return m_pixmaps[index.row()];
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
		m_pixmaps.clear();

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


void SlideGroupListModel::generatePixmap(int row)
{
// 	return;
	Slide * slide = m_sortedSlides.at(row);
	
	int icon_w = m_iconSize.width();
	int icon_h = m_iconSize.height();
	
	if(!m_scene)
	{
		m_scene = new MyGraphicsScene(MyGraphicsScene::Preview);
		m_scene->setSceneRect(m_sceneRect);
	}
	
	m_scene->setSlide(slide);

	QPixmap icon(icon_w,icon_h);
	QPainter painter(&icon);
	painter.fillRect(0,0,icon_w,icon_h,Qt::white);
	
	m_scene->render(&painter,QRectF(0,0,icon_w,icon_h),m_sceneRect);
	painter.setPen(Qt::black);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(0,0,icon_w-1,icon_h-1);
	
	// clear() so we can free memory, stop videos, etc
	m_scene->clear();
	
	m_pixmaps[row] = icon;
}
 
QVariant SlideGroupListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	return QString("%1").arg(section);	
}
