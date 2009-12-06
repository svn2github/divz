#include "SlideGroupSettingsDialog.h"
#include "ui_SlideGroupSettingsDialog.h"
#include "model/SlideGroup.h"
#include "DocumentListModel.h"
#include "MainWindow.h"
#include <QMessageBox>

SlideGroupSettingsDialog::SlideGroupSettingsDialog(SlideGroup *g, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SlideGroupSettingsDialog),
    m_slideGroup(g),
    m_model(new DocumentListModel(MainWindow::mw()->currentDocument()))
{
	m_ui->setupUi(this);
	
	if(m_slideGroup->endOfGroupAction() == SlideGroup::LoopToStart)
		m_ui->rNothing->setChecked(true);
	else
	if(m_slideGroup->endOfGroupAction() == SlideGroup::GotoNextGroup)
		m_ui->rChange->setChecked(true);
	else
		m_ui->rJump->setChecked(true);
		
	m_ui->jumpToBox->setModel(m_model);
	m_ui->jumpToBox->setCurrentIndex(m_slideGroup->jumpToGroupIndex());
	
	//connect(m_ui->rChange, SIGNAL(toggled(bool)), this, SLOT(autoChangeGroup(bool)));
	
	m_ui->title->setText(m_slideGroup->groupTitle().isEmpty() ? QString("Group %1").arg(m_slideGroup->groupNumber()+1) : m_slideGroup->groupTitle());
	//connect(m_ui->title, SIGNAL(textChanged(const QString&)), this, SLOT(titleChanged(const QString&)));
	
	setWindowTitle("Slide Group Settings");
	setWindowIcon(QIcon(":/data/icon-d.png"));
	
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
	m_slideGroup->setEndOfGroupAction(m_ui->rNothing->isChecked() ? SlideGroup::LoopToStart : 
					  m_ui->rChange->isChecked()  ? SlideGroup::GotoNextGroup :
					  SlideGroup::GotoGroupIndex);
	int idx = m_ui->jumpToBox->currentIndex();
	if(m_slideGroup->endOfGroupAction() == SlideGroup::GotoGroupIndex && idx < 0)
	{
		QMessageBox::critical(this,"No Existing Group Selected","You must select a group to jump to if you select the option titled 'Jump to another group'");
		return;
	}
	m_slideGroup->setJumpToGroupIndex(idx);
	
	m_slideGroup->setInheritFadeSettings(m_ui->btnUseApp->isChecked());
	m_slideGroup->setCrossFadeSpeed(m_ui->speedBox->value());
	m_slideGroup->setCrossFadeQuality(m_ui->qualityBox->value());
	m_slideGroup->setGroupTitle(m_ui->title->text());
	close();
}


void SlideGroupSettingsDialog::autoChangeGroup(bool flag)
{
    //m_slideGroup->setAutoChangeGroup(flag);
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
