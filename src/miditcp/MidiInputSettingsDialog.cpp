#include "MidiInputSettingsDialog.h"
#include "ui_MidiInputSettingsDialog.h"

#include "MidiInputAdapter.h"

MidiInputSettingsDialog::MidiInputSettingsDialog(MidiInputAdapter *adapter, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MidiInputSettingsDialog)
    , m_adap(adapter)
    , m_chooseKeyMode(false)
    , m_currentRow(-1)
{
	ui->setupUi(this);
	
	// Turn adapter config mode on
	// Read current host/port from input adapter
	// Update UI with host/port
	// Connect signal to adapter to read receiver status and update status lable
	// Connect slot to adapter when connect pushed to apply new host/port and reconnect
	
	// Read available actions from input adapter
	// Foreach action:
	//	- Create table row
	//	- set action name
	//	- set action type (fader/button)
	//	- query adapter for current midi key number
	//	- connect button to change midi key (change bg to yellow, change btn text to "Save")
	// Connect signal from adapter midiKeyReceved() to populate "Current value" column when value changes
	
	// When ok pressed
	//	create new QMap<midi key, action pointer>
	//	set mappings on adapter
	// 	then turn config mode off
	
	//m_adap->setConfigMode(true);
	
	connect(m_adap, SIGNAL(connectionStatusChanged(bool)), this, SLOT(connectionStatusChanged(bool)));
	connect(m_adap, SIGNAL(midiKeyReceived(int, int)), this, SLOT(midiKeyReceived(int, int)));
	
	setupUi();
}

MidiInputSettingsDialog::~MidiInputSettingsDialog()
{
	delete ui;
}

void MidiInputSettingsDialog::setupUi()
{
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	//ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	
	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(okClicked()));
	
	connectionStatusChanged( m_adap->isConnected() );
	
	// Prime our hash with mappings from the adapter
	m_mappings = m_adap->mappings();
	
	
	QList<MidiInputAction*> actions = m_adap->availableActions();
	ui->tableWidget->setRowCount(actions.size());
	
	int row = 0;
	int col = 0;
	
// 	QTableWidgetItem *prototype = new QTableWidgetItem();
// 	// setup your prototype
// 	prototype->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
	
	foreach(MidiInputAction *action, actions)
	{
//		qDebug() << "MidiIputSettingsDialog: row:"<<row<<", action:"<<action->name();
			
// 		QTableWidgetItem *item = prototype->clone();
// 		item->setText(action->name());
		
		//ui->tableWidget->setItem(row, col++, item);
		
		// Action Name
		ui->tableWidget->setItem(row, col++, new QTableWidgetItem(action->name()));
		
		// Input type - Fader/Button
		ui->tableWidget->setItem(row, col++, new QTableWidgetItem(action->isFader() ? "Slider" : "Button"));
		
		// Current key mapped
		int key = m_adap->keyForAction(action);
		ui->tableWidget->setItem(row, col++, new QTableWidgetItem(tr("%1").arg(key)));
		
		// Current value - will be changed on input
		ui->tableWidget->setItem(row, col++, new QTableWidgetItem("-"));
		
		m_actionsByRow << action;
		row ++;
		col = 0;
	}
	
	connect(ui->tableWidget, SIGNAL(cellActivated(int,int)), this, SLOT(cellActivated(int,int)));
	connect(ui->tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(cellActivated(int,int)));
	connect(ui->tableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(cellDoubleClicked(int,int)));
	connect(ui->changeBtn, SIGNAL(toggled(bool)), this, SLOT(changeKeyMode(bool)));
	
	ui->tableWidget->resizeColumnsToContents();
	ui->tableWidget->resizeRowsToContents();
}

void MidiInputSettingsDialog::cellActivated(int row, int /*column */)
{
	//qDebug() << "MidiInputSettingsDialog::cellActivated: "<<row;
	m_currentRow = row;
	ui->changeBtn->setChecked(true);
}

void MidiInputSettingsDialog::cellDoubleClicked(int row, int /*column */)
{
	m_currentRow = row;
}


void MidiInputSettingsDialog::okClicked()
{
	m_adap->setMappings(m_mappings);
}

void MidiInputSettingsDialog::applyHost()
{
	m_adap->setHost(ui->hostEdit->text(), ui->portBox->value());
}

void MidiInputSettingsDialog::connectionStatusChanged(bool flag)
{
	ui->connectStatusLabel->setText( flag ? "<font color='green'><b>Connected</b></font>" : "<font color='red'><i>Disconnected</i></font>" );
}

void MidiInputSettingsDialog::midiKeyReceived(int key, int val)
{
	if(m_chooseKeyMode)
	{
		MidiInputAction *action = m_actionsByRow[m_currentRow];
		
		QList<int> keys = m_mappings.keys(action);
		int oldKey = keys.isEmpty() ? -1 : keys.first();
		m_mappings[oldKey] = 0;
		
		m_mappings[key] = action;
		
		QTableWidgetItem *item = ui->tableWidget->item(m_currentRow, 2); // 2 is the "current key" column
		item->setText(tr("%1").arg(key));
		
		m_chooseKeyMode = false;
		ui->changeBtn->setChecked(false);
	}
	
	
	if(m_mappings.value(key))
	{
		MidiInputAction *action = m_mappings.value(key);
		int row = m_actionsByRow.indexOf(action);
		QTableWidgetItem *item = ui->tableWidget->item(row, 3); // 3 is the "current value" column
		
		if(action->isFader())
			item->setText(tr("%1").arg(val));
		else
			item->setText(val == 0 ? "Up" : "Down" );
	}
	else
	{
		qDebug() << "MidiInputSettingsDialog::midiKeyReceived: Key "<<key<<" received, but not mapped";
	}
}

void MidiInputSettingsDialog::changeKeyMode(bool flag)
{
	if(flag && m_currentRow < 0)
	{
		QMessageBox::information(this, tr("MIDI Input Settings"), "Select a row first, please");
		ui->changeBtn->setChecked(false);
		return;
	}
	
	m_chooseKeyMode = flag;
}

void MidiInputSettingsDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) 
	{
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
	}
}
