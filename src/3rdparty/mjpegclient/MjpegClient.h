#ifndef MjpegClient_H
#define MjpegClient_H

#include <QObject>
#include <QByteArray>
#include <QTcpSocket>

#define MJPEG_TEST 1

#ifdef MJPEG_TEST
  #include <QLabel>
#endif

class MjpegClient: public QObject
{
	Q_OBJECT
public:
	MjpegClient(QObject *parent = 0);
	~MjpegClient();
	
	bool connectTo(const QString& host, int port, QString url = "/");
	void exit();
	QString errorString(){ return m_socket->errorString(); }

signals:
	void socketDisconnected();
	void socketError(QAbstractSocket::SocketError);
	void socketConnected();
	
	void newImage(QImage);
	
private slots:
	void dataReady();
	void processBlock();

private:
	void log(const QString&);
	QTcpSocket *m_socket;
	
	QString m_boundary;
	bool m_firstBlock;
	
	int m_blockSize;
	QByteArray m_dataBlock;
	
#ifdef MJPEG_TEST
	QLabel *m_label;
#endif

};

#endif // MjpegClient_H

