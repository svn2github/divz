#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileSystemModel>
#include <QSettings>
#include <QDesktopWidget>
#include <QDebug>
#include <QTimer>
#include <QPixmapCache>
#include <QPainter>

#include "3rdparty/md5/qtmd5.h"
#include "AppSettings.h"
#include "DirectoryListModel.h"
#include "../exiv2-0.18.2-qtbuild/src/image.hpp"

#include <string>
#include <iostream>
#include <cassert>


#define CACHE_DIR "photoworkspace-imageiconcache"

class MediaBrowser
{
public:
	static QRegExp videoRegexp;
	static QRegExp imageRegexp;
	static bool isImage(const QString&);
	static bool isVideo(const QString&);
	static QPixmap iconForImage(const QString & file, const QSize & size);
};

QRegExp MediaBrowser::videoRegexp = QRegExp("(wmv|mpeg|mpg|avi|wmv|flv|mov|mp4|m4a|3gp|3g2|mj2|mjpeg|ipod|m4v|gsm|swf|dv|dvd|asf|mtv|roq|aac|ac3|aiff|alaw|iif)",Qt::CaseInsensitive);
QRegExp MediaBrowser::imageRegexp = QRegExp("(png|jpg|bmp|svg|xpm|gif)",Qt::CaseInsensitive);

bool MediaBrowser::isVideo(const QString &extension) { return extension.indexOf(videoRegexp) == 0; }
bool MediaBrowser::isImage(const QString &extension) { return extension.indexOf(imageRegexp) == 0; }

