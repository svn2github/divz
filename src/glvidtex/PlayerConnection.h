#ifndef PlayerConnection_H
#define PlayerConnection_H

#include <QtGui>
#include <QAbstractSocket>

class GLPlayerClient;
class GLWidgetSubview;
class GLDrawable;
class GLSceneGroup;
class GLScene;

class PlayerConnection;
class PlayerSubviewsModel : public QAbstractListModel
{
	Q_OBJECT
public:
	PlayerSubviewsModel(PlayerConnection*);
	~PlayerSubviewsModel();
	
	int rowCount(const QModelIndex &/*parent*/) const;
	QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex &index, const QVariant & value, int role) ;
	
private slots:
	void subviewAdded(GLWidgetSubview*);
	void subviewRemoved(GLWidgetSubview*);
	void subviewChanged(GLWidgetSubview*);
	
private:
	PlayerConnection *m_conn;

};


class PlayerConnection : public QObject
{
	Q_OBJECT
	
	Q_PROPERTY(QString name		READ name WRITE setName);
	Q_PROPERTY(QString host		READ host WRITE setHost);
	Q_PROPERTY(QString user		READ user WRITE setUser);
	Q_PROPERTY(QString pass		READ pass WRITE setPass);
	Q_PROPERTY(QRect screenRect	READ screenRect WRITE setScreenRect);
	Q_PROPERTY(QRect viewportRect	READ viewportRect WRITE setViewportRect);
	Q_PROPERTY(QSizeF canvasSize READ canvasSize WRITE setCanvasSize);
	Q_PROPERTY(Qt::AspectRatioMode aspectRatioMode READ aspectRatioMode WRITE setAspectRatioMode);

	Q_PROPERTY(bool isConnected	READ isConnected);
	Q_PROPERTY(QString lastError	READ lastError);
	
public:
	PlayerConnection(QObject *parent=0);
	PlayerConnection(QByteArray&, QObject *parent=0);
	~PlayerConnection();
	
	QByteArray toByteArray();
	void fromByteArray(QByteArray&);
	
	QList<GLWidgetSubview*> subviews() { return m_subviews; }
	
	bool testConnection();
	
	QString name() { return m_name; }
	QString host() { return m_host; }
	QString user() { return m_user; }
	QString pass() { return m_pass; }
	QRect screenRect() { return m_screenRect; }
	QRect viewportRect() { return m_viewportRect; }
	QSizeF canvasSize() { return m_canvasSize; }
	Qt::AspectRatioMode aspectRatioMode() { return m_aspectRatioMode; }
	
	bool isConnected() { return m_isConnected; }
	QString lastError() { return m_lastError; }
	
	QString playerVersion() { return m_playerVersion; }
		
	QStringList videoInputs(bool *hasReceivedResponse=0);

public slots:
	void addSubview(GLWidgetSubview*);
	void removeSubview(GLWidgetSubview*);
	
	void connectPlayer();
	void disconnectPlayer();
	
	void setName(const QString&);
	void setHost(const QString&);
	void setUser(const QString&);
	void setPass(const QString&);
	void setScreenRect(const QRect&);
	void setViewportRect(const QRect&);
	void setCanvasSize(const QSizeF&);
	void setAspectRatioMode(Qt::AspectRatioMode);
	
	void setGroup(GLSceneGroup *group, GLScene *initialScene=0);
	void setScene(GLScene*); // must be part of 'group'
	void setUserProperty(GLDrawable *, QVariant value, QString propertyName="");
	void setVisibility(GLDrawable *, bool isVisible=true);
	
	void queryProperty(GLDrawable *, QString propertyName="");
	
	void fadeBlack(bool flag);
	void setCrossfadeSpeed(int ms);
	
signals:
	void subviewAdded(GLWidgetSubview*);
	void subviewRemoved(GLWidgetSubview*);
	
	void connected();
	void disconnected();
	
	void loginSuccess();
	void loginFailure();
	
	void playerError(const QString&);
	
	void propQueryResponse(GLDrawable *drawable, QString propertyName, const QVariant& value);
	
	void playerNameChanged(const QString&);
	
protected:
	friend class PlayerSubviewsModel;
	
	QList<GLWidgetSubview*> m_subviews;
	
private slots:
	void subviewChanged(GLWidgetSubview*);
	void receivedMap(QVariantMap);
	void clientConnected();
	void clientDisconnected();
	void socketError(QAbstractSocket::SocketError);
	
private:
	void setError(const QString&, const QString& cmd="");
	void sendCommand(QVariantList reply);
	
	QString m_name;
	QString m_host;
	QString m_user;
	QString m_pass;
	QRect m_screenRect;
	QRect m_viewportRect;
	QSizeF m_canvasSize;
	Qt::AspectRatioMode m_aspectRatioMode;
	
	QString m_playerVersion;
	
	GLPlayerClient *m_client;	
	
	GLSceneGroup *m_group;
	GLScene *m_scene;
	
	bool m_isConnected;
	QString m_lastError;
	
	QVariantList m_preconnectionCommandQueue;
	
	QStringList m_videoInputs;
	bool m_videoIputsReceived;
};


class PlayerConnectionList : public QAbstractListModel
{
	Q_OBJECT
public:
	PlayerConnectionList(QObject *parent=0);
	~PlayerConnectionList();
	
	int rowCount(const QModelIndex &/*parent*/) const;
	QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(const QModelIndex &index, const QVariant & value, int role) ;
	
	QByteArray toByteArray();
	void fromByteArray(QByteArray);
	
	QList<PlayerConnection*> players() { return m_players; }
	
	int size() { return m_players.size(); }
	PlayerConnection *at(int x) { return x>=0 && x<m_players.size()?m_players.at(x):0; }

public slots:
	void addPlayer(PlayerConnection *);
	void removePlayer(PlayerConnection *);
		
signals:
	void playerAdded(PlayerConnection *);
	void playerRemoved(PlayerConnection *);
	
private slots:
	void playerNameChanged(const QString&);
	
private:
	QList<PlayerConnection *> m_players;

};


#endif
