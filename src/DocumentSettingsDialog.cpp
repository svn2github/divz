#include "DocumentSettingsDialog.h"
#include "ui_DocumentSettingsDialog.h"

DocumentSettingsDialog::DocumentSettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DocumentSettingsDialog)
{
    m_ui->setupUi(this);
}

DocumentSettingsDialog::~DocumentSettingsDialog()
{
    delete m_ui;
}

void DocumentSettingsDialog::changeEvent(QEvent *e)
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
