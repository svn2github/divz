#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QCloseEvent>

#include <QSplitter>
#include <QListWidget>
#include <QPushButton>

#include "model/Document.h"
#include "DocumentListModel.h"
#include "SlideEditorWindow.h"
#include "SlideGroupViewer.h"
#include "OutputViewer.h"
#include "OutputSetupDialog.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	
	static Document * currentDocument() { return &m_doc; }

public slots:
	void setLiveGroup(SlideGroup*);
	void editGroup(SlideGroup*);
	void deleteGroup(SlideGroup*);
	void previewSlideGroup(SlideGroup*);

protected slots:
	void groupsDropped(QList<SlideGroup*> list);
	
	void groupSelected(const QModelIndex &);
	void groupDoubleClicked(const QModelIndex &);

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
	void setupCentralWidget();

	Ui::MainWindow *m_ui;
	DocumentListModel m_docModel;
	SlideEditorWindow m_editWin;
	SlideGroupViewer *m_previewWidget;
	OutputViewer *m_liveMonitor;
	SlideGroupViewer *m_liveView;

	QSplitter * m_splitter;
	QSplitter * m_splitter2;
	QListView * m_groupView;
	QPushButton * m_btnSendOut;
	QListWidget * m_outputList;
	QTabWidget * m_outputTabs;

	OutputSetupDialog *m_outputDialog;
	
	static Document m_doc;
	
	
};

#endif // MAINWINDOW_H