QPixmap MediaBrowser::iconForImage(const QString & file, const QSize & size)
{
	QPixmap cache;
	QDir path(QString("%1/%2").arg(AppSettings::cachePath()).arg(CACHE_DIR));
	if(!path.exists())
		QDir(AppSettings::cachePath()).mkdir(CACHE_DIR);

	QString cacheFile = QString("%1/%2/%3-%4x%5")
				.arg(AppSettings::cachePath())
				.arg(CACHE_DIR)
				.arg(MD5::md5sum(file))
				.arg(size.width())
				.arg(size.height());


	//qDebug() << "MediaBrowser::iconForImage: file:"<<file<<", size:"<<size<<", cacheFile: "<<cacheFile;
	//QPixmap orig(file);
	//orig = orig.scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	//orig.detach();

	//return orig;

	if(!QPixmapCache::find(cacheFile,cache))
	{
		if(QFile(cacheFile).exists())
		{
			cache.load(cacheFile);
			QPixmapCache::insert(cacheFile,cache);
			//qDebug() << "MediaBrowser::iconForImage: file:"<<file<<", size:"<<size<<": hit DISK (loaded scaled from disk cache)";
		}
		else
		{
			bool gotThumb = false;
			Exiv2::Image::AutoPtr exiv = Exiv2::ImageFactory::open(file.toStdString()); 
			if(exiv.get() != 0)
			{
				exiv->readMetadata();
				Exiv2::ExifData& exifData = exiv->exifData();
				if (exifData.empty()) 
				{
					qDebug() << file << ": No Exif data found in the file";
				}
				Exiv2::ExifThumb exifThumb(exifData);
				std::string thumbExt = exifThumb.extension();
				if (thumbExt.empty()) 
				{
					qDebug() << file << ": Image does not contain an Exif thumbnail";
				}
				else
				{
					Exiv2::DataBuf buf = exifThumb.copy();
					if (buf.size_ != 0) 
					{
// 						qDebug() << file << ": Attempting to load thumnail (" << exifThumb.mimeType() << ", "
// 							<< buf.size_ << " Bytes)";
						
						QPixmap thumb;
						if(!thumb.loadFromData(buf.pData_, buf.size_))
						{
							qDebug() << file << "QPixmap::fromData() failed";
						}
						else
						{
							
							// adjust thumbnail for proper AR
							// The code for finding the image resolution is from exiv's source - actions.cpp
							long xdim = 0;
							long ydim = 0;
							Exiv2::ExifData::const_iterator md;
							
							md = exifData.findKey(Exiv2::ExifKey("Exif.Image.ImageWidth"));
							if (md == exifData.end()) 
								md = exifData.findKey(Exiv2::ExifKey("Exif.Photo.PixelXDimension"));
							if (md != exifData.end() && md->count() > 0) 
								xdim = md->toLong();
							
							md = exifData.findKey(Exiv2::ExifKey("Exif.Image.ImageLength"));
							if (md == exifData.end()) 
								md = exifData.findKey(Exiv2::ExifKey("Exif.Photo.PixelYDimension"));
							if (md != exifData.end() && md->count() > 0) 
								ydim = md->toLong();
							
							if (xdim != 0 && ydim != 0) 
							{
								// calculate AR of the actual image, and if the thumnail doesnt match
								// AR of image, then trim top and bottom of thumb, assuming camera
								// added black bars
								
								double bigAR   = (double)xdim / (double)ydim;
								double smallAR = (double)thumb.width() / (double)thumb.height();
								
								// convert to an integer by moving the decimal over 2 places inorder
								// to only compare AR's with two decimal places precision, ignoring 
								// very small AR differences
								int bigARi   = (int)(bigAR * 100);
								int smallARi = (int)(smallAR * 100);
								
								if(bigARi != smallARi)
								{
									int properThumbHeight = thumb.width() / bigAR;
									int blackBarHeight = (thumb.height() - properThumbHeight) / 2;
									
									// in test images, it seemed to be exactly 1 px off, hence the +/- 1(*2)
									QRect correctRect(0,blackBarHeight + 1,thumb.width(),thumb.height() - blackBarHeight * 2 - 2);
									thumb = thumb.copy(correctRect);
									 
									//qDebug() << "Adjusted thumb rect to"<<correctRect<<" ("<<bigARi<<"!="<<smallARi<<")";
								}
								else
								{
									//qDebug() << "Thumb not adjusted: bigARi:"<<bigARi<<",smallARi:"<<smallARi;
								}
							}
							else
							{
								//qDebug() << "Unable to check thumbnail resolution because could not find exif image res of original image.";
							}
							
							// adjust thumbnail for proper rotation
							QString rotateSensor = exifData["Exif.Image.Orientation"].toString().c_str();
							int rotationFlag = rotateSensor.toInt(); 
							int rotateDegrees = rotationFlag == 1 ||
									rotationFlag == 2 ? 0 :
									rotationFlag == 7 ||
									rotationFlag == 8 ? -90 :
									rotationFlag == 3 ||
									rotationFlag == 4 ? -180 :
									rotationFlag == 5 ||
									rotationFlag == 6 ? -270 :
									0;
							QTransform t = QTransform().rotate(rotateDegrees);
							thumb = thumb.transformed(t);
									
							// scale to desired size
							cache = thumb.scaled(size,Qt::KeepAspectRatio,Qt::SmoothTransformation);
							
							// center thumb if rotated
							if(abs(rotateDegrees) == 90 || abs(rotateDegrees) == 270)
							{
								QPixmap centeredCache(size);
								centeredCache.fill(Qt::transparent);
								int pos = size.width() /2 - cache.width() / 2;
								QPainter painter(&centeredCache);
								painter.drawPixmap(pos,0,cache);
								cache = centeredCache;
								//qDebug() << file << " * Centered rotated pixmap in frame";
							}
							
							cache.save(cacheFile,"PNG");
							////qDebug() << "MyQFileIconProvider::icon(): image file: caching to:"<<cacheFile<<" for "<<file;
							QPixmapCache::insert(cacheFile,cache);
							
							gotThumb = true;
							
// 							qDebug() << file << ": Succesfully loaded Exiv thumnail and scaled to size, wrote to: "<<cacheFile;
						}
					}
				}
			}
			
			if(!gotThumb)
			{
				QPixmap orig(file);
				if(orig.isNull())
				{
					cache = QPixmap();
					QPixmapCache::insert(cacheFile,cache);
					qDebug() << "MediaBrowser::iconForImage: file:"<<file<<", size:"<<size<<": load INVALID (Can't load original)";
				}
				else
				{
					cache = orig.scaled(size,Qt::KeepAspectRatio,Qt::SmoothTransformation);
					cache.save(cacheFile,"PNG");
					////qDebug() << "MyQFileIconProvider::icon(): image file: caching to:"<<cacheFile<<" for "<<file;
					QPixmapCache::insert(cacheFile,cache);
					//qDebug() << "MediaBrowser::iconForImage: file:"<<file<<", size:"<<size<<": load GOOD (loaded original and scaled)";
					//QApplication::processEvents();
				}
			}
		}
	}
	else
	{
		//qDebug() << "MediaBrowser::iconForImage: file:"<<file<<", size:"<<size<<": hit RAM (scaled image already in ram)";
	}

	//qDebug() << "MediaBrowser::data: iconForImage: file:"<<file<<", cacheKey:"<<cache.cacheKey();

	return cache;
}

class MyQFileIconProvider : public QFileIconProvider
{
public:
	MyQFileIconProvider() : QFileIconProvider(), m_iconSize(MEDIABROWSER_LIST_ICON_SIZE)  {}

