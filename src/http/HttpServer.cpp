#include "HttpServer.h"

#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QUrl>

#define logMessage(a) qDebug() << "[INFO]"<< QDateTime::currentDateTime().toString() << a;

// HttpServer is the the class that implements the simple HTTP server.
HttpServer::HttpServer(quint16 port, QObject* parent)
	: QTcpServer(parent)
	, m_disabled(false)
{
	listen(QHostAddress::Any, port);
	
	//registerFor("/hello", this, "sampleResponseSlot(HttpServer*,QTcpSocket*,QStringList)");
}
	
void HttpServer::incomingConnection(int socket)
{
	if (m_disabled)
		return;

	// When a new client connects, the server constructs a QTcpSocket and all
	// communication with the client is done over this QTcpSocket. QTcpSocket
	// works asynchronously, this means that all the communication is done
	// in the two slots readClient() and discardClient().
	QTcpSocket* s = new QTcpSocket(this);
	connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
	connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
	s->setSocketDescriptor(socket);

	//logMessage("New Connection");
}

void HttpServer::pause()
{
	m_disabled = true;
}

void HttpServer::resume()
{
	m_disabled = false;
}
/*
void HttpServer::registerFor(const QString& path, QObject *receiver, const char * method)
{
	QByteArray norm = QMetaObject::normalizedSignature(method);
	int idx = receiver->metaObject()->indexOfMethod(norm);
	if(idx < 0)
	{
		qDebug() << "Method "<<norm<<" ("<<method<<") does not exist on that object";
		return;
	}
	idx = receiver->metaObject()->indexOfSlot(norm);
	if(idx < 0)
	{
		qDebug() << "Method "<<norm<<" ("<<method<<") exists but IS NOT a slot!";
	}
	qDebug() << "Congrats! Method "<<norm<<" ("<<method<<") exists at idx "<<idx<<" and is a slot.";
	
	m_receivers[path] = QObjectMethodPair(receiver,method);
}*/
	
typedef QPair<QByteArray, QByteArray> ByteArrayPair;

void HttpServer::readClient()
{
	if (m_disabled)
		return;

	// This slot is called when the client sent data to the server. The
	// server looks if it was a get request and sends a very simple HTML
	// document back.
	QTcpSocket* socket = (QTcpSocket*)sender();
	if (socket->canReadLine()) 
	{
		QString line = socket->readLine();
		QStringList tokens = QString(line).split(QRegExp("[ \r\n][ \r\n]*"));
		logMessage(tokens);
		// sample list: ("GET", "/link?test=time", "HTTP/1.1", "")
		
		
		if (tokens[0] == "GET") 
		{
			QUrl req(tokens[1]);
			
			QString path = QUrl::fromPercentEncoding(req.encodedPath());
			QStringList pathElements = path.split('/');
			if(!pathElements.isEmpty() && pathElements.at(0).trimmed().isEmpty())
				pathElements.takeFirst(); // remove the first empty element
			//logMessage(pathElements);
			
			QList<QPair<QByteArray, QByteArray> > encodedQuery = req.encodedQueryItems();
			
			QStringMap map;
			foreach(ByteArrayPair bytePair, encodedQuery)
				map[QUrl::fromPercentEncoding(bytePair.first)] = QUrl::fromPercentEncoding(bytePair.second);
			
			//logMessage(map);
			
			dispatch(socket,pathElements,map);
		}
		else
		{
			//os << "HTTP/1.0 404 Not Found\r\n";
			respond(socket,QString("HTTP/1.0 404 Not Found"));
		}
		
			
		socket->close();
	
		if (socket->state() == QTcpSocket::UnconnectedState) 
		{
			delete socket;
			logMessage("Connection closed");
		}
		
		
	}
}

