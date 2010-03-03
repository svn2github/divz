#include "CameraClient.h"
#include <QVariant>
#include <QImage>
#include <QTimer>

CameraClient::CameraClient(QObject *parent) 
	: QObject(parent)
	, m_socket(0)
	, m_blockSize(0)
	, m_dataBlock("")
{
}
CameraClient::~CameraClient()
{
}
	
bool CameraClient::connectTo(const QString& host, int port)
{
	if(m_socket)
		m_socket->abort();
		
	qDebug() << "CameraClient::connectTo: host:"<<host<<", port:"<<port;
	m_socket = new QTcpSocket(this);
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(dataReady()));
	connect(m_socket, SIGNAL(disconnected()), this, SIGNAL(socketDisconnected()));
	connect(m_socket, SIGNAL(connected()), this, SIGNAL(socketConnected()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(socketError(QAbstractSocket::SocketError)));
	
	m_blockSize = 0;
	m_socket->connectToHost(host,port);
	
	return true;
}

void CameraClient::log(const QString& str)
{
	qDebug() << "CameraClient::log(): "<<str;
// 	if(m_log)
// 		m_log->log(str);
}

void CameraClient::dataReady()
{
	if (m_blockSize == 0) 
	{
		char data[256];
		int bytes = m_socket->readLine((char*)&data,256);
		
		if(bytes == -1)
			qDebug() << "CameraClient::dataReady: Could not read line from socket";
		else
			sscanf((const char*)&data,"%d",&m_blockSize);
		//qDebug() << "Read:["<<data<<"], size:"<<m_blockSize;
		log(QString("[DEBUG] CameraClient::dataReady(): blockSize: %1 (%2)").arg(m_blockSize).arg(m_socket->bytesAvailable()));
		if(m_blockSize <= 0)
			m_socket->readAll();
	}
	
	if (m_socket->bytesAvailable() < m_blockSize)
		return;
	
	m_dataBlock = m_socket->read(m_blockSize);
	m_blockSize = 0;
	
	if(m_dataBlock.size() > 0)
	{
		//qDebug() << "Data ("<<m_dataBlock.size()<<"/"<<m_blockSize<<"): "<<m_dataBlock;
		//log(QString("[DEBUG] CameraClient::dataReady(): dataBlock: \n%1").arg(QString(m_dataBlock)));

		QImage img;
		img.loadFromData(m_dataBlock);
		emit newImage(img);
	}
	
	
	if(m_socket->bytesAvailable())
	{
		QTimer::singleShot(0, this, SLOT(dataReady()));
	}
}


void CameraClient::exit()
{
	if(m_socket)
	{
		m_socket->abort();
		m_socket->disconnectFromHost();
		//m_socket->waitForDisconnected();
		delete m_socket;
		m_socket = 0;
	}
}