	QIcon icon(const QFileInfo& info) const
	{
// 		DeepProgressIndicator * d = DeepProgressIndicator::indicatorForObject(dynamic_cast<void*>(const_cast<MyQFileIconProvider*>(this)));
// 		if(d)
// 			d->step();

		QApplication::processEvents();
// 		if(MediaBrowser::isVideo(info.suffix()))
// 		{
// 			//qDebug() << "MyQFileIconProvider::icon(): video file:"<<info.absoluteFilePath();
// 			return QVideoProvider::iconForFile(info.absoluteFilePath());
// 			//return QFileIconProvider::icon(info);
// 		}
// 		else
		if(MediaBrowser::isImage(info.suffix()))
		{
			//qDebug() << "MyQFileIconProvider::icon(): image file:"<<info.absoluteFilePath();
			return MediaBrowser::iconForImage(info.absoluteFilePath(),m_iconSize);
		}
		else
		{
			return QFileIconProvider::icon(info);
		}
	}

	void setIconSize(QSize s) { m_iconSize = s; }
private:
	QSize m_iconSize;

};




MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, m_scrollToHackCount(0)
{
	ui->setupUi(this);
	
	setupFileBrowser();
	
	loadWindowState();
	
	setWindowTitle("Photo Workspace");
	
	connect(ui->sizeBox, SIGNAL(valueChanged(int)), this, SLOT(sizeBoxChanged(int)));
	sizeBoxChanged(50);
}


void MainWindow::sizeBoxChanged(int percent)
{
	double perc = ((double)percent) / 100.0;
	int stdSize = 160;
	int newSize = stdSize * perc;
	int newHeight = newSize * 0.75;
	
	QSize iconSize(newSize,newHeight);
	
	m_iconProvider->setIconSize(iconSize);
	m_dirModel->setIconSize(iconSize);
	ui->mainList->setIconSize(iconSize);
	ui->mainList->setGridSize(iconSize + QSize(3,3));
	
	qDebug() << "sizeBoxChanged(): int:"<<percent<<", perc:"<<perc<<", iconSize: "<<iconSize;
}

void MainWindow::loadWindowState()
{
	QDesktopWidget *d = QApplication::desktop();
	QRect primary = d->screenGeometry(d->primaryScreen());

	QSettings settings;
	QSize sz = settings.value("mainwindow/size").toSize();
	if(sz.isValid())
		resize(sz);

	QPoint p = settings.value("mainwindow/pos").toPoint();
	if(p.isNull())
		p = primary.topLeft();
	if(p.y() < 0)
		p.setY(1);
	move(p);
	
	restoreState(settings.value("mainwindow/state").toByteArray());
	ui->leftSplitter->restoreState(settings.value("mainwindow/left_split").toByteArray());
	ui->rightSplitter->restoreState(settings.value("mainwindow/right_split").toByteArray());
	ui->mainSplitter->restoreState(settings.value("mainwindow/main_split").toByteArray());

}

void MainWindow::saveWindowState()
{
	QSettings settings;
	settings.setValue("mainwindow/size",size());
	settings.setValue("mainwindow/pos",pos());
	settings.setValue("mainwindow/state",saveState());
	settings.setValue("mainwindow/left_split",ui->leftSplitter->saveState());
	settings.setValue("mainwindow/right_split",ui->rightSplitter->saveState());
	settings.setValue("mainwindow/main_split",ui->mainSplitter->saveState());
	
	
	QModelIndex cur = ui->folderTree->currentIndex();
	if(cur.isValid())
	{
		QString path = m_fsModel->filePath(cur);
		settings.setValue("mainwindow/filebrowser/last_path",path);
	}
	else
	{
		qDebug() << "saveWindowState(): No current index";
	}
	
	
}

void MainWindow::closeEvent(QCloseEvent*)
{
	saveWindowState();
}

