#include "HttpUserSetupDialog.h"
#include "ui_HttpUserSetupDialog.h"

HttpUserSetupDialog::HttpUserSetupDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::HttpUserSetupDialog)
{
	ui->setupUi(this);
}

HttpUserSetupDialog::~HttpUserSetupDialog()
{
	delete ui;
}

void HttpUserSetupDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type())
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}
