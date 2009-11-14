#ifndef IMPORTGROUPDIALOG_H
#define IMPORTGROUPDIALOG_H

#include <QtGui/QDialog>

class DocumentListModel;
class Document;
namespace Ui {
    class ImportGroupDialog;
}

class ImportGroupDialog : public QDialog 
{
	Q_OBJECT
public:
	ImportGroupDialog(QWidget *parent = 0);
	~ImportGroupDialog();

protected:
	void changeEvent(QEvent *e);
protected slots:
	void browse();
	void fileSelected(const QString& file="");
	void slotAccept();

private:
	Ui::ImportGroupDialog *m_ui;
	DocumentListModel *m_model;
	Document *m_doc;
};

#endif // IMPORTGROUPDIALOG_H
