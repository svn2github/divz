#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QtGui/QDialog>

#include ".build/ui_ConnectDialog.h"

class ConnectDialog : public QDialog {
    Q_OBJECT
public:
    ConnectDialog(QWidget *parent = 0);
    ~ConnectDialog();

    QString host() { return m_ui->host->text(); }
    int port() { return m_ui->port->value(); }

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ConnectDialog *m_ui;
};

#endif // CONNECTDIALOG_H
