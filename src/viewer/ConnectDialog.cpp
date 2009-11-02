#include "ConnectDialog.h"
#include "ui_ConnectDialog.h"

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ConnectDialog)
{
    m_ui->setupUi(this);
}

ConnectDialog::~ConnectDialog()
{
    delete m_ui;
}

void ConnectDialog::changeEvent(QEvent *e)
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
