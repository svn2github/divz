#ifndef TEXTIMPORTDIALOG_H
#define TEXTIMPORTDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class TextImportDialog;
}

class SlideGroup;

class TextImportDialog : public QDialog 
{
	Q_OBJECT
public:
	TextImportDialog(QWidget *parent = 0);
	~TextImportDialog();


protected slots:
	void templateChanged(SlideGroup*);
	
	void doImport();
	void browseBtn();
	
protected:
	void changeEvent(QEvent *e);
	
	void setupUi();

private:
	Ui::TextImportDialog *m_ui;
	
	SlideGroup *m_template;
};

#endif // TEXTIMPORTDIALOG_H
