 #include "DirectoryListModel.h"

// #include <QPixmap>
// #include <QTimer>
// #include <QAbstractListModel>
// #include <QFileIconProvider>
// #include <QFileInfo>

#include <QPainter>
#include <QPixmapCache>
#include <QDebug>
#include <QThreadPool>

#include "3rdparty/md5/qtmd5.h"
#include "AppSettings.h"

#include "../exiv2-0.18.2-qtbuild/src/image.hpp"

#include <string>
#include <iostream>
#include <cassert>

#define CACHE_DIR "dirlistmodel-iconcache"

#define SMALLEST_STD_SIZE QSize(160,120)

bool operator< ( const QSize & s1, const QSize & s2 )
{
	return s1.width()   < s2.width() && 
		s1.height() < s2.height();
}
bool operator> ( const QSize & s1, const QSize & s2 )
{
	return s1.width()   > s2.width() && 
		s1.height() > s2.height();
}
bool operator<= ( const QSize & s1, const QSize & s2 )
{
	return s1.width()   <= s2.width() && 
		s1.height() <= s2.height();
}

LoadImageTask::LoadImageTask(const QFileInfo& info, const QSize& requestedSize, QList<QSize> standardSizesNeeded, ThreadedIconProvider *provider)
	: QObject()
	, QRunnable()
	, m_info(info)
	, m_size(requestedSize)
	, m_provider(provider)
	, m_need(standardSizesNeeded)
{
}

void LoadImageTask::run()
{
	QString file = m_info.canonicalFilePath();
	
	int rotateDegrees = 0;
	
	try
	{
		Exiv2::Image::AutoPtr exiv = Exiv2::ImageFactory::open(file.toStdString()); 
		
		if(exiv.get() != 0)
		{
			exiv->readMetadata();
			Exiv2::ExifData& exifData = exiv->exifData();
			if (exifData.empty()) 
			{
				qDebug() << file << ": No Exif data found in the file";
			}
			else
			{
				// adjust thumbnail for proper rotation
				QString rotateSensor = exifData["Exif.Image.Orientation"].toString().c_str();
				int rotationFlag = rotateSensor.toInt(); 
				rotateDegrees = rotationFlag == 1 ||
						rotationFlag == 2 ? 0 :
						rotationFlag == 7 ||
						rotationFlag == 8 ? -90 :
						rotationFlag == 3 ||
						rotationFlag == 4 ? -180 :
						rotationFlag == 5 ||
						rotationFlag == 6 ? -270 :
						0;
			}
		}
	}
	catch (Exiv2::AnyError& e) 
	{
		qDebug() << "Caught Exiv2 exception"; //: '" << e << "'";
	}
	
	bool needThumb = false;
	// First, to get the user something quick, return a scaled-up exif thumbnail
	if(needThumb)
	{
//		bool gotThumb = false;
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
					qDebug() << file << ": Attempting to load thumnail (" << exifThumb.mimeType() << ", "
						<< buf.size_ << " Bytes)";
					
					QImage thumb;
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
							
						QSize thumbSize = thumb.size();
						
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
						// This will look dirty if scaled above thumbnail size - hence the thread
						// below loading the original image and scaling down to this size instead of
						// scaling thumnail up. But the goal here is not a perfect image, its *any* 
						// image, returned as quickly as possible to the user, while a higher-quality
						// image follows soon after.
						thumb = thumb.scaled(m_size,Qt::KeepAspectRatio,Qt::SmoothTransformation);
						
						// center thumb if rotated
						if(abs(rotateDegrees) == 90 || abs(rotateDegrees) == 270)
						{
							QImage centeredCache(m_size,QImage::Format_ARGB32_Premultiplied);
							centeredCache.fill((uint)QColor(0,0,0,0).rgba());
							int pos = m_size.width() /2 - thumb.width() / 2;
							QPainter painter(&centeredCache);
							painter.drawImage(pos,0,thumb);
							thumb = centeredCache;
							//qDebug() << file << " * Centered rotated pixmap in frame";
						}
						
// 						// Even tho this is the exif thumb, we can try and save
// 						// the preload thread some work if its within the small size.
// 						// Note we're using the size before rotation and clipping
// 						// for comaprrison.
// 						if(thumbSize <= SMALLEST_STD_SIZE)
// 						{
// 							SMALLEST_STD_SIZE
// 							emit imageLoaded(m_info,m_size,scaled,true);
// 							QString key = cacheKey(info,SMALLEST_STD_SIZE);
// 							QPixmapCache::insert(key,thumb);
// 							thumb.save(key,"PNG");
// 						}
// 						
						emit imageLoaded(m_info,m_size,thumb,true);
						
						
// 						cache.save(cacheFile,"PNG");
// 						////qDebug() << "MyQFileIconProvider::icon(): image file: caching to:"<<cacheFile<<" for "<<file;
// 						QPixmapCache::insert(cacheFile,cache);
						
						//gotThumb = true;
// 						validReturn = true;
						
						qDebug() << file << ": Succesfully loaded Exiv thumnail and scaled to size "<<m_size; //, wrote to: "<<cacheFile;
					}
				}
			}
		}
	}
	
	// Load the image
	QImage image(file);
	if(image.isNull())
	{
		qDebug() << file << ": FAILED to load original image";
		emit taskFinished(m_info);
	}
	
	qDebug() << file << ": Succesfully loaded original image";
	
	QTransform rotationTransform = QTransform().rotate(rotateDegrees);
	
	if(rotateDegrees != 0)
		image = image.transformed(rotationTransform);
	
	// Scale the user-requested size first
	QImage scaled = image.scaled(m_size,Qt::KeepAspectRatio);
	
	
	// center image if rotated
	if(abs(rotateDegrees) == 90 || abs(rotateDegrees) == 270)
	{
		QImage centeredCache(m_size,QImage::Format_ARGB32_Premultiplied);
		centeredCache.fill((uint)QColor(0,0,0,0).rgba());
		int pos = m_size.width()/2 - scaled.width()/2;
		QPainter painter(&centeredCache);
		painter.drawImage(pos,0,scaled);
		scaled = centeredCache;
		//qDebug() << file << " * Centered rotated pixmap in frame";
	}
	
	
	emit imageLoaded(m_info,m_size,scaled,true);
	qDebug() << file << ": Scaled orig to requested size "<<m_size<<", actual size:"<<scaled.size();

