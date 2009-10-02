#ifndef DOCUMENTSETTINGSDIALOG_H
#define DOCUMENTSETTINGSDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class DocumentSettingsDialog;
}

class DocumentSettingsDialog : public QDialog {
    Q_OBJECT
public:
    DocumentSettingsDialog(QWidget *parent = 0);
    ~DocumentSettingsDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DocumentSettingsDialog *m_ui;
};

#endif // DOCUMENTSETTINGSDIALOG_H
