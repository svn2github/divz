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

class SongRecord;
class SongBrowser;
class QFileInfo;
class OutputInstance;
class OutputControl;

class MainWindow : public QMainWindow {
	Q_OBJECT
public /*static*/:
	static MainWindow * mw() { return static_mainWindow; }

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	
	Document * currentDocument() { return m_doc; }
	QRect standardSceneRect(double aspectRatio = -1);
	
	OutputInstance * outputInst(int id) { return m_outputInstances.contains(id) ? m_outputInstances[id] : 0; }
	OutputControl * outputControl(int id) { return m_outputControls.contains(id) ? m_outputControls[id] : 0; }
	SlideGroupViewControl * viewControl(int id) { return m_viewControls.contains(id) ? m_viewControls[id] : 0; }
	
	
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
	
	
	void imageImportTool();

protected slots:
	void groupsDropped(QList<SlideGroup*> list);
	
	void groupSelected(const QModelIndex &);
	void groupDoubleClicked(const QModelIndex &);
	
	void songSelected(SongRecord*);
	
	void fileSelected(const QFileInfo&);
	void setSelectedBackground(const QFileInfo&);
	
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
	void setupOutputViews();
	void setupCentralWidget();
	void setupSongList();
	void setupMediaBrowser();
	
	Ui::MainWindow *m_ui;
	SlideEditorWindow * m_editWin;
	DocumentListModel * m_docModel;
	SlideGroupViewer  * m_previewWidget;
	OutputViewer      * m_liveMonitor;
	OutputInstance    * m_liveView;
	
	SongBrowser * m_songBrowser;

	QSplitter   * m_splitter;
	QSplitter   * m_splitter2;
	QListView   * m_groupView;
	QPushButton * m_btnSendOut;
	QListWidget * m_outputList;
	QTabWidget  * m_outputTabs;

	OutputSetupDialog * m_outputDialog;
	
	Document * m_doc;

	QTimer	* m_autosaveTimer;
	
	SlideGroupViewControl * m_viewControl;
	
	QMap<int, OutputInstance *> m_outputInstances;
	QMap<int, SlideGroupViewControl *> m_viewControls;
	QMap<int, OutputControl *> m_outputControls;
	
	OutputInstance * liveInst();// { return output(AppSettings::taggedOutput("live")->id()); }
	SlideGroupViewControl * liveCtrl();// { return viewControl(AppSettings::taggedOutput("live")->id()); }
	
	/** static */
	static MainWindow * static_mainWindow;	


};

#endif // MAINWINDOW_H
