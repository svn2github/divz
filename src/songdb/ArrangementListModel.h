#ifndef ArrangementListModel_H
#define ArrangementListModel_H

#include <QAbstractListModel>
#include <QList>
#include <QGraphicsView>
#include <QTimer>

#include "songdb/SongRecordListModel.h"

class Document;
class MyGraphicsScene;
#include <QMimeData>
class SlideGroup;
class Slide;
class AbstractItem;

class ArrangementListModel : public QAbstractListModel
{
Q_OBJECT

public:
	ArrangementListModel(QObject *parent = 0);
	~ArrangementListModel();
	
	void setBlockList(const QStringList&);
	QString blockFromIndex(const QModelIndex &index);
	QString blockAt(int);
	QModelIndex indexForRow(int row) const;
	
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
				int role = Qt::DisplayRole) const;
				
	/* Drag and Drop Support */
	Qt::ItemFlags flags(const QModelIndex &index) const;
	Qt::DropActions supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

	QStringList mimeTypes () const { QStringList x; x << itemMimeType() << interListMimeType(); return x; }
 	QMimeData * mimeData(const QModelIndexList & indexes) const;
 	bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);
 	
 	/* Editing Support */
 	bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
 	
 	// Not from AbstractListModel, just for utility
	static QString itemMimeType() { return "application/x-dviz-arrangement-listmodel-item"; }
	static QString interListMimeType() { return "application/x-dviz-arrangement-listmodel-item-inter-list"; }
	
	
signals:
	void blocksDropped(QStringList);
	void blockListChanged(QStringList);

private:
	void internalSetup();
	void needPixmap(QString);
	
	QStringList m_blocks;
	
	QPixmap generatePixmap(QString);
	QHash<int,QPixmap> m_pixmaps;
};

#endif
