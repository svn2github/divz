#ifndef SONGSLIDEGROUPLISTMODEL_H
#define SONGSLIDEGROUPLISTMODEL_H

#include "SlideGroupListModel.h"
#include <QTextDocument>

class SongSlideGroupListModel : public SlideGroupListModel
{
	Q_OBJECT
	
public:
	SongSlideGroupListModel(SlideGroup *g = 0, QObject *parent = 0);
	~SongSlideGroupListModel();
	
	QVariant data(const QModelIndex &index, int role) const;
	
protected:
	void generatePixmap(int);
	
	QTextDocument *m_text;
};

#endif
