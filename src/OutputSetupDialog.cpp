#include "OutputSetupDialog.h"
#include "ui_outputsetupdialog.h"
#include "model/Output.h"
#include "AppSettings.h"
#include "MainWindow.h"
#include "model/Document.h"

#include <QDesktopWidget>
#include <QDebug>
#include <QMessageBox>

OutputSetupDialog::OutputSetupDialog(QWidget *parent) :
	QDialog(parent)
	, m_ui(new Ui::OutputSetupDialog)
	, m_output(0)
	, m_outputIdx(0)
	
{
	m_ui->setupUi(this);
	setWindowTitle(tr("Output Setup"));
	setWindowIcon(QIcon(":/data/icon-d.png"));

	setupOutputList();
	setupScreenList();
	
	connect(m_ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)));
	connect(m_ui->outputName, SIGNAL(textChanged(const QString&)), this, SLOT(slotOutputNameChanged(const QString&)));
	
	connect(m_ui->customLeft,   SIGNAL(valueChanged(int)), this, SLOT(slotCustX(int)));
	connect(m_ui->customTop,    SIGNAL(valueChanged(int)), this, SLOT(slotCustY(int)));
	connect(m_ui->customWidth,  SIGNAL(valueChanged(int)), this, SLOT(slotCustW(int)));
	connect(m_ui->customHeight, SIGNAL(valueChanged(int)), this, SLOT(slotCustH(int)));
	
	
	connect(m_ui->stayOnTop, SIGNAL(toggled(bool)), this, SLOT(slotStayOnTop(bool)));
	
	connect(m_ui->mjpegEnabled, SIGNAL(toggled(bool)), this, SLOT(slotMjpegEnabled(bool)));
	connect(m_ui->mjpegEnabled, SIGNAL(toggled(bool)), m_ui->mjpegEnabled2, SLOT(setChecked(bool)));
	connect(m_ui->mjpegEnabled2, SIGNAL(toggled(bool)), m_ui->mjpegEnabled, SLOT(setChecked(bool)));
	
	
	connect(m_ui->mjpegPort, SIGNAL(valueChanged(int)), this, SLOT(slotMjpegPort(int)));
	connect(m_ui->mjpegPort, SIGNAL(valueChanged(int)), m_ui->mjpegPort2, SLOT(setValue(int)));
	connect(m_ui->mjpegPort2, SIGNAL(valueChanged(int)), m_ui->mjpegPort, SLOT(setValue(int)));
	
	
	connect(m_ui->mjpegFps, SIGNAL(valueChanged(int)), this, SLOT(slotMjpegFps(int)));
	connect(m_ui->mjpegFps, SIGNAL(valueChanged(int)), m_ui->mjpegFps2, SLOT(setValue(int)));
	connect(m_ui->mjpegFps2, SIGNAL(valueChanged(int)), m_ui->mjpegFps, SLOT(setValue(int)));
	
	
	connect(m_ui->cbOutputEnabled, SIGNAL(stateChanged(int)), this, SLOT(slotOutputEnabledStateChanged(int)));
// 	connect(m_ui->rbListen, SIGNAL(toggled(bool)), this, SLOT(slotNetRoleChanged(bool)));
	
	/*connect(m_ui->hostname, SIGNAL(textChanged(const QString&)), this, SLOT(slotHostChanged(const QString&)));
	connect(m_ui->connectPort, SIGNAL(valueChanged(int)), this, SLOT(slotPortChanged(int)));
	*/
	connect(m_ui->listenPort, SIGNAL(valueChanged(int)), this, SLOT(slotPortChanged(int)));
	//connect(m_ui->allowMultipleIncomming, SIGNAL(stateChanged(int)), this, SLOT(slotAllowMultChanged(int)));
	
	connect(m_ui->btnNewOutput, SIGNAL(clicked()), this, SLOT(slotNew()));
	connect(m_ui->btnDelOutput, SIGNAL(clicked()), this, SLOT(slotDel()));
	
	m_ui->screenListView->setEnabled(false);
	m_ui->btnDelOutput->setEnabled(false);
	m_ui->label_2->setText("");

	//connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void OutputSetupDialog::slotNew()
{
	Output *x = new Output();
	AppSettings::addOutput(x);
	setupOutputList();
	setOutput(x);
	m_ui->outputListView->selectRow(m_outputIdx);
}

void OutputSetupDialog::slotDel()
{
	if(!m_output)
		return;
	if(m_output->isSystem())
	{
		qDebug("slotDel: Not deleting because isSystem output");
		return;
	}
	AppSettings::removeOutput(m_output);
	setupOutputList();
	m_output = 0;
}

