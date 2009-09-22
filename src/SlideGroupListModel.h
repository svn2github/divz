#ifndef SLIDEGROUPLISTMODEL_H
#define SLIDEGROUPLISTMODEL_H

#include <QAbstractListModel>
#include <QList>

class Slide;
class SlideGroup;

class SlideGroupListModel : public QAbstractListModel
{
Q_OBJECT

public:
	SlideGroupListModel(SlideGroup *g = 0, QObject *parent = 0);
	
	void setSlideGroup(SlideGroup*);
	Slide * slideFromIndex(const QModelIndex &index);
	Slide * slideAt(int);
	QModelIndex indexForSlide(Slide *slide) const;
	QModelIndex indexForRow(int row) const;
	
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
				int role = Qt::DisplayRole) const;
	
private:
	SlideGroup * m_slideGroup;
	QList<Slide*> m_sortedSlides;
};

#endif
