#ifndef SLIDEGROUPLISTMODEL_H
#define SLIDEGROUPLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QGraphicsView>
#include <QTimer>
#include <QRect>
#include <QSize>

#include "MyGraphicsScene.h"
#include "model/AbstractItem.h"

#include <math.h>

class Slide;
class SlideGroup;

class SlideGroupListModel : public QAbstractListModel
{
Q_OBJECT

public:
	SlideGroupListModel(SlideGroup *g = 0, QObject *parent = 0);
	~SlideGroupListModel();
	
	void setSceneRect(QRect);
	QRect sceneRect(){ return m_sceneRect; }
	void setIconSize(QSize);
	QSize iconSize() { return m_iconSize; }
	
	void setSlideGroup(SlideGroup*);
	Slide * slideFromIndex(const QModelIndex &index);
	Slide * slideAt(int);
	QModelIndex indexForSlide(Slide *slide) const;
	QModelIndex indexForRow(int row) const;
	
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
				int role = Qt::DisplayRole) const;
	
private slots:
	void slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);
	void modelDirtyTimeout();
	
private:
	void internalSetup();
	void generatePixmap(int);
	void adjustIconAspectRatio();
	
	SlideGroup * m_slideGroup;
	QList<Slide*> m_sortedSlides;
	QList<Slide*> m_dirtySlides;
	QHash<int,QPixmap> m_pixmaps;
	
	MyGraphicsScene * m_scene;
	QGraphicsView * m_view;
	
	QTimer * m_dirtyTimer;
	
	QSize m_iconSize;
	
	QRect m_sceneRect;
};

#endif
