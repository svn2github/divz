#include "HttpServer.h"
#include "SimpleTemplate.h"

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

#include "HttpUserUtil.h"
#include "3rdparty/md5/qtmd5.h"

#define FILE_BUFFER_SIZE 4096

#define logMessage(a) qDebug() << "[INFO]"<< qPrintable(QDateTime::currentDateTime().toString()) << a;

#define HTTP_USER_COOKIE "dvz.user"
#define HTTP_USER_COOKIE_EXPIRES 365

HttpServer::HttpServer(quint16 port, QObject* parent)
	: QTcpServer(parent)
	, m_disabled(false)
	, m_socket(0)
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
		m_socket = socket;
	
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
 		
 		// Read cookies from header
 		m_cookies.clear();
 		m_setCookies.clear();
 		
		QStringList cookies = request.value("Cookie").split(QRegExp("\\s*;\\s*"));
		foreach(QString cookieData, cookies)
		{
			QStringList data = cookieData.split("=");
			if(data.size() < 2)
				continue;
			
			QString name  = QUrl::fromPercentEncoding(data.takeFirst().toAscii()).replace("+", " ");
			QString value = QUrl::fromPercentEncoding(data.takeFirst().toAscii()).replace("+", " ");
			
			m_cookies[name] = value;
			//qDebug() << "HttpServer::readClient(): [cookies] Received: "<<name<<" = "<<value; 
		}
		
		// Attempt to find a user from the header
		loadCurrentUser();
		
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
			map[QUrl::fromPercentEncoding(bytePair.first).replace("+", " ")] = QUrl::fromPercentEncoding(bytePair.second).replace("+", " ");
		
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
					
					map[QUrl::fromPercentEncoding(keyValue[0]).replace("+", " ")] = QUrl::fromPercentEncoding(keyValue[1]).replace("+", " ");
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

void HttpServer::loginPage(QTcpSocket *socket, const QStringList &path, const QStringMap &query, QString loginUrl, QString templateFile/* = ""*/)
{
	QStringList pathCopy = path;
	//pathCopy.takeFirst(); 
	QString action = pathCopy.isEmpty() ? "" : pathCopy.takeFirst().toLower();
	
	QString ip = socket->peerAddress().toString();
	
	if(templateFile.isEmpty())
		templateFile = "data/http/login.tmpl";
		
	SimpleTemplate tmpl(templateFile);
	
	if(HttpUserUtil::instance()->currentUser())
	{
		tmpl.param("logout", true);
		setUserCookie(0);
	}
	
	if(query.contains("user") &&
	   (query.contains("pass") || query.contains("passmd5")))
	{
		//qDebug() << "HttpServer::loginPage(): query:"<<query;
		
		QString user = query.value("user");
		HttpUser *userData = HttpUserUtil::instance()->getUser(user);
		if(userData)
		{
			bool ok = false;
			QString passmd5 = query.value("passmd5");
			if(!passmd5.isEmpty())
			{
				QString correctPassEncoded = MD5::md5sum(QString("%1%2").arg(userData->pass()).arg(ip));
				if(correctPassEncoded == passmd5)
					ok = true;
			}
			else
			if(query.value("pass") == userData->pass())
			{
				ok = true;
			}
			
			if(ok)
			{
				HttpUserUtil::instance()->setCurrentUser(userData);
				setUserCookie(userData);
				
				QString urlFrom = QUrl::fromPercentEncoding(query.value("from").toAscii()).replace("+", " ");
				
				if(urlFrom.isEmpty())
					urlFrom = "/";
				
				//qDebug() << "HttpServer::loginPage(): urlFrom:"<<urlFrom;
				
				qDebug() << "HttpServer::loginPage(): Authenticated user "<<userData->user()<<", level "<<userData->level()<<", redirecting to "<<urlFrom;
				
				redirect(socket, urlFrom);
				
				return;
			}
		}
		
		tmpl.param("login-error", true);
	}

	tmpl.param("login_url", loginUrl);
	tmpl.param("urlfrom",	query.value("from"));
	tmpl.param("user",	query.value("user"));
	tmpl.param("ip",	ip);

	Http_Send_Ok(socket) << tmpl.toString();	
}

void HttpServer::redirect(QTcpSocket *socket, const QString &url, bool addExpiresHeader)
{
	QHttpResponseHeader header(QString("HTTP/1.0 302 Moved Temporarily"));
	header.setValue("Location", url);
	
	if(addExpiresHeader)
	{
		QDateTime time = QDateTime::currentDateTime().addDays(30);
		QString expires = time.toString("ddd, dd MMM yyyy hh:mm:ss 'GMT'");
		header.setValue("Expires", expires);
	}
	

	respond(socket, header);
}

