#include "OutputServer.h"

#include "model/SlideGroup.h"
#include "model/Slide.h"

#include <QNetworkInterface>
#include <QDomDocument>

OutputServer::OutputServer(QObject *parent)
	: QTcpServer(parent)
{
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
	
	connect(this, SIGNAL(_cmdSetSlideGroup(SlideGroup *, int)), thread, SLOT(cmdSetSlideGroup(SlideGroup *, int)), Qt::QueuedConnection);
	connect(this, SIGNAL(_cmdSetSlide(int)), thread, SLOT(cmdSetSlide(int)), Qt::QueuedConnection);
	connect(this, SIGNAL(_cmdAddFilter(int)), thread, SLOT(cmdAddFilter(int)), Qt::QueuedConnection);
	connect(this, SIGNAL(_cmdDelFilter(int)), thread, SLOT(cmdDelFilter(int)), Qt::QueuedConnection);
	connect(this, SIGNAL(_cmdFadeClear(bool)), thread, SLOT(cmdFadeClear(bool)), Qt::QueuedConnection);
	connect(this, SIGNAL(_cmdFadeBlack(bool)), thread, SLOT(cmdFadeBlack(bool)), Qt::QueuedConnection);
	connect(this, SIGNAL(_cmdSetBackroundColor(const QString&)), thread, SLOT(cmdSetBackroundColor(const QString&)), Qt::QueuedConnection);
	connect(this, SIGNAL(_cmdSetOverlaySlide(Slide*)), thread, SLOT(cmdSetOverlaySlide(Slide*)), Qt::QueuedConnection);
	connect(this, SIGNAL(_cmdSetLiveBackground(const QString&,bool)), thread, SLOT(cmdSetLiveBackground(const QString&,bool)), Qt::QueuedConnection);
	connect(this, SIGNAL(_cmdSetAutoResizeTextEnabled(bool)), thread, SLOT(cmdSetAutoResizeTextEnabled(bool)), Qt::QueuedConnection);
	connect(this, SIGNAL(_cmdSetFadeSpeed(int)), thread, SLOT(cmdSetFadeSpeed(int)), Qt::QueuedConnection);
	connect(this, SIGNAL(_cmdSetFadeQuality(int)), thread, SLOT(cmdSetFadeQuality(int)), Qt::QueuedConnection);
	connect(this, SIGNAL(_cmdSetAspectRatio(double)), thread, SLOT(cmdSetAspectRatio(double)), Qt::QueuedConnection);
	
	thread->start();
	qDebug() << "OutputServer: Client Connected, Socket Descriptor:"<<socketDescriptor;
}

void OutputServer::cmdSetSlideGroup(SlideGroup *d, int d2) { emit _cmdSetSlideGroup(d,d2); }
void OutputServer::cmdSetSlide(int d) { emit _cmdSetSlide(d); }
void OutputServer::cmdAddFilter(int d) { emit _cmdAddFilter(d); }
void OutputServer::cmdDelFilter(int d) { emit _cmdDelFilter(d); }
void OutputServer::cmdFadeClear(bool d) { emit _cmdFadeClear(d); }
void OutputServer::cmdFadeBlack(bool d) { emit _cmdFadeBlack(d); }
void OutputServer::cmdSetBackroundColor(const QString&d) { emit _cmdSetBackroundColor(d); }
void OutputServer::cmdSetOverlaySlide(Slide*d) { emit _cmdSetOverlaySlide(d); }
void OutputServer::cmdSetLiveBackground(const QString&d,bool d2) { emit _cmdSetLiveBackground(d,d2); }
void OutputServer::cmdSetAutoResizeTextEnabled(bool d) { emit _cmdSetAutoResizeTextEnabled(d); }
void OutputServer::cmdSetFadeSpeed(int d) { emit _cmdSetFadeSpeed(d); }
void OutputServer::cmdSetFadeQuality(int d) { emit _cmdSetFadeQuality(d); }
void OutputServer::cmdSetAspectRatio(double d) { emit _cmdSetAspectRatio(d); }

/** Thread **/

OutputServerThread::OutputServerThread(int socketDescriptor, QObject *parent)
    : QThread(parent), m_socketDescriptor(socketDescriptor)
{
	m_stringy = new QJson::Serializer;
}

