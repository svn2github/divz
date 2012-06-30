#include "HttpUserSetupDialog.h"
#include "ui_HttpUserSetupDialog.h"

#include "HttpUserUtil.h"

#include <QComboBox>

HttpUserSetupDialog::HttpUserSetupDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::HttpUserSetupDialog)
{
	ui->setupUi(this);
	
	QTableWidget * tableWidget = ui->httpUsers;
	QList<HttpUser*> users = HttpUserUtil::instance()->users();
	
	tableWidget->setRowCount(users.size());
	int row = 0;
	foreach(HttpUser *user, users)
	{
		tableWidget->setItem(row, 0, new QTableWidgetItem(user->user()));
		tableWidget->setItem(row, 1, new QTableWidgetItem(user->pass()));
		tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(user->level())));
		
		QComboBox *levelCombo = new QComboBox();
		
		levelCombo->addItems(QStringList() << "Guest" << "User" << "Admin");
		levelCombo->setCurrentIndex(user->level());
		
		tableWidget->setCellWidget(row, 2, levelCombo);
		 
		row ++;
	}
	
	tableWidget->resizeColumnsToContents();
	tableWidget->resizeRowsToContents();
	
	connect(ui->addUserBtn, SIGNAL(clicked()), this, SLOT(addUser()));
	connect(ui->delUserBtn, SIGNAL(clicked()), this, SLOT(delUser()));
	connect(tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(adjustTableSize()));
	
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotAccepted()));
	
}

void HttpUserSetupDialog::addUser()
{
	QTableWidget * tableWidget = ui->httpUsers;
	int row = tableWidget->rowCount();
	tableWidget->setRowCount(row+1);
	tableWidget->setItem(row,0,new QTableWidgetItem("newuser"));
	tableWidget->setItem(row,1,new QTableWidgetItem("newpass"));
	tableWidget->setItem(row,2,new QTableWidgetItem("0"));
		
	QComboBox *levelCombo = new QComboBox();
	
	levelCombo->addItems(QStringList() << "Guest" << "User" << "Admin");
	levelCombo->setCurrentIndex(0);
	
	tableWidget->setCellWidget(row, 2, levelCombo);
	
}

void HttpUserSetupDialog::delUser()
{
	QTableWidget * tableWidget = ui->httpUsers;
	tableWidget->removeRow(tableWidget->currentRow());
}

void HttpUserSetupDialog::adjustTableSize()
{
	QTableWidget * tableWidget = ui->httpUsers;

	tableWidget->resizeColumnsToContents();
	tableWidget->resizeRowsToContents();
}

void HttpUserSetupDialog::slotAccepted()
{
	QTableWidget * tableWidget = ui->httpUsers;
	
	QList<HttpUser *> userList;
	
	for(int row=0; row<tableWidget->rowCount(); row++)
	{
		QTableWidgetItem *userItem  = tableWidget->item(row, 0);
		QTableWidgetItem *passItem  = tableWidget->item(row, 1);
		
		QWidget *levelWidget = tableWidget->cellWidget(row, 2);
		QComboBox *levelCombo = dynamic_cast<QComboBox*>(levelWidget);
		
		userList << new HttpUser(userItem->text(), passItem->text(), (HttpUser::UserLevel)levelCombo->currentIndex());
	}
	
	HttpUserUtil::instance()->setUsers(userList);
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
