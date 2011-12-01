#include "MidiServerThread.h"
#include <QtNetwork>

#include "MidiServer.h"

MidiServerThread::MidiServerThread(int socketDescriptor, MidiServer *parent)
	: QThread(parent)
	, m_socketDescriptor(socketDescriptor)
{
	m_socket = new QTcpSocket();
	connect(parent, SIGNAL(midiFrameReady(int, int, int)), this, SLOT(midiFrameReady(int,int,int))); 
}

void MidiServerThread::run()
{
	connect(m_socket, SIGNAL(disconnected()), this, SLOT(quit()));
	
	if(!m_socket->setSocketDescriptor(m_socketDescriptor)) 
	{
		emit error(m_socket->error());
		return;
	}
	
	qDebug() << "MidiServerThread: Connection from "<<m_socket->peerAddress().toString(); //, Socket Descriptor:"<<socketDescriptor;
	
	m_isConnected = true;
	
	// Run event loop
	exec();
}

void MidiServerThread::midiFrameReady(int a, int b, int c)
{
	char frameData[FRAME_SIZE];
	memset(&frameData, 0, FRAME_SIZE);
	sprintf((char*)&frameData,"%d %d %d\n",a,b,c);
	
	m_socket->write((const char*)&frameData,FRAME_SIZE);
}

MidiServerThread::~MidiServerThread()
{
	if(m_isConnected)
	{
		m_socket->abort();
		m_isConnected = false;
	}
	
	delete m_socket;
	m_socket = 0;
}
