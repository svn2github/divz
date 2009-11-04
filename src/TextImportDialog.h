#ifndef TEXTIMPORTDIALOG_H
#define TEXTIMPORTDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class TextImportDialog;
}

class TextImportDialog : public QDialog {
    Q_OBJECT
public:
    TextImportDialog(QWidget *parent = 0);
    ~TextImportDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TextImportDialog *m_ui;
};

#endif // TEXTIMPORTDIALOG_H
