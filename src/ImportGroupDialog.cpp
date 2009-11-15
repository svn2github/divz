#include "ImportGroupDialog.h"
#include "ui_ImportGroupDialog.h"
#include "DocumentListModel.h"
#include "model/Document.h"
#include <QFileInfo>
#include <QCompleter>
#include <QDirModel>
#include <QMessageBox>
#include <QFileDialog>
#include "AppSettings.h"
#include "MainWindow.h"

ImportGroupDialog::ImportGroupDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::ImportGroupDialog),
	m_model(new DocumentListModel),
	m_doc(0)
{
	m_ui->setupUi(this);
	
	QCompleter *completer = new QCompleter(this);
	QDirModel *dirModel = new QDirModel(completer);
	completer->setModel(dirModel);
	//completer->setMaxVisibleItems(10);
	completer->setCompletionMode(QCompleter::PopupCompletion);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setWrapAround(true);
	m_ui->file->setCompleter(completer);
	
	connect(m_ui->file, SIGNAL(returnPressed()), this, SLOT(fileSelected()));
	connect(m_ui->browseBtn, SIGNAL(clicked()), this, SLOT(browse()));
	connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(slotAccept()));
	
	m_ui->group->setModel(m_model);
}

ImportGroupDialog::~ImportGroupDialog()
{
	delete m_ui;
}

void ImportGroupDialog::fileSelected(const QString& tmp)
{
	QString fileName = tmp;
	if(fileName.isEmpty())
		fileName = m_ui->file->text();
		
	QFileInfo info(fileName);
	if(!info.exists() || fileName.isEmpty())
	{
		m_ui->groupWidget->setEnabled(false);
		return;
	}
	else
	{
		if(m_doc)
			delete m_doc;
			
		m_doc = new Document(fileName);
		m_model->setDocument(m_doc);
		m_ui->groupWidget->setEnabled(true);
	}
}

void ImportGroupDialog::browse()
{
	QString curFile = m_ui->file->text();
	if(curFile.trimmed().isEmpty())
		curFile = AppSettings::previousPath("last-dviz-file-import");
	if(curFile.trimmed().isEmpty())
		curFile = AppSettings::previousPath("last-dviz-file");

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select DViz File to Import From"), curFile, tr("DViz File (*.dviz, *.dvz);;DViz XML File (*.dvizx *.dvx *.xml);;Any File (*.*)"));
	if(fileName != "")
	{
		AppSettings::setPreviousPath("last-dviz-file-import",fileName);
		m_ui->file->setText(fileName);
		fileSelected(fileName);
	}
}


void ImportGroupDialog::slotAccept()
{
	if(!m_doc)
		return;
	int idx = m_ui->group->currentIndex();
	if(idx < 0)
	{
		QMessageBox::critical(this,"No Group Selected","You must select a group to import.");
		return;
	}

	SlideGroup * group = m_model->groupAt(idx);
	SlideGroup * clone = group->clone();
	clone->setGroupNumber(MainWindow::mw()->currentDocument()->numGroups());
	MainWindow::mw()->currentDocument()->addGroup(clone);
	
	delete m_doc;
	delete m_model;
	close();
}


void ImportGroupDialog::changeEvent(QEvent *e)
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
