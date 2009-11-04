#include "TextImportDialog.h"
#include "ui_TextImportDialog.h"

TextImportDialog::TextImportDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::TextImportDialog)
{
    m_ui->setupUi(this);
}

TextImportDialog::~TextImportDialog()
{
    delete m_ui;
}

void TextImportDialog::changeEvent(QEvent *e)
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
