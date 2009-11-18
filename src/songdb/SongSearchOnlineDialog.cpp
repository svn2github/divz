#include "SongSearchOnlineDialog.h"
#include "ui_SongSearchOnlineDialog.h"

SongSearchOnlineDialog::SongSearchOnlineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SongSearchOnlineDialog)
{
    ui->setupUi(this);
}

SongSearchOnlineDialog::~SongSearchOnlineDialog()
{
    delete ui;
}

void SongSearchOnlineDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
