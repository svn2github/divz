#ifndef DOCUMENTLISTMODEL_H
#define DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QGraphicsView>
#include <QTimer>

#include "model/Document.h"
#include "MyGraphicsScene.h"


class DocumentListModel : public QAbstractListModel
{
Q_OBJECT

public:
	DocumentListModel(Document *d = 0, QObject *parent = 0);
	~DocumentListModel();
	
	void setDocument(Document*);
	SlideGroup * groupFromIndex(const QModelIndex &index);
	SlideGroup * groupAt(int);
	QModelIndex indexForGroup(SlideGroup *g) const;
	QModelIndex indexForRow(int row) const;
	
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
				int role = Qt::DisplayRole) const;
	
signals:
 	void modelChanged();
	
private slots:
 	void slideGroupChanged(SlideGroup *g, QString groupOperation, Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);
 	void modelDirtyTimeout();
	
private:
// 	void generatePixmap(int);
	
	Document * m_doc;
	QList<SlideGroup*> m_sortedGroups;
// 	QHash<int,QPixmap> m_pixmaps;
	
// 	MyGraphicsScene * m_scene;
// 	QGraphicsView * m_view;
	
 	QTimer * m_dirtyTimer;
};

#endif
