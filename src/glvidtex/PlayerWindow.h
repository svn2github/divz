#ifndef PlayerWindow_H
#define PlayerWindow_H

#include <QWidget>
#include <QVariant>
#include <QTimer>

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
class GLPlaylistItem;
class GLRectDrawable;
class GLDrawable;
class SharedMemorySender;

#include "../livemix/VideoSource.h"

class PlayerWindow;	
class PlayerCompatOutputStream : public VideoSource
{
	Q_OBJECT
	
protected:
	friend class PlayerWindow;
	PlayerCompatOutputStream(PlayerWindow *parent=0);

public:
	//virtual ~StaticVideoSource() {}

	VideoFormat videoFormat() { return VideoFormat(VideoFrame::BUFFER_IMAGE,QVideoFrame::Format_ARGB32); }
	//VideoFormat videoFormat() { return VideoFormat(VideoFrame::BUFFER_BYTEARRAY,QVideoFrame::Format_ARGB32); }
	
	const QImage & image() { return m_image; }
	int fps() { return m_fps; }

public slots:
	void setFps(int);

signals:
	void frameReady();

private slots:
	void renderScene();
	void setImage(QImage);
	
private:
	PlayerWindow *m_win;
	QImage m_image;
	int m_fps;
	QTimer m_frameReadyTimer;
};


class PlayerWindow : public QWidget
{
	Q_OBJECT
public:
	PlayerWindow(QWidget *parent=0);
	
	GLSceneGroup *group() { return m_group; }
	//GLScene *scene() {  return m_scene; }
	
public slots:
	void setGroup(GLSceneGroup*);
	void setScene(GLScene*);

private slots:
	void receivedMap(QVariantMap);
	
/*	// for testing
	void sendTestMap();
	void slotConnected();*/
	
	void currentPlaylistItemChanged(GLPlaylistItem*);
	void playlistTimeChanged(double);
	
	//void drawableIsVisible(bool);
	void opacityAnimationFinished();

	void setBlack(bool black=true);

protected:
	friend class PlayerCompatOutputStream;
	QGraphicsScene *graphicsScene() { return m_graphicsScene; }

private:
	void sendReply(QVariantList);
	
	GLPlayerServer *m_server;
	
	bool m_loggedIn;
	
	QString m_validUser;
	QString m_validPass;
	
	QString m_playerVersionString;
	int m_playerVersion;
	

	GLSceneGroup *m_group;
	QPointer<GLScene> m_scene;
	QPointer<GLScene> m_oldScene;
	
	QGraphicsView *m_graphicsView;
	QGraphicsScene *m_graphicsScene;
	
	GLWidget *m_glWidget;
	
	bool m_useGLWidget;
	
/*	// for testing
	GLPlayerClient *m_client;*/
	VideoInputSenderManager *m_vidSendMgr;
	
	VideoEncoder *m_outputEncoder;
	SharedMemorySender *m_shMemSend;
	
	int m_xfadeSpeed;
	
	PlayerCompatOutputStream *m_compatStream;
	
	//GLRectDrawable *m_blackOverlay;
	bool m_isBlack;
	GLScene *m_blackScene;
	QPointer<GLScene> m_scenePreBlack;
	QList<GLDrawable*> m_oldDrawables;
};

#endif
