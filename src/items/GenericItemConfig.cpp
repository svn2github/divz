#include "GenericItemConfig.h"
#include "ui_GenericItemConfig.h"

GenericItemConfig::GenericItemConfig(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::GenericItemConfig)
{
    m_ui->setupUi(this);
}

GenericItemConfig::~GenericItemConfig()
{
    delete m_ui;
}

void GenericItemConfig::changeEvent(QEvent *e)
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
