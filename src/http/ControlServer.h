#ifndef ControlServer_H

#include "HttpServer.h"
class MainWindow;

class ControlServer : public HttpServer
{
	Q_OBJECT
public:
	ControlServer(quint16 port, QObject* parent = 0);
	
protected:
	void dispatch(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query, const QHttpRequestHeader &request);
	void screenListGroups(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
	void screenLoadGroup(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
	
private:
	MainWindow * mw;


};

#endif

