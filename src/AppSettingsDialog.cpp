#include "AppSettingsDialog.h"
#include "ui_AppSettingsDialog.h"
#include "AppSettings.h"
#include "OutputSetupDialog.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>

#include <QDirModel>
#include <QCompleter>
static void AppSettingsDialog_setupGenericDirectoryCompleter(QLineEdit *lineEdit)
{
	QCompleter *completer = new QCompleter(lineEdit);
	QDirModel *dirModel = new QDirModel(completer);
	completer->setModel(dirModel);
	//completer->setMaxVisibleItems(10);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(true);
	lineEdit->setCompleter(completer);
}

AppSettingsDialog::AppSettingsDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::AppSettingsDialog)
{
	m_ui->setupUi(this);
	m_ui->cbUseOpenGL->setChecked(AppSettings::useOpenGL());
	connect(m_ui->cbUseOpenGL, SIGNAL(toggled(bool)), this, SLOT(slotUseOpenGLChanged(bool)));
	connect(m_ui->btnConfigOutputs, SIGNAL(clicked()), this, SLOT(slotConfigOutputs()));
	//connect(m_ui->btnConfigGrid, SIGNAL(clicked()), this, SLOT(slotConfigGrid()));
	
	m_ui->cacheBox->setValue(AppSettings::pixmapCacheSize());
	m_ui->speedBox->setValue(AppSettings::crossFadeSpeed());
	m_ui->qualityBox->setValue(AppSettings::crossFadeQuality());
	
	m_ui->autosaveBox->setValue(AppSettings::autosaveTime());

	// Yes, I'm cheating by not adding a proper accessor to AppSettings - I dont feel
	// like waiting for the entire source tree to recompile right now. Maybe later.
	QSettings settings;
	int maxBackups = settings.value("max-backups","10").toInt();
	m_ui->maxBackups->setValue(maxBackups);
	
	if(AppSettings::liveEditMode() == AppSettings::PublishEdit)
		m_ui->editModePublished->setChecked(true);
	else
	if(AppSettings::liveEditMode() == AppSettings::SmoothEdit)
		m_ui->editModeSmooth->setChecked(true);
	else
		m_ui->editModeLive->setChecked(true);
	
	setWindowTitle("Program Settings");
	
	connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotAccepted()));
	
	m_ui->diskCacheSizeBase->setVisible(false);
	m_ui->diskCacheBox->setText(AppSettings::cacheDir().absolutePath());
	connect(m_ui->diskCacheBrowseBtn, SIGNAL(clicked()), this, SLOT(slotDiskCacheBrowse()));
	
	AppSettingsDialog_setupGenericDirectoryCompleter(m_ui->diskCacheBox);
	
	m_ui->templateFolder->setText(AppSettings::templateStorageFolder());
	connect(m_ui->templateFolderBrowse, SIGNAL(clicked()), this, SLOT(slotTemplateFolderBrowse()));
	
	AppSettingsDialog_setupGenericDirectoryCompleter(m_ui->templateFolder);
	
	m_ui->titlesafeAmount->setValue(AppSettings::titlesafeAmount());
	
	// apply signal changes
	m_ui->httpControlEnabled->setChecked(false);
	m_ui->httpControlEnabled->setChecked(true);
	
	m_ui->httpControlEnabled->setChecked(AppSettings::httpControlEnabled());
	m_ui->httpControlPort->setValue(AppSettings::httpControlPort());
	
	// apply signal changes
	m_ui->httpViewerEnabled->setChecked(false);
	m_ui->httpViewerEnabled->setChecked(true);
	
	m_ui->httpViewerEnabled->setChecked(AppSettings::httpViewerEnabled());
	m_ui->httpViewerPort->setValue(AppSettings::httpViewerPort());
	
	connect(m_ui->httpControlPort, SIGNAL(valueChanged(int)), this, SLOT(controlPortChanged(int)));
	connect(m_ui->httpControlUrlLabel, SIGNAL(linkActivated(const QString&)), this, SLOT(linkActivated(const QString&)));
	
	connect(m_ui->httpViewerPort, SIGNAL(valueChanged(int)), this, SLOT(viewerPortChanged(int)));
	connect(m_ui->httpViewerUrlLabel, SIGNAL(linkActivated(const QString&)), this, SLOT(linkActivated(const QString&)));
	
	m_ui->hotkeyBlack->setText(AppSettings::hotkeySequence("black"));
	m_ui->hotkeyClear->setText(AppSettings::hotkeySequence("clear"));
	m_ui->hotkeyLogo->setText(AppSettings::hotkeySequence("logo"));
	
	m_ui->hotkeyNextSlide->setText(AppSettings::hotkeySequence("next-slide"));
	m_ui->hotkeyNextGroup->setText(AppSettings::hotkeySequence("next-group"));
	m_ui->hotkeyPrevSlide->setText(AppSettings::hotkeySequence("prev-slide"));
	m_ui->hotkeyPrevGroup->setText(AppSettings::hotkeySequence("prev-group"));

	
	controlPortChanged(AppSettings::httpControlPort());
	viewerPortChanged(AppSettings::httpViewerPort());
	
	
	QTableWidget * tableWidget = m_ui->resourceTranslations;
	AppSettings::ResourcePathTranslations res = AppSettings::resourcePathTranslations();
	tableWidget->setRowCount(res.size());
	int row = 0;
	foreach(QStringPair pair, res)
	{
		tableWidget->setItem(row,0,new QTableWidgetItem(pair.first));
		tableWidget->setItem(row,1,new QTableWidgetItem(pair.second));
	}
	
	tableWidget->resizeColumnsToContents();
	tableWidget->resizeRowsToContents();
	
	connect(m_ui->resourceAddBtn, SIGNAL(clicked()), this, SLOT(addResBtn()));
	connect(m_ui->resourceDelBtn, SIGNAL(clicked()), this, SLOT(delResBtn()));
	connect(tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(adjustTableSize()));
	
}

