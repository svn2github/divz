#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include <QDir>
#include <QLabel>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QShowEvent>
#include <QCache>

#include <QRunnable>

namespace Ui {
    class MainWindow;
}

class PreloadImageTask : public QObject, public QRunnable
{
	Q_OBJECT
public:
	PreloadImageTask(QString file);
	void run();
	
signals:
	void imageLoaded(const QString&, const QImage&);
	
private:	
	QString m_file;
};

class MainWindow : public QMainWindow 
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected slots:
	void showLoadDialog();
	void loadFolder();
	
	void nextImage();
	void prevImage();
	void setCurrentImage(int);
	
	void loadGotoBoxValue();
	
	void imageLoaded(const QString&, const QImage&);
	
protected:
	bool event(QEvent*);
	void changeEvent(QEvent *e);
	void resizeEvent(QResizeEvent *);
	void showEvent(QShowEvent *);
	void adjustViewScaling();
	
	bool queuePreload(QString file);
	
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
	
	QCache<QString, QImage> m_cache;
	QString m_filesInProcess;
	
	int m_lookBehind;
	int m_lookAhead;
	
	
private:
	Ui::MainWindow *m_ui;
};

#endif // MAINWINDOW_H
