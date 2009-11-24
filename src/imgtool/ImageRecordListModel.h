#ifndef ImageRecordListModel_H
#define ImageRecordListModel_H

#include <QAbstractListModel>
#include <QList>

#include "ImageRecord.h"

class ImageRecord;
class QTimer;

class ImageRecordListModel : public QAbstractListModel
{
	Q_OBJECT
	
private:
	ImageRecordListModel();

public:
	static ImageRecordListModel * instance();

	ImageRecord * recordFromIndex(const QModelIndex &index);
	ImageRecord * recordAt(int);

	QModelIndex indexForRecord(ImageRecord *) const;
	QModelIndex indexForRow(int row) const;
	
	// for filtering ("searching") the list
	void filter(const QString&);

	// QAbstractListModel:: 
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
				int role = Qt::DisplayRole) const;

protected slots:
	// ImageRecord::add/delete notifies us via protected functions about new/removed
	friend class ImageRecord;
	void addRecord(ImageRecord*);
	void removeRecord(ImageRecord*);
	
private slots:	
	// notify listeners about change
	void modelDirtyTimeout();
	
	// slot to catch title changes
	void recordChanged(ImageRecord*, QString field, QVariant value);
	
private:
	void populateList();
	void sortList();
	
	QString m_filter;
	QList<ImageRecord*> m_list;
	QList<ImageRecord*> m_dirtyRecords;
	
	QTimer * m_dirtyTimer;
	
	bool m_populating;
	
	static ImageRecordListModel * static_instance;
};



#endif

