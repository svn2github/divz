#ifndef DOCUMENTLISTMODEL_H
#define DOCUMENTLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QGraphicsView>
#include <QTimer>

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
	Qt::DropActions supportedDropActions() const { return Qt::MoveAction; }

	QStringList mimeTypes () const { QStringList x; x<<itemMimeType(); return x; }
 	QMimeData * mimeData(const QModelIndexList & indexes) const;
 	bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);
 	
 	/* Editing Support */
 	bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
 	
 	// Not from AbstractListModel, just for utility
	QString itemMimeType() const { return "application/x-dviz-document-listmodel-item"; }
		
		
	void releaseDocument();
	
	
signals:
	void groupsDropped(QList<SlideGroup*>);

private slots:
 	void slideGroupChanged(SlideGroup *g, QString groupOperation, Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);
 	void modelDirtyTimeout();
 	void aspectRatioChanged(double);
	
private:
	void internalSetup();
	
	Document * m_doc;
	QList<SlideGroup*> m_sortedGroups;
	QList<SlideGroup*> m_dirtyGroups;
	
	QPixmap generatePixmap(SlideGroup*);
	void adjustIconAspectRatio();
	QHash<int,QPixmap> m_pixmaps;
	QSize m_iconSize;
	QRect m_sceneRect;
	//MyGraphicsScene * m_scene;
	
 	QTimer * m_dirtyTimer;
};

#endif
