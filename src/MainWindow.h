#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

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

public slots:
	void setLiveGroup(SlideGroup*);
	void editGroup(SlideGroup*);
	void deleteGroup(SlideGroup*);
	void previewSlideGroup(SlideGroup*);

protected slots:
	void groupSelected(const QModelIndex &);
	void groupDoubleClicked(const QModelIndex &);

	void actionEditGroup();
	void actionNewGroup();
	void actionDelGroup();

	void setupOutputs();

	//void groupSetLive(const QModelIndex &);

	//void slotListContextMenu(const QPoint &);
	
protected:
	void changeEvent(QEvent *e);
	
	void openSlideEditor(SlideGroup *g);

private:
	Ui::MainWindow *m_ui;
	Document m_doc;
	DocumentListModel m_docModel;
	SlideEditorWindow m_editWin;
	SlideGroupViewer *m_previewWidget;
	OutputViewer *m_liveMonitor;
	SlideGroupViewer *m_liveView;

	OutputSetupDialog *m_outputDialog;
};

#endif // MAINWINDOW_H
