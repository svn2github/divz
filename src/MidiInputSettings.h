#ifndef MIDIINPUTSETTINGS_H
#define MIDIINPUTSETTINGS_H

#include <QDialog>

namespace Ui {
    class MidiInputSettings;
}

class MidiInputAdapter;

class MidiInputSettings : public QDialog 
{
	Q_OBJECT
public:
	MidiInputSettings(MidiInputAdapter *adapter, QWidget *parent = 0);
	~MidiInputSettings();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::MidiInputSettings *ui;
	MidiInputAdapter *m_adapter;
};

#endif // MIDIINPUTSETTINGS_H
