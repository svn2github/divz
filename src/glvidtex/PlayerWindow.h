#ifndef PlayerWindow_H
#define PlayerWindow_H

#include <QWidget>
#include <QVariant>

class GLPlayerServer;

// // for testing
// class GLPlayerClient;

class GLSceneGroup;
class GLScene;
class GLWidget;
class QGraphicsView;
class QGraphicsScene;
class VideoInputSenderManager;
class VideoEncoder;

class PlayerWindow : public QWidget
{
	Q_OBJECT
public:
	PlayerWindow(QWidget *parent=0);
	
	GLSceneGroup *group() { return m_group; }
	GLScene *scene() {  return m_scene; }
	
public slots:
	void setGroup(GLSceneGroup*);
	void setScene(GLScene*);

private slots:
	void receivedMap(QVariantMap);
	
/*	// for testing
	void sendTestMap();
	void slotConnected();*/
	
private:
	void sendReply(QVariantList);
	
	GLPlayerServer *m_server;
	
	bool m_loggedIn;
	
	QString m_validUser;
	QString m_validPass;
	
	QString m_playerVersionString;
	int m_playerVersion;
	

	GLSceneGroup *m_group;
	GLScene *m_scene;
	
	QGraphicsView *m_graphicsView;
	QGraphicsScene *m_graphicsScene;
	
	GLWidget *m_glWidget;
	
	bool m_useGLWidget;
	
/*	// for testing
	GLPlayerClient *m_client;*/
	VideoInputSenderManager *m_vidSendMgr;
	
	VideoEncoder *m_outputEncoder;
};

#endif
