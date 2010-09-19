#ifndef SLIDEGROUPLISTMODEL_H
#define SLIDEGROUPLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QGraphicsView>
#include <QTimer>
#include <QRect>
#include <QSize>

class MyGraphicsScene;
class AbstractItem;

#include <math.h>

#include <QStringList>
#include <QPointer>
#include <QHash>

class Slide;
class SlideGroup;

class SlideGroupListModel : public QAbstractListModel
{
Q_OBJECT

public:
	SlideGroupListModel(SlideGroup *g = 0, QObject *parent = 0);
	virtual ~SlideGroupListModel();
	
	void setSceneRect(QRect);
	QRect sceneRect(){ return m_sceneRect; }
	void setIconSize(QSize);
	QSize iconSize() { return m_iconSize; }
	
	void setSlideGroup(SlideGroup*);
	Slide * slideFromIndex(const QModelIndex &index);
	Slide * slideAt(int);
	QModelIndex indexForSlide(Slide *slide) const;
	QModelIndex indexForRow(int row) const;
	
	/* ::QAbstractListModel */
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant & value, int role = Qt::EditRole);
	QVariant headerData(int section, Qt::Orientation orientation,
				int role = Qt::DisplayRole) const;

	/* Drag and Drop Support */
	Qt::ItemFlags flags(const QModelIndex &index) const;
	Qt::DropActions supportedDropActions() const { return Qt::MoveAction | Qt::CopyAction; }

	QStringList mimeTypes () const { QStringList x; x<<itemMimeType(); return x; }
 	QMimeData * mimeData(const QModelIndexList & indexes) const;
 	bool dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent);
 	
 	// Not from AbstractListModel, just for utility
	QString itemMimeType() const { return "application/x-dviz-slidegroup-listmodel-item"; }
 	
 	
 	void setQueuedIconGenerationMode(bool flag);
	bool queuedIconGenerationMode() { return m_queuedIconGenerationMode; }
 	

signals:
	void slidesDropped(QList<Slide*>);
	void repaintList();
	
public slots:	
	void releaseSlideGroup();
	
private slots:
	void slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);
	void modelDirtyTimeout();
	void modelDirtyTimeout2();
	void aspectRatioChanged(double);
	
	void makePixmaps();
	
	void turnOffQueuedIconGeneration();
	
protected:
	virtual QPixmap generatePixmap(Slide*);
	virtual QPixmap renderScene(MyGraphicsScene*);
	QPixmap defaultPendingPixmap();
	void markSlideDirty(Slide*, bool pixmapDirty=true);
	
	void regenerateBlankPixmap();
	
	void needPixmap(Slide*);
	
	void internalSetup();
	void adjustIconAspectRatio();
	
	SlideGroup * m_slideGroup;
	QList<Slide*> m_sortedSlides;
	QList<Slide*> m_dirtySlides;
	QList<Slide*> m_dirtySlides2;
	QList<Slide*> m_pixmapOk;
	QList<Slide*> m_needPixmaps;
	QHash<int,QPixmap> m_pixmaps;
	
	MyGraphicsScene * m_scene;
	//QGraphicsView * m_view;
	
	QTimer * m_dirtyTimer;
	QTimer * m_dirtyTimer2;
	
	QSize m_iconSize;
	QRect m_sceneRect;
	
	QPixmap m_pendingPixmap;
	
	QHash<Slide*, MyGraphicsScene*> m_dataLoadPending;
	
	bool m_queuedIconGenerationMode;

 	QTimer m_needPixmapTimer;
 	
 	QTimer m_queueStateChangeTimer;
 	
 	QPixmap * m_blankPixmap;

};

#endif
