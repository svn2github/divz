#include "TestServer.h"

#include <QTcpSocket>
#include <QTextStream>

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>

class SimpleTemplate
{
public:
	SimpleTemplate(const QString& file, bool isRawTemplateData = false);
	
	void param(const QString &param, const QString &value);
	
	QString toString();
	
private:
	QHash<QString,QString> m_params;
	
	QString m_data;
	QString m_file;
};

SimpleTemplate::SimpleTemplate(const QString& file, bool isRawTemplateData)
{
	if(isRawTemplateData)
	{
		m_data = file;
	}
	else
	{
		QFile fileHandle(file);
		if(!fileHandle.open(QIODevice::ReadOnly))
		{
			qWarning("SimpleTemplate: Unable to open '%s' for reading", qPrintable(file));
		}
		else
		{
			QTextStream os(&fileHandle);
			m_data = os.readAll();
			fileHandle.close();
		}
	}
}

void SimpleTemplate::param(const QString &param, const QString &value)
{
	m_params[param] = value;
}

QString SimpleTemplate::toString()
{
	QString data = m_data;
	foreach(QString key, m_params.keys())
		data.replace(QString("%%%1%%").arg(key), m_params.value(key));
	return data;
}



TestServer::TestServer(quint16 port, QObject* parent)
	: HttpServer(port,parent)
{}
	
void TestServer::dispatch(QTcpSocket *socket, const QStringList &path, const QStringMap &query)
{
	
	//generic404(socket,path,query);
	QString pathStr = path.join("/");
	//qDebug() << "pathStr: "<<pathStr;
	
	if(pathStr.startsWith("test.html"))
	{
		SimpleTemplate tmpl("test.tmpl");
		tmpl.param("time",QDateTime::currentDateTime().toString());
		
		respond(socket,QString("HTTP/1.0 200 OK"));
		QTextStream os(socket);
		os.setAutoDetectUnicode(true);
		
		os << tmpl.toString();
	}
	else
	if(pathStr.startsWith("data/"))
	{
		
		
		if(!pathStr.contains(".."))
		{
			QFileInfo fileInfo(pathStr);
			QString abs = fileInfo.canonicalFilePath();
			//QFile file(QDir::currentPath() + "/" + abs);
			QFile file(abs);
			if(!file.open(QIODevice::ReadOnly))
			{
				respond(socket,QString("HTTP/1.0 500 Unable to Open Resource"));
				QTextStream os(socket);
				os.setAutoDetectUnicode(true);
				
				os << "<h1>Unable to Open Resource</h1>\n"
				<< "Unable to open resource <code>" << abs <<"</code>";
				return;
			}
			
			qDebug() << "Serving resource: "<<abs;
			
			QString ext = fileInfo.suffix().toLower();
			QString contentType =	ext == "png"  ? "image/png" : 
						ext == "jpg"  ? "image/jpg" :
						ext == "jpeg" ? "image/jpeg" :
						ext == "gif"  ? "image/gif" :
						ext == "css"  ? "text/css" :
						ext == "html" ? "text/html" :
						ext == "js"   ? "text/javascript" :
						"application/octet-stream";
					
			QHttpResponseHeader header(QString("HTTP/1.0 200 OK"));
			header.setValue("content-type", contentType);
		
			respond(socket,header);
			
			char buffer[4096];
			while(!file.atEnd())
			{
				qint64 bytesRead = file.read(buffer,4096);
				if(bytesRead < 0)
				{
					break;
				}
				else
				{
					socket->write(buffer, bytesRead);
				}
			}
			
			file.close();
		}
		else
		{
			respond(socket,QString("HTTP/1.0 500 Invalid Resource Path"));
			QTextStream os(socket);
			os.setAutoDetectUnicode(true);
			
			os << "<h1>Invalid Resource Path</h1>\n"
			<< "Sorry, but you cannot use '..' in a resource path.";
			return;
		}
	}
	else
	{
		
		respond(socket,QString("HTTP/1.0 404 Not Found"));
		QTextStream os(socket);
		os.setAutoDetectUnicode(true);
		
		os << "<h1>Oops!</h1>\n"
		<< "Hey, my bad! I can't find \"<code>"<<toPathString(path,query)<<"</code>\"! Sorry!";
	}
	
}
