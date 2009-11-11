#include "AppSettingsDialog.h"
#include "ui_AppSettingsDialog.h"
#include "AppSettings.h"
#include "OutputSetupDialog.h"
#include <QFileDialog>

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
