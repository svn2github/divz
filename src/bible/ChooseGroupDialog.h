#ifndef CHOOSEGROUPDIALIG_H
#define CHOOSEGROUPDIALIG_H

#include "../DocumentListModel.h"
#include "../model/SlideGroup.h"
#include <QDialog>

namespace Ui {
    class ChooseGroupDialog;
}

class ChooseGroupDialog : public QDialog 
{
	Q_OBJECT
public:
	ChooseGroupDialog(QWidget *parent = 0);
	~ChooseGroupDialog();
	
	SlideGroup * selectedGroup() { return m_selectedGroup; }

protected:
	void changeEvent(QEvent *e);
	
protected slots:
	void slotAccept();

private:
	DocumentListModel *m_model;
	SlideGroup * m_selectedGroup;
	
	Ui::ChooseGroupDialog *ui;
};

#endif // CHOOSEGROUPDIALIG_H
