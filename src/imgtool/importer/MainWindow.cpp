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


#include "ImageRecord.h"
#include "ImageRecordListModel.h"


#include "../exiv2-0.18.2-qtbuild/src/image.hpp"
#include <string>
#include <iostream>
#include <cassert>

MainWindow::MainWindow(QWidget *parent) 
	: QMainWindow(parent)
	, m_pixmap(0)
	, m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);
        connect(m_ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(m_ui->browseBtn, SIGNAL(clicked()), this, SLOT(slotBrowse()));
	connect(m_ui->loadBtn, SIGNAL(clicked()), this, SLOT(loadFolder()));
	connect(m_ui->importFolder, SIGNAL(returnPressed()), this, SLOT(loadFolder()));
	connect(m_ui->nextBtn, SIGNAL(clicked()), this, SLOT(nextImage()));
	connect(m_ui->prevBtn, SIGNAL(clicked()), this, SLOT(prevImage()));
	
	m_ui->importFolder->setText("/home/josiah/devel/dviz-root/trunk/src/imgtool/samples/nikon01/");
	
	m_ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	m_ui->graphicsView->setRenderHint( QPainter::Antialiasing, true );

	m_scene = new QGraphicsScene();

	m_ui->graphicsView->setScene(m_scene);
	m_scene->setSceneRect(0,0,1024,768);
	
	m_pixmapItem = new QGraphicsPixmapItem();
	m_pixmapItem->setPos(0,0);
	m_scene->addItem(m_pixmapItem);
}

MainWindow::~MainWindow()
{
	delete m_ui;
	if(m_pixmap)
		delete m_pixmap;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
	adjustViewScaling();
}

