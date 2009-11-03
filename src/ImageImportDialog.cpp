#include "ImageImportDialog.h"
#include ".build/ui_ImageImportDialog.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QProgressDialog>

#include <QFileDialog>
#include <QMessageBox>

#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/Document.h"
#include "model/TextBoxItem.h"
#include "model/ItemFactory.h"
#include "AppSettings.h"
#include "DocumentListModel.h"

ImageImportDialog::ImageImportDialog(Document *d, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ImageImportDialog),
    m_doc(d),
    m_model(new DocumentListModel())
{
    m_ui->setupUi(this);
    connect(m_ui->importBrowse, SIGNAL(clicked()), this, SLOT(importDirBrowse()));
    connect(m_ui->copyToBrowse, SIGNAL(clicked()), this, SLOT(copyToDirBrowse()));
    m_model->setDocument(d);
    m_ui->existingGroup->setModel(m_model);
}

ImageImportDialog::~ImageImportDialog()
{
    delete m_ui;
}

void ImageImportDialog::importDirBrowse()
{
	QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select Images Folder to Import"),
						 AppSettings::previousPath("images"),
						 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(!dirPath.isEmpty())
	{
		m_ui->importFolder->setText(dirPath);
		m_ui->newGroupName->setText(QDir(dirPath).dirName());
	}
}
void ImageImportDialog::copyToDirBrowse()
{
	QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select Destination Folder"),
						 AppSettings::previousPath("images"),
						 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(!dirPath.isEmpty())
	{
		m_ui->copyToFolder->setText(dirPath);
	}
}

void ImageImportDialog::accept()
{
	QDir dir(m_ui->importFolder->text());
	if(!dir.exists())
	{
		QMessageBox::critical(0,"Import Folder Not Found","Sorry, but the folder you entered in the 'Import Folder' box does not exist.");
		return;
	}

	bool copyFiles = m_ui->copyFiles->isChecked();
	QDir copyDest(m_ui->copyToFolder->text());
	if(copyFiles && !copyDest.exists())
	{
		QMessageBox::critical(0,"Destination Folder Not Found","Sorry, but the folder you entered in the 'Copy To Folder' box does not exist.");
		return;
	}

	bool removeOriginal = m_ui->removeOrig->isChecked();
	bool renameDups = m_ui->renameDuplicates->isChecked();

	QStringList filters;
	filters << "*.bmp";
	filters << "*.png";
	filters << "*.jpg";
	filters << "*.jpeg";
	filters << "*.xpm";
	filters << "*.svg";
	QStringList list = dir.entryList(filters);

	qDebug() << "list: "<<list;

	if(list.size() <= 0)
	{
		QMessageBox::critical(this,"No Images Found","No images found in the folder you chose.");
		return;
	}

	SlideGroup *group;

	if(m_ui->addImagesToExisting->isChecked())
	{
		int idx = m_ui->existingGroup->currentIndex();
		if(idx < 0)
		{
			QMessageBox::critical(this,"No Existing Group Selected","You must select a group to add images to if you select the option titled 'Add to Existing Group'");
			return;
		}

		group = m_model->groupAt(idx);
	}
	else
	{
	    group = new SlideGroup();
	    QString newGroupName = m_ui->newGroupName->text();
	    group->setGroupTitle(newGroupName.isEmpty() ? dir.dirName() : newGroupName);
	}

	group->setAutoChangeGroup(!m_ui->flagLoop->isChecked());
	bool autoChange = m_ui->flagAutoChange->isChecked();
	double changeTime = m_ui->changeTime->value();

	bool showNames = m_ui->flagShowNames->isChecked();
	bool dontShowCameraNames = m_ui->flagNotDefault->isChecked();


	QProgressDialog progress("Importing Files", "Stop Import", 0, list.size(), this);
	progress.setMinimumDuration(500);

	int slideNum = 0;
	foreach(QString file, list)
	{
		progress.setValue(slideNum);
		if(progress.wasCanceled())
		{
			if(m_ui->createNewGroup->isChecked())
				delete group;
			return;
		}

		Slide * slide = new Slide();
		AbstractVisualItem * bg = dynamic_cast<AbstractVisualItem*>(slide->background());

		QString path = QString("%1/%2").arg(dir.absolutePath()).arg(file);
		qDebug() << "Making slide for:"<<path;

		if(copyFiles)
		{
			QString newPath = QString("%1/%2").arg(copyDest.absolutePath(),file);
			QFile origFile(path);
			if(QFile(newPath).exists() && renameDups)
				newPath = newPath.replace(".",QString("-%1.").arg(slideNum));
			if(!origFile.copy(newPath))
				qDebug() << "Cannot copy "<<path<<" to "<<newPath;

			if(removeOriginal)
			{
				origFile.remove();
				path = newPath;
				qDebug() << "Removed original, new path: "<<newPath;
			}

		}

		bg->setFillType(AbstractVisualItem::Image);
		bg->setFillImageFile(path);

		if(showNames)
		{
		    QString baseName = QFileInfo(file).baseName();
		    bool addText = true;
		    if(dontShowCameraNames)
			if(baseName.indexOf(QRegExp("^[a-zA-Z]{2,5}[0-9]{2,10}$")) > -1)
			    addText = false;

		    if(addText)
		    {
			TextBoxItem * t = new TextBoxItem();
			t->setContentsRect(QRect(0,0,400,10));
			t->setText(QString("<font family='Tahoma,Verdana,Sans-Serif'>%1</font>").arg(baseName));
			t->setShadowEnabled(true);
			t->setShadowBlurRadius(7);
			t->setOutlineEnabled(true);
			t->setOutlinePen(QPen(Qt::black,1.5));
			t->setItemId(ItemFactory::nextId());
			t->setItemName(QString("TextBoxItem%1").arg(t->itemId()));
			t->changeFontSize(52.0);
			t->setZValue(100);
			slide->addItem(t);
		    }

		}

		if(autoChange)
		    slide->setAutoChangeTime(changeTime);
		slide->setSlideNumber(slideNum);

		group->addSlide(slide);

		slideNum++;

	}

	m_doc->addGroup(group);

	progress.setValue(list.size());

	QDialog::accept();
}

void ImageImportDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
