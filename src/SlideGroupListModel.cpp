#include "SlideGroupListModel.h"
#include <QDebug>

#include "model/SlideGroup.h"
#include "model/Slide.h"

SlideGroupListModel::SlideGroupListModel(SlideGroup *g, QObject *parent)
		: QAbstractListModel(parent), m_slideGroup(0) 
{
	if(m_slideGroup)
		setSlideGroup(g);
}

bool slide_num_compare(Slide *a, Slide *b)
{
	return (a && b) ? a->slideNumber() < b->slideNumber() : true;
}

void SlideGroupListModel::setSlideGroup(SlideGroup *g)
{
	//emit modelAboutToBeReset();
	qDebug() << "SlideGroupListModel::setSlideGroup: setting slide group:"<<g->groupNumber();
	m_slideGroup = g;
	
	QList<Slide*> slist = g->slideList();
	qSort(slist.begin(), slist.end(), slide_num_compare);
	m_sortedSlides = slist;
	
	//emit modelReset();
}
	
int SlideGroupListModel::rowCount(const QModelIndex &parent) const
{
	int rc = m_slideGroup ? m_slideGroup->numSlides() : 0;
	
	//qDebug() << "SlideGroupListModel::rowCount: rc:"<<rc;
	return rc;
}

Slide * SlideGroupListModel::slideFromIndex(const QModelIndex &index)
{
	return m_sortedSlides.at(index.row());
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
	else
		return QVariant();
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
