#ifndef PlayerWindow_H
#define PlayerWindow_H

#include <QWidget>

#include "GLWidget.h"

class GLPlayerServer;

// // for testing
// class GLPlayerClient;

class PlayerWindow : public GLWidget
{
	Q_OBJECT
public:
	PlayerWindow(QWidget *parent=0);

private slots:
	void receivedMap(QVariantMap);
	
// 	// for testing
// 	void sendTestMap();
// 	void slotConnected();
	
private:
	GLPlayerServer *m_server;
	
// 	// for testing
// 	GLPlayerClient *m_client;
};

#endif
