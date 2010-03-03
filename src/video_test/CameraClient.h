#ifndef CameraClient_H
#define CameraClient_H


#include <QObject>
#include <QByteArray>
#include <QTcpSocket>
#include <QImage>

class CameraClient : public QObject
{
	Q_OBJECT
public:
	CameraClient(QObject *parent = 0);
	~CameraClient();
	
	bool connectTo(const QString& host, int port);
	void exit();
	QString errorString(){ return m_socket->errorString(); }

signals:
	void newImage(QImage);
	void socketDisconnected();
	void socketError(QAbstractSocket::SocketError);
	void socketConnected();
	
private slots:
	void dataReady();
	
private:
	void log(const QString&);
	QTcpSocket *m_socket;
	
	int m_blockSize;
	QByteArray m_dataBlock;
};

#endif
