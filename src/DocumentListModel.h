#ifndef DOCUMENTLISTMODEL_H
#define DOCUMENTLISTMODEL_H

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
	
	void setSceneRect(QRect);
	QRect sceneRect(){ return m_sceneRect; }
	void setIconSize(QSize);
	QSize iconSize() { return m_iconSize; }
	
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
				int role = Qt::DisplayRole) const;
				
	/* Drag and Drop Support */
	Qt::ItemFlags flags(const QModelIndex &index) const;
	Qt::DropActions supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

	QStringList mimeTypes () const { QStringList x; x << itemMimeType() << SongRecordListModel::itemMimeType(); return x; }
 	QMimeData * mimeData(const QModelIndexList & indexes) const;
 	bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);
 	
 	/* Editing Support */
 	bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
 	
 	// Not from AbstractListModel, just for utility
	static QString itemMimeType() { return "application/x-dviz-document-listmodel-item"; }
	//QString interListItemMimeType() const { return "application/x-dviz-document-listmodel-item-bytearray"; }
		
		
	void releaseDocument();
	
	void setQueuedIconGenerationMode(bool flag);
	bool queuedIconGenerationMode() { return m_queuedIconGenerationMode; }
	
	
signals:
	void groupsDropped(QList<SlideGroup*>);

private slots:
 	void slideGroupChanged(SlideGroup *g, QString groupOperation, Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);
 	void modelDirtyTimeout();
 	void aspectRatioChanged(double);
 	
 	void makePixmaps();
	
private:
	void internalSetup();
	void needPixmap(SlideGroup*);
	
	Document * m_doc;
	QList<SlideGroup*> m_sortedGroups;
	QList<SlideGroup*> m_dirtyGroups;
	
	QList<SlideGroup*> m_needPixmaps;
	
	QPixmap generatePixmap(SlideGroup*);
	void adjustIconAspectRatio();
	QHash<int,QPixmap> m_pixmaps;
	QSize m_iconSize;
	QRect m_sceneRect;
	//MyGraphicsScene * m_scene;
	
 	QTimer * m_dirtyTimer;
 	
 	QTimer m_needPixmapTimer;
 	
 	bool m_queuedIconGenerationMode;
 	
 	static QPixmap * m_blankPixmap;
 	static int m_blankPixmapRefCount;
};

#endif
