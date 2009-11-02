#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H


class OutputInstance;

#include <QObject>
#include <QByteArray>
#include <QTcpSocket>

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
	QString errorString() { return m_lastError; }
	void exit();

signals:
	void error(const QString&msg);
	void aspectRatioChanged(double);
	
private slots:
	
	void socketError(QAbstractSocket::SocketError socketError);
	
	void dataReady();
	void processBlock();
	
protected:
	void cmdSetSlideGroup(const QVariant &,int);
	void cmdAddfilter(int);
	void cmdDelFilter(int);
	void cmdSetOverlaySlide(const QVariant &);
	void cmdSetLiveBackground(const QString &, bool);

private:
	void log(const QString&);
	QTcpSocket *m_socket;
	
	QString m_lastError;
	
	MainWindow * m_log;
	OutputInstance * m_inst;
	
	int m_blockSize;
	QByteArray m_dataBlock;
	QJson::Parser * m_parser;

};

#endif