/*	// Procede down the list (e.g. largest to smallest) so we can discard
	// the largest sizes as we go downward.
	while(!m_need.isEmpty())
	{
		QSize size = m_need.takeLast();
		image = image.scaled(size,Qt::KeepAspectRatio);
		
		// center image if rotated
		if(abs(rotateDegrees) == 90 || abs(rotateDegrees) == 270)
		{
			QImage centeredCache(size,QImage::Format_ARGB32_Premultiplied);
			centeredCache.fill((uint)QColor(0,0,0,0).rgba());
			int pos = size.width()/2 - image.width()/2;
			QPainter painter(&centeredCache);
			painter.drawImage(pos,0,image);
			image = centeredCache;
			//qDebug() << file << " * Centered rotated pixmap in frame";
		}
		
		emit imageLoaded(m_info,size,image,false);
		
		qDebug() << file << ": Scaled orig to std size "<<size<<", actual size:"<<image.size();
	}*/
	
	qDebug() << file << ": Thread finished";
	emit taskFinished(m_info);
}
	
// 	void imageLoaded(const QFileInfo&, const QSize&, const QImage&, bool explicitRequest, bool finished);
	
/*private:	
	QString m_file;
	QSize m_size;
	ThreadedIconProvider * m_provider;*/


ThreadedIconProvider::ThreadedIconProvider()
	: QObject()
	, m_queueSize(0)
{}

