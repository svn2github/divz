#include "LoadDialog.h"
#include ".build/ui_LoadDialog.h"
#include "AppSettings.h"

#include <QDesktopServices>
#include <QFileDialog>

LoadDialog::LoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadDialog)
{
	ui->setupUi(this);
	connect(ui->browseBtn, SIGNAL(clicked()), this, SLOT(slotBrowse()));
	connect(ui->copyBrowseBtn, SIGNAL(clicked()), this, SLOT(slotCopyBrowse()));
	QString str = AppSettings::previousPath("importfolder");
	if(str.isEmpty())
		str = "/home/josiah/devel/dviz-root/trunk/src/imgtool/samples/003/";
		//str = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
	ui->importFolder->setText(str);
	
	str = AppSettings::previousPath("copyfolder");
	ui->copyToFolder->setText(str);
	
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotAccepted()));
}

void LoadDialog::slotAccepted()
{
	AppSettings::setPreviousPath("importfolder",batchFolder());
	AppSettings::setPreviousPath("copyfolder",copyFolder());
}


void LoadDialog::slotBrowse()
{
	QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select Folder to Process"), 
        		ui->importFolder->text(), 
        		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(!dirPath.isEmpty())
	{
		ui->importFolder->setText(dirPath);
	}
}

void LoadDialog::slotCopyBrowse()
{
	QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select Destination Folder"), 
        		ui->copyToFolder->text(), 
        		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(!dirPath.isEmpty())
	{
		ui->copyToFolder->setText(dirPath);
	}
}


LoadDialog::~LoadDialog()
{
    delete ui;
}

void LoadDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
