#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QShowEvent>
#include <QModelIndex>

class QFileSystemModel;
class DirectoryListModel;
class MyQFileIconProvider;

namespace Ui {
    class MainWindow;
}

#define MEDIABROWSER_LIST_ICON_SIZE QSize(200,150)

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void changeEvent(QEvent *e);
	
	void saveWindowState();
	void loadWindowState();
	
	void setupFileBrowser();
	
	void closeEvent(QCloseEvent*);
	void showEvent(QShowEvent*);

private slots:
	void fileBrowserShowEvent();
	void fileBrowserDoubleClicked(const QModelIndex&);
	
	void sizeBoxChanged(int);

private:
	Ui::MainWindow *ui;
	QFileSystemModel * m_fsModel;
	
	DirectoryListModel * m_dirModel;
	
	int m_scrollToHackCount;
	
	MyQFileIconProvider * m_iconProvider;
};

#endif // MAINWINDOW_H
