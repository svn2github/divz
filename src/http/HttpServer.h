#ifndef HttpServer_H
#define HttpServer_H

#include <QTcpServer>
#include <QHttpResponseHeader>
#include <QMap>

// #ifndef QObjectMethodPair
// typedef QPair<QObject*, const char*> QObjectMethodPair;
// #endif

#ifndef QStringMap
typedef QMap<QString,QString> QStringMap;
#endif

// HttpServer is the the class that implements the simple HTTP server.
class HttpServer : public QTcpServer
{
	Q_OBJECT
public:
	HttpServer(quint16 port, QObject* parent = 0);
	
	void incomingConnection(int socket);
	void pause();
	void resume();
	
// 	// Method must have the following signature:
// 		// (HttpServer *server, QTcpSocket *socket, const QStringList pathElements, const QStringMap query);
// 	void registerFor(const QString& path, QObject *receiver, const char * method);
	
	static QString toPathString(const QStringList &pathElements, const QStringMap &query, bool encoded=true);
	
	void generic404(QTcpSocket *socket, const QStringList &path = QStringList(), const QStringMap & map = QStringMap());
	
protected:
	void respond(QTcpSocket *socket, const QHttpResponseHeader &);
	void respond(QTcpSocket *socket, const QHttpResponseHeader &,const QByteArray &);
	
	virtual void dispatch(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
		
private slots:
	void readClient();
	void discardClient();
	//, QStringMap
	void sampleResponseSlot(HttpServer *, QTcpSocket *, QStringList);

private:
	bool m_disabled;
/*	
	QMap<QString, QObjectMethodPair> m_receivers;*/
};
 
#endif
