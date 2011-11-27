#ifndef MidiServerThread_H
#define MidiServerThread_H

#define FRAME_SIZE 64

#include <QThread>
#include <QTcpSocket>

class MidiServer;

class MidiServerThread : public QThread
{
	Q_OBJECT

public:
	MidiServerThread(int socketDescriptor, MidiServer *parent);
	~MidiServerThread();

	void run();

private slots:
	void midiFrameReady(int a, int b, int c);
	
signals:
	void error(QTcpSocket::SocketError socketError);

private:
	QTcpSocket *m_socket;
	int m_socketDescriptor;
	bool m_isConnected;
};

#endif
