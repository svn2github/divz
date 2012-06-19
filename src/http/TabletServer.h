#ifndef TabletServer_H

#include "HttpServer.h"
class MainWindow;

class TabletServer : public HttpServer
{
	Q_OBJECT
public:
	TabletServer(quint16 port, QObject* parent = 0);
	
protected:
	void dispatch(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
	void mainScreen(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
	//void screenLoadGroup(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
	
private:
	MainWindow * mw;


};

#endif

