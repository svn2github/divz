#ifndef MidiReader_H
#define MidiReader_H

#define  MIDI_DEVICE  "/dev/sequencer"

#include <QFile>
#include <QThread>
#include <QTimer>

class MidiReader : public QThread
{
	Q_OBJECT
public:
	MidiReader(const QString& device = MIDI_DEVICE, int chan=176, QObject *parent=0);
	~MidiReader();
	
	void run();
	
signals:
	void midiFrameReady(int a, int b, int c);
	
private:
	QString m_device;
	QFile m_file;
	int m_midiChan;
	unsigned char m_inpacket[4];
	unsigned char m_lastPak;
	unsigned char m_finalPak[3];
	int m_readCount;
	QTimer m_pollTimer;
	
	bool m_killed;
};

#endif
