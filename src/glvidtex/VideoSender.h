#ifndef VideoSender_H
#define VideoSender_H

#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>
#include <QImageWriter>
#include <QTimer>
#include <QImage>

class SimpleV4L2;
#include "../livemix/VideoFrame.h"
#include "../livemix/VideoSource.h"


class VideoSender : public QTcpServer
{
	Q_OBJECT
	
public:
	VideoSender(QObject *parent = 0);
	~VideoSender();
	
	void setAdaptiveWriteEnabled(bool flag) { m_adaptiveWriteEnabled = flag; }
	bool adaptiveWriteEnabled() { return m_adaptiveWriteEnabled; }
	
	VideoSource *videoSource() { return m_source; }
	VideoFrame frame() { return m_frame; }
	
	void setVideoSource(VideoSource *source);
// 	QString myAddress();

signals: 
	void receivedFrame();
	
public slots:
	void disconnectVideoSource();

private slots:
	void frameReady();
	
protected:
	void incomingConnection(int socketDescriptor);
	
private:
	bool m_adaptiveWriteEnabled;
	VideoSource *m_source;
	VideoFrame m_frame;
	
};


class QImage;
class VideoSenderThread : public QThread
{
    Q_OBJECT

public:
	VideoSenderThread(int socketDescriptor, bool adaptiveWriteEnabled, QObject *parent = 0);
	~VideoSenderThread();
	
	void run();
	void setSender(VideoSender*);

signals:
	void error(QTcpSocket::SocketError socketError);

public slots:
	void frameReady();

private:
	int m_socketDescriptor;
	QTcpSocket * m_socket;
	
	bool m_adaptiveWriteEnabled;
	bool m_sentFirstHeader;
	VideoSender *m_sender;
};


#endif //VideoSender_H

