#include <QtGui>
#include <QtNetwork>

#include "MidiReceiver.h"
#include "MidiServerThread.h"

MidiReceiver::MidiReceiver(bool verbose, QObject *parent)
	: QObject(parent)
	, m_socket(0)
	, m_connected(false)
	, m_host("")
	, m_port(0)
	, m_verbose(verbose)
	, m_autoReconnect(true)
{
}

MidiReceiver::~MidiReceiver()
{
	if(m_socket)
		exit();
		
	//quit();
	//wait();
}



bool MidiReceiver::start(const QString& host, int port)
{
	QString ipAddress = host;
	
	// find out which IP to connect to
	if(ipAddress.isEmpty())
	{
		QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
		
		// use the first non-localhost IPv4 address
		for (int i=0; i < ipAddressesList.size(); ++i) 
		{
			if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
				ipAddressesList.at(i).toIPv4Address()) 
			{
				ipAddress = ipAddressesList.at(i).toString();
				break;
			}
		}
	}
	
	// if we did not find one, use IPv4 localhost
	if (ipAddress.isEmpty())
		ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	
	m_host = ipAddress;
	m_port = port;

	if(m_socket)
	{
		disconnect(m_socket, 0, this, 0);
		m_dataBlock.clear();
		
		m_socket->abort();
		m_socket->disconnectFromHost();
		//m_socket->waitForDisconnected();
		m_socket->deleteLater();
// 		delete m_socket;
		m_socket = 0;
	}
	
	m_socket = new QTcpSocket(this);
	connect(m_socket, SIGNAL(readyRead()),    this,   SLOT(dataReady()));
	connect(m_socket, SIGNAL(disconnected()), this,   SLOT(lostConnection()));
	connect(m_socket, SIGNAL(disconnected()), this, SIGNAL(socketDisconnected()));
	connect(m_socket, SIGNAL(connected()),    this, SIGNAL(socketConnected()));
	connect(m_socket, SIGNAL(connected()),    this,   SLOT(connectionReady()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(socketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(lostConnection(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(emitError(QAbstractSocket::SocketError)));
	
	m_socket->connectToHost(ipAddress, port);
	
	return true;
}


void MidiReceiver::connectionReady()
{
	//qDebug() << "Connected";
	m_connected = true;
	
	emit connected();
	emit connectionStatusChanged(m_connected);
}


void MidiReceiver::lostConnection()
{
	m_connected = false;
	emit connectionStatusChanged(false);
	
	if(m_autoReconnect)
	{
		if(m_verbose)
			qDebug() << "MidiReceiver::lostSonnection: Lost server, attempting to reconnect in 1sec";
		QTimer::singleShot(1000,this,SLOT(reconnect()));
	}
	else
	{
		exit();
	}
}

void MidiReceiver::lostConnection(QAbstractSocket::SocketError error)
{
	//qDebug() << "MidiReceiver::lostConnection("<<error<<"):" << m_socket->errorString();
	
	if(error == QAbstractSocket::ConnectionRefusedError)
		lostConnection();
}


void MidiReceiver::reconnect()
{
	//log(QString("Attempting to reconnect to %1:%2%3").arg(m_host).arg(m_port).arg(m_url));
	if(m_port > 0)
		start(m_host,m_port);
}


void MidiReceiver::dataReady()
{
	if(!m_connected)
	{
		m_dataBlock.clear();
		return;
	}
	QByteArray bytes = m_socket->readAll();
	//qDebug() << "MidiReceiver::dataReady(): Reading from socket:"<<m_socket<<", read:"<<bytes.size()<<" bytes"; 
	if(bytes.size() > 0)
	{
		m_dataBlock.append(bytes);
//		qDebug() << "dataReady(), read bytes:"<<bytes.count()<<", buffer size:"<<m_dataBlock.size();
		
		processBlock();
	}
}

void MidiReceiver::processBlock()
{
	if(!m_connected)
		return;
		
	while(m_dataBlock.size() >= FRAME_SIZE)
	{
		QByteArray header = m_dataBlock.left(FRAME_SIZE);
		m_dataBlock.remove(0,FRAME_SIZE);
		
		const char *headerData = header.constData();
		int a, b, c;
		sscanf(headerData,"%d %d %d\n",&a, &b, &c);
		
		if(m_verbose)
			printf("MidiReceiver: Received MIDI packet: %d %d %d\n",a,b,c);
		
		emit midiFrameReady(a,b,c);
	}
}


void MidiReceiver::exit()
{
	if(m_socket)
	{
		if(m_verbose)
			qDebug() << "MidiReceiver::exit: Discarding old socket:"<<m_socket;
		disconnect(m_socket,0,this,0);
		m_dataBlock.clear();
		
		//qDebug() << "MidiReceiver::exit: Quiting video receivier";
		m_socket->abort();
		m_socket->disconnectFromHost();
		//m_socket->waitForDisconnected();
		m_socket->deleteLater();
		//delete m_socket;
		m_socket = 0;
	}
}


void MidiReceiver::emitError(QAbstractSocket::SocketError socketError)
{
	switch (socketError) 
	{
		case QAbstractSocket::RemoteHostClosedError:
			break;
		case QAbstractSocket::HostNotFoundError:
			emit error(tr("The host was not found. Please check the "
						"host name and port settings."));
			break;
		case QAbstractSocket::ConnectionRefusedError:
			emit error(tr("The connection was refused by the peer. "
						"Make sure the server is running, "
						"and check that the host name and port "
						"settings are correct."));
			break;
		default:
			emit error(tr("The following error occurred: %1.")
						.arg(m_socket->errorString()));
	}
	
}
