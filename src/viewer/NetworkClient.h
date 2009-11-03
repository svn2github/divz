#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H


class OutputInstance;

#include <QObject>
#include <QByteArray>
#include <QTcpSocket>

#include "OutputServer.h"

namespace QJson
{
	class Parser;
}
class MainWindow;

class NetworkClient : public QObject
{
	Q_OBJECT
public:
	NetworkClient(QObject *parent = 0);
	~NetworkClient();
	
	void setLogger(MainWindow*);
	void setInstance(OutputInstance*);
	bool connectTo(const QString& host, int port);
	void exit();
	QString errorString(){ return m_socket->errorString(); }

signals:
	void aspectRatioChanged(double);
	void socketDisconnected();
	void socketError(QAbstractSocket::SocketError);
	void socketConnected();
	
private slots:
	void dataReady();
	void processBlock();
	void processCommand(OutputServer::Command, QVariant, QVariant, QVariant);
	
	
	
protected:
	void cmdSetSlideGroup(const QVariant &,int);
	void cmdAddfilter(int);
	void cmdDelFilter(int);
	void cmdSetOverlaySlide(const QVariant &);
	void cmdSetLiveBackground(const QString &, bool);

private:
	void log(const QString&);
	QTcpSocket *m_socket;
	
	MainWindow * m_log;
	OutputInstance * m_inst;
	
	int m_blockSize;
	QByteArray m_dataBlock;
	QJson::Parser * m_parser;

};

#endif
