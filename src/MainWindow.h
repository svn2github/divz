#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QCloseEvent>

#include <QSplitter>
#include <QListWidget>
#include <QPushButton>

#include <QListView>
#include <QLineEdit>
#include <QComboBox>

#include <QSortFilterProxyModel>
#include <QtSql>
#include <QDebug>
#include <QSqlTableModel>

#include "model/Document.h"
#include "DocumentListModel.h"
#include "SlideEditorWindow.h"
#include "SlideGroupViewer.h"
#include "OutputViewer.h"
#include "OutputSetupDialog.h"

#define FALLBACK_SCREEN_RECT QRectF(0,0,1024,768)

namespace Ui {
	class MainWindow;
}

class SlideGroupViewControl;
class SongSlideGroup;
class SongRecord;
class SongRecordListModel;

class MainWindow : public QMainWindow {
	Q_OBJECT
public /*static*/:
	static MainWindow * mw() { return static_mainWindow; }

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	
	Document * currentDocument() { return m_doc; }
	QRect standardSceneRect(double aspectRatio = -1);
	
signals:
	void documentChanged(Document*);
	void docSettingsChanged(Document*);
	void aspectRatioChanged(double);
	void appSettingsChanged();

public slots:
	void setLiveGroup(SlideGroup*, Slide *slide=0);
	void editGroup(SlideGroup*, Slide *slide=0);
	void deleteGroup(SlideGroup*);
	void previewSlideGroup(SlideGroup*);
	bool openFile(const QString &);
	void saveFile(const QString &file = "");
	void clearAllOutputs();
	void nextGroup();

protected slots:
	void groupsDropped(QList<SlideGroup*> list);
	
	void groupSelected(const QModelIndex &);
	void groupDoubleClicked(const QModelIndex &);
	
	void songDoubleClicked(const QModelIndex &);
	void songFilterChanged(const QString&);
	void songFilterReset();
	void songSearchReturnPressed();
	void addNewSong();
	void songCreated(SongRecord*);
	void editSongAccepted();
	void editSongInDB();
	void deleteCurrentSong();

	void actionEditGroup();
	void actionNewGroup();
	void actionDelGroup();
	void actionGroupProperties();

	void actionAppSettingsDialog();
	void actionDocSettingsDialog();

	void actionAboutDviz();
	void actionDvizWebsite();
	
	void actionOpen();
	bool actionSave();
	bool actionSaveAs();
	void actionNew();
	
	void saveWindowState();
	void loadWindowState();
	
protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *event);
	
	void openSlideEditor(SlideGroup *g,Slide *slide=0);
	
private:
	void setupOutputList();
	void setupOutputControl();
	void setupOutputViews();
	void setupCentralWidget();
	void setupSongList();

	Ui::MainWindow *m_ui;
	SlideEditorWindow * m_editWin;
	DocumentListModel * m_docModel;
	SlideGroupViewer  * m_previewWidget;
	OutputViewer      * m_liveMonitor;
	SlideGroupViewer  * m_liveView;

	QSplitter   * m_splitter;
	QSplitter   * m_splitter2;
	QListView   * m_groupView;
	QPushButton * m_btnSendOut;
	QListWidget * m_outputList;
	QTabWidget  * m_outputTabs;

	QListView     * m_songList;
	QLineEdit     * m_songSearch;
	QComboBox     * m_searchOpt;
	QPushButton   * m_clearSearchBtn;
	SongRecordListModel * m_songListModel;

	OutputSetupDialog * m_outputDialog;
	
	Document * m_doc;
	
	SlideGroupViewControl * m_viewControl;
	
	/** static */
	static MainWindow * static_mainWindow;	
};

#endif // MAINWINDOW_H
