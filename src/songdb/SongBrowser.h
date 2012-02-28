#ifndef SONGBROWSER_H
#define SONGBROWSER_H

#include <QWidget>

class QModelIndex;
class QByteArray;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QSplitter;
class MyQListView;
class SongRecordListModel;
class SongRecord;
class SlideGroup;

class SongBrowser : public QWidget
{
	Q_OBJECT
	
	Q_PROPERTY(bool editingEnabled READ editingEnabled WRITE setEditingEnabled);
	Q_PROPERTY(bool filteringEnabled READ filteringEnabled WRITE setFilteringEnabled);
	Q_PROPERTY(bool previewEnabled READ previewEnabled WRITE setPreviewEnabled);
	
public:
	SongBrowser(QWidget *parent=0);
	~SongBrowser();
	
	bool editingEnabled()	{ return m_editingEnabled; }
	bool filteringEnabled()	{ return m_filteringEnabled; }
	bool previewEnabled()	{ return m_previewEnabled; }
	
	QByteArray saveState();
	bool restoreState (const QByteArray & state);
	
	SlideGroup *currentTemplate() { return m_template; } 
	
signals:
	void songSelected(SongRecord*);
	
public slots:
	void setFilter(const QString& filter="");
	void clearFilter() { setFilter(); }
	
	void setEditingEnabled(bool);
	void setFilteringEnabled(bool);
	void setPreviewEnabled(bool);

protected slots:
	friend class MyQListView;
	void songDoubleClicked(const QModelIndex &);
	void songSingleClicked(const QModelIndex &);
	void songFilterChanged(const QString&);
	void songFilterReset();
	void songSearchReturnPressed();
	void addNewSong();
	void songCreated(SongRecord*);
	void editSongAccepted();
	void editSongInDB();
	void deleteCurrentSong();
	
	void templateChanged(SlideGroup*);
	
	void searchOnline();
	
protected:
	void setupUI();

	bool m_editingEnabled;
	bool m_filteringEnabled;
	bool m_previewEnabled;
	
	QWidget	      * m_searchBase;
	QWidget       * m_editingButtons;
	MyQListView   * m_songList;
	QLineEdit     * m_songSearch;
	QPushButton   * m_clearSearchBtn;
	SongRecordListModel * m_songListModel;
	QTextEdit     * m_songTextPreview;
	QSplitter     * m_splitter;
	QPushButton   * m_onlineBtn;
	
	SlideGroup    * m_template;
	

};


#endif
