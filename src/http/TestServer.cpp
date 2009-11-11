#include "TestServer.h"
#include "SimpleTemplate.h"

#include <QTcpSocket>
#include <QTextStream>

#include <QDateTime>
#include <QFile>
#include <QFileInfo>

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
		
		Http_Send_Ok(socket) 
			<< tmpl.toString();
	}
	else
	if(pathStr.startsWith("data/"))
	{
		serveFile(socket,pathStr);
	}
	else
	{
		Http_Send_404(socket) 
			<< "<h1>Oops!</h1>\n"
			<< "Hey, my bad! I can't find \"<code>"<<toPathString(path,query)<<"</code>\"! Sorry!";
	}
	
}
