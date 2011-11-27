#include "MidiServer.h"
#include "MidiServerThread.h"
#include <QDebug>

MidiServer::MidiServer(QObject *parent)
	: QTcpServer(parent)
{

}

void MidiServer::start(int outputPort)
{
	if(listen(QHostAddress::Any,outputPort))
	{
		qDebug() << "MidiServer: Listening on port"<<outputPort;
	}
	else
	{
		qDebug() << "MidiServer: [ERROR] Unable start server on port"<<outputPort;
	}
}

void MidiServer::incomingConnection(int socketDescriptor)
{
	MidiServerThread *thread = new MidiServerThread(socketDescriptor, this);
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	thread->start();
}

void MidiServer::sendMidiFrame(int a, int b, int c)
{
	emit midiFrameReady(a, b, c); // MidiServerThread listens for this signal
}