void OutputSetupDialog::slotOutputListCellActivated(int row,int)
{
	setOutput(AppSettings::outputs().at(row));
}

void OutputSetupDialog::slotScreenListCellActivated(int row,int)
{
	m_output->setScreenNum(row);
}

void OutputSetupDialog::slotOutputEnabledStateChanged(int state)
{
	if(!m_output)
		return;
		
	m_output->setIsEnabled(state ? true : false);
	
	m_ui->outputListView->item(m_outputIdx,1)->setText(state ? "Yes" : "No");
}

void OutputSetupDialog::slotNetRoleChanged(bool listen)
{
	if(!m_output)
		return;
		
	m_output->setNetworkRole(listen ? Output::Server : Output::Client);
}


void OutputSetupDialog::slotOutputNameChanged(const QString& name)
{
	if(!m_output)
		return;
	m_output->setName(name);
	m_ui->outputListView->item(m_outputIdx,0)->setText(name);
}

void OutputSetupDialog::slotCustX(int x)
{
	if(!m_output)
		return;
	QRect r = m_output->customRect();
	r = QRect(x,r.y(),r.width(),r.height());
	m_output->setCustomRect(r);
}


void OutputSetupDialog::slotCustY(int x)
{
	if(!m_output)
		return;
	QRect r = m_output->customRect();
	r = QRect(r.x(),x,r.width(),r.height());
	m_output->setCustomRect(r);
}


void OutputSetupDialog::slotCustW(int x)
{
	if(!m_output)
		return;
	QRect r = m_output->customRect();
	r = QRect(r.x(),r.y(),x,r.height());
	m_output->setCustomRect(r);
}


void OutputSetupDialog::slotCustH(int x)
{
	if(!m_output)
		return;
	QRect r = m_output->customRect();
	r = QRect(r.x(),r.y(),r.width(),x);
	m_output->setCustomRect(r);
}

void OutputSetupDialog::slotHostChanged(const QString& name)
{
	if(!m_output)
		return;
	m_output->setHost(name);
}

void OutputSetupDialog::slotPortChanged(int x)
{
	if(!m_output)
		return;
	m_output->setPort(x);
}

void OutputSetupDialog::slotAllowMultChanged(int x)
{
	if(!m_output)
		return;
	m_output->setAllowMultiple(x ? true : false);
}

void OutputSetupDialog::slotStayOnTop(bool x)
{
	if(!m_output)
		return;
	m_output->setStayOnTop(x);
}

void OutputSetupDialog::slotMjpegEnabled(bool x)
{
	if(!m_output)
		return;
	m_output->setMjpegServerEnabled(x);
}

void OutputSetupDialog::slotMjpegPort(int x)
{
	if(!m_output)
		return;
	m_output->setMjpegServerPort(x);
}

void OutputSetupDialog::slotMjpegFps(int x)
{
	if(!m_output)
		return;
	m_output->setMjpegServerFPS(x);
}

void OutputSetupDialog::slotTabChanged(int tab)
{
	if(!m_output)
		return;
	m_output->setOutputType((Output::OutputType)tab);
}

void OutputSetupDialog::setOutput(Output *output)
{
	m_outputIdx = AppSettings::outputs().indexOf(output);
	m_output = output;

	setWindowTitle(QString(tr("Output Setup - %1")).arg(output->name()));
	
	m_ui->screenListView->selectRow(output->screenNum());
	
	int tabIdx = (int)m_output->outputType();
	m_ui->tabWidget->setCurrentIndex(tabIdx); 
	m_ui->outputName->setText(output->name());
	m_ui->cbOutputEnabled->setChecked(output->isEnabled());
	
	QRect r = m_output->customRect();
	m_ui->customLeft->setValue(r.x());
	m_ui->customTop->setValue(r.y());
	m_ui->customWidth->setValue(r.width());
	m_ui->customHeight->setValue(r.height());
	
	Output::NetworkRole role = output->networkRole();
// 	if(role == Output::Server)
// 		m_ui->rbListen->setChecked(true);
// 	else
// 		m_ui->rbConnect->setChecked(true);
		
// 	m_ui->hostname->setText(output->host());
// 	m_ui->connectPort->setValue(output->port());
	m_ui->listenPort->setValue(output->port());
	//m_ui->allowMultipleIncomming->setChecked(output->allowMultiple());
	
	m_ui->btnDelOutput->setEnabled(output->isSystem() ? false : true);
	
	m_ui->outputName->setEnabled(output->isSystem() ? false:true);
	m_ui->label_2->setText( m_output->isSystem() ? "This is a system output, you cannot change the name." : "");
	
	m_ui->screenListView->setEnabled(true);
	
	
	m_ui->stayOnTop->setChecked(output->stayOnTop());
	
	m_ui->mjpegEnabled->setChecked(output->mjpegServerEnabled());
	m_ui->mjpegPort->setValue(output->mjpegServerPort());
	m_ui->mjpegFps->setValue(output->mjpegServerFPS());
	
	
	// flush any changes the previosly-selected output back into the list view
	//setupOutputList();
}

