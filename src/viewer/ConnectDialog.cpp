#include "ConnectDialog.h"
#include "ui_ConnectDialog.h"

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ConnectDialog)
{
    m_ui->setupUi(this);

    QSettings s;
    m_ui->host->setText(s.value("last-ip"));
    m_ui->port->setText(s.value("last-port").toInt());
    m_ui->reconnect->setChecked(s.value("last-reconnect").toBool());

}

void ConnectDialog::accept()
{
    QSetting s;
    s.setValue("last-ip",m_ui->host->text());
    s.setValue("last-port",m_ui->port->value());
    s.setValue("last-reconnect",m_ui->reconnect->isChecked());
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
