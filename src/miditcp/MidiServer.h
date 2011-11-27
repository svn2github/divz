#ifndef MidiServer_H
#define MidiServer_H

#include <QTcpServer>

class MidiServer : public QTcpServer
{
	Q_OBJECT

public:
	MidiServer(QObject *parent = 0);
	
	void start(int port=3729);
	
signals:
	void midiFrameReady(int a, int b, int c);
	
public slots:
	void sendMidiFrame(int a, int b, int c);

protected:
	void incomingConnection(int socketDescriptor);

private:
	
};


#endif
