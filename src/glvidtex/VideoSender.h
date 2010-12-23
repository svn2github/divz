#ifndef VideoSender_H
#define VideoSender_H

#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>
#include <QImageWriter>
#include <QTimer>
#include <QImage>
#include <QMutex>

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
	VideoFrame *frame();
	VideoFrame *scaledFrame();
	
	void setVideoSource(VideoSource *source);
// 	QString myAddress();

	int transmitFps() { return m_transmitFps; }
	QSize transmitSize() { return m_transmitSize; }
		
signals: 
	void receivedFrame();
	
public slots:
	void disconnectVideoSource();
	void setTransmitFps(int fps=-1); // auto fps based on source if -1
	void setTransmitSize(const QSize& size=QSize()); // null size means auto size based on input

private slots:
	void frameReady();
	void fpsTimer();
	
protected:
	void incomingConnection(int socketDescriptor);
	
private:
	bool m_adaptiveWriteEnabled;
	VideoSource *m_source;
	QPointer<VideoFrame> m_frame;
	QPointer<VideoFrame> m_scaledFrame;
	QSize m_transmitSize;
	int m_transmitFps;
	QTimer m_fpsTimer;
	QMutex m_frameMutex;
	
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

