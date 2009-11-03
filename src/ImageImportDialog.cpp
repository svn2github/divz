#include "ImageImportDialog.h"
#include "ui_ImageImportDialog.h"

ImageImportDialog::ImageImportDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ImageImportDialog)
{
    m_ui->setupUi(this);
}

ImageImportDialog::~ImageImportDialog()
{
    delete m_ui;
}

void ImageImportDialog::changeEvent(QEvent *e)
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
