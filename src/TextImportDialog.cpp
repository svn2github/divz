#include "TextImportDialog.h"
#include "ui_TextImportDialog.h"

#include "model/SlideTemplateManager.h"

TextImportDialog::TextImportDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::TextImportDialog)
{
	m_ui->setupUi(this);
	setupUi();
}

TextImportDialog::~TextImportDialog()
{
	delete m_ui;
}

void TextImportDialog::setupUi()
{
	// Find the template
	QSettings settings;
	int templateId = settings.value("textimport/template-id",0).toInt();
	if(templateId > 0)
		m_template = SlideTemplateManager::instance()->findTemplate(SlideTemplateManager::Generic,templateId);
	
	// Setup the template selector widget
	TemplateSelectorWidget *tmplWidget = new TemplateSelectorWidget(SlideTemplateManager::Generic,"Template:",this);
	if(m_template)
		tmplWidget->setSelectedGroup(m_template);
	else
		m_template = tmplWidget->selectedGroup();
		
	connect(tmplWidget, SIGNAL(currentGroupChanged(SlideGroup*)), this, SLOT(templateChanged(SlideGroup*)));
	
	// Connect slots
	connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(doImport()));
	connect(m_ui->browse,    SIGNAL(clicked()),  this, SLOT(browseBtn()));
	
}

void TextImportDialog::browseBtn()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Text File"),
		AppSettings::previousPath("textimport"),
		tr("Text Files (*.txt);;Any File (*.*)")
	);
	
	if(!fileName.isEmpty())
		m_ui->browse->setText(fileName);
}

void TextImportDialog::templateChanged(SlideGroup* group)
{
	m_template = group;
	QSettings().setValue("textimport/template-id", m_template ? m_template->groupId() : 0);
}

void TextImportDialog::doImport()
{
	QString fileName = m_ui->browse->text();
	AppSettings::setPreviousPath("textimport",fileName);
	
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this,tr("Can't Read File"),QString(tr("Unable to open %1")).arg(fileName));
		return;
	}

	QStringList lines;
	QTextStream stream(&file);
	while( ! stream.atEnd() )
		lines << stream.readLine();
		
	// TODO: Finish processing and do everything else 
}

void TextImportDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) 
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default:
			break;
	}
}
