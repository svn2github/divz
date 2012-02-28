#ifndef JpegServer_H
#define JpegServer_H

#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>
#include <QImageWriter>
#include <QGraphicsScene>
#include <QTimer>
#include <QTime>

class JpegServer : public QTcpServer
{
	Q_OBJECT
	
public:
	JpegServer(QObject *parent = 0);
	
	void setAdaptiveWriteEnabled(bool flag) { m_adaptiveWriteEnabled = flag; }
	bool adaptiveWriteEnabled() { return m_adaptiveWriteEnabled; }
	
	void setScene(QGraphicsScene *scene);
	QGraphicsScene *scene() { return m_scene; }
	
	void setFps(int fps);
	int fps() { return m_fps; }
	
// 	QString myAddress();
	void onlyRenderOnSlideChange(bool flag=true);

public slots:
	void slideChanged();

private slots:
	void generateNextFrame();
	
signals:
	void frameReady(QImage);

protected:
	void incomingConnection(int socketDescriptor);

private:
	void updateRects();
	
	QGraphicsScene *m_scene;
	int m_fps;
	QTimer m_timer;
	bool m_adaptiveWriteEnabled;
	
	QRect m_targetRect;
	QRect m_sourceRect;
	
	int m_timeAccum;
	int m_frameCount;
	
	QTime m_time;
	
	bool m_onlyRenderOnSlideChange;
	bool m_slideChanged;
	QImage m_cachedImage;

};


class QImage;
class JpegServerThread : public QThread
{
    Q_OBJECT

public:
	JpegServerThread(int socketDescriptor, bool adaptiveWriteEnabled, QObject *parent = 0);
	~JpegServerThread();
	
	void run();

signals:
	void error(QTcpSocket::SocketError socketError);

public slots:
	void imageReady(QImage);

private:
	void writeHeaders();
	
	int m_socketDescriptor;
	QTcpSocket * m_socket;
	
	QByteArray m_boundary;
	QImageWriter m_writer;
	bool m_adaptiveWriteEnabled;
	int m_adaptiveIgnore;
};


#endif //JpegServer_H

