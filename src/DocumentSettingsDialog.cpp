#include "DocumentSettingsDialog.h"
#include "ui_DocumentSettingsDialog.h"

DocumentSettingsDialog::DocumentSettingsDialog(Document *d, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::DocumentSettingsDialog),
    m_doc(d)
{
    m_ui->setupUi(this);
    m_ui->aspectRatio->setValue(m_doc->aspectRatio());
    connect(m_ui->btn16x9, SIGNAL(clicked()),  this, SLOT(set16x9()));
    connect(m_ui->btn4x3,  SIGNAL(clicked()),  this, SLOT(set4x3()));
    connect(this,          SIGNAL(accepted()), this, SLOT(accepted()));
}

DocumentSettingsDialog::~DocumentSettingsDialog()
{
    delete m_ui;
}

void DocumentSettingsDialog::set4x3()  { m_ui->aspectRatio->setValue((double)4/(double)3); }
void DocumentSettingsDialog::set16x9() { m_ui->aspectRatio->setValue((double)16/(double)9); }

void DocumentSettingsDialog::accepted()
{
	if(m_doc->aspectRatio() != m_ui->aspectRatio->value())
		m_doc->setAspectRatio(m_ui->aspectRatio->value());
}

void DocumentSettingsDialog::changeEvent(QEvent *e)
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
