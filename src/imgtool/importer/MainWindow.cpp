#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QImage>
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QImageWriter>


#include "../exiv2-0.18.2-qtbuild/src/image.hpp"
#include <string>
#include <iostream>
#include <cassert>

MainWindow::MainWindow(QWidget *parent) 
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
        connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui->browseBtn, SIGNAL(clicked()), this, SLOT(slotBrowse()));
	connect(ui->loadBtn, SIGNAL(clicked()), this, SLOT(loadFile()));
	connect(ui->filename, SIGNAL(returnPressed()), this, SLOT(loadFile()));
}

MainWindow::~MainWindow()
{
        delete ui;
}

void MainWindow::slotBrowse()
{
        QString file = QFileDialog::getOpenFileName(this, tr("Select Image"), "", tr("Image Files (*.png *.jpg *.bmp *.svg *.xpm *.gif);;Any File (*.*)"));
	if(!file.isEmpty())
	{
		ui->filename->setText(file);
		loadFile();
	}
}

#define ADD_DATUM(prefix,md) \
		QString key = QString("[" #prefix "] %1").arg(md->key().c_str()); \
		QString value = md->toString().c_str(); \
		textKeys << key; \
		textMap[key] = value; 

void MainWindow::loadFile()
{
	QString file = ui->filename->text();
	
	if(file.isEmpty() || !QFile(file).exists())
	{
		QMessageBox::critical(this,tr("File Does Not Exist"),QString(tr("I'm sorry, but %1 does not exist. Please check the file and try again.")).arg(file));
		return;
	}
	
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
// 	ui->list->clear();
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
	
	
	ui->list->setRowCount(textKeys.size());
	for(int i=0; i<textKeys.size(); i++)
	{
		ui->list->setItem(i, 0, new QTableWidgetItem(textKeys[i]));
		ui->list->setItem(i, 1, new QTableWidgetItem(textMap.value(textKeys[i])));
		
		outputStream << textKeys[i] << "\t" << textMap.value(textKeys[i]) << "\n";
	}
	
	outputFile.close();
	
	ui->list->resizeColumnsToContents();
	ui->list->resizeRowsToContents();
	
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
