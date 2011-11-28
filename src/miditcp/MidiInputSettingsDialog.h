#ifndef MIDIINPUTSETTINGS_H
#define MIDIINPUTSETTINGS_H

#include <QDialog>

#include "MidiInputAdapter.h"

namespace Ui {
    class MidiInputSettingsDialog;
};


class MidiInputSettingsDialog : public QDialog 
{
	Q_OBJECT
public:
	MidiInputSettingsDialog(MidiInputAdapter *adapter, QWidget *parent = 0);
	~MidiInputSettingsDialog();

protected:
	void changeEvent(QEvent *e);
	
	void setupUi();
	
private slots:
	void okClicked();
	void applyHost();
	void connectionStatusChanged(bool);
	void midiKeyReceived(int key, int val);
	void cellActivated(int row, int /*column */);
	void changeKeyMode(bool);
	void cellDoubleClicked(int,int);

private:
	Ui::MidiInputSettingsDialog *ui;
	MidiInputAdapter *m_adap;
	QHash<int,MidiInputAction*> m_mappings;
	QList<MidiInputAction*> m_actionsByRow;
	bool m_chooseKeyMode;
	int m_currentRow;
};

#endif // MIDIINPUTSETTINGS_H
