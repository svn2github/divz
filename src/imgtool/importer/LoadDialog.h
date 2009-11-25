#ifndef LOADDIALOG_H
#define LOADDIALOG_H

#include <QDialog>

#include ".build/ui_LoadDialog.h"
class LoadDialog : public QDialog {
	Q_OBJECT
public:
	LoadDialog(QWidget *parent = 0);
	~LoadDialog();
	
	int lookAhead() {  return ui->lookAhead->value(); }
	int lookBehind() {  return ui->lookBehind->value(); }
	int cacheSize() {  return ui->cacheSize->value(); }
	QString batchFolder() {  return ui->importFolder->text(); }
	QString copyFolder() { return ui->copyToFolder->text(); }
	bool copyFiles() { return ui->copyFiles->isChecked(); }
	bool removeFiles() { return ui->removeFlag->isChecked(); }
	
protected slots:
	void slotBrowse();
	void slotCopyBrowse();
	void slotAccepted();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::LoadDialog *ui;
};

#endif // LOADDIALOG_H
