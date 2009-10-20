#ifndef SLIDEITEMLISTMODEL_H
#define SLIDEITEMLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QGraphicsView>
#include <QTimer>
#include <QRect>
#include <QSize>

#include "MyGraphicsScene.h"
#include "model/AbstractItem.h"

#include <math.h>

#include <QStringList>
#include <QPointer>

class Slide;
class SlideGroup;

class SlideItemListModel : public QAbstractListModel
{
Q_OBJECT

public:
	SlideItemListModel(Slide *slide = 0, QObject *parent = 0);
	virtual ~SlideItemListModel();
	
// 	void setSceneRect(QRect);
// 	QRect sceneRect(){ return m_sceneRect; }
	void setIconSize(QSize);
	QSize iconSize() { return m_iconSize; }
	
	void setSlide(Slide*);
	AbstractItem * itemFromIndex(const QModelIndex &index);
	AbstractItem * itemAt(int);
	QModelIndex indexForItem(AbstractItem *item) const;
	QModelIndex indexForRow(int row) const;
	
	/* ::QAbstractListModel */
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
				int role = Qt::DisplayRole) const;

	/* Drag and Drop Support */
	Qt::ItemFlags flags(const QModelIndex &index) const;
	Qt::DropActions supportedDropActions() const { return Qt::MoveAction; }

	QStringList mimeTypes () const { QStringList x; x<<itemMimeType(); return x; }
 	QMimeData * mimeData(const QModelIndexList & indexes) const;
 	bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);
 	
 	// Not from AbstractListModel, just for utility
	QString itemMimeType() const { return "application/x-dviz-slideitem-listmodel-item"; }
 	
 	

signals:
	void itemsDropped(QList<AbstractItem*>);
	
public slots:	
	void releaseSlide();
	
private slots:
	void itemChanged(AbstractItem *item, QString operation, QString fieldName, QVariant value);
	void modelDirtyTimeout();
	//void aspectRatioChanged(double);
	
	
protected:
	virtual void generatePixmap(int);
	void internalSetup();
	void adjustIconAspectRatio();
	
	Slide * m_slide;
	QList<AbstractItem*> m_sortedItems;
	QList<AbstractItem*> m_dirtyItems;
	QHash<int,QPixmap> m_pixmaps;
	
	//MyGraphicsScene * m_scene;
	//QGraphicsView * m_view;
	
	QTimer * m_dirtyTimer;
	
	QSize m_iconSize;
	//QRect m_sceneRect;
};

#endif
