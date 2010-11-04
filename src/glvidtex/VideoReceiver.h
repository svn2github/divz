#ifndef VideoReceiver_H
#define VideoReceiver_H

#include <QObject>
#include <QByteArray>
#include <QTcpSocket>

#include <QThread>
#include <QImage>
#include <QTime>

#define MJPEG_TEST 1

#ifdef MJPEG_TEST
  #include <QLabel>
#endif

#include "../livemix/VideoSource.h"
#include "../livemix/VideoFrame.h"

class VideoReceiver: public VideoSource
{
	Q_OBJECT
public:
	VideoReceiver(QObject *parent = 0);
	~VideoReceiver();
	
	bool connectTo(const QString& host, int port=-1, QString url = "/", const QString& user="", const QString& pass="");
	void exit();
	QString errorString(){ return m_socket->errorString(); }
	
	QSize autoResize() { return m_autoResize; }
	void setAutoResize(QSize size) { m_autoResize = size; }
	
	bool autoReconnect() { return m_autoReconnect; }
	void setAutoReconnect(bool flag) { m_autoReconnect = flag; }
	
	QString host() { return m_host; }
	int port() { return m_port; }
	QString path() { return m_url; }
	
	int msecTo(int timestamp);
	
	VideoFormat videoFormat() { return VideoFormat(VideoFrame::BUFFER_BYTEARRAY,QVideoFrame::Format_RGB32); }
	
signals:
	void socketDisconnected();
	void socketError(QAbstractSocket::SocketError);
	void socketConnected();
	
private slots:
	void dataReady();
	void processBlock();
	void lostConnection();
	void lostConnection(QAbstractSocket::SocketError);
	void reconnect();
	void connectionReady();

private:
	QTime timestampToQTime(int);
	void log(const QString&);
	QTcpSocket *m_socket;
	
	QString m_boundary;
	bool m_firstBlock;
	
	QByteArray m_dataBlock;
	
	QSize m_autoResize;
	bool m_autoReconnect;
	
	QString m_host;
	int m_port;
	QString m_url;
	QString m_user;
	QString m_pass;
	
	bool m_httpJpegCompatMode;
	
	int m_byteCount;
	
	VideoFrame m_frame;
	
#ifdef MJPEG_TEST
	QLabel *m_label;
	int m_frameCount;
	int m_latencyAccum;
	QTime m_time;	
	bool m_debugFps;
#endif

};

#endif // VideoReceiver_H