bool ThreadedIconProvider::requestIcon(const QFileInfo& info, const QSize& size)
{
	// If already in RAM, return size requested and stop there
	if(isSizeCached(info,size))
	{
		QPixmap tmp;
		QPixmapCache::find(cacheKey(info,size),tmp);
// 		return tmp;
		qDebug() << "ThreadedIconProvider::requestIcon: Icon already cached for "<<info.canonicalFilePath()<<" at size "<<size; 
		emit iconReady(info,tmp);
		return true;
	}
	
// 	// Not in RAM, so return *something* and queue thread to generate requested size
// 	QPixmap returnPixmap;
// 	bool validReturn = false;
// 	
// 	// First, check for a thumnail to scale up
// 	if(isSizeCached(info,SMALLEST_STD_SIZE))
// 	{
// 		QPixmap tmp;
// 		QPixmapCache::find(cacheKey(info,SMALLEST_STD_SIZE),tmp);
// 		returnPixmap = tmp.scaled(size);
// 		validReturn = true;
// 	}
	
// 	// If exif thumbnail failed, return empty pixmap (but not invalid - OpenGL hates invalid pixmaps) 
// 	if(!validReturn)
// 	{
// 		returnPixmap = QPixmap(size);
// 		returnPixmap.fill(Qt::transparent);
// 	}
	
	
	QString can = info.canonicalFilePath();
	m_sizesInProcess[can] = size;
	
	m_queueSizeMutex.lock();
	
	if(m_inProcess.isEmpty())
		QTimer::singleShot(25,this,SLOT(processFirstItem()));
	
	if(!m_inProcess.contains(can))
		m_inProcess.append(can);
	
	emit queueSizeChanged(m_inProcess.size());
		
	m_queueSizeMutex.unlock();
			
	
	
	return false;
	
// 	return returnPixmap;
}

int ThreadedIconProvider::queueSize()
{
	int copy;
	
	m_queueSizeMutex.lock();
	copy = m_inProcess.size();
	m_queueSizeMutex.unlock();
	
	return copy;
}

void ThreadedIconProvider::clearQueue()
{
	m_queueSizeMutex.lock();
	m_inProcess.clear();
	m_queueSizeMutex.unlock();
}

void ThreadedIconProvider::prioritize(const QFileInfo& inf)
{
	m_queueSizeMutex.lock();
	
	QString can = inf.canonicalFilePath();
	int index = m_inProcess.indexOf(can);
	if(index < 0)
	{
		qDebug() << "ThreadedIconProvider::prioritize: file "<<can<<" not found, not prioritizing";
		m_queueSizeMutex.unlock();
		return;
	}
	
	QList<QString> newList;
	newList << m_inProcess.at(index);
	if(index > 0)
		newList << m_inProcess.at(index-1);
	if(index < m_inProcess.size() - 1)
		newList << m_inProcess.at(index+1);
		
	qDebug() << "ThreadedIconProvider::prioritize: Found "<<can<<" at "<<index<<", initial newList:"<<newList;
	
		
	while(!m_inProcess.isEmpty())
	{
		QString item;
		if(index < m_inProcess.size())
		{
			item = m_inProcess.takeAt(index);
		}
		else
		{
			item = m_inProcess.takeFirst();
		}
		
		if(!newList.contains(item))
			newList.append(item);
	}
	
	m_inProcess = newList;
	
	
	m_queueSizeMutex.unlock();
}


/*
signals:
	void iconReady(const QFileInfo&, const QPixmap&);
	
	void queueSizeChanged(int);
	void queueEmpty();
	*/
// protected:
	
bool ThreadedIconProvider::isSizeCached(const QFileInfo& info, const QSize& size)
{
	QPixmap tmp;
	QString key = cacheKey(info,size);
	bool flag = QPixmapCache::find(key,tmp);
	if(!flag)
	{
		QFile file(key);
		if(file.exists())
		{
			tmp.load(key);
			QPixmapCache::insert(key,tmp);
			flag = true;
		}
	}
	return flag;
}