void MainWindow::setupFileBrowser()
{
	m_fsModel = new QFileSystemModel;
	
	m_fsModel->setResolveSymlinks(true);
	m_fsModel->setFilter(QDir::AllDirs | QDir::Drives | QDir::NoDotAndDotDot);
	
	QModelIndex root = m_fsModel->setRootPath(QDir::rootPath());
	
	ui->folderTree->setModel(m_fsModel);
	ui->folderTree->setRootIndex(root);
	
	ui->folderTree->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui->folderTree->header()->setResizeMode(1, QHeaderView::ResizeToContents);
	ui->folderTree->header()->setResizeMode(2, QHeaderView::ResizeToContents);
	ui->folderTree->header()->setResizeMode(3, QHeaderView::ResizeToContents);
	
	ui->folderTree->setSelectionBehavior(QAbstractItemView::SelectRows);
	//ui->folderTree->setRootIsDecorated(false);
	//ui->folderTree->setItemsExpandable(false);
	ui->folderTree->setSortingEnabled(true);
	ui->folderTree->header()->setSortIndicator(0, Qt::AscendingOrder);
	ui->folderTree->header()->setStretchLastSection(false);
	ui->folderTree->setTextElideMode(Qt::ElideMiddle);
// 	ui->folderTree->setEditTriggers(QAbstractItemView::EditKeyPressed);
// 	ui->folderTree->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui->folderTree, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(fileBrowserDoubleClicked(const QModelIndex&)));
	connect(ui->folderTree, SIGNAL(clicked(const QModelIndex&)), this, SLOT(fileBrowserDoubleClicked(const QModelIndex&)));


	m_dirModel = new DirectoryListModel;
	m_iconProvider = new MyQFileIconProvider;
	m_dirModel->setIconProvider(m_iconProvider);
	ui->mainList->setModel(m_dirModel);
	
	//ui->mainList->setAlternatingRowColors(true);
	//ui->mainList->setIconSize(m_iconSize);
	//ui->mainList->setIconSize(MEDIABROWSER_LIST_ICON_SIZE);
	//ui->mainList->setIconSize(m_iconSize);
	//ui->mainList->setWrapping(true);
	ui->mainList->setWordWrap(true);
	ui->mainList->setGridSize(MEDIABROWSER_LIST_ICON_SIZE + QSize(3,3));
	//ui->mainList->setLayoutMode(QListView::Batched);
	//ui->mainList->setFlow(QListView::LeftToRight);
	ui->mainList->setResizeMode(QListView::Adjust);
	ui->mainList->setViewMode(QListView::IconMode);
	
	connect(ui->mainList, SIGNAL(clicked(QModelIndex)), m_dirModel, SLOT(prioritize(QModelIndex)));
	
// 	QPalette pal = ui->mainList->viewport()->palette();
// 	pal.setColor(QPalette::Window, Qt::black);
// 	ui->mainList->viewport()->setPalette(pal);


	QString stylesheet =
		" QListView {"
		"     show-decoration-selected: 1;" /* make the selection span the entire width of the view */
		"     background: black;"
		" }"
		""
		" QListView::item:alternate {"
		"     background: #EEEEEE;"
		" }"
		""
		" QListView::item:selected {"
		"     background: yellow;"
		" }"
		""
// 		" QListView::item:selected:!active {"
// 		"     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
// 		"                                 stop: 0 #ABAFE5, stop: 1 #8588B2);"
// 		" }"
// 		""
// 		" QListView::item:selected:active {"
// 		"     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
// 		"                                 stop: 0 #6a6ea9, stop: 1 #888dd9);"
// 		" }"
		""
// 		" QListView::item:hover {"
// 		"     background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
// 		"                                 stop: 0 #FAFBFE, stop: 1 #DCDEF1);"
// 		" }"
		;

	ui->mainList->setStyleSheet(stylesheet);
	
}

void MainWindow::showEvent(QShowEvent*)
{
	QTimer::singleShot(10,this,SLOT(fileBrowserShowEvent()));
}

void MainWindow::fileBrowserShowEvent()
{
	QSettings settings;
	
	QString path = settings.value("mainwindow/filebrowser/last_path",QDir::currentPath()).toString();
	qDebug() << "setupFileBrowser(): Last path:"<<path;
	QModelIndex cur = m_fsModel->index(path);
	ui->folderTree->setCurrentIndex(cur);
	
	ui->folderTree->scrollTo(cur,QAbstractItemView::PositionAtCenter);
	QApplication::processEvents();
	
	// This is *very* hackish - the 'scrollTo()' function seems not work if the tree is not visible on screen.
	// Not only that, but it also doesnt take into account scrollbars and other items changing the scrollable 
	// space inside the viewport when it expands the tree, so therefore, the first time scrollTo() is called,
	// the item is left out of the viewport due to items changing. So, we need to call this scrollTo() routine
	// *again* inorder to get scrollTo() to be called with a "stable" expanded tree now. 
	// Now, I'm sure there is probably a better way - I don't know though - this works for now.
	
	if(m_scrollToHackCount == 0)
		m_dirModel->setDirectory(path);	
	
	if(m_scrollToHackCount++ < 2)
		QTimer::singleShot(250,this,SLOT(fileBrowserShowEvent()));
}

void MainWindow::fileBrowserDoubleClicked(const QModelIndex& idx)
{
	if(!idx.isValid())
		return;
		
	QString path = m_fsModel->filePath(idx);
	
	m_dirModel->setDirectory(path);
	
	ui->mainList->scrollTo(m_dirModel->index(0,0));

}


MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type())
	{
	    case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	    default:
		break;
	}
}
