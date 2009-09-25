#include "DocumentListModel.h"
#include <QDebug>

#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif

#include <QIcon>


#include "model/SlideGroup.h"
#include "model/Slide.h"

DocumentListModel::DocumentListModel(Document *d, QObject *parent)
		: QAbstractListModel(parent), m_doc(d),/* m_scene(0), m_view(0),*/ m_dirtyTimer(0)
{
	if(m_doc)
		setDocument(d);
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
	
	QList<SlideGroup*> glist = doc->groupList();
	qSort(glist.begin(), glist.end(), group_num_compare);
	m_sortedGroups = glist;
}

void DocumentListModel::slideGroupChanged(SlideGroup *g, QString groupOperation, Slide */*slide*/, QString /*slideOperation*/, AbstractItem */*item*/, QString /*operation*/, QString /*fieldName*/, QVariant /*value*/)
{
	if(!m_dirtyTimer)
	{
		m_dirtyTimer = new QTimer(this);
		connect(m_dirtyTimer, SIGNAL(timeout()), this, SLOT(modelDirtyTimeout()));
		
		m_dirtyTimer->setSingleShot(true);
	
	}
	
	if(groupOperation == "remove")
	{
		// if a slide was removed, assume all pixmaps are invalid since the order could have changed
// 		m_pixmaps.clear();
	}
	
// 	if(slide)
// 		qDebug() << "DocumentListModel::slideChanged: slide#:"<<slide->slideNumber();
	
	if(m_dirtyTimer->isActive())
		m_dirtyTimer->stop();
		
// 	m_pixmaps.remove(m_sortedSlides.indexOf(slide));
	m_dirtyTimer->start(250);
	if(!m_dirtyGroups.contains(g))
		m_dirtyGroups << g;
}

void DocumentListModel::modelDirtyTimeout()
{
	//emit modelChanged();
	qSort(m_dirtyGroups.begin(),
	      m_dirtyGroups.end(), 
	      group_num_compare);
	
	QModelIndex top    = indexForGroup(m_dirtyGroups.first()), 
	            bottom = indexForGroup(m_dirtyGroups.last());
	qDebug() << "SDocumentListModel::modelDirtyTimeout: top:"<<top<<", bottom:"<<bottom;
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

static quint32 documentModelUidCounter = 0;
	
QModelIndex DocumentListModel::indexForGroup(SlideGroup *g) const
{
	documentModelUidCounter++;
	return createIndex(m_sortedGroups.indexOf(g),0,documentModelUidCounter);
}

QModelIndex DocumentListModel::indexForRow(int row) const
{
	documentModelUidCounter++;
	return createIndex(row,0,documentModelUidCounter);
}

QVariant DocumentListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
	{
		//qDebug() << "DocumentListModel::data: invalid index";
		return QVariant();
	}
	
	if (index.row() >= m_sortedGroups.size())
	{
		//qDebug() << "DocumentListModel::data: index out of range at:"<<index.row();
		return QVariant();
	}
	
	if (role == Qt::DisplayRole)
	{
		//qDebug() << "DocumentListModel::data: VALID:"<<index.row();
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
// 		if(!m_pixmaps.contains(index.row()))
// 		{
// 			DocumentListModel * self = const_cast<DocumentListModel*>(this);
// 			self->generatePixmap(index.row());
// 		}
		
		//return QPixmap(":/images/ok.png");
		//return m_pixmaps[index.row()];
		SlideGroup *g = m_sortedGroups.at(index.row());
		if(!g->iconFile().isEmpty())
		{
			return QIcon(g->iconFile());
		}
		else
		{
			return QVariant();
		}
	}
	else
		return QVariant();
}
/*
void DocumentListModel::generatePixmap(int row)
{
	//qDebug("generatePixmap: Row#%d: Begin", row);
	Slide * slide = m_sortedSlides.at(row);
	
	int icon_w = 64;
	int icon_h = icon_w*0.75;
	
	QRect sceneRect(0,0,1024,768);
	if(!m_view)
	{
		//qDebug("generatePixmap: Row#%d: QGraphicsView setup: View not setup, Initalizing view and scene...", row);
		m_view = new QGraphicsView();
		m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
		//m_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
		
		m_scene = new MyGraphicsScene();
		m_scene->setSceneRect(sceneRect);
		m_view->setScene(m_scene);
	}
	
	//qDebug("generatePixmap: Row#%d: Setting slide...", row);
	m_scene->setSlide(slide);
	
	//qDebug("generatePixmap: Row#%d: Clearing icon...", row);
	QPixmap icon(icon_w,icon_h);
	QPainter painter(&icon);
	painter.fillRect(0,0,icon_w,icon_h,Qt::white);
	
	//qDebug("generatePixmap: Row#%d: Painting...", row);
	
	m_view->render(&painter,QRectF(0,0,icon_w,icon_h),sceneRect);
	painter.setPen(Qt::black);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(0,0,icon_w-1,icon_h-1);
	
	//qDebug("generatePixmap: Row#%d: Clearing Scene...", row);
	m_scene->clear();
	
	//qDebug("generatePixmap: Row#%d: Caching pixmap...", row);
	m_pixmaps[row] = icon;
	//qDebug("generatePixmap: Row#%d: Done.", row);
}
 */
QVariant DocumentListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	//qDebug() << "DocumentListModel::headerData: requested data for:"<<section;
	
	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Group %1").arg(section);
}
