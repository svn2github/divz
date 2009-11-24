#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include <QDir>
#include <QLabel>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QResizeEvent>

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
	
	void nextImage();
	void prevImage();
	void setCurrentImage(int);
	
protected:
	bool event(QEvent*);
	void changeEvent(QEvent *e);
	void resizeEvent(QResizeEvent *);
	void adjustViewScaling();
	
	QDir m_batchDir;
	QStringList m_fileList;
	QHash<QString,QString> m_newFileMap;
	
	bool m_copyFiles;
	QDir m_copyDest;
	bool m_removeOriginals;
	
	int m_currentFile;
	
	QPixmap * m_pixmap;
	QLabel * m_label;
	
	QGraphicsScene * m_scene;
	QGraphicsPixmapItem * m_pixmapItem;
	
private:
	Ui::MainWindow *m_ui;
};

#endif // MAINWINDOW_H