QList<QSize> ThreadedIconProvider::standardSizes()
{
	if(m_standardSizes.isEmpty())
	{
		m_standardSizes
			<< QSize(160,120)
// 			<< QSize(320,240)
			<< QSize(640,480)
// 			<< QSize(800,600)
			<< QSize(1024,768)
// 			<< QSize(1600,1200)
			;
	}
	return m_standardSizes;
}
	
QSize ThreadedIconProvider::nextLargestStandardSize(const QSize& size)
{
	QList<QSize> sizes = standardSizes();
	foreach(QSize tmp, sizes)
		if(tmp > size)
			return tmp;
	return QSize(-1,-1);
	
}

QString ThreadedIconProvider::cacheKey(const QFileInfo& info, const QSize& size)
{
	QDir path(QString("%1/%2").arg(AppSettings::cachePath()).arg(CACHE_DIR));
	if(!path.exists())
		QDir(AppSettings::cachePath()).mkdir(CACHE_DIR);

	QString cacheFile = QString("%1/%2/%3-%4x%5")
				.arg(AppSettings::cachePath())
				.arg(CACHE_DIR)
				.arg(MD5::md5sum(info.canonicalFilePath()))
				.arg(size.width())
				.arg(size.height());
	return cacheFile;
}

// private slots:
void ThreadedIconProvider::imageLoaded(const QFileInfo& info, const QSize& size, const QImage& image, bool explicitRequest)
{
	QPixmap pixmap = QPixmap::fromImage(image);
	
	QString key = cacheKey(info,size);
	
	QPixmapCache::insert(key,pixmap);
	pixmap.save(key,"PNG");
	
	if(explicitRequest)
	{
		QString can = info.canonicalFilePath();
		if(m_sizesInProcess[can] == size)
		{
			qDebug() << "ThreadedIconProvider::imageLoaded: file:"<<info.canonicalFilePath()<<" finished, queue size:"<<queueSize()<<", because size "<<size<<" = "<<m_sizesInProcess[can]; 
			emit iconReady(info,pixmap);
		}
		else
		{
			qDebug() << "ThreadedIconProvider::imageLoaded: Not notifying of updated pixmap for "<<can<<", because size "<<size<<" is not the latest in process:"<<m_sizesInProcess[can]; 
		}
		
		m_sizesInProcess.remove(can);
	}
}

void ThreadedIconProvider::taskFinished(const QFileInfo& info)
{
	m_queueSizeMutex.lock();
	
	m_inProcess.removeAll(info.canonicalFilePath());
	
	emit queueSizeChanged(m_inProcess.size());
	
// 	qDebug() << "m_inProcess:"<<m_inProcess.isEmpty() <<","<<m_inProcess;
	
	if(m_inProcess.isEmpty())
	{
		emit queueEmpty();
	}
	else
	{
		processFirstItem();
	}
		
	m_queueSizeMutex.unlock();
}

void ThreadedIconProvider::processFirstItem()
{
	if(m_inProcess.isEmpty())
		return;
		
	QString file = m_inProcess.takeFirst();
	QFileInfo info(file);
	
	qDebug() << "ThreadedIconProvider::taskFinished: next file:"<<info.canonicalFilePath()<<" finished, queue size:"<<m_inProcess.size();
	
	QSize size = m_sizesInProcess[file];
// 	m_sizesInProcess.remove(file);
	
	QList<QSize> std = standardSizes();
	QList<QSize> standardSizesNeeded;
	foreach(QSize tmp, std)
		if(!isSizeCached(info,tmp))
			standardSizesNeeded << tmp;
		
	LoadImageTask * task = new LoadImageTask(info,size,standardSizesNeeded,this);
	connect(task, SIGNAL(imageLoaded(QFileInfo,QSize,QImage,bool)), this, SLOT(imageLoaded(QFileInfo,QSize,QImage,bool)));
	connect(task, SIGNAL(taskFinished(QFileInfo)), this, SLOT(taskFinished(QFileInfo)));
	
	// QThreadPool takes ownership and deletes 'task' automatically
	QThreadPool::globalInstance()->start(task);
}
	
// 	int m_queueSize;
// 	QMutex m_queueSizeMutex;
// 	QList<QSize> standardSizes;


