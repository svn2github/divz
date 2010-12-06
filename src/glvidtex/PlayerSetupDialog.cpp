#include "PlayerSetupDialog.h"
#include "ui_PlayerSetupDialog.h"

PlayerSetupDialog::PlayerSetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayerSetupDialog)
{
    ui->setupUi(this);
}

PlayerSetupDialog::~PlayerSetupDialog()
{
    delete ui;
}