OutputServerThread::~OutputServerThread()
{
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

void OutputServerThread::cmdSetSlideGroup(SlideGroup *group, int startSlide)
{
	QString xmlString;
	QDomDocument doc;
	QTextStream out(&xmlString);
	
	// This element contains all the others.
	QDomElement rootElement = doc.createElement("group");

	group->toXml(rootElement);
	
	// Add the root (and all the sub-nodes) to the document
	doc.appendChild(rootElement);
	
	//Add at the begining : <?xml version="1.0" ?>
	QDomNode noeud = doc.createProcessingInstruction("xml","version=\"1.0\" ");
	doc.insertBefore(noeud,doc.firstChild());
	//save in the file (4 spaces indent)
	doc.save(out, 4);
	
	sendCmd("setSlideGroup","slideGroup",xmlString,"startSlide",startSlide);
}

void OutputServerThread::cmdSetSlide(int d) { sendCmd("setSlide", "slideNum", d); }
void OutputServerThread::cmdAddFilter(int d) { sendCmd("addFilter", "filterId", d); }
void OutputServerThread::cmdDelFilter(int d) { sendCmd("delFilter", "filterId", d); }
void OutputServerThread::cmdFadeClear(bool d) { sendCmd("fadeClear", "flag", d); }
void OutputServerThread::cmdFadeBlack(bool d) { sendCmd("fadeBlack", "flag", d); }
void OutputServerThread::cmdSetBackroundColor(const QString& d) { sendCmd("setBackroundColor", "color", d); }
void OutputServerThread::cmdSetOverlaySlide(Slide* slide) 
{ 
	QString xmlString;
	QDomDocument doc;
	QTextStream out(&xmlString);
	
	// This element contains all the others.
	QDomElement rootElement = doc.createElement("slide");

	slide->toXml(rootElement);
	
	// Add the root (and all the sub-nodes) to the document
	doc.appendChild(rootElement);
	
	//Add at the begining : <?xml version="1.0" ?>
	QDomNode noeud = doc.createProcessingInstruction("xml","version=\"1.0\" ");
	doc.insertBefore(noeud,doc.firstChild());
	//save in the file (4 spaces indent)
	doc.save(out, 4);
	
	sendCmd("setOverlaySlide", "slide", xmlString); 
}
void OutputServerThread::cmdSetLiveBackground(const QString&d,bool d2) { sendCmd("setLiveBackground", "fileName", d, "wait", d2); }
void OutputServerThread::cmdSetAutoResizeTextEnabled(bool d) { sendCmd("setAutoResizeTextEnabled", "flag", d); }
void OutputServerThread::cmdSetFadeSpeed(int d) { sendCmd("setFadeSpeed", "value", d); }
void OutputServerThread::cmdSetFadeQuality(int d) { sendCmd("setFadeQuality", "value", d); }
void OutputServerThread::cmdSetAspectRatio(double d) { sendCmd("setAspectRatio", "value", d); }

void OutputServerThread::sendCmd(const QString &cmd, const QString &arg, QVariant value, const QString &arg2, QVariant value2)
{
	QVariantMap map;
	map["cmd"] = cmd;
	map[arg] = value;
	if(!arg2.isEmpty())
		map[arg2] = value2;
	sendMap(map);
}

void OutputServerThread::sendMap(const QVariantMap& map)
{
	static int frameCounter = 0;
 	frameCounter++;
 	//qDebug() << "OutputServerThread: [START] Writing Frame#:"<<frameCounter;
 	
 	bool ok;
	m_stringy->serialize(map,m_socket,&ok);
	
	//QByteArray json = m_stringy->serialize(map);
	
	qDebug() << "OutputServerThread: Send Map, ok?"<<ok; //:"<<map<<", JSON:"<<json;
	/*
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_0);
	out << (quint16)0;
	out << json;
	out.device()->seek(0);
	out << (quint16)(block.size() - sizeof(quint16));
	
	m_socket->write(block);*/
	
	
	//qDebug() << "OutputServerThread: [END] Writing Frame#:"<<frameCounter;
}



//    m_socket->write(block);
/*    m_socket->disconnectFromHost();
    m_socket->waitForDisconnected();*/
//! [4]
