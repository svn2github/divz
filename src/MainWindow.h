#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "model/Document.h"
#include "DocumentListModel.h"
#include "SlideEditorWindow.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected slots:
	void groupSelected(const QModelIndex &);
	
protected:
	void changeEvent(QEvent *e);
	
	void openSlideEditor(SlideGroup *g);

private:
	Ui::MainWindow *m_ui;
	Document m_doc;
	DocumentListModel m_docModel;
	SlideEditorWindow m_editWin;
	
};

#endif // MAINWINDOW_H
