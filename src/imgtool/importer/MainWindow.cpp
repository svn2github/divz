#include "MainWindow.h"
#include ".build/ui_MainWindow.h"

#include <QImage>
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QImageWriter>
#include <QDesktopServices>
#include <QtOpenGL/QGLWidget>
#include <QProgressDialog>
#include <QThreadPool>

#include "ImageRecord.h"
#include "ImageRecordListModel.h"

#include "LoadDialog.h"
#include "AppSettings.h"

#include <QUuid>

#include <string>
#include <iostream>
#include <cassert>

#define DEBUG_PRELOAD 1 


namespace {

	QString removeLangPrefix(const QString & orig)
	{
		QString copy = orig;
		return copy.replace(QRegExp("^lang=\"[^\"]+\"\\s*"),"");
	}
		
}


MainWindow::MainWindow(QWidget *parent) 
	: QMainWindow(parent)
	, m_pixmap(0)
	, m_ui(new Ui::MainWindow)
	, m_lookBehind(1)
	, m_lookAhead(4)
	, m_changingCombobox(false)
	, m_onlyShowHigh(false)
{
	m_ui->setupUi(this);
        connect(m_ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(m_ui->loadBtn, SIGNAL(clicked()), this, SLOT(showLoadDialog()));
// 	connect(m_ui->importFolder, SIGNAL(returnPressed()), this, SLOT(loadFolder()));
	connect(m_ui->nextBtn, SIGNAL(clicked()), this, SLOT(nextImage()));
	connect(m_ui->prevBtn, SIGNAL(clicked()), this, SLOT(prevImage()));
	connect(m_ui->gotoBox, SIGNAL(editingFinished()), this, SLOT(loadGotoBoxValue()));
	
	connect(m_ui->graphicsView, SIGNAL(nextImage()), this, SLOT(nextImage()));
	connect(m_ui->graphicsView, SIGNAL(prevImage()), this, SLOT(prevImage()));
	
	connect(m_ui->imageSlider, SIGNAL(valueChanged(int)), this, SLOT(bufferSliderChange(int)));
	
	connect(&m_sliderBufferTimer, SIGNAL(timeout()), this, SLOT(sliderChangeFinished()));
	m_sliderBufferTimer.setSingleShot(true);
	m_sliderBufferTimer.setInterval(500);
	
	connect(&m_loadOriginalTimer, SIGNAL(timeout()), this, SLOT(loadOriginalSize()));
	m_loadOriginalTimer.setSingleShot(true);
	m_loadOriginalTimer.setInterval(250);
	
	connect(m_ui->lastBatchBtn, SIGNAL(clicked()), this, SLOT(useLastBatch()));
	connect(m_ui->lastTagsBtn, SIGNAL(clicked()), this, SLOT(useLastTags()));
	connect(m_ui->lastTitleBtn, SIGNAL(clicked()), this, SLOT(useLastTitle()));
	connect(m_ui->lastDescriptionBtn, SIGNAL(clicked()), this, SLOT(useLastDescription()));
	connect(m_ui->lastLocationBtn, SIGNAL(clicked()), this, SLOT(useLastLocation()));
	
	
	connect(m_ui->highCopyBrowseBtn, SIGNAL(clicked()), this, SLOT(highCopyBrowse()));	
	m_ui->highCopyFolder->setText(AppSettings::previousPath("highcopyfolder"));
	

	m_scene = new QGraphicsScene();

	m_ui->graphicsView->setScene(m_scene);
	m_scene->setSceneRect(0,0,1024,768);
	
	m_pixmapItem = new QGraphicsPixmapItem();
	m_pixmapItem->setPos(0,0);
	m_scene->addItem(m_pixmapItem);
	
	m_cache.setMaxCost(512); 
	
	// Using similar formula as CPAN's config for the -j option: nbr cpus + 1
	QThreadPool::globalInstance()->setMaxThreadCount(QThread::idealThreadCount() + 1);
}

MainWindow::~MainWindow()
{
	writeMetaData();
	
	// Cleanup
	Exiv2::XmpParser::terminate();

	delete m_ui;
	if(m_pixmap)
		delete m_pixmap;
}

void MainWindow::bufferSliderChange(int value)
{
	m_loadOriginalTimer.stop();
	m_sliderBufferTimer.stop();
	
	if(m_currentFile != value)
	{
		m_currentFile = value;
		
		m_sliderBufferTimer.start();
	}
}

void MainWindow::sliderChangeFinished()
{
	setCurrentImage(m_currentFile);
}

void MainWindow::showLoadDialog()
{
	LoadDialog d(this);
	if(d.exec())
	{
		QString folder = d.batchFolder();
		
		m_batchDir = QDir(folder);
		if(!m_batchDir.exists())
		{
			QMessageBox::critical(0,"Folder Not Found","Sorry, but the folder you entered in the 'Batch Folder' box does not exist.");
			return;
		}
	
		m_copyFiles = d.copyFiles();
		m_copyDest = QDir(d.copyFolder());
		if(!m_copyDest.exists())
		{
			QMessageBox::critical(0,"Destination Folder Not Found","Sorry, but the folder you entered in the 'Copy To Folder' box does not exist.");
			return;
		}
		
		m_cache.setMaxCost(d.cacheSize());
		m_lookBehind = d.lookBehind();
		m_lookAhead = d.lookAhead();
		
		loadFolder();
	}
}

void MainWindow::loadFolder(const QString& folder, bool copy, const QString& copyDest, bool onlyShowHigh)
{
	m_onlyShowHigh = onlyShowHigh;
	
	m_batchDir = QDir(folder);
	if(!m_batchDir.exists())
	{
		QMessageBox::critical(0,"Folder Not Found","Sorry, but the folder you entered in the 'Batch Folder' box does not exist.");
		return;
	}

	m_copyFiles = copy;
	m_copyDest = QDir(copyDest);
	if(!m_copyDest.exists())
	{
		QMessageBox::critical(0,"Destination Folder Not Found","Sorry, but the folder you entered in the 'Copy To Folder' box does not exist.");
		return;
	}
	
	loadFolder();
}

#define ADD_DATUM(prefix,md) \
		QString key = QString("[" #prefix "] %1").arg(md->key().c_str()); \
		QString value = md->toString().c_str(); \
		textKeys << key; \
		textMap[key] = value; 

void MainWindow::loadFolder()
{
	
	
	QStringList filters;
// 	filters << "*.bmp";
// 	filters << "*.png";
	filters << "*.jpg";
	filters << "*.jpeg";
// 	filters << "*.xpm";
// 	filters << "*.svg";
	
	m_batchDir.setNameFilters(filters);
	
	m_fileList.clear();
	
// 	QString abs = m_batchDir.absolutePath();
// 	foreach(QString file, files) 
// 		m_fileList << QString("%1/%2").args(abs,file);
		
	QProgressDialog progress(this);
	QString progressLabel = QString(tr("Loading files from %1...")).arg(m_batchDir.absolutePath());
	progress.setLabelText(progressLabel);
	progress.setMinimum(0);
	progress.setMaximum(0);
	progress.setAutoClose(false);
	progress.setAutoReset(false);
	progress.setWindowTitle("Loading Folder");
	progress.show();
	
	QApplication::processEvents();


	QDirIterator it(m_batchDir); //, QDirIterator::Subdirectories);
	while (it.hasNext()) 
	{
		if(it.filePath().isEmpty())
		{
			it.next();
			continue;
		}
		
		if(progress.wasCanceled())
			break;
		
		QApplication::processEvents();
		
		
		if(m_onlyShowHigh)
		{
			Exiv2::Image::AutoPtr exiv = Exiv2::ImageFactory::open(it.filePath().toStdString()); 
			assert(exiv.get() != 0);
			exiv->readMetadata();
			
			Exiv2::XmpData& xmpData = exiv->xmpData();
			
			QString rating = xmpData["Xmp.xmp.rating"].toString().c_str();
	// 		qDebug() << "Trying: "<<it.filePath()<<", rating:"<<rating;
			if(rating.toInt() < 5)
			{
				it.next();
				continue;
			}
		}
		
		
		
		m_fileList.append(it.filePath());
		qDebug() << "Adding: "<<it.filePath();
		
		progress.setLabelText(QString("%1 (%2)").arg(progressLabel).arg(m_fileList.size()));
		
		it.next();
	}
	
	
	progress.close();
	
	qSort(m_fileList);
	
	
	//m_ui->progressBar->setMaximum(m_fileList.size());
	m_ui->imageSlider->setMaximum(m_fileList.size());
	m_ui->gotoBox->setSuffix(QString(tr(" / %1")).arg(m_fileList.size()));
	m_ui->gotoBox->setMaximum(m_fileList.size()-1);
	
	if(m_fileList.isEmpty())
	{
		QMessageBox::critical(this,tr("No JPEG Files Found"),QString(tr("I'm sorry, but no JPEG files were found in %1. Please check the folder and try again.")).arg(m_batchDir.absolutePath()));
		return;
	}
	
	
	setCurrentImage(0);
	
	m_ui->imageSlider->setEnabled(true);
	m_ui->gotoBox->setEnabled(true);
	m_ui->groupBox->setEnabled(true);
	m_ui->nextBtn->setFocus();
}

void MainWindow::nextImage()
{
	if(m_fileList.isEmpty())
		return;
		
	m_currentFile ++;
	if(m_currentFile >= m_fileList.size())
		m_currentFile = 0;
	setCurrentImage(m_currentFile);
}

void MainWindow::prevImage()
{
	if(m_fileList.isEmpty())
		return;
		
	m_currentFile --;
	if(m_currentFile < 0)
		m_currentFile = m_fileList.size() - 1;
	setCurrentImage(m_currentFile);
}

QMutex mutex;
static int activeThreads = 0;
const int imageSize = 1024 * 4;

PreloadImageTask::PreloadImageTask(QString file)
	: QObject()
	, QRunnable()
	, m_file(file) {}

void PreloadImageTask::run()
{
	mutex.lock();
	activeThreads++;
	mutex.unlock();
	
	QImage image(m_file);
	
	if(DEBUG_PRELOAD)
		qDebug() << m_file<<":"<<(image.numBytes() / 1024 / 1024) << "MB ( active:"<<activeThreads<<")";
	
	QImage scaled = image.scaled(QSize(1024,768),Qt::KeepAspectRatio);
	emit imageLoaded(m_file,image,scaled); 
	
	mutex.lock();
	activeThreads--;
	mutex.unlock();
}

void MainWindow::loadGotoBoxValue()
{
	setCurrentImage(m_ui->gotoBox->value());
}

void MainWindow::setCurrentImage(int num)
{
	if(m_fileList.isEmpty() || num < 0 || num >= m_fileList.size())
		return;
	
	m_currentFile = num;
	
	if(m_changingCombobox)
		return;
		
	if(m_ui->gotoBox->value() != num)
	{
		m_changingCombobox = true;
		m_ui->gotoBox->setValue(num);
		m_changingCombobox = false;
	}
		
	try
	{
		writeMetaData();
	}
	catch(std::exception ex)
	{
		qDebug() << "Caught exception in writeMetaData(): "<<ex.what();
	}
		
	//m_ui->progressBar->setValue(num);
	
	QString path = m_fileList[num];
	QString file = QFileInfo(path).fileName();
	
	setWindowTitle(QString(tr("Image Importer - %1")).arg(file));
	
	m_ui->filenameLabel->setText(file);
	
	initMetaData();
	
	if(m_pixmap)
		delete m_pixmap;
		
	QRect imgRect;
	
	QString smallKey = QString("%1_sm").arg(path);
	
	QImage *img = 0;
	
	if((img = m_cache.object(smallKey)) != 0)
	{
		m_loadOriginalTimer.stop();
		
		if(DEBUG_PRELOAD)
			qDebug() << "MainWindow::setCurrentImage: Cache hit on smallKey "<<smallKey;
		setCursor(Qt::BusyCursor);
		
		QTime t;
		t.start();
		QPixmap px = QPixmap::fromImage(*img);
 		qDebug() << "MainWindow::setCurrentImage: fromImge conversion took "<<t.elapsed()<<"ms";
		
		m_pixmapItem->setPixmap(px);
		imgRect = px.rect();
		
		setCursor(Qt::ArrowCursor);
		
		m_loadOriginalTimer.start();
	}
	else
	if((img = m_cache.object(path)) != 0)
	{
		if(DEBUG_PRELOAD)
			qDebug() << "MainWindow::setCurrentImage: Cache hit on "<<file;
		setCursor(Qt::BusyCursor);
		
		QTime t;
		t.start();
		QPixmap px = QPixmap::fromImage(*img);
 		qDebug() << "MainWindow::setCurrentImage: fromImge took "<<t.elapsed()<<"ms";
		
		t.restart();
		m_pixmapItem->setPixmap(px);
		qDebug() << "MainWindow::setCurrentImage: setPixmap took "<<t.elapsed()<<"ms";
		
		imgRect = px.rect();
		
		setCursor(Qt::ArrowCursor);
	}
	else
	{
		if(DEBUG_PRELOAD)
			qDebug() << "MainWindow::setCurrentImage: Cache MISS on "<<file;
		setCursor(Qt::BusyCursor);
		QPixmap origPixmap(path);
		//m_pixmap = new QPixmap();
		m_pixmapItem->setPixmap(origPixmap); //.scaledToWidth(1024));
		imgRect = origPixmap.rect(); //0,0,1024,768);
		setCursor(Qt::ArrowCursor);
				
		imageLoaded(path, origPixmap.toImage(), origPixmap.scaled(QSize(1024,768),Qt::KeepAspectRatio).toImage());
	}
	
	
	QTime t;
	t.start();
	
	// translate it out by 1/2 width/height so it rotates around center, rather than rotating around top-left corner
	int x = imgRect.width()  / 2;
	int y = imgRect.height() / 2;
	m_pixmapItem->setTransform(QTransform().translate(x, y).rotate(m_rotateDegrees).translate(-x, -y));
	
	// adjust width/height if rotated so that the "adjust view scaling" fits entire image in scene
	imgRect = m_pixmapItem->transform().mapRect(imgRect);
	
	m_scene->setSceneRect(imgRect);
	m_ui->graphicsView->adjustViewScaling();
	
	qDebug() << "MainWindow::setCurrentImage: transforming scene took "<<t.elapsed()<<"ms";
	
	
	QTimer::singleShot(5, this, SLOT(prepQueue()));
}

void MainWindow::loadOriginalSize()
{
	if(m_fileList.isEmpty() || m_currentFile < 0 || m_currentFile >= m_fileList.size())
		return;

	QString path = m_fileList[m_currentFile];
	QString file = QFileInfo(path).fileName();
	
	QImage *img = 0;
	
	if((img = m_cache.object(path)) != 0)
	{
		if(DEBUG_PRELOAD)
			qDebug() << "MainWindow::loadOriginalSize: Cache hit on "<<file;
		setCursor(Qt::BusyCursor);
		
		QTime t;
		t.start();
		QPixmap px = QPixmap::fromImage(*img);
 		qDebug() << "MainWindow::loadOriginalSize: fromImge conversion took "<<t.elapsed()<<"ms";
		
		QRect imgRect = px.rect();
		
		// translate it out by 1/2 width/height so it rotates around center, rather than rotating around top-left corner
		int x = imgRect.width()  / 2;
		int y = imgRect.height() / 2;
		m_pixmapItem->setTransform(QTransform().translate(x, y).rotate(m_rotateDegrees).translate(-x, -y));
		
		// adjust width/height if rotated so that the "adjust view scaling" fits entire image in scene
		imgRect = m_pixmapItem->transform().mapRect(imgRect);
		
		m_ui->graphicsView->setUpdatesEnabled(false);
		m_scene->setSceneRect(imgRect);
		m_pixmapItem->setPixmap(px);
		m_ui->graphicsView->adjustViewScaling();
		m_ui->graphicsView->setUpdatesEnabled(true);
		
		setCursor(Qt::ArrowCursor);
	}
	else
	{
		qDebug() << "MainWindow::loadOriginalSize: Could not find origianl size for "<<path;
	}
	
}

void MainWindow::prepQueue()
{
	int numQueued = 0;
	
	int it = m_currentFile;
	while(it ++ < m_fileList.size() -1 && it - m_currentFile <= m_lookAhead)
	{
		if(queuePreload(m_fileList[it]))
			numQueued ++;
	}
	
	it = m_currentFile;
	while(it -- > 0 && m_currentFile - it <= m_lookBehind)
	{
		if(queuePreload(m_fileList[it]))
			numQueued ++;
	}
	
	if(DEBUG_PRELOAD && numQueued>0)
		qDebug() << "MainWindow::prepQueue: Queued"<<numQueued<<"for preload";
}

bool MainWindow::queuePreload(QString path)
{
	if(!m_cache.contains(path) && 
	   !m_filesInProcess.contains(path))
	{
		m_filesInProcess.append(path);
		
		if(DEBUG_PRELOAD)
			qDebug() << "MainWindow::queuePreload: Queueing "<<path<<" for preload";
		PreloadImageTask *preload = new PreloadImageTask(path);
		connect(preload, SIGNAL(imageLoaded(QString,QImage,QImage)), this, SLOT(imageLoaded(QString,QImage,QImage)));
		
		// QThreadPool takes ownership and deletes 'preload' automatically
		QThreadPool::globalInstance()->start(preload);
		
		return true;
	}
	
	return false;
}

void MainWindow::imageLoaded(const QString& file, const QImage& img, const QImage &scaled)
{
	int mb = img.numBytes() / 1024 / 1024;
	
	QImage * cacheable = new QImage(img);
	m_cache.insert(file, cacheable, mb);
	m_filesInProcess.remove(file);
	
	int smallMb = -1;
	if(!scaled.isNull())
	{
// 		smallMb = scaled.numBytes() / 1024 / 1024;
// 		QImage * cacheableSmall = new QImage(scaled);
// 		m_cache.insert(QString("%1_sm").arg(file), cacheableSmall, smallMb);
	}
	
	
	if(DEBUG_PRELOAD)
		qDebug() << "MainWindow::imageLoaded: Loaded"<<file<<" ("<<mb<<"MB, smallMb:"<<smallMb<<"MB )"; //<< ( m_filesInProcess.isEmpty() ? "" : QString(", still waiting on %1 images.").arg(m_filesInProcess.size()));
	
}

void MainWindow::initMetaData(const QString &pathTmp)
{
	if(m_fileList.isEmpty())
		return;
		
// 	m_ui->title->setText(file);
	QString path = pathTmp;
	if(path.isEmpty())
		path = m_fileList[m_currentFile];
	QString file = QFileInfo(path).fileName();
	
	/*
		Image ID	- 	Exif.Image.UniqueID, tag # 0xa420 - 128bit length, generate with QUuid
		Batch Name	- 	Xmp. ??
		Tags		- 	Xmp.dc.subject
		Description	-	Xmp.dc.description
		Title		-	Xmp.dc.title
		Location	-	Xmp. ??
		Rating		-	Xmp.xmp.Rating (1-5), Xmp.MicrosoftPhoto.Rating (%)
	*/

	m_exivImage = Exiv2::ImageFactory::open(path.toStdString()); 
	assert(m_exivImage.get() != 0);
	m_exivImage->readMetadata();
	
// 	QFile outputFile("output.txt");
// 	if(!outputFile.open(QFile::WriteOnly | QFile::Truncate))
// 	{
// 		qDebug() << "Unable to open output.txt for writing.";
// 	}
// 	
// 	QTextStream outputStream(&outputFile);
	
// 	Exiv2::ExifData& exifData = m_exivImage->exifData();
// 	QString imageId = exifData["Exif.Image.UniqueID"].toString().c_str();
// 	if(imageId.isEmpty())
// 	{
// 		imageId = QUuid::createUuid().toString();
// 		exifData["Exif.Image.UniqueID"] = (char *)imageId.constData();
// 		m_exivImage->setExifData(exifData);
// 	}
	
	
	
	Exiv2::XmpData& xmpData = m_exivImage->xmpData();
	
	QString imageId = xmpData["Xmp.dc.uniqueid"].toString().c_str();
	if(imageId.isEmpty())
	{
		imageId = QUuid::createUuid().toString();
		xmpData["Xmp.dc.uniqueid"] = imageId.toStdString();
		m_exivImage->setXmpData(xmpData);
	}
	
	// needed for datestamp and rotation
	Exiv2::ExifData& exifData = m_exivImage->exifData();
		
	
	ImageRecord * ref = ImageRecord::retrieveImageId(imageId);
	if(!ref)
	{
		ref = new ImageRecord(file);
		ImageRecord::addRecord(ref);
		
		QDateTime dt = QDateTime::fromString(exifData["Exif.Photo.DateTimeOriginal"].toString().c_str(), "yyyy:MM:dd HH:mm:ss");
		qDebug() << "MainWindow::initMetaData: "<<file<<": Date/Time taken: "<<dt.toString();
		
		ref->setDatestamp(dt);
		ref->setImageId(imageId);
		
// 		ImageRecord * ref2 = ImageRecord::retrieveImageId(imageId);
// 		if(!ref2)
// 		{
// 			qDebug() << "can't re-retrieve imageId"<<imageId;
// 			abort();
// 		}
	}
	else
	{
		//qDebug() << "MainWindow::initMetaData: "<<file<<": Loaded record#"<<ref->recordId()<<" from database";
	}
	
	
	QString rotateSensor = exifData["Exif.Image.Orientation"].toString().c_str();
	int rotationFlag = rotateSensor.toInt(); 
	m_rotateDegrees = rotationFlag == 1 ||
	 		  rotationFlag == 2 ? 0 :
			  rotationFlag == 7 ||
			  rotationFlag == 8 ? -90 :
			  rotationFlag == 3 ||
			  rotationFlag == 4 ? -180 :
			  rotationFlag == 5 ||
			  rotationFlag == 6 ? -270 :
			  0;
	
	//qDebug() << "rotationFlag:"<<rotationFlag<<", m_rotateDegrees: "<<m_rotateDegrees;;
	
	QString batch		= xmpData["Xmp.dc.batch"]	.toString().c_str();
	QString location	= xmpData["Xmp.dc.coverage"]	.toString().c_str();
	QString tags 		= xmpData["Xmp.dc.subject"]	.toString().c_str();
	QString description 	= xmpData["Xmp.dc.description"]	.toString().c_str();
	QString title 		= xmpData["Xmp.dc.title"]	.toString().c_str();
	QString rating	 	= xmpData["Xmp.xmp.rating"]	.toString().c_str();
	
	
	if(!batch.isEmpty())
		m_ui->batchname->setText(batch);
	else
		m_ui->batchname->setText("");
	
	if(!location.isEmpty())
		m_ui->location->setText(location);
	else
		m_ui->location->setText("");
		
	if(!tags.isEmpty())
		m_ui->tags->setText(tags);
	else
		m_ui->tags->setText("");
		
	if(!description.isEmpty())
		m_ui->description->setText(removeLangPrefix(description));
	else
		m_ui->description->setText("");
		
	if(!title.isEmpty())
		m_ui->title->setText(removeLangPrefix(title));
	else
		m_ui->title->setText("");
		
	if(!rating.isEmpty())
		m_ui->rating->setValue(rating.toInt());
	else
		m_ui->rating->setValue(0);
		
	/*
	
	for (Exiv2::XmpData::const_iterator md = xmpData.begin();
		md != xmpData.end(); ++md) 
	{
		ADD_DATUM("XMP",md);
	}
	
	if (xmpData.empty()) 
	{
		qDebug() << "XmpData: No XMP Data found in "<<file;
	}*/
	
	
	
}

void MainWindow::useLastTitle() 
{
	m_ui->title->setText(m_lastTitle);
}

void MainWindow::useLastDescription()
{
	m_ui->description->setText(m_lastDescription);
}

void MainWindow::useLastTags()
{
	m_ui->tags->setText(m_lastTags);
}

void MainWindow::useLastLocation()
{
	m_ui->location->setText(m_lastLocation);
}

void MainWindow::useLastBatch()
{
	m_ui->batchname->setText(m_lastBatch);
}

void MainWindow::highCopyBrowse()
{
	
	QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select Destination Folder"), 
        		m_ui->highCopyFolder->text(), 
        		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(!dirPath.isEmpty())
	{
		m_ui->highCopyFolder->setText(dirPath);
		AppSettings::setPreviousPath("highcopyfolder",dirPath);
	}
}

void MainWindow::writeMetaData()
{
	if(!m_exivImage.get())
		return;
	
	
	if(m_fileList.isEmpty() || m_currentFile < 0 || m_currentFile >= m_fileList.size())
		return;
		
	Exiv2::XmpData& xmpData = m_exivImage->xmpData();
	QString imageId = xmpData["Xmp.dc.uniqueid"].toString().c_str();
	
	QString path = m_fileList[m_currentFile];
// 	qDebug() << "MainWindow::writeMetaData: "<<file<<": Writing meta data for imageId: "<<imageId;
	
	ImageRecord * ref = ImageRecord::retrieveImageId(imageId);
	if(!ref)
	{
		ref = new ImageRecord(path);
		ImageRecord::addRecord(ref);
		
		Exiv2::ExifData& exifData = m_exivImage->exifData();
		QDateTime dt = QDateTime::fromString(exifData["Exif.Photo.DateTimeOriginal"].toString().c_str(), "yyyy:MM:dd HH:mm:ss");
		qDebug() << "MainWindow::writeMetaData: "<<path<<": Date/Time taken: "<<dt.toString();
		ref->setDatestamp(dt);
	}
	else
	{
// 		qDebug() << "MainWindow::writeMetaData: "<<path<<": Loaded record#"<<ref->recordId()<<" from database";
	}
	
	
	QString batch = m_ui->batchname->text();
	ref->setBatchName(batch);
	xmpData["Xmp.dc.batch"]		= batch.toStdString();
	
	m_lastBatch = batch;
	
	QString location = m_ui->location->text();
	ref->setLocation(location);
	xmpData["Xmp.dc.coverage"]	= location.toStdString();
	
	m_lastLocation = location;
	
	
	QString tags = m_ui->tags->text();
	ref->setTags(tags);
	
	m_lastTags = tags;
	
// 	for (Exiv2::XmpData::iterator md = xmpData.begin();
// 		md != xmpData.end(); ++md) 
// 		if(md->key().c_str() &&
// 		   strcmp(md->key().c_str(),"Xmp.dc.subject") == 0)
// 			md = xmpData.erase(md);
// 		
// 	xmpData["Xmp.dc.subject"]	= tags.toStdString();
	
	QString description = m_ui->description->text();
	ref->setDescription(description);
	xmpData["Xmp.dc.description"]	= description.toStdString();
	
	m_lastDescription = description;
	
	QString title = m_ui->title->text();
	ref->setTitle(title);
	xmpData["Xmp.dc.title"]		= title.toStdString();
	
	m_lastTitle = title; 
	
	int rating = m_ui->rating->value();
	ref->setRating(rating);
	QString ratingStr = QString("%1").arg(rating);
	xmpData["Xmp.xmp.rating"]	= ratingStr.toStdString();

	
// 	// Serialize the XMP data and output the XMP packet
// 	std::string xmpPacket;
// 	if (0 != Exiv2::XmpParser::encode(xmpPacket, xmpData)) {
// 		throw Exiv2::Error(1, "Failed to serialize XMP data");
// 	}
// 	//std::cout << xmpPacket << "\n";
// 	qDebug() << "xmpPacket: "<<xmpPacket.c_str();

	m_exivImage->setXmpData(xmpData);
	m_exivImage->writeMetadata();

	
	if(rating >= 5 && m_ui->highCopyFlag->isChecked())
	{
		QDir copyDest = QDir(m_ui->highCopyFolder->text());
		QFile origFile(path);
		QString file = QFileInfo(path).fileName();
		QString newPath = QString("%1/%2").arg(copyDest.absolutePath(),file);
		if(!origFile.copy(newPath))
			qDebug() << "Cannot copy "<<path<<" to "<<newPath;
		else
			qDebug() << "Successfully copied "<<path<<" to "<<newPath;
	}
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) 
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}


 bool MainWindow::event(QEvent *event)
 {
	if (event->type() == QEvent::KeyPress) 
	{
		QKeyEvent *ke = static_cast<QKeyEvent *>(event);
		//qDebug() << "MainWindow::event: key:"<<ke->key();
		if(ke->key() >= 0x30 && ke->key() <= 0x35)
		{
			int rating = ke->key() - 0x30;
			m_ui->rating->setValue(rating);
			qDebug() << "MainWindow::event: key:"<<ke->key()<<", rating:"<<rating;
		}
		else
		if(ke->key() >= 0x01000030 && ke->key() <= 0x01000034)
		{
			int rating = ke->key() - 0x01000030 + 1;
			m_ui->rating->setValue(rating);
			qDebug() << "MainWindow::event: key:"<<ke->key()<<", rating:"<<rating;
		}
		else
// 		if(ke->key() == Qt::Key_F2)
// 		{
// 			showRenameDialog();
// 		}
// 		else
		{
			//qDebug() << "MainWindow::event: unused key:"<<ke->key();
		}
	}
	
	return QWidget::event(event);
}
