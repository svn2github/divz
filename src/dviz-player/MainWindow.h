#ifndef MainWindow_H
#define MainWindow_H

#include <QtGui>
#include <QtNetwork>

//class VariantMapServer;
class VariantMapClient;
class GLScene;
class GLWidget;
class GLImageHttpDrawable;

class MainWindow : public QWidget
{
	Q_OBJECT
public:
	MainWindow(QString host, int cmdPort=9977, int videoPort=8930);
	
protected slots:
	void receivedMap(QVariantMap);
	
	void setScreen(QRect);
	void setBlack(bool);
	void setCrossfadeSpeed(int ms);
	void setIgnoreAR(bool);
	void sendCommand(QVariantList reply);
	void setStreamURL(QString);
	
	void pingServer();
	void clientConnected();
	void disconnectClient();
	void socketError(QAbstractSocket::SocketError socketError);
	void lostConnection();
	void connectTimeout();
	void reconnect();
	void clientDisconnected();
	
	void connectClient();
	

protected:
	QString m_host;
	int m_cmdPort;
	int m_vidPort;
	
	QTimer m_connectTimer;
	QTimer m_pingTimer;
	QTimer m_pingDeadTimer;
	
	bool m_isConnected;
	
	
// 	VariantMapServer *m_server;
	VariantMapClient *m_client;

	bool m_isBlack;
	int m_xfadeSpeed;
	GLScene *m_scene;
	GLWidget *m_glWidget;
	GLImageHttpDrawable *m_drw;
	
};



#endif