void MainWindow::adjustViewScaling()
{
	float sx = ((float)m_ui->graphicsView->width()) / m_scene->width();
	float sy = ((float)m_ui->graphicsView->height()) / m_scene->height();

	float scale = qMin(sx,sy);
	m_ui->graphicsView->setTransform(QTransform().scale(scale,scale));
        //qDebug("Scaling: %.02f x %.02f = %.02f",sx,sy,scale);
	m_ui->graphicsView->update();
	//m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
	//m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}


void MainWindow::slotBrowse()
{
        // Show a file open dialog at QDesktopServices::PicturesLocation.
        QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select Folder to Process"), 
        		QDesktopServices::storageLocation(QDesktopServices::PicturesLocation), 
        		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(!dirPath.isEmpty())
	{
		m_ui->importFolder->setText(dirPath);
		loadFolder();
	}
}

#define ADD_DATUM(prefix,md) \
		QString key = QString("[" #prefix "] %1").arg(md->key().c_str()); \
		QString value = md->toString().c_str(); \
		textKeys << key; \
		textMap[key] = value; 

void MainWindow::loadFolder()
{
	QString folder = m_ui->importFolder->text();
	
	m_batchDir = QDir(m_ui->importFolder->text());
	if(!m_batchDir.exists())
	{
		QMessageBox::critical(0,"Folder Not Found","Sorry, but the folder you entered in the 'Batch Folder' box does not exist.");
		return;
	}

	m_copyFiles = m_ui->copyFiles->isChecked();
	m_copyDest = QDir(m_ui->copyToFolder->text());
	if(!m_copyDest.exists())
	{
		QMessageBox::critical(0,"Destination Folder Not Found","Sorry, but the folder you entered in the 'Copy To Folder' box does not exist.");
		return;
	}
	
	QStringList filters;
// 	filters << "*.bmp";
// 	filters << "*.png";
	filters << "*.jpg";
	filters << "*.jpeg";
// 	filters << "*.xpm";
// 	filters << "*.svg";
	m_fileList = m_batchDir.entryList(filters);
	
	if(m_fileList.isEmpty())
	{
		QMessageBox::critical(this,tr("No JPEG Files Found"),QString(tr("I'm sorry, but no JPEG files were found in %1. Please check the folder and try again.")).arg(m_batchDir.absolutePath()));
		return;
	}
	
	setCurrentImage(0);
	
	m_ui->groupBox->setEnabled(true);
}

void MainWindow::nextImage()
{
	m_currentFile ++;
	if(m_currentFile >= m_fileList.size())
		m_currentFile = 0;
	setCurrentImage(m_currentFile);
}

void MainWindow::prevImage()
{
	m_currentFile --;
	if(m_currentFile < 0)
		m_currentFile = m_fileList.size() - 1;
	setCurrentImage(m_currentFile);
}


void MainWindow::setCurrentImage(int num)
{
	m_currentFile = num;
	
	QString file = m_fileList[num];
	QString path = QString("%1/%2").arg(m_batchDir.absolutePath()).arg(file);
	//QFileInfo fileInfo(path);
	
	m_ui->filenameLabel->setText(file);
	
	if(m_pixmap)
		delete m_pixmap;
		
	QPixmap origPixmap(path);
	//m_pixmap = new QPixmap();
	m_pixmapItem->setPixmap(origPixmap.scaledToWidth(1024));
	// pillow
}

/*
	
	QImage img;
	if(!img.load(file))
	{
		QMessageBox::critical(this,tr("Problem Loading File"),QString(tr("I'm sorry, but there was a problem loading %1. Please check the file and try again.")).arg(file));
		return;
	}
	
// 	img.setText("test","hello");
// 	img.save(file);
	
//	QString fmt = "png";
// 	QImageWriter writer;
// 	writer.setFormat("png");
// 	if (writer.supportsOption(QImageIOHandler::Description))
// 		qDebug() << fmt <<" supports embedded text";
// 	else
// 		qDebug() << fmt <<" DOES NOT";
// 
// 	m_ui->list->clear();
// 	QStringList textKeys = img.textKeys();
	
//	qDebug() << "Text Keys in"<<file<<":"<<textKeys;

	Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(file.toStdString()); //argv[1]);
	assert(image.get() != 0);
	image->readMetadata();
	
	QFile outputFile("output.txt");
	if(!outputFile.open(QFile::WriteOnly | QFile::Truncate))
	{
		qDebug() << "Unable to open output.txt for writing.";
	}
	
	QTextStream outputStream(&outputFile);
	
	QStringList textKeys;
	QHash<QString,QString> textMap;
	
	Exiv2::ExifData& exifData = image->exifData();
	for (Exiv2::ExifData::const_iterator md = exifData.begin();
		md != exifData.end(); ++md) 
	{
		ADD_DATUM("EXIF",md);
	}
	if (exifData.empty()) 
	{
		qDebug() << "ExifData: No Exif data found in "<<file;
	}
	
	
	Exiv2::IptcData& iptcData = image->iptcData();
	for (Exiv2::IptcData::const_iterator md = iptcData.begin();
		md != iptcData.end(); ++md) 
	{
		ADD_DATUM("IPTC", md);
	}
	if (iptcData.empty()) 
	{
		qDebug() << "IptcData: No IPTC data found in "<<file;
	}
	
	
	Exiv2::XmpData& xmpData = image->xmpData();
	for (Exiv2::XmpData::const_iterator md = xmpData.begin();
		md != xmpData.end(); ++md) 
	{
		ADD_DATUM("XMP",md);
	}
	
	if (xmpData.empty()) 
	{
		qDebug() << "XmpData: No XMP Data found in "<<file;
	}
	
	
	m_ui->list->setRowCount(textKeys.size());
	for(int i=0; i<textKeys.size(); i++)
	{
		m_ui->list->setItem(i, 0, new QTableWidgetItem(textKeys[i]));
		m_ui->list->setItem(i, 1, new QTableWidgetItem(textMap.value(textKeys[i])));
		
		outputStream << textKeys[i] << "\t" << textMap.value(textKeys[i]) << "\n";
	}
	
	outputFile.close();
	
	m_ui->list->resizeColumnsToContents();
	m_ui->list->resizeRowsToContents();
	
}*/

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
// 	if (event->type() == QEvent::KeyPress) {
// 		QKeyEvent *ke = static_cast<QKeyEvent *>(event);
// 		if (ke->key() == Qt::Key_Tab) {
// 		// special tab handling here
// 		return true;
// 		}
// 	} else if (event->type() == MyCustomEventType) {
// 		MyCustomEvent *myEvent = static_cast<MyCustomEvent *>(event);
// 		// custom event handling here
// 		return true;
// 	}
	
	return QWidget::event(event);
 }
 
 
 