#ifndef IMPORTGROUPDIALOG_H
#define IMPORTGROUPDIALOG_H

#include <QtGui/QDialog>

class SlideGroup;
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
	
	SlideGroup *selectedGroup() { return m_selectedGroup; }

	bool addSelectedToMainWindow() { return m_addSelectedToMainWindow; }
	void setAddSelectedToMainWindow(bool flag) { m_addSelectedToMainWindow=flag; }

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
	SlideGroup *m_selectedGroup;
	bool m_addSelectedToMainWindow;
};

#endif // IMPORTGROUPDIALOG_H