#define NEED_PIXMAP_TIMEOUT 100
#define NEED_PIXMAP_TIMEOUT_FAST 100

QPixmap * DirectoryListModel::m_blankPixmap = 0;
int DirectoryListModel::m_blankPixmapRefCount = 0;

DirectoryListModel::DirectoryListModel(QObject *parent)
	: QAbstractListModel(parent)
	, m_iconProvider(0)
	, m_lockRowcount(false)
	, m_iconSize(200,150)
	, m_nameLengthMax(16)
	, m_dirNull(false)
{
	qRegisterMetaType<QFileInfo>("QFileInfo");
	
	if(!m_blankPixmap)
	{
		//double ratio = AppSettings::liveAspectRatio();
		//double ratio = 4/3;
		m_blankPixmap = new QPixmap(m_iconSize); //48,(int)(48 * (1/ratio)));
// 		m_blankPixmap->fill(Qt::lightGray);
		m_blankPixmap->fill(Qt::transparent);
// 		QPainter painter(m_blankPixmap);
// 		painter.setPen(QPen(Qt::black,1,Qt::DotLine));
// 		painter.drawRect(m_blankPixmap->rect().adjusted(0,0,-1,-1));
// 		painter.end();
	}
	
	m_blankPixmapRefCount ++;
	
	m_iconTypeCache[QFileIconProvider::Folder] = iconProvider()->icon(QFileIconProvider::Folder);
	m_iconTypeCache[QFileIconProvider::Drive]  = iconProvider()->icon(QFileIconProvider::Drive);
	m_iconTypeCache[QFileIconProvider::File]   = iconProvider()->icon(QFileIconProvider::File);
	
	connect(&m_needPixmapTimer, SIGNAL(timeout()), this, SLOT(makePixmaps()));
	
	// Using similar formula as CPAN's config for the -j option: nbr cpus + 1
	QThreadPool::globalInstance()->setMaxThreadCount(1);//QThread::idealThreadCount() + 1);
	
	m_threadedIconProvider = new ThreadedIconProvider();
	connect(m_threadedIconProvider, SIGNAL(iconReady(QFileInfo, QPixmap)), this, SLOT(iconReady(QFileInfo, QPixmap)));
}

DirectoryListModel::~DirectoryListModel()
{
	m_blankPixmapRefCount --;
	if(m_blankPixmapRefCount <= 0)
	{
		delete m_blankPixmap;
		m_blankPixmap = 0;
	}

}
	
// QAbstractListModel::
int DirectoryListModel::rowCount ( const QModelIndex & /*parent */) const
{
	return m_entryList.size();
}

QVariant DirectoryListModel::data ( const QModelIndex & index, int role ) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= rowCount())
		return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		//QString file = fileInfo(index).fileName();
		//return file;
		return QVariant();
		//return file.left(m_nameLengthMax) + (file.length() > m_nameLengthMax ? "..." : "");
	}
	else
	if (role == Qt::ToolTipRole)
	{
		return fileInfo(index).fileName();
	}
	else if(Qt::DecorationRole == role)
	{
		QFileInfo info = fileInfo(index);
		QString   key  = cacheKey(info);
		
		QPixmap icon;
		if(!QPixmapCache::find(key,icon))
		{
			DirectoryListModel * self = const_cast<DirectoryListModel*>(this);
 			if(info.isFile())
 			{
 				if(!m_threadedIconProvider->requestIcon(info, m_iconSize))
 				{
 					//self->needPixmap(info.canonicalFilePath());
 				}
 				return *m_blankPixmap;
				
 			}
 			else
 			{
 				QFileIconProvider::IconType type = 
					    info.isDir() ?  QFileIconProvider::Folder:
					    info.isFile() ? QFileIconProvider::File  :
					    info.isRoot() ? QFileIconProvider::Drive :
					    QFileIconProvider::File;
				
				QIcon iconObject = m_iconTypeCache[type];
				icon = iconObject.pixmap(m_iconSize);
 				
 				QPixmapCache::insert(key,icon);
 			}
		}
		
		return icon;
	}
	else
		return QVariant();
}

