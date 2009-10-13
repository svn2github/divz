#include "SlideSettingsDialog.h"
#include "ui_SlideSettingsDialog.h"
#include "model/Slide.h"
#include "items/BackgroundConfig.h"
#include <QMessageBox>


SlideSettingsDialog::SlideSettingsDialog(Slide *slide, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SlideSettingsDialog),
    m_slide(slide)
{
    m_ui->setupUi(this);
    m_ui->slideChangeTime->setValue(slide->autoChangeTime());
    connect(m_ui->slideChangeTime, SIGNAL(valueChanged(double)), this, SLOT(setAutoChangeTime(double)));

    // Not implemented yet, so hide from UI
    m_ui->boxConfigBg->setVisible(false);
    connect(m_ui->btnConfigBg, SIGNAL(clicked()), this, SLOT(configBg()));
}

SlideSettingsDialog::~SlideSettingsDialog()
{
    delete m_ui;
}

void SlideSettingsDialog::setAutoChangeTime(double d)
{
    m_slide->setAutoChangeTime(d);
}

void SlideSettingsDialog::configBg()
{
    QMessageBox::information(this,"Not Implemented","Sorry, but this button has not been implemented yet!");
}

void SlideSettingsDialog::changeEvent(QEvent *e)
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
