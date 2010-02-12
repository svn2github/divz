#include "ConnectDialog.h"
#include "ui_ConnectDialog.h"
#include <QSettings>

ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ConnectDialog)
{
	m_ui->setupUi(this);
	
	QSettings s;
	m_ui->host->setText(s.value("last-ip","localhost").toString());
	m_ui->port->setValue(s.value("last-port",7777).toInt());
	m_ui->reconnect->setChecked(s.value("last-reconnect",true).toBool());

}

void ConnectDialog::accept()
{
	QSettings s;
	s.setValue("last-ip",m_ui->host->text());
	s.setValue("last-port",m_ui->port->value());
	s.setValue("last-reconnect",m_ui->reconnect->isChecked());
	QDialog::accept();
}

ConnectDialog::~ConnectDialog()
{
	delete m_ui;
}

void ConnectDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) 
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}
