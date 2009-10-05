#include "GridDialog.h"
#include "ui_GridDialog.h"

GridDialog::GridDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::GridDialog)
{
    m_ui->setupUi(this);
}

GridDialog::~GridDialog()
{
    delete m_ui;
}

void GridDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
