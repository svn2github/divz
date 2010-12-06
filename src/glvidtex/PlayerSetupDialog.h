#ifndef PLAYERSETUPDIALOG_H
#define PLAYERSETUPDIALOG_H

#include <QDialog>

namespace Ui {
    class PlayerSetupDialog;
}

class PlayerSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerSetupDialog(QWidget *parent = 0);
    ~PlayerSetupDialog();

private:
    Ui::PlayerSetupDialog *ui;
};

#endif // PLAYERSETUPDIALOG_H