void AppSettingsDialog::adjustTableSize()
{
	QTableWidget * tableWidget = m_ui->resourceTranslations;

	tableWidget->resizeColumnsToContents();
	tableWidget->resizeRowsToContents();
}


void AppSettingsDialog::slotDiskCacheBrowse()
{
	QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select A Cache Location"),
						 AppSettings::cacheDir().absolutePath(),
						 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(!dirPath.isEmpty())
	{
		m_ui->diskCacheBox->setText(dirPath);
	}
}

void AppSettingsDialog::slotTemplateFolderBrowse()
{
	QString dirPath = QFileDialog::getExistingDirectory(this, tr("Select A Template Storage Location"),
						 AppSettings::templateStorageFolder(),
						 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if(!dirPath.isEmpty())
	{
		m_ui->templateFolder->setText(dirPath);
	}
}


void AppSettingsDialog::controlPortChanged(int port)
{
	QString url = QString("http://%1:%2/").arg(AppSettings::myIpAddress()).arg(port);
	m_ui->httpControlUrlLabel->setText(QString("Control URL is: <a href='%1'>%1</a>").arg(url));
}

void AppSettingsDialog::viewerPortChanged(int port)
{
	QString url = QString("http://%1:%2/").arg(AppSettings::myIpAddress()).arg(port);
	m_ui->httpViewerUrlLabel->setText(QString("Viewer URL is: <a href='%1'>%1</a>").arg(url));
}

void AppSettingsDialog::linkActivated(const QString& link)
{
	if(!QDesktopServices::openUrl(QUrl(link)))
	{
		QMessageBox::critical(this, "Unable to Open Link","Sorry, I was unable to launch a web browser for the link you requested.");
	}
}

void AppSettingsDialog::slotAccepted()
{
	AppSettings::setPixmapCacheSize( m_ui->cacheBox->value());
	AppSettings::setCrossFadeSpeed(m_ui->speedBox->value());
	AppSettings::setCrossFadeQuality(m_ui->qualityBox->value());
	AppSettings::setLiveEditMode(m_ui->editModeSmooth->isChecked() ? AppSettings::PublishEdit :
				     m_ui->editModeSmooth->isChecked() ? AppSettings::SmoothEdit : 
				     					 AppSettings::LiveEdit);
	AppSettings::setAutosaveTime(m_ui->autosaveBox->value());
	AppSettings::setCacheDir(QDir(m_ui->diskCacheBox->text()));
	AppSettings::setTemplateStorageFolder(m_ui->templateFolder->text());
	
	AppSettings::setHttpControlEnabled(m_ui->httpControlEnabled->isChecked());
	AppSettings::setHttpControlPort(m_ui->httpControlPort->value());
	
	AppSettings::setHttpViewerEnabled(m_ui->httpViewerEnabled->isChecked());
	AppSettings::setHttpViewerPort(m_ui->httpViewerPort->value());
	
	AppSettings::setHotkeySequence("black",m_ui->hotkeyBlack->text());
	AppSettings::setHotkeySequence("clear",m_ui->hotkeyClear->text());
	AppSettings::setHotkeySequence("logo",m_ui->hotkeyLogo->text());
	
	AppSettings::setHotkeySequence("next-slide",m_ui->hotkeyNextSlide->text());
	AppSettings::setHotkeySequence("next-group",m_ui->hotkeyNextGroup->text());
	AppSettings::setHotkeySequence("prev-slide",m_ui->hotkeyPrevSlide->text());
	AppSettings::setHotkeySequence("prev-group",m_ui->hotkeyPrevGroup->text());

	QTableWidget * tableWidget = m_ui->resourceTranslations;
	AppSettings::ResourcePathTranslations res;
	if(tableWidget->rowCount() > 0)
	{
		for(int row=0; row<tableWidget->rowCount(); row++)
		{
			QTableWidgetItem *first = tableWidget->item(row,0);
			QTableWidgetItem *second = tableWidget->item(row,1);
			if(first && second)
				res.append(QStringPair(first->text(), second->text()));
		}
	}
	
	AppSettings::setResourcePathTranslations(res);
	
	// Yes, I'm cheating by not adding a proper accessor to AppSettings - I dont feel
	// like waiting for the entire source tree to recompile right now. Maybe later.
	QSettings settings;
	settings.setValue("max-backups",m_ui->maxBackups->value());
	
	AppSettings::setTitlesafeAmount(m_ui->titlesafeAmount->value());
	
	close();
}

void AppSettingsDialog::slotUseOpenGLChanged(bool f)
{
	//qDebug("use opengl changed: %d", f?1:0);
	AppSettings::setUseOpenGL(f);
}

void AppSettingsDialog::slotConfigOutputs()
{
	OutputSetupDialog d(this);
	d.exec();
}

// void AppSettingsDialog::slotConfigGrid()
// {
// 	GridDialog d(this);
// 	d.exec();
// }


void AppSettingsDialog::addResBtn()
{
	QTableWidget * tableWidget = m_ui->resourceTranslations;
	int row = tableWidget->rowCount();
	tableWidget->setRowCount(row+1);
	tableWidget->setItem(row,0,new QTableWidgetItem("C:\\"));
	tableWidget->setItem(row,1,new QTableWidgetItem("/"));
}

void AppSettingsDialog::delResBtn()
{
	QTableWidget * tableWidget = m_ui->resourceTranslations;
	tableWidget->removeRow(tableWidget->currentRow());
}



AppSettingsDialog::~AppSettingsDialog()
{
	delete m_ui;
}

void AppSettingsDialog::changeEvent(QEvent *e)
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