// DirectoryListModel::
void DirectoryListModel::setDirectory(const QDir& d)
{
	m_threadedIconProvider->clearQueue();
	m_needPixmaps.clear();
	m_dir = d;
	m_listLoaded = false;
	loadEntryList();
}

void DirectoryListModel::setDirectory(const QString& str)
{
	if(str.isEmpty())
	{
		QStringList list; 
		setEntryList(list);
	}
	else
	{
		setDirectory(QDir(str));
	}
}

QModelIndex DirectoryListModel::indexForFile(const QString& file)
{
	return indexForRow(m_entryList.indexOf(file));
}

QModelIndex DirectoryListModel::indexForFile(const QFileInfo& info)
{
	return indexForFile(info.canonicalFilePath());
}

QModelIndex DirectoryListModel::indexForRow(int row)
{
	return createIndex(row,0);
}


QString DirectoryListModel::cacheKey(const QFileInfo& info) const
{
	return QString("dviz-dlm-%1-%2x%3").arg(info.canonicalFilePath()).arg(m_iconSize.width()).arg(m_iconSize.height());
}

QFileInfo DirectoryListModel::fileInfo(int row) const
{
	return row < m_entryList.size() ? 
		QFileInfo(m_entryList.at(row)) : 
		QFileInfo();
}

QFileInfo DirectoryListModel::fileInfo(const QModelIndex &idx) const
{
	return fileInfo(idx.row());
}

void DirectoryListModel::prioritize(const QFileInfo &inf)
{
	m_threadedIconProvider->prioritize(inf);
}

QFileIconProvider * DirectoryListModel::iconProvider() 
{
	return m_iconProvider ? m_iconProvider : &m_defaultIconProvider;
}

void DirectoryListModel::setIconProvider(QFileIconProvider *provider)
{
	m_iconProvider = provider;
}
	
void DirectoryListModel::setFilters(const QStringList &list)
{
	m_filters = list;
	m_listLoaded = false;
	loadEntryList();
}

void DirectoryListModel::setIconSize(const QSize& size)
{
	m_threadedIconProvider->clearQueue();
	m_needPixmaps.clear();
	foreach(QString info, m_entryList)
		QPixmapCache::remove(cacheKey(QFileInfo(info)));
		
	m_iconSize = size;
	foreach(QString info, m_entryList)
	{
		//needPixmap(info);
		QFileInfo fi(info);
		if(!m_threadedIconProvider->requestIcon(fi, m_iconSize))
 			needPixmap(info);
 	}
}

/** slots **/
void DirectoryListModel::makePixmaps()
{
	if(m_needPixmaps.isEmpty())
	{
		m_needPixmapTimer.stop();
		return;
	}
	
	QFileInfo info = QFileInfo(m_needPixmaps.takeFirst());
// 	qDebug() << "DirectoryListModel::makePixmaps: file:"<<info.canonicalFilePath()<<", remaining size:"<<m_needPixmaps.size();
	
	QString key = cacheKey(info);
	QPixmapCache::remove(key);
		
	QPixmap icon = generatePixmap(info);
	QPixmapCache::insert(key,icon);
	
	m_needPixmapTimer.stop();
	
	QModelIndex idx = indexForFile(info);
	dataChanged(idx,idx);
	
	if(!m_needPixmaps.isEmpty())
		m_needPixmapTimer.start(NEED_PIXMAP_TIMEOUT_FAST);
}
	
