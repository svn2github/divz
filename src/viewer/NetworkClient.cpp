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

NetworkClient::NetworkClient(QObject *parent) 
	: QObject(parent)
	, m_socket(0)
	, m_lastError("")
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
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	
	log(QString("[INFO] NetworkClient: Created socket, connecting to %1:%2 ...").arg(host).arg(port));
	
	m_blockSize = 0;
	m_socket->connectToHost(host,port);
}

void NetworkClient::log(const QString& str)
{
	if(m_log)
		m_log->log(str);
}

void NetworkClient::dataReady()
{/*
	QDataStream in(m_socket);
	in.setVersion(QDataStream::Qt_4_0);*/
	/*
	if (m_blockSize == 0) 
	{
		if (m_socket->bytesAvailable() < (int)sizeof(quint16))
			return;
		in >> m_blockSize;
		log(QString("[DEBUG] NetworkClient::dataReady(): blockSize: %1").arg(m_blockSize));
	}
	
	if (m_socket->bytesAvailable() < m_blockSize)
		return;
	
	m_dataBlock.clear();
	in >> m_dataBlock;
	
	log(QString("[DEBUG] NetworkClient::dataReady(): dataBlock: %1").arg(QString(m_dataBlock)));*/

	processBlock();
}

void NetworkClient::processBlock()
{
	bool ok;
	QVariant result = m_parser->parse(m_socket,&ok);
	//m_parser->parse(m_dataBlock, &ok);
	if(!ok)
	{
		m_lastError = QString("Error in data at %1: %2\nData: %3").arg(m_parser->errorLine()).arg(m_parser->errorString()).arg(QString(m_dataBlock));
		//log(QString("[ERROR] %1").arg(m_lastError));
		emit error(m_lastError);
	}
	else
	{
		QVariantMap map = result.toMap();
		QString cmd = map["cmd"].toString();
		qDebug() << "NetworkClient::processBlock: cmd:"<<cmd;
		
		// valid commands:
		//	setSlideGroup
		//		args: slideGroup - XML-encoded group
		//		      startSlide - integer slide nbr
		//	setSlide
		//		args: slideNum - integer slide number
		//	addFilter
		//		args: filterId - integer filter id
		//	delFilter
		//		args: filterId - integer filter id
		//	fadeBlack
		//		args: flag - boolean flag
		// 	fadeClear
		//		args: flag - boolean flag
		//	setBackgroundColor
		//		args: color - QString color description
		//	setOverlaySlide
		//		args: slide - XML encoded slide
		//	setLiveBackground
		//		args: fileName - QString
		//		      wait - boolean
		//	setAutoResizeTextEnabled
		//		args: flag - boolean 
		//	setFadeSpeed
		//		args: value - integer
		//	setFadeQuality
		//		args: value - integer
// // // // // // 		//	slideChanged
// // // // // // 		//		slide - XML encoded version of the new slide
		// 	setAspectRatio
		//		value - double ar
		
		if(cmd == "setSlideGroup")
			cmdSetSlideGroup(map["slideGroup"],map["startSlide"].toInt());
		else 
		if(cmd == "setSlide")
			m_inst->setSlide(map["slideNum"].toInt());
		else 
		if(cmd == "addFilter")
			cmdAddfilter(map["filterId"].toInt());
		else 
		if(cmd == "delFilter")
			cmdDelFilter(map["filterId"].toInt());
		else 
		if(cmd == "fadeClear")
			m_inst->fadeClearFrame(map["flag"].toBool());
		else 
		if(cmd == "fadeBlack")
			m_inst->fadeBlackFrame(map["flag"].toBool());
		else 
		if(cmd == "setBackgroundColor")
			m_inst->setBackground(QColor(map["color"].toString()));
		else 
		if(cmd == "setOverlaySlide")
			cmdSetOverlaySlide(map["slide"]);
		else 
		if(cmd == "setLiveBackground")
			cmdSetLiveBackground(map["fileName"].toString(),map["wait"].toBool());
		else 
		if(cmd == "setAutoResizeTextEnabled")
			m_inst->setAutoResizeTextEnabled(map["flag"].toBool());
		else 
		if(cmd == "setFadeSpeed")
			m_inst->setFadeSpeed(map["value"].toInt());
		else 
		if(cmd == "setFadeQuality")
			m_inst->setFadeQuality(map["value"].toInt());
		else 
		if(cmd == "setAspectRatio")
			emit aspectRatioChanged(map["value"].toDouble());
		else
			log(QString("[DEBUG] Unknown Command: '%1'").arg(cmd));
	}
}

void NetworkClient::cmdSetSlideGroup(const QVariant& var, int start)
{
	const QString & data = var.toString();
	
	QString *error = new QString();
	QDomDocument doc;
	if (!doc.setContent(data, false, error)) 
	{
		QMessageBox::critical(0, tr("Parsing error"), tr("Unable to parse file data. The error was: %2").arg(*error));
		throw(0);
		return;
	}
	
	delete error;
	error = 0;
	
	QDomElement element = doc.documentElement(); // The root node
	
	SlideGroup *g = 0;
		
	if (element.tagName() == "song")
	{
		//qDebug("Document::fromXml: Group type: Song");
		g = new SongSlideGroup();
	}
	else
	{
		//qDebug("Document::fromXml: Group type: Generic");
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
	// TODO
}

void NetworkClient::cmdDelFilter(int id)
{
	// TODO
}

void NetworkClient::cmdSetOverlaySlide(const QVariant& var)
{
	const QString & data = var.toString();
	
	QString *error = new QString();
	QDomDocument doc;
	if (!doc.setContent(data, false, error)) 
	{
		QMessageBox::critical(0, tr("Parsing error"), tr("Unable to parse file data. The error was: %2").arg(*error));
		throw(0);
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
		delete m_socket;
		m_socket = 0;
	}
}

void NetworkClient::socketError(QAbstractSocket::SocketError socketError)
{
	m_lastError = "";
	switch (socketError) 
	{
		case QAbstractSocket::RemoteHostClosedError:
			break;
		case QAbstractSocket::HostNotFoundError:
			m_lastError = tr("The host was not found. Please check the host name and port settings.");
			break;
		case QAbstractSocket::ConnectionRefusedError:
			m_lastError = tr("The connection was refused by the peer. "
					"Make sure the DViz server is running, "
					"and check that the host name and port "
					"settings are correct.");
			break;
		default:
			m_lastError = tr("The following error occurred: %1.").arg(m_socket->errorString());
	}
	if(!m_lastError.isEmpty())
		emit error(m_lastError);
}
/*	
private:
	QTcpSocket *m_socket;
	
	QString m_lastError;
	
	SimpleLogger * m_log;
	OutputInstance * m_inst;

*/
