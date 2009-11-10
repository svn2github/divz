#include "OutputServer.h"

#include "model/SlideGroup.h"
#include "model/Slide.h"

#include <QNetworkInterface>
#include <QDomDocument>

#include "model/Document.h"
#include "MainWindow.h"

OutputServer::OutputServer(QObject *parent)
	: QTcpServer(parent)
{
	qRegisterMetaType<OutputServer::Command>("OutputServer::Command");
	qRegisterMetaType<QVariant>("QVariant");

}
	
void OutputServer::setInstance(OutputInstance * inst)
{
	m_inst = inst;
}


QString OutputServer::myAddress()
{
	QString ipAddress;
	
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// use the first non-localhost IPv4 address
	for (int i = 0; i < ipAddressesList.size(); ++i) 
	{
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
		    ipAddressesList.at(i).toIPv4Address())
			ipAddress = ipAddressesList.at(i).toString();
	}
	
	// if we did not find one, use IPv4 localhost
	if (ipAddress.isEmpty())
		ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	
	return QString("dviz://%1:%2/").arg(ipAddress).arg(serverPort());
}

void OutputServer::incomingConnection(int socketDescriptor)
{
	OutputServerThread *thread = new OutputServerThread(socketDescriptor);
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));	
	connect(this, SIGNAL(commandReady(OutputServer::Command,QVariant,QVariant,QVariant)), thread, SLOT(sendCommand(OutputServer::Command,QVariant,QVariant,QVariant)), Qt::QueuedConnection);
	
	thread->start();
	qDebug() << "OutputServer: Client Connected, Socket Descriptor:"<<socketDescriptor;
	
	//sendCommand(OutputServer::SetAspectRatio, MainWindow::mw()->currentDocument()->aspectRatio());
}

void OutputServer::sendCommand(OutputServer::Command cmd,QVariant a,QVariant b,QVariant c)
{
	emit commandReady(cmd,a,b,c);
}


/** Thread **/

OutputServerThread::OutputServerThread(int socketDescriptor, QObject *parent)
    : QThread(parent), m_socketDescriptor(socketDescriptor)
{
	m_stringy = new QJson::Serializer;
}

OutputServerThread::~OutputServerThread()
{
	m_socket->disconnectFromHost();
	//m_socket->waitForDisconnected();

	delete m_stringy;
	delete m_socket;
}

void OutputServerThread::run()
{
	m_socket = new QTcpSocket();
	
	if (!m_socket->setSocketDescriptor(m_socketDescriptor)) 
	{
		emit error(m_socket->error());
		return;
	}
	
	// enter event loop
	exec();
	
	// when imageReady() signal arrives, write data with header to socket
}

void OutputServerThread::sendCommand(OutputServer::Command cmd,QVariant v1,QVariant v2,QVariant v3)
{
	QVariantMap map;
	map["cmd"] = (int)cmd;
	if(!v1.isNull())
		map["v1"] = v1;
	if(!v2.isNull())
		map["v2"] = v2;
	if(!v3.isNull())
		map["v3"] = v3;
	sendMap(map);
}

void OutputServerThread::sendMap(const QVariantMap& map)
{
	//QByteArray json = m_stringy->serialize(map);
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);
	stream << map;
	
	
	
	//qDebug() << "OutputServerThread: Send Map:"<<map<<", JSON:"<<json;
	
	char data[256];
	sprintf(data, "%d\n", array.size());
	m_socket->write((const char*)&data,strlen((const char*)data));
	//qDebug() << "block size: "<<strlen((const char*)data)<<", data:"<<data;
	
	m_socket->write(array);
	//qDebug() << "json size: "<<json.size();
}



