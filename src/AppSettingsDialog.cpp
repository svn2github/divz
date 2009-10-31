#include "AppSettingsDialog.h"
#include "ui_AppSettingsDialog.h"
#include "AppSettings.h"
#include "OutputSetupDialog.h"
//#include "GridDialog.h"

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
	
	if(AppSettings::liveEditMode() == AppSettings::SmoothEdit)
		m_ui->editModeSmooth->setChecked(true);
	else
		m_ui->editModeLive->setChecked(true);
	
	setWindowTitle("Program Settings");
	
	connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotAccepted()));
}

void AppSettingsDialog::slotAccepted()
{
	AppSettings::setPixmapCacheSize( m_ui->cacheBox->value());
	AppSettings::setCrossFadeSpeed(m_ui->speedBox->value());
	AppSettings::setCrossFadeQuality(m_ui->qualityBox->value());
	AppSettings::setLiveEditMode(m_ui->editModeSmooth->isChecked() ? AppSettings::SmoothEdit : AppSettings::LiveEdit);
	AppSettings::setAutosaveTime(m_ui->autosaveBox->value());
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
