#ifndef ViewServer_H

#include "HttpServer.h"
class MainWindow;

#include <QSize>
#include <QRect>
class MyGraphicsScene;

class ViewServer : public HttpServer
{
	Q_OBJECT
public:
	ViewServer(quint16 port, QObject* parent = 0);
	
protected:
	void dispatch(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query, const QHttpRequestHeader &request);
	
	void reqCheckForChange(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
	void reqSendImage(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
	void reqSendPage(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
	
private:
	MainWindow * mw;
	MyGraphicsScene * m_scene;
	QSize m_iconSize;
	QRect m_sceneRect;

};

#endif

