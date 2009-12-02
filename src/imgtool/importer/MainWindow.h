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
#include <QTimer>
#include <QRunnable>

#include "../exiv2-0.18.2-qtbuild/src/image.hpp"


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
	void imageLoaded(const QString&, const QImage&, const QImage&);
	
private:	
	QString m_file;
};

class MainWindow : public QMainWindow 
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	
public slots:
	void loadFolder(const QString& folder, bool copy=false, const QString& copyDest="", bool onlyShowHigh=false);

	void nextImage();
	void prevImage();
	void setCurrentImage(int);
	
protected slots:
	void showLoadDialog();
	void loadFolder();
	
	void loadGotoBoxValue();
	
	void imageLoaded(const QString&, const QImage&, const QImage&);
	
	void prepQueue();
	
	void bufferSliderChange(int);
	void sliderChangeFinished();
	
	void loadOriginalSize();
	
// 	void showRenameDialog();

	void useLastTitle();
	void useLastDescription();
	void useLastTags();
	void useLastLocation();
	void useLastBatch();
	
	void highCopyBrowse();
	

protected:
	bool event(QEvent*);
	void changeEvent(QEvent *e);
	
	bool queuePreload(QString file);
	
	void initMetaData(const QString& path="");
	void writeMetaData();
	
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
	
	Exiv2::Image::AutoPtr m_exivImage;
	
	int m_rotateDegrees;
	
	QTimer m_sliderBufferTimer;
	QTimer m_loadOriginalTimer;
	
	bool m_changingCombobox;
	
	QString m_lastTitle;
	QString m_lastDescription;
	QString m_lastTags;
	QString m_lastLocation;
	QString m_lastBatch;
	
	bool m_onlyShowHigh;
	
private:
	Ui::MainWindow *m_ui;
};

#endif // MAINWINDOW_H
