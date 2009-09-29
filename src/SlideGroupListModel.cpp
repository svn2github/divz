#include "SlideGroupListModel.h"
#include <QDebug>

#ifndef QT_NO_OPENGL
# include <QtOpenGL/QGLWidget>
#endif


#include "model/SlideGroup.h"
#include "model/Slide.h"
#include "RenderOpts.h"

SlideGroupListModel::SlideGroupListModel(SlideGroup *g, QObject *parent)
		: QAbstractListModel(parent), m_slideGroup(0), m_scene(0), m_view(0), m_dirtyTimer(0), m_iconSize(96,0), m_sceneRect(0,0,1024,768)
{
	if(m_slideGroup)
		setSlideGroup(g);
	
	setSceneRect(m_sceneRect);
}

SlideGroupListModel::~SlideGroupListModel()
{
	if(m_scene)
	{
		delete m_scene;
		m_scene = 0;
	}
	
// 	if(m_view)
// 	{
// 		delete m_view;
// 		m_view = 0;
// 	}
}

bool slide_num_compare(Slide *a, Slide *b)
{
	return (a && b) ? a->slideNumber() < b->slideNumber() : true;
}

void SlideGroupListModel::setSlideGroup(SlideGroup *g)
{
	//qDebug() << "SlideGroupListModel::setSlideGroup: setting slide group:"<<g->groupNumber();
	if(m_slideGroup && m_slideGroup != g)
	{
		disconnect(m_slideGroup,0,this,0);
	}
	
	if(m_slideGroup != g)
		connect(g,SIGNAL(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)),this,SLOT(slideChanged(Slide *, QString, AbstractItem *, QString, QString, QVariant)));
	
	m_slideGroup = g;
	
	internalSetup();
}

void SlideGroupListModel::internalSetup()
{
	QList<Slide*> slist = m_slideGroup->slideList();
	qSort(slist.begin(), slist.end(), slide_num_compare);
	m_sortedSlides = slist;

	QModelIndex top    = indexForSlide(m_sortedSlides.first()),
		    bottom = indexForSlide(m_sortedSlides.last());
	//qDebug() << "SlideGroupListModel::internalSetup: top:"<<top<<", bottom:"<<bottom;

	dataChanged(top,bottom);
}

void SlideGroupListModel::slideChanged(Slide *slide, QString slideOperation, AbstractItem */*item*/, QString /*operation*/, QString /*fieldName*/, QVariant /*value*/)
{
	if(!m_dirtyTimer)
	{
		m_dirtyTimer = new QTimer(this);
		connect(m_dirtyTimer, SIGNAL(timeout()), this, SLOT(modelDirtyTimeout()));
		
		m_dirtyTimer->setSingleShot(true);
	
	}
	
	if(slideOperation == "remove" || slideOperation == "add")
	{
		// if a slide was removed/added, assume all pixmaps are invalid since the order could have changed
		m_pixmaps.clear();
		internalSetup();
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
	//emit modelChanged();
}
	
int SlideGroupListModel::rowCount(const QModelIndex &/*parent*/) const
{
	int rc = m_slideGroup ? m_slideGroup->numSlides() : 0;
	
	//qDebug() << "SlideGroupListModel::rowCount: rc:"<<rc;
	return rc;
}

Slide * SlideGroupListModel::slideFromIndex(const QModelIndex &index)
{
	return m_sortedSlides.at(index.row());
}

Slide * SlideGroupListModel::slideAt(int row)
{
	return m_sortedSlides.at(row);
}

static quint32 uidCounter = 0;
	
QModelIndex SlideGroupListModel::indexForSlide(Slide *slide) const
{
	uidCounter++;
	return createIndex(m_sortedSlides.indexOf(slide),0,uidCounter);
}

QModelIndex SlideGroupListModel::indexForRow(int row) const
{
	uidCounter++;
	return createIndex(row,0,uidCounter);
}

QVariant SlideGroupListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
	{
		//qDebug() << "SlideGroupListModel::data: invalid index";
		return QVariant();
	}
	
	if (index.row() >= m_sortedSlides.size())
	{
		//qDebug() << "SlideGroupListModel::data: index out of range at:"<<index.row();
		return QVariant();
	}
	
	if (role == Qt::DisplayRole)
	{
		//qDebug() << "SlideGroupListModel::data: VALID:"<<index.row();
		return QString("Slide %1").arg(m_sortedSlides.at(index.row())->slideNumber()+1);
	}
	else if(Qt::DecorationRole == role)
	{
		if(!m_pixmaps.contains(index.row()))
		{
			SlideGroupListModel * self = const_cast<SlideGroupListModel*>(this);
			self->generatePixmap(index.row());
		}
		
		//return QPixmap(":/images/ok.png");
		return m_pixmaps[index.row()];
	}
	else
		return QVariant();
}

void SlideGroupListModel::setSceneRect(QRect r)
{
	m_sceneRect = r;
	adjustIconAspectRatio();
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
	return;
	
	//qDebug("generatePixmap: Row#%d: Begin", row);
	Slide * slide = m_sortedSlides.at(row);
	
	int icon_w = m_iconSize.width(); //96;
	int icon_h = m_iconSize.height(); //(int)(icon_w*0.75);
	
	if(!m_scene)
	{
		//qDebug("generatePixmap: Row#%d: QGraphicsView setup: View not setup, Initalizing view and scene...", row);
// 		m_view = new QGraphicsView();
// 		m_view->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
		//m_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
/*		#ifndef QT_NO_OPENGL
		if(!RenderOpts::DisableOpenGL)
		{
			m_view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
			qDebug("SlideGroupListModel: Loaded OpenGL viewport.");
		}
		#endif*/
		
		m_scene = new MyGraphicsScene(MyGraphicsScene::Preview);
		m_scene->setSceneRect(m_sceneRect);
		//m_view->setScene(m_scene);
		
// 		float sx = ((float)m_view->width()) / m_scene->width();
// 		float sy = ((float)m_view->height()) / m_scene->height();
// 	
// 		float scale = qMax(sx,sy);
		//m_view->setTransform(QTransform().scale(scale,scale));
//		m_view->update();
	}
	
	//qDebug("generatePixmap: Row#%d: Setting slide...", row);
	m_scene->setSlide(slide);
	
	//qDebug("generatePixmap: Row#%d: Clearing icon...", row);
	QPixmap icon(icon_w,icon_h);
	QPainter painter(&icon);
	painter.fillRect(0,0,icon_w,icon_h,Qt::white);
	
	//qDebug("generatePixmap: Row#%d: Painting...", row);
	
	m_scene->render(&painter,QRectF(0,0,icon_w,icon_h),m_sceneRect);
	painter.setPen(Qt::black);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(0,0,icon_w-1,icon_h-1);
	
	//qDebug("generatePixmap: Row#%d: Clearing Scene...", row);
	m_scene->clear();
	
	//qDebug("generatePixmap: Row#%d: Caching pixmap...", row);
	m_pixmaps[row] = icon;
	//qDebug("generatePixmap: Row#%d: Done.", row);
}
 
QVariant SlideGroupListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();
	
	//qDebug() << "SlideGroupListModel::headerData: requested data for:"<<section;
	
	if (orientation == Qt::Horizontal)
		return QString("Column %1").arg(section);
	else
		return QString("Row %1").arg(section);
}