void HttpServer::loadCurrentUser()
{
	QString userCookie = cookie(HTTP_USER_COOKIE);
	
	QString ip = m_socket->peerAddress().toString();
	
	HttpUserUtil::instance()->setCurrentUser(0);
	
	if(!userCookie.isEmpty())
	{
		QStringList data = userCookie.split(":");
		if(data.length() < 2)
			return;
			
		QString user        = QUrl::fromPercentEncoding(data.takeFirst().toAscii()).replace("+", " ");
		QString passEncoded = QUrl::fromPercentEncoding(data.takeFirst().toAscii()).replace("+", " ");
		
		if(user.isEmpty())
			return;
		
		HttpUser *userData = HttpUserUtil::instance()->getUser(user);
		if(!userData)
			return;
			
		QString correctPassEncoded = MD5::md5sum(QString("%1%2").arg(userData->pass()).arg(ip));
		
		if(passEncoded == correctPassEncoded)	
			HttpUserUtil::instance()->setCurrentUser(userData);
	}
}

void HttpServer::setUserCookie(HttpUser *userData)
{
	QString ip = m_socket->peerAddress().toString();
	
	if(!userData)
	{
		setCookie(HTTP_USER_COOKIE, ":", HTTP_USER_COOKIE_EXPIRES);
	}
	else
	{
		QString user = QUrl::toPercentEncoding(userData->user()).replace(" ", "+");
		
		QString correctPassEncoded = MD5::md5sum(QString("%1%2").arg(userData->pass()).arg(ip));
		QString pass = QUrl::toPercentEncoding(correctPassEncoded).replace(" ", "+");
		
		QString userCookie = QString("%1:%2").arg(user).arg(pass);
	
		setCookie(HTTP_USER_COOKIE, userCookie, HTTP_USER_COOKIE_EXPIRES);
	}
}


void HttpServer::setCookie(QString key, QString value, int expiresDays)
{
	setCookie(key, value, QDateTime::currentDateTime().addDays(expiresDays));
}

void HttpServer::setCookie(QString key, QString value, QDateTime expires, QString domain, QString path)
{
	QString encodedValue = QUrl::toPercentEncoding(value).replace(" ", "+");
	if(expires.isValid())
		encodedValue += "; Expires=" + expires.toString("ddd, dd MMM yyyy hh:mm:ss 'GMT'");
	
	if(!domain.isEmpty())
		encodedValue += "; Domain=" + domain;
	
	if(!path.isEmpty())
		encodedValue += "; Path=" + path;
	
	m_setCookies[key] = encodedValue;
	
	// Override cookie value from headers so subsequent calls to cookie() return this value instead of the one from the header
	m_cookies[key] = value;
}

void HttpServer::respond(QTcpSocket *socket, const QHttpResponseHeader &tmp)
{
	QTextStream os(socket);
	os.setAutoDetectUnicode(true);
	
	QHttpResponseHeader header = tmp;
	
	//header.setValue("Connection", "Keep-Alive");
	
	QStringList cookieHeaders;
	foreach(QString cookieName, m_setCookies.keys())
	{
		QString name = QUrl::toPercentEncoding(cookieName).replace(" ", "+");
		QString header = QString("Set-Cookie: %1=%2").arg(name).arg(m_setCookies[cookieName]);
		cookieHeaders.append(header);
		
		//qDebug() << "HttpServer::respond(): [cookies] "<<header;
	}
	
	if(!header.hasKey("content-type") &&
	   !header.hasKey("Content-Type"))
		header.setValue("Content-Type", "text/html; charset=\"utf-8\"");
	
	// We have to chop off the blank line from the toString() method so we can add on 
	// the cookie headers ourselves before sending to the client.
	// We have to add the "set-cookie" headers manually (e.g. instead of using header.setvalue())
	// because setValue() overwrites the previous value if the same key (e.g. "Set-Cookie") is used
	// again in the same header. Since we could have more tha one cookie being set in a single
	// respond() call, we have to make our own list of Set-Cookie headers and add them in manually.
	
	QString headers = header.toString();
	headers = headers.left(headers.length() - 2); // chop off the blank line at the end
	
	headers += cookieHeaders.join("\r\n");
	headers += "\r\n"; // add in blank line again
	
	//qDebug() << "HttpServer::respond(): [final headers] "<<qPrintable(headers);
	
	os << headers;
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
	QStringList buffer;
	foreach(QString element, pathElements)
		buffer << "/" << QUrl::toPercentEncoding(element).replace(" ", "+");
	if(!query.isEmpty())
	{
		buffer << "?";
		QStringList list;
		foreach(QString key, query.keys())
		{
			if(encoded)
			{
				list << QUrl::toPercentEncoding(key).replace(" ", "+")
				     << "="
				     << QUrl::toPercentEncoding(query.value(key)).replace(" ", "+");
			}
			else
			{
				list << key
				     << "="
				     << query.value(key);
			}
		}
		buffer << list.join("&");
	}
	
	return buffer.join("");
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
	generic404(socket, pathElements, query);
}

void HttpServer::discardClient()
{
	QTcpSocket* socket = (QTcpSocket*)sender();
	socket->deleteLater();
}

