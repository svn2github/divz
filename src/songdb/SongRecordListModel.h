#ifndef SONGRECORDLISTMODEL_H
#define SONGRECORDLISTMODEL_H

#include <QAbstractListModel>
#include <QList>

#include "SongRecord.h"

class SongRecord;
class QTimer;

class SongRecordListModel : public QAbstractListModel
{
	Q_OBJECT
	
private:
	SongRecordListModel();

public:
	static SongRecordListModel * instance();

	SongRecord * songFromIndex(const QModelIndex &index);
	SongRecord * songAt(int);

	QModelIndex indexForSong(SongRecord *) const;
	QModelIndex indexForRow(int row) const;
	
	// for filtering ("searching") the list
	void filter(const QString&);

	// QAbstractListModel:: 
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
				int role = Qt::DisplayRole) const;

protected slots:
	// songrecord::add/delete notifies us via protected functions about new/removed
	friend class SongRecord;
	void addSong(SongRecord*);
	void removeSong(SongRecord*);
	
private slots:	
	// notify listeners about change
	void modelDirtyTimeout();
	
	// slot to catch title changes
	void songChanged(SongRecord*, QString field, QVariant value);
	
private:
	void populateSongList();
	void sortList();
	
	QString m_filter;
	QList<SongRecord*> m_songList;
	QList<SongRecord*> m_dirtySongs;
	
	QTimer * m_dirtyTimer;
	
	bool m_populating;
	
	static SongRecordListModel * static_instance;
};



#endif

