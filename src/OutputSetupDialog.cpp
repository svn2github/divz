#include "OutputSetupDialog.h"
#include "ui_outputsetupdialog.h"
#include "model/Output.h"
#include "AppSettings.h"

#include <QDesktopWidget>
#include <QDebug>

OutputSetupDialog::OutputSetupDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::OutputSetupDialog)
{
	m_ui->setupUi(this);
	setupOutputList();
	setupScreenList();
}

void OutputSetupDialog::setupScreenList()
{
	QDesktopWidget *d = QApplication::desktop();

	QTableWidget * tbl = m_ui->screenListView;
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
}

void OutputSetupDialog::setupOutputList()
{
	QList<Output*> outputs = AppSettings::outputs();

	QTableWidget * tbl = m_ui->outputListView;

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
				QString("%1 x %2 at (%2,%d)").arg(r.width()).arg(r.height()).arg(r.x()).arg(r.y()) :
			    out->outputType() == Output::Network ?
				QString("Network") :
			    "Unknown");
		tbl->setItem(row,2,t);

		row++;
	}
}

void OutputSetupDialog::accepted()
{
	deleteLater();
}

void OutputSetupDialog::rejected()
{
	deleteLater();
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
