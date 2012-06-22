#include "HttpServer.h"

#include <QDateTime>
#include <QTcpServer>
#include <QTcpSocket>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#define FILE_BUFFER_SIZE 4096

#define logMessage(a) qDebug() << "[INFO]"<< qPrintable(QDateTime::currentDateTime().toString()) << a;

HttpServer::HttpServer(quint16 port, QObject* parent)
	: QTcpServer(parent)
	, m_disabled(false)
{
	listen(QHostAddress::Any, port);
	
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
		logMessage(qPrintable(socket->peerAddress().toString()) << qPrintable(tokens.join(" ")));
		// sample list: ("GET", "/link?test=time", "HTTP/1.1", "")
		
		// Read in HTTP headers
 		QStringList headerBuffer;
 		headerBuffer.append(line); // add first line
 		while(socket->canReadLine())
 		{
 			QString line = socket->readLine();
 			//qDebug() << "HttpServer::readClient(): " << qPrintable(line);
 			if(line.isEmpty())
 				break;
 			headerBuffer.append(line);
 		}
 		
 		QHttpRequestHeader request(headerBuffer.join(""));
		
 		// Decode request path
 		
 		//QUrl req(tokens[1]);
		QUrl req(request.path());
		
		QString path = QUrl::fromPercentEncoding(req.encodedPath());
		QStringList pathElements = path.split('/');
		if(!pathElements.isEmpty() && pathElements.at(0).trimmed().isEmpty())
			pathElements.takeFirst(); // remove the first empty element
		//logMessage(pathElements);
		
		QList<QPair<QByteArray, QByteArray> > encodedQuery = req.encodedQueryItems();
		
		QStringMap map;
		foreach(QByteArrayPair bytePair, encodedQuery)
			map[QUrl::fromPercentEncoding(bytePair.first).replace("+"," ")] = QUrl::fromPercentEncoding(bytePair.second).replace("+"," ");
		
		//if (tokens[0] == "GET") 
		if (request.method() == "GET")
		{
			//logMessage(map);
			dispatch(socket, pathElements, map);
		}
		else
		if (request.method() == "POST")
		{
			QByteArray postData = socket->readAll();
			
			int contentLength = request.value("Content-Length").toInt();
			while(postData.size() < contentLength)
			{
				// Dangerous, I know...could block....
				socket->waitForReadyRead();
				QByteArray tmp = socket->readAll();
				postData.append(tmp);
			}
			
			//qDebug() << "HttpServer::readClient(): Final postData:" << postData;
			
			if(request.value("Content-Type").indexOf("application/x-www-form-urlencoded") > -1)
			{
				QList<QByteArray> pairs = postData.split('&');
				foreach(QByteArray pair, pairs)
				{
					QList<QByteArray> keyValue = pair.split('=');
					if(keyValue.size() < 2)
						continue;
					
					map[QUrl::fromPercentEncoding(keyValue[0]).replace("+"," ")] = QUrl::fromPercentEncoding(keyValue[1]).replace("+"," ");
				}
				
				//qDebug() << "Debug: Decoded POST data: "<<map<<" from "<<QString(postData);
				
				dispatch(socket, pathElements, map);
			}
			else
			{
				respond(socket,QString("HTTP/1.0 500 Content-Type for POST must be application/x-www-form-urlencoded"));
			}
		}
		else
		{
			respond(socket,QString("HTTP/1.0 500 Method not used"));
		}
			
		bool closeSocket = true;
		
		/*
		if(headers.hasKey("Connection") &&
		   headers.value ("Connection") == "Keep-Alive")
		{
			qDebug() << "HttpServer::readClient(): Keep-Alive!";
			closeSocket = false;
		}
		*/
		
		if(closeSocket)
			socket->close();
		
		if (socket->state() == QTcpSocket::UnconnectedState) 
		{
			if(!closeSocket)
				socket->close();
				
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
		
	header.setValue("Connection", "Keep-Alive");
	
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
				list << QUrl::toPercentEncoding(key)
				     << "="
				     << QUrl::toPercentEncoding(query.value(key));
			}
			else
			{
				list << key
				     << "="
				     << query.value(key);
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

bool HttpServer::serveFile(QTcpSocket *socket, const QString &pathStr, bool addExpiresHeader)
{
	if(!pathStr.contains(".."))
	{
		QFileInfo fileInfo(pathStr);
		
		// Workaround to allow serving resources by assuming the Qt resources start with ":/"
		QString abs = pathStr.startsWith(":/") ? pathStr : fileInfo.canonicalFilePath();
		
		// Attempt to serve a gzipped version of the file if it exists
// 		QString gzipFile = tr("%1.gz").arg(abs);
// 		bool gzipEncoding = false;
// 		if(QFile::exists(gzipFile))
// 		{
// 			gzipEncoding = true;
// 			abs = gzipFile;
// 		}
			
		QFile file(abs);
		if(!file.open(QIODevice::ReadOnly))
		{
			respond(socket,QString("HTTP/1.0 500 Unable to Open Resource"));
			QTextStream os(socket);
			os.setAutoDetectUnicode(true);
			
			os << "<h1>Unable to Open Resource</h1>\n"
			   << "Unable to open resource <code>" << abs <<"</code>";
			return false;
		}
		
		QString ext = fileInfo.suffix().toLower();
		QString contentType =	ext == "png"  ? "image/png" : 
					ext == "jpg"  ? "image/jpg" :
					ext == "jpeg" ? "image/jpeg" :
					ext == "gif"  ? "image/gif" :
					ext == "css"  ? "text/css" :
					ext == "html" ? "text/html" :
					ext == "js"   ? "text/javascript" :
					ext == "gz"   ? "application/x-gzip" :
					ext == "ico"  ? "image/vnd.microsoft.icon" :
					ext == "manifest" ? "text/cache-manifest"  : /* to support Safari's manifest files */
					"application/octet-stream";
				
		logMessage(QString("[FILE] OK (%2) %1").arg(abs).arg(contentType));
		
		QHttpResponseHeader header(QString("HTTP/1.0 200 OK"));
		header.setValue("Content-Type", contentType);
		
		if(addExpiresHeader)
		{
			QDateTime time = QDateTime::currentDateTime().addDays(30);
			QString expires = time.toString("ddd, dd MMM yyyy hh:mm:ss 'GMT'");
			header.setValue("Expires", expires);
		}
		
// 		if(gzipEncoding)
// 		{
// 			header.setValue("Vary", "Accept-Encoding");
// 			header.setValue("Content-Encoding", "gzip");
// 		}
	
		respond(socket,header);
		
		char buffer[FILE_BUFFER_SIZE];
		while(!file.atEnd())
		{
			qint64 bytesRead = file.read(buffer,FILE_BUFFER_SIZE);
			if(bytesRead < 0)
			{
				file.close();
				return false;
			}
			else
			{
				socket->write(buffer, bytesRead);
			}
		}
		
		file.close();
		return true;
	}
	else
	{
		respond(socket,QString("HTTP/1.0 500 Invalid Resource Path"));
		QTextStream os(socket);
		os.setAutoDetectUnicode(true);
		
		os << "<h1>Invalid Resource Path</h1>\n"
		<< "Sorry, but you cannot use '..' in a resource path.";
		return false;
	}
}

void HttpServer::dispatch(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query)
{
	generic404(socket,pathElements,query);
}

void HttpServer::discardClient()
{
	QTcpSocket* socket = (QTcpSocket*)sender();
	socket->deleteLater();
}
