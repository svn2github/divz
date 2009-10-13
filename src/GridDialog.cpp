#include "GridDialog.h"
#include "ui_GridDialog.h"
#include "AppSettings.h"

#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>

GridDialog::GridDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::GridDialog)
{
	m_ui->setupUi(this);
	
	m_ui->cbSnapToGrid->setChecked(AppSettings::gridEnabled());
	connect(m_ui->cbSnapToGrid, SIGNAL(toggled(bool)), this, SLOT(snapToGridEnabled(bool)));
	
	m_ui->gridSize->setValue(AppSettings::gridSize().width());
	connect(m_ui->gridSize, SIGNAL(valueChanged(double)), this, SLOT(setGridSize(double)));
	
	m_ui->cbThirds->setChecked(AppSettings::thirdGuideEnabled());
	connect(m_ui->cbThirds, SIGNAL(toggled(bool)), this, SLOT(setThirdEnabled(bool)));
	
	connect(m_ui->linkLabel, SIGNAL(linkActivated(const QString&)), this, SLOT(linkActivated(const QString&)));
	
	setWindowTitle("Grids and Guidelines");
}

GridDialog::~GridDialog()
{
	delete m_ui;
}

void GridDialog::snapToGridEnabled(bool flag)
{
	AppSettings::setGridEnabled(flag);
}

void GridDialog::setGridSize(double d)
{
	AppSettings::setGridSize(QSizeF(d,d));
}

void GridDialog::setThirdEnabled(bool flag)
{
	AppSettings::setThirdGuideEnabled(flag);
}

void GridDialog::linkActivated(const QString& link)
{
	if(!QDesktopServices::openUrl(QUrl(link)))
	{
		QMessageBox::critical(this, "Unable to Open Link","Sorry, I was unable to launch a web browser for the link you requested.");
	}
}

void GridDialog::changeEvent(QEvent *e)
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