void HttpServer::respond(QTcpSocket *socket, const QHttpResponseHeader &tmp)
{
	QTextStream os(socket);
	os.setAutoDetectUnicode(true);
	
	QHttpResponseHeader header = tmp;
	if(!header.hasKey("content-type"))
		header.setValue("content-type", "text/html; charset=\"utf-8\"");
	
	os << header.toString();
	//os << "\r\n";
	os.flush();
	
}

void HttpServer::respond(QTcpSocket *socket, const QHttpResponseHeader &tmp,const QByteArray &data)
{
	respond(socket,tmp);
	
	socket->write(data);
}

QString HttpServer::toPathString(const QStringList &pathElements, const QStringMap &query, bool encoded)
{
	QStringList list;
	foreach(QString element, pathElements)
		list << "/" << QUrl::toPercentEncoding(element);
	if(!query.isEmpty())
	{
		list << "?";
		foreach(QString key, query.keys())
		{
			if(encoded)
			{
				list << QUrl::toPercentEncoding(key);
				list << "=";
				list << QUrl::toPercentEncoding(query.value(key));
			}
			else
			{
				list << key << "=" << query.value(key);
			}
		}
	}
	return list.join("");
}

void HttpServer::generic404(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query)
{
	respond(socket,QString("HTTP/1.0 404 Not Found"));
	QTextStream os(socket);
	os.setAutoDetectUnicode(true);
	
	os << "<h1>File Not Found</h1>\n"
	   << "Sorry, <code>"<<toPathString(pathElements,query)<<"</code> was not found.";
}

void HttpServer::dispatch(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query)
{
// 	bool consumed = false;	
// 	for(int endElement = pathElements.size();
// 		endElement > -1;
// 		endElement --)
// 	{
// 		QStringList subList = pathElements.mid(0,endElement);
// 		
// 		QString path = QString("/%1")
// 			.arg(subList.join("/"));
// 		logMessage(QString("Position %2: Checking path '%1'...").arg(path).arg(endElement));
// 		if(m_receivers.contains(path))
// 		{
// 			QObjectMethodPair slot = m_receivers[path];
// 			
// 			logMessage(QString("Path '%1' consumed by method '%2'").arg(path,slot.second));
// 			
// 			if(!QMetaObject::invokeMethod(slot.first, slot.second, 
// 				Q_ARG(HttpServer*,this),
// 				Q_ARG(QTcpSocket*,socket),
// 				Q_ARG(QStringList,pathElements)
// // 				Q_ARG(QStringMap,query)
// 			))
// 			{
// 				qDebug() << this << "Unable to invoke handler method:" << slot.first << slot.second;
// 			}
// 			
// 			
// 			consumed = true;
// 			break;
// 		}
// 	}
	
// 	if(!consumed)
// 	{
		generic404(socket,pathElements,query);
		
// 		logMessage(QString("Query string not consumed, giving default response."));
// 		
// 		
// 		respond(socket,QString("HTTP/1.0 200 Ok"));
// 		
// 		QTextStream os(socket);
// 		os.setAutoDetectUnicode(true);
// 		
// 		os <<	"<h1>Nothing to see here</h1>\n"
// 			<< QDateTime::currentDateTime().toString() << "\n"
// 			<< "<a href='/link?time=" << QDateTime::currentDateTime().toString() << "'>Click here to reload</a>";
//	}
		
}

void HttpServer::discardClient()
{
	QTcpSocket* socket = (QTcpSocket*)sender();
	socket->deleteLater();

	//logMessage("Connection closed");
}

void HttpServer::sampleResponseSlot(HttpServer *server, QTcpSocket *socket, QStringList path/*, QStringMap query*/)
{
	qDebug() << "In sampleResponseSlot";
	respond(socket,QString("HTTP/1.0 200 Ok"));
	
	QTextStream os(socket);
	os.setAutoDetectUnicode(true);
	
	os <<	"<h1>Hello World!</h1>\n"
		<< "What wonderful things God has wrought!<br>\n"
		<< "<a href='/link?time=" << QDateTime::currentDateTime().toString() << "'>Click here to reload</a>";
}
