#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow 
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected slots:
	void slotBrowse();
	void loadFolder();
	
protected:
	void changeEvent(QEvent *e);
	
	QStringList fileList;
	QHash<QString,QString> newFileMap;
	bool removeOriginals;
	
	int currentFile;
	
private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
