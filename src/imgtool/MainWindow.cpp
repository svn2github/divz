#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QImage>
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

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
	
	ui->list->clear();
	QStringList textKeys = img.textKeys();
	
        qDebug() << "Text Keys in"<<file<<":"<<textKeys;

        ui->list->setRowCount(textKeys.size());
	for(int i=0; i<textKeys.size(); i++)
	{
		ui->list->setItem(i, 0, new QTableWidgetItem(textKeys[i]));
                ui->list->setItem(i, 1, new QTableWidgetItem(img.text(textKeys[i])));
	}
	
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
