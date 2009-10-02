#ifndef DOCUMENTSETTINGSDIALOG_H
#define DOCUMENTSETTINGSDIALOG_H

#include <QtGui/QDialog>
#include "model/Document.h"

namespace Ui {
    class DocumentSettingsDialog;
}

class DocumentSettingsDialog : public QDialog {
    Q_OBJECT
public:
    DocumentSettingsDialog(Document * doc, QWidget *parent = 0);
    ~DocumentSettingsDialog();

protected slots:
    void accepted();
    void set4x3();
    void set16x9();
    
protected:
    void changeEvent(QEvent *e);

private:
    Ui::DocumentSettingsDialog *m_ui;
    Document * m_doc;
};

#endif // DOCUMENTSETTINGSDIALOG_H
