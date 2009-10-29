#ifndef JpegServer_H
#define JpegServer_H

#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>

class JpegServer : public QTcpServer
{
	Q_OBJECT
	
public:
	JpegServer(QObject *parent = 0);
	
	void setProvider(QObject *provider, const char * signalName, bool deleteImage = true);
	
	QString myAddress();

protected:
	void incomingConnection(int socketDescriptor);

private:
	QObject * m_imageProvider;
	const char * m_signalName;
	bool m_deleteImage;

};


class QImage;
class JpegServerThread : public QThread
{
    Q_OBJECT

public:
	JpegServerThread(int socketDescriptor, bool deleteImage, QObject *parent = 0);
	~JpegServerThread();
	
	void run();

signals:
	void error(QTcpSocket::SocketError socketError);

public slots:
	void imageReady(QImage*);

private:
	void writeHeaders();
	
	int m_socketDescriptor;
	bool m_deleteImage;
	QTcpSocket * m_socket;
	
	QByteArray m_boundary;
	
};


#endif //JpegServer_H

