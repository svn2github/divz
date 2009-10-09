#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QCloseEvent>

#include <QSplitter>
#include <QListWidget>
#include <QPushButton>

#include <QTableView>
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

namespace Ui {
	class MainWindow;
}

class SlideGroupViewControl;

class MainWindow : public QMainWindow {
	Q_OBJECT
public /*static*/:
	static MainWindow * mw() { return static_mainWindow; }

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	
	Document * currentDocument() { return &m_doc; }
	QRect standardSceneRect(double aspectRatio = -1);
	
signals:
	void documentChanged(Document*);
	void docSettingsChanged(Document*);
	void aspectRatioChanged(double);
	void appSettingsChanged();

public slots:
	void setLiveGroup(SlideGroup*);
	void editGroup(SlideGroup*);
	void deleteGroup(SlideGroup*);
	void previewSlideGroup(SlideGroup*);
	void openFile(const QString &);
	void saveFile(const QString &file = "");

protected slots:
	void groupsDropped(QList<SlideGroup*> list);
	
	void groupSelected(const QModelIndex &);
	void groupDoubleClicked(const QModelIndex &);
	
	void songDoubleClicked(const QModelIndex &);
	void songFilterChanged(const QString&);
	void songFilterReset();

	void actionEditGroup();
	void actionNewGroup();
	void actionDelGroup();

	void actionAppSettingsDialog();
	void actionDocSettingsDialog();

	void actionAboutDviz();
	void actionDvizWebsite();
	
protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *event);
	
	void openSlideEditor(SlideGroup *g);

private:
	void setupOutputList();
	void setupOutputControl();
	void setupOutputViews();
	void setupCentralWidget();
	void setupSongList();

	Ui::MainWindow *m_ui;
	SlideEditorWindow m_editWin;
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

	QTableView    * m_songList;
	QLineEdit     * m_songSearch;
	QComboBox     * m_searchOpt;
	QPushButton   * m_clearSearchBtn;
	QSortFilterProxyModel * m_songProxyModel;
	QSqlTableModel * m_songTableModel;

	OutputSetupDialog * m_outputDialog;
	
	Document m_doc;
	
	SlideGroupViewControl * m_viewControl;
	
	/** static */
	static MainWindow * static_mainWindow;	
};

#endif // MAINWINDOW_H
