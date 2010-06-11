#include "ChooseGroupDialog.h"
#include "ui_ChooseGroupDialog.h"
#include "../MainWindow.h"


ChooseGroupDialog::ChooseGroupDialog(QWidget *parent) :
	QDialog(parent),
	m_selectedGroup(0),
	ui(new Ui::ChooseGroupDialog)
{
	ui->setupUi(this);
	
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));

	m_model = MainWindow::mw()->documentListModel();
	ui->group->setModel(m_model);
}

ChooseGroupDialog::~ChooseGroupDialog()
{
	delete ui;
}

void ChooseGroupDialog::slotAccept()
{
	int idx = ui->group->currentIndex();
	if(idx >= 0)
	{
// 		QMessageBox::critical(this,"No Group Selected","You must select a group to import.");
// 		return;
		m_selectedGroup = m_model->groupAt(idx);
	}
	
	close();
}

void ChooseGroupDialog::changeEvent(QEvent *e)
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