void DirectoryListModel::iconReady(const QFileInfo& info, const QPixmap& icon)
{
	qDebug() << "DirectoryListModel::iconReady: file:"<<info.canonicalFilePath()<<", queue size:"<<m_threadedIconProvider->queueSize();
	
	QString key = cacheKey(info);
	QPixmapCache::remove(key);
	
	QString file = info.canonicalFilePath();
	m_needPixmaps.removeAll(file);
		
	//QPixmap icon = generatePixmap(info);
	QPixmapCache::insert(key,icon);
	
 	m_needPixmapTimer.stop();
	
	QModelIndex idx = indexForFile(info);
	dataChanged(idx,idx);
	
	qDebug() << "DirectoryListModel::iconReady: file:"<<info.canonicalFilePath()<<", finished storing.";
	
 	if(!m_needPixmaps.isEmpty())
 		m_needPixmapTimer.start(NEED_PIXMAP_TIMEOUT_FAST);
}

	// for QFileSystemWatcher
// 	void directoryChanged ( const QString & path );
// 	void fileChanged ( const QString & path );
	

/** private **/
QPixmap DirectoryListModel::generatePixmap(const QFileInfo& info)
{
// 	qDebug() << "DirectoryListModel::generatePixmap: file:"<<info.canonicalFilePath();
	
	QIcon icon = iconProvider()->icon(info);
	if(icon.isNull())
	{
		QFileIconProvider::IconType type = 
					    info.isDir() ? QFileIconProvider::Folder :
					    info.isFile() ? QFileIconProvider::File  :
					    info.isRoot() ? QFileIconProvider::Drive :
					    QFileIconProvider::File;
		icon = iconProvider()->icon(type);
		//qDebug() << "DirectoryListModel::generatePixmap: file:"<<info.canonicalFilePath()<<", type:"<<type<<", null?"<<icon.isNull();
	}
	
	if(icon.isNull())
		return *m_blankPixmap;
		
	QPixmap pixmap = icon.pixmap(m_iconSize);
	if(pixmap.isNull())
		return *m_blankPixmap;
	
	return pixmap;
}

void DirectoryListModel::needPixmap(const QString& file)
{
// 	qDebug() << "DirectoryListModel::needPixmap: file:"<<file;
	if(!m_needPixmaps.contains(file))
		m_needPixmaps.append(file);
	if(!m_needPixmapTimer.isActive())
		m_needPixmapTimer.start(NEED_PIXMAP_TIMEOUT);
}

bool DirectoryListModel_sortFileList(QString a, QString b)
{
	QFileInfo ia(a), ib(b);
	if((ia.isDir() && ib.isDir()) ||
	   (ia.isFile() && ib.isFile()))
		return a.toLower() < b.toLower();
	else
	if(ia.isDir() && ib.isFile())
		return true;
	else
		return false;
}
	
void DirectoryListModel::setEntryList(const QStringList & list)
{
	m_threadedIconProvider->clearQueue();
	m_needPixmaps.clear();
	
	m_dirNull = false;
	m_dir = QDir();
	
	m_listLoaded = true;
	
	if(!m_entryList.isEmpty())
	{
		beginRemoveRows(QModelIndex(),0,m_entryList.size()); // hack - yes, I know
		m_entryList.clear();
		endRemoveRows();
	}
	
	beginInsertRows(QModelIndex(),0,m_entryList.size());
	
	m_entryList = list;
	
	qSort(m_entryList.begin(),m_entryList.end(),DirectoryListModel_sortFileList);
	
	endInsertRows();

}
	
void DirectoryListModel::loadEntryList()
{
	if(m_listLoaded)
		return;
	m_listLoaded = true;
	
	if(!m_entryList.isEmpty())
	{
		beginRemoveRows(QModelIndex(),0,m_entryList.size()); // hack - yes, I know
		m_entryList.clear();
		endRemoveRows();
	}
	
	QStringList entries = m_dir.entryList(m_filters, 
		QDir::AllDirs | 
		QDir::Drives  | 
		QDir::NoDotAndDotDot |
		QDir::Files
		);
	QString path = m_dir.absolutePath();
	
	beginInsertRows(QModelIndex(),0,entries.size());
	foreach(QString file, entries)
	{
		m_entryList << QFileInfo(QString("%1/%2").arg(path,file)).canonicalFilePath();
		//needPixmap(file);
	}
	qSort(m_entryList.begin(),m_entryList.end(),DirectoryListModel_sortFileList);
	endInsertRows();
}

