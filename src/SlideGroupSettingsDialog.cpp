#include "SlideGroupSettingsDialog.h"
#include "ui_SlideGroupSettingsDialog.h"

SlideGroupSettingsDialog::SlideGroupSettingsDialog(SlideGroup *g, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SlideGroupSettingsDialog),
    m_slideGroup(g)
{
	m_ui->setupUi(this);
	m_ui->rNothing->setChecked(!m_slideGroup->autoChangeGroup());
	connect(m_ui->rChange, SIGNAL(toggled(bool)), this, SLOT(autoChangeGroup(bool)));
	m_ui->title->setText(m_slideGroup->groupTitle().isEmpty() ? QString("Group %1").arg(m_slideGroup->groupNumber()+1) : m_slideGroup->groupTitle());
	connect(m_ui->title, SIGNAL(textChanged(const QString&)), this, SLOT(titleChanged(const QString&)));
	
	setWindowTitle("Slide Group Settings");
	
	if(m_slideGroup->inheritFadeSettings())
		m_ui->btnUseApp->setChecked(true);
	else
		m_ui->btnUseGroup->setChecked(true);
	m_ui->speedBox->setValue(m_slideGroup->crossFadeSpeed());
	m_ui->qualityBox->setValue(m_slideGroup->crossFadeQuality());
	connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotAccepted()));
}

SlideGroupSettingsDialog::~SlideGroupSettingsDialog()
{
    delete m_ui;
}



void SlideGroupSettingsDialog::slotAccepted()
{
	m_slideGroup->setInheritFadeSettings(m_ui->btnUseApp->isChecked());
	m_slideGroup->setCrossFadeSpeed(m_ui->speedBox->value());
	m_slideGroup->setCrossFadeQuality(m_ui->qualityBox->value());
	close();
}


void SlideGroupSettingsDialog::autoChangeGroup(bool flag)
{
    m_slideGroup->setAutoChangeGroup(flag);
}

void SlideGroupSettingsDialog::titleChanged(const QString& title)
{
    m_slideGroup->setGroupTitle(title);
}

void SlideGroupSettingsDialog::changeEvent(QEvent *e)
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