void OutputSetupDialog::setupScreenList()
{
	QDesktopWidget *d = QApplication::desktop();

	QTableWidget * tbl = m_ui->screenListView;
	
	tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
	connect(tbl, SIGNAL(cellClicked(int,int)), this, SLOT(slotScreenListCellActivated(int,int)));
	
	tbl->setHorizontalHeaderLabels(QStringList() << "Screen #" << "Size/Position");

	int ns = d->numScreens();
	tbl->setRowCount(ns);

	QTableWidgetItem *prototype = new QTableWidgetItem();
	// setup your prototype
	prototype->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);


	for(int screenNum = 0; screenNum < ns; screenNum ++)
	{
		QRect geom = d->screenGeometry(screenNum);
		QString diz = QString("%1 x %2 at (%3,%4)")
						.arg(geom.width())
						.arg(geom.height())
						.arg(geom.left())
						.arg(geom.top());
		qDebug() << "setupScreenList(): screenNum:"<<screenNum<<", diz:"<<diz;
		
		QTableWidgetItem *t = prototype->clone();
		t->setText(QString("Screen %1").arg(screenNum+1));
		tbl->setItem(screenNum,0,t);
		
		t = prototype->clone();
		t->setText(diz);
		tbl->setItem(screenNum,1,t);
	}
	
	tbl->resizeColumnsToContents();
	tbl->resizeRowsToContents();
}


void OutputSetupDialog::setupOutputList()
{
	QList<Output*> outputs = AppSettings::outputs();

	QTableWidget * tbl = m_ui->outputListView;
	tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
	connect(tbl, SIGNAL(cellClicked(int,int)), this, SLOT(slotOutputListCellActivated(int,int)));
	
	tbl->setHorizontalHeaderLabels(QStringList() << "Name" << "Enabled?" << "Type");

	tbl->setRowCount(outputs.size());

	QTableWidgetItem *prototype = new QTableWidgetItem();
	// setup your prototype
	prototype->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

	int row=0;
	foreach(Output*out, outputs)
	{
		QTableWidgetItem *t = prototype->clone();
		t->setText(out->name());
		tbl->setItem(row,0,t);

		/*
		t = prototype->clone();
		t->setText(out->isSystem() ? "Yes" : "No");
		tbl->setItem(row,1,t);
		*/

		t = prototype->clone();
		t->setText(out->isEnabled() ? "Yes" : "No");
		tbl->setItem(row,1,t);

		t = prototype->clone();
		QRect r = out->customRect();
		t->setText(out->outputType() == Output::Screen ?
				QString("Screen %1").arg(out->screenNum()+1) :
			    out->outputType() == Output::Custom ?
				QString("%1 x %2 at (%3,%4)").arg(r.width()).arg(r.height()).arg(r.x()).arg(r.y()) :
			    out->outputType() == Output::Network ?
				QString("Network") :
			    "Unknown");
		tbl->setItem(row,2,t);

		row++;
	}
	
	tbl->resizeColumnsToContents();
	tbl->resizeRowsToContents();
}

void OutputSetupDialog::accept()
{
	AppSettings::save();

	Output *out = AppSettings::outputs().at(0);
	if(out)
	{
		double ar = out->aspectRatio();

		if(ar > -1)
		{
			if(MainWindow::mw()->currentDocument() &&
			   ar != MainWindow::mw()->currentDocument()->aspectRatio())
			{
				if(QMessageBox::question(
					this,
					"Aspect Ratio Different",
					"The output you just chose has a different aspect ratio than the current document. Do you want to set the document's aspect ratio to match the output aspect ratio?",
					QMessageBox::Save | QMessageBox::Discard,
					QMessageBox::Save
				) == QMessageBox::Save)
				{
					MainWindow::mw()->currentDocument()->setAspectRatio(ar);
				}
			}
		}
	}

	QDialog::accept();
}

void OutputSetupDialog::reject()
{
	AppSettings::load();
	//deleteLater();
	QDialog::reject();
}


OutputSetupDialog::~OutputSetupDialog()
{
	delete m_ui;
}

void OutputSetupDialog::changeEvent(QEvent *e)
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
