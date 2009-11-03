#include "OutputInstance.h"
#include "NetworkClient.h"

#include <QColor>
#include <QVariant>
#include <QFileInfo>
#include <QMessageBox>

#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "songdb/SongSlideGroup.h"
#include "3rdparty/qjson/parser.h"
#include "MainWindow.h"

#include "OutputServer.h" // OutputServer::Command enum is needed

NetworkClient::NetworkClient(QObject *parent) 
	: QObject(parent)
	, m_socket(0)
	, m_log(0)
	, m_inst(0)
	, m_blockSize(0)
	, m_dataBlock("")
	, m_parser(new QJson::Parser)
{
}
NetworkClient::~NetworkClient()
{
	delete m_parser;
}
	
void NetworkClient::setLogger(MainWindow *log)
{
	m_log = log;
}

void NetworkClient::setInstance(OutputInstance *inst)
{
	m_inst = inst;
}

bool NetworkClient::connectTo(const QString& host, int port)
{
	if(m_socket)
		m_socket->abort();
		
	m_socket = new QTcpSocket(this);
	connect(m_socket, SIGNAL(readyRead()), this, SLOT(dataReady()));
	connect(m_socket, SIGNAL(disconnected()), this, SIGNAL(socketDisconnected()));
	connect(m_socket, SIGNAL(connected()), this, SIGNAL(socketConnected()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(socketError(QAbstractSocket::SocketError)));
	
	m_blockSize = 0;
	m_socket->connectToHost(host,port);
	
	return true;
}

void NetworkClient::log(const QString& str)
{
	if(m_log)
		m_log->log(str);
}

void NetworkClient::dataReady()
{
	if (m_blockSize == 0) 
	{
		char data[256];
		int bytes = m_socket->readLine((char*)&data,256);
		
		if(bytes == -1)
			qDebug() << "NetworkClient::dataReady: Could not read line from socket";
		else
			sscanf((const char*)&data,"%d",&m_blockSize);
		//qDebug() << "Read:["<<data<<"], size:"<<m_blockSize;
		//log(QString("[DEBUG] NetworkClient::dataReady(): blockSize: %1 (%2)").arg(m_blockSize).arg(m_socket->bytesAvailable()));
	}
	
	if (m_socket->bytesAvailable() < m_blockSize)
		return;
	
	m_dataBlock = m_socket->read(m_blockSize);
	m_blockSize = 0;
	
	if(m_dataBlock.size() > 0)
	{
		//qDebug() << "Data ("<<m_dataBlock.size()<<"/"<<m_blockSize<<"): "<<m_dataBlock;
		//log(QString("[DEBUG] NetworkClient::dataReady(): dataBlock: \n%1").arg(QString(m_dataBlock)));

		processBlock();
	}
	
	
	if(m_socket->bytesAvailable())
	{
		QTimer::singleShot(0, this, SLOT(dataReady()));
	}
}

void NetworkClient::processBlock()
{
	bool ok;
	//QVariant result = m_parser->parse(m_socket,&ok);
	QVariant result = m_parser->parse(m_dataBlock, &ok);
	if(!ok)
	{
		log(QString("[ERROR] Error in data at %1: %2\nData: %3").arg(m_parser->errorLine()).arg(m_parser->errorString()).arg(QString(m_dataBlock)));
	}
	else
	{
		QVariantMap map = result.toMap();
		OutputServer::Command cmd = (OutputServer::Command)map["cmd"].toInt();
		//qDebug() << "NetworkClient::processBlock: cmd#:"<<cmd;
		
		processCommand(cmd,map["v1"],map["v2"],map["v3"]);
	}
}

void NetworkClient::processCommand(OutputServer::Command cmd, QVariant a, QVariant b, QVariant c)
{
	switch(cmd)
	{
		case OutputServer::SetSlideGroup:
			cmdSetSlideGroup(a,b.toInt());
			break;
		case OutputServer::SetSlide:
			m_inst->setSlide(a.toInt());
			break;
		case OutputServer::AddFilter:
			cmdAddfilter(a.toInt());
			break; 
		case OutputServer::DelFilter:
			cmdDelFilter(a.toInt());
			break; 
		case OutputServer::FadeClear:
			m_inst->fadeClearFrame(a.toBool());
			break; 
		case OutputServer::FadeBlack:
			m_inst->fadeBlackFrame(a.toBool());
			break; 
		case OutputServer::SetBackgroundColor:
			m_inst->setBackground(QColor(a.toString()));
			break; 
		case OutputServer::SetOverlaySlide:
			cmdSetOverlaySlide(a);
			break; 
		case OutputServer::SetLiveBackground:
			cmdSetLiveBackground(a.toString(),b.toBool());
			break; 
		case OutputServer::SetTextResize:
			m_inst->setAutoResizeTextEnabled(a.toBool());
			break; 
		case OutputServer::SetFadeSpeed:
			m_inst->setFadeSpeed(a.toInt());
			break; 
		case OutputServer::SetFadeQuality:
			m_inst->setFadeQuality(a.toInt());
			break; 
		case OutputServer::SetAspectRatio:
			emit aspectRatioChanged(a.toDouble());
			break; 
		default:
			qDebug() << "Command Not Handled: "<<(int)cmd;
			log(QString("[DEBUG] Unknown Command: '%1'").arg(cmd));
	}; 
}

void NetworkClient::cmdSetSlideGroup(const QVariant& var, int start)
{
	const QString & data = var.toString();
	
	QString *error = new QString();
	QDomDocument doc;
	if (!doc.setContent(data, false, error)) 
	{
		log(data);
		QMessageBox::critical(0, tr("Parsing error"), tr("Unable to parse XML slide group data. The error was: %2").arg(*error));
		//throw(0);
		return;
	}
	
	delete error;
	error = 0;
	
	QDomElement element = doc.documentElement(); // The root node
	
	SlideGroup *g = 0;
		
	if (element.tagName() == "song")
	{
		qDebug("cmdSetSlideGroup: Group type: Song");
		g = new SongSlideGroup();
	}
	else
	{
		qDebug("cmdSetSlideGroup: Group type: Generic");
		g = new SlideGroup();
	}
	
	//qDebug("Document::fromXml: Converting group from xml...");
	// restore the item, and delete it if something goes wrong
	if (!g->fromXml(element)) 
	{
		qDebug("Document::fromXml: group fromXml failed, removing");
		delete g;
	}
	else
	{
	
		if(g->groupNumber()<0)
			g->setGroupNumber(1);
		
		m_inst->setSlideGroup(g,start);
	}
}

void NetworkClient::cmdAddfilter(int id)
{
	AbstractItemFilter * filter = AbstractItemFilter::filterById(id);
	if(id && filter)
		m_inst->addFilter(filter);
}

void NetworkClient::cmdDelFilter(int id)
{
	AbstractItemFilter * filter = AbstractItemFilter::filterById(id);
	if(id && filter)
		m_inst->removeFilter(filter);
}

void NetworkClient::cmdSetOverlaySlide(const QVariant& var)
{
	const QString & data = var.toString();
	
	QString *error = new QString();
	QDomDocument doc;
	if (!doc.setContent(data, false, error)) 
	{
		log(data);
		QMessageBox::critical(0, tr("Parsing error"), tr("Unable to parse XML overlay slide. The error was: %2").arg(*error));
		//throw(0);
		return;
	}
	
	delete error;
	error = 0;
	
	QDomElement element = doc.documentElement(); // The root node
	
	Slide *s = new Slide();
	
	// restore the item, and delete it if something goes wrong
	if (!s->fromXml(element))
	{
		delete s;
	}
	else
	{
		m_inst->setOverlaySlide(s);
	}
}

void NetworkClient::cmdSetLiveBackground(const QString& file, bool flag)
{
	m_inst->setLiveBackground(QFileInfo(file),flag);
}
	

void NetworkClient::exit()
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

/*	
private:
	QTcpSocket *m_socket;
	
	QString m_lastError;
	
	SimpleLogger * m_log;
	OutputInstance * m_inst;

*/
