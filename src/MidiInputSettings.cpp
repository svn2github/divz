#include "MidiInputSettings.h"
#include "ui_MidiInputSettings.h"

#include "MidiInputAdapter.h

MidiInputSettings::MidiInputSettings(MidiInputAdapter *adapter, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MidiInputSettings)
    , m_adapter(adapter)
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
}

MidiInputSettings::~MidiInputSettings()
{
	delete ui;
}

void MidiInputSettings::changeEvent(QEvent *e)
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
