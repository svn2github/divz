#include "PlayerConnection.h"

#include "GLPlayerClient.h"
#include "GLPlayerCommandNames.h"
#include "GLWidget.h"
#include "GLDrawable.h"
#include "GLSceneGroup.h"

//////////////////////////////////////////////////////////////////////////////

PlayerSubviewsModel::PlayerSubviewsModel(PlayerConnection *conn)
	: m_conn(conn)
{
}
PlayerSubviewsModel::~PlayerSubviewsModel()
{
	disconnect(m_conn, 0, this, 0);
}
	
int PlayerSubviewsModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m_conn->m_subviews.size();
}

QVariant PlayerSubviewsModel::data( const QModelIndex & index, int role ) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= rowCount(QModelIndex()))
		return QVariant();
	
	if (role == Qt::DisplayRole || Qt::EditRole == role)
	{
		GLWidgetSubview *d = m_conn->m_subviews.at(index.row());
		QString value = d->title().isEmpty() ? QString("Subview %1").arg(index.row()+1) : d->title();
		return value;
	}
// 	else if(Qt::DecorationRole == role)
// 	{
// 		GLSceneLayout *lay = m_scene->m_layouts.at(index.row());
// 		return lay->pixmap();
// 	}
	else
		return QVariant();
}
Qt::ItemFlags PlayerSubviewsModel::flags(const QModelIndex &index) const
{
	if (index.isValid())	
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable; //| Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled ;
}
bool PlayerSubviewsModel::setData(const QModelIndex &index, const QVariant & value, int /*role*/)
{
	if (!index.isValid())
		return false;
	
	if (index.row() >= rowCount(QModelIndex()))
		return false;
	
	GLWidgetSubview *d = m_conn->m_subviews.at(index.row());
	qDebug() << "PlayerSubviewsModel::setData: "<<this<<" row:"<<index.row()<<", value:"<<value; 
	if(value.isValid() && !value.isNull())
	{
		d->setTitle(value.toString());
		dataChanged(index,index);
		return true;
	}
	return false;
}
	
void PlayerSubviewsModel::subviewAdded(GLWidgetSubview *sub)
{
	connect(sub, SIGNAL(subviewChanged(GLWidgetSubview*)), this, SLOT(subviewChanged(GLWidgetSubview*)));
	
	beginInsertRows(QModelIndex(),m_conn->m_subviews.size()-1,m_conn->m_subviews.size());
	
	QModelIndex top    = createIndex(m_conn->m_subviews.size()-2, 0),
		    bottom = createIndex(m_conn->m_subviews.size()-1, 0);
	dataChanged(top,bottom);
	
	endInsertRows();
}

void PlayerSubviewsModel::subviewRemoved(GLWidgetSubview *sub)
{
	disconnect(sub, 0, this, 0);
	
	beginRemoveRows(QModelIndex(),0,m_conn->m_subviews.size()+1);
	
	int idx = m_conn->m_subviews.indexOf(sub);
	QModelIndex top    = createIndex(idx, 0),
		    bottom = createIndex(m_conn->m_subviews.size(), 0);
	dataChanged(top,bottom);
	
	endRemoveRows();
}

void PlayerSubviewsModel::subviewChanged(GLWidgetSubview *sub)
{
	if(!sub)
		return;
	
	int row = m_conn->m_subviews.indexOf(sub);
	if(row < 0)
		return;
	
	QModelIndex idx = createIndex(row, row);
	dataChanged(idx, idx);
}
	
//////////////////////////////////////////////////////////////////////////////

PlayerConnection::PlayerConnection(QObject *parent)
	: QObject(parent)
	, m_client(0)
	, m_isConnected(false)
{
	
}

PlayerConnection::PlayerConnection(QByteArray& ba, QObject *parent)
	: QObject(parent)
	, m_client(0)
	, m_isConnected(false)
{
	fromByteArray(ba);
}

PlayerConnection::~PlayerConnection()
{
	if(m_client)
	{
		m_client->exit();
		m_client->deleteLater();
		m_client = 0;
	}
}
	

QByteArray PlayerConnection::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);

	
	QVariantMap map;
	map["name"]	= m_name;
	map["host"] 	= m_host;
	map["user"] 	= m_user;
	map["pass"] 	= m_pass;
	map["screen"] 	= m_screenRect;
	map["viewport"]	= m_viewportRect;
	map["armode"]	= (int)m_aspectRatioMode;
	
	QVariantList views;
	foreach(GLWidgetSubview *view, m_subviews )
		views << view->toByteArray();
	
	map["views"] = views;
	
	stream << map;
	
	return array;
}

void PlayerConnection::fromByteArray(QByteArray& array)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	if(map.isEmpty())
		return;
	
	m_name = map["name"].toString();
	m_host = map["host"].toString();
	m_user = map["user"].toString();
	m_pass = map["pass"].toString();
	m_screenRect = map["screen"].toRect();
	m_viewportRect = map["viewport"].toRect();
	m_aspectRatioMode = (Qt::AspectRatioMode)map["armode"].toInt();
	
	m_subviews.clear();
	QVariantList views = map["views"].toList();
	foreach(QVariant var, views)
	{
		QByteArray data = var.toByteArray();
		
		GLWidgetSubview *subview = new GLWidgetSubview();
		subview->fromByteArray(data);
		m_subviews << subview;
	}
}

void PlayerConnection::addSubview(GLWidgetSubview *sub)
{
	m_subviews << sub;
	emit subviewAdded(sub);
	
	sendCommand(QVariantList() 
		<< "cmd" 	<< GLPlayer_AddSubview
		<< "data"	<< sub->toByteArray());
}

void PlayerConnection::removeSubview(GLWidgetSubview *sub)
{
	emit subviewRemoved(sub);
	m_subviews.removeAll(sub);
	
	sendCommand(QVariantList() 
		<< "cmd" 	<< GLPlayer_RemoveSubview
		<< "subviewid"	<< sub->subviewId());
}

void PlayerConnection::connectPlayer()
{
	m_client = new GLPlayerClient();
	m_client->connectTo(m_host,9977);
	
	connect(m_client, SIGNAL(socketConnected()), this, SLOT(clientConnected()));
	connect(m_client, SIGNAL(receivedMap(QVariantMap)), this, SLOT(receivedMap(QVariantMap)));
	connect(m_client, SIGNAL(sockectDisconnected()), this, SLOT(clientDisconnected()));
	connect(m_client, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	
	sendCommand(QVariantList() 
		<< "cmd" 	<< GLPlayer_Login
		<< "user"	<< m_user
		<< "pass"	<< m_pass);
}

void PlayerConnection::clientConnected()
{
	m_isConnected = true;
	
	if(!m_preconnectionCommandQueue.isEmpty())
		foreach(QVariant var, m_preconnectionCommandQueue)
			m_client->sendMap(var.toMap());
		
	emit connected();
}

void PlayerConnection::disconnectPlayer()
{
	/// TODO disconnect socket and lost
	disconnect(m_client, 0, this, 0);

	m_isConnected = false;

	m_client->exit();
	m_client->deleteLater();
	m_client = 0;
	
	emit disconnected();
}

void PlayerConnection::socketError(QAbstractSocket::SocketError)
{
	/// TODO handle error conditions
}

void PlayerConnection::clientDisconnected()
{
	disconnectPlayer();
}

void PlayerConnection::setName(const QString& value)
{
	m_name = value;
	emit playerNameChanged(value);
}
void PlayerConnection::setHost(const QString& value)
{
	m_host = value;
}
void PlayerConnection::setUser(const QString& value)
{
	m_user = value;
}
void PlayerConnection::setPass(const QString& value)
{
	m_pass = value;
}
void PlayerConnection::setScreenRect(const QRect& rect)
{
	m_screenRect = rect;	
	sendCommand(QVariantList() 
		<< "cmd" 	<< GLPlayer_SetScreen
		<< "rect"	<< QRectF(rect));
}
void PlayerConnection::setViewportRect(const QRect& rect)
{
	m_viewportRect = rect;
	
	sendCommand(QVariantList() 
		<< "cmd" 	<< GLPlayer_SetViewport
		<< "viewport"	<< QRectF(rect));
}
void PlayerConnection::setAspectRatioMode(Qt::AspectRatioMode mode)
{
	m_aspectRatioMode = mode;
	
	sendCommand(QVariantList() 
		<< "cmd" 	<< GLPlayer_SetIgnoreAR
		<< "flag"	<< (mode == Qt::IgnoreAspectRatio ? true : false));
}

void PlayerConnection::setGroup(GLSceneGroup *group, GLScene *initialScene)
{
	if(!group)
		return;
	
	sendCommand(QVariantList() 
		<< "cmd" 	<< GLPlayer_LoadSlideGroup
		<< "data"	<< group->toByteArray());

	if(initialScene)
		setScene(initialScene);
}

void PlayerConnection::setScene(GLScene* scene) // must be part of 'group'
{
	if(!scene)
		return;
	
	m_scene = scene;
	
	sendCommand(QVariantList() 
		<< "cmd" 	<< GLPlayer_SetSlide
		<< "sceneid"	<< scene->sceneId());	
}

void PlayerConnection::setUserProperty(GLDrawable *gld, QVariant value, QString propertyName)
{
	if(!gld)
		return;
	
	sendCommand(QVariantList() 
		<< "cmd" 	<< GLPlayer_SetUserProperty
		<< "drawableid"	<< gld->id()
		<< "name"	<< propertyName
		<< "value"	<< value);	
}

void PlayerConnection::setVisibility(GLDrawable *gld, bool isVisible)
{
	if(!gld)
		return;
	
	sendCommand(QVariantList() 
		<< "cmd" 	<< GLPlayer_SetVisibility
		<< "drawableid"	<< gld->id()
		<< "value"	<< isVisible);	
}

void PlayerConnection::queryProperty(GLDrawable *gld, QString propertyName)
{
	if(!gld)
		return;
	
	sendCommand(QVariantList() 
		<< "cmd" 	<< GLPlayer_QueryProperty
		<< "drawableid"	<< gld->id()
		<< "name"	<< propertyName);	
}
	
/*signals:
	void subviewAdded(GLWidgetSubview*);
	void subviewRemoved(GLWidgetSubview*);
	
	void connected();
	void disconnected();
	
	void playerError(const QString&);
	
	void propQueryResponse(GLDrawable *drawable, QString propertyName, const QVariant& value);*/
	
void PlayerConnection::subviewChanged(GLWidgetSubview *sub)
{
	// The PlayerWindow class (the receiver) automatically removes the 
	// subview and re-adds it, so we just send the add command
	sendCommand(QVariantList() 
		<< "cmd" 	<< GLPlayer_AddSubview
		<< "data"	<< sub->toByteArray());
}

void PlayerConnection::receivedMap(QVariantMap map)
{
	// TODO parse map response and emit signals based on contents
	// right now, we handle:
	// - login responses 
	//	  if okay, emit connected, else emit playerError and close socket
	//      - set m_playerVersion when we get it
	// - property query responses
	//	- emit propQueryResponse 
	
	QString cmd = map["cmd"].toString();
	
	m_lastError = "";
	
	if(cmd == GLPlayer_Login)
	{
		// parse login response
		QString status = map["status"].toString();
		if(status == "error")
		{
			setError(map["message"].toString(), "Login Error");
			emit loginFailure();
		}
		else
		{
			m_playerVersion = map["version"].toString();
			emit loginSuccess();
		}
		
	}
	else
	if(cmd == GLPlayer_QueryProperty)
	{
		QString status = map["status"].toString();
		if(status == "error")
		{
			setError(map["message"].toString(), "Property Query Error");
		}
		else
		if(!m_scene)
		{
			setError("No scene set. Set scene first before querying property.", "Property Query Error");
		}
		else
		{
			QString name = map["cmd"].toString();
			QVariant value = map["value"];
			int id = map["drawableid"].toInt();
			
			GLDrawable *gld = m_scene->lookupDrawable(id);
			if(!gld)
			{
				setError("Cannot find drawable from query.", "Property Query Error");
			}
			else
			{
				emit propQueryResponse(gld, name, value);
			}
		}
	}
	else
	if(map["status"].toString() == "error")
	{
		setError(map["message"].toString(), cmd);
	}
	else
	{
		qDebug() << "PlayerConnection::receivedMap: [INFO] Command not handled: "<<cmd<<", map:"<<map;
	}
}

void PlayerConnection::setError(const QString& error, const QString &cmd)
{
	m_lastError = error;
	qDebug() << "PlayerConnection: [ERROR] "<<cmd<<": "<<m_lastError;
	emit playerError(m_lastError);
}

void PlayerConnection::sendCommand(QVariantList reply)
{
	QVariantMap map;
	if(reply.size() % 2 != 0)
	{
		qDebug() << "PlayerConnection::sendCommand: [WARNING]: Odd number of elelements in reply: "<<reply;
	}
	
	for(int i=0; i<reply.size(); i+=2)
	{
		if(i+1 >= reply.size())
			continue;
		
		QString key = reply[i].toString();
		QVariant value = reply[i+1];
		
		map[key] = value;
	}
	
	
	qDebug() << "PlayerConnection::sendCommand: [DEBUG] map:"<<map;
	
	if(m_client && m_isConnected)
		m_client->sendMap(map);
	else 
		m_preconnectionCommandQueue << map;
}



	
/*protected:
	QString m_name;
	QString m_host;
	QString m_user;
	QString m_pass;
	QRect m_screenRect;
	QRect m_viewportRect;
	
	QString m_playerVersion;
	
	QList<GLWidgetSubview*> m_subviews;
	
	GLPlayerClient *m_client;*/
	

//////////////////////////////////////////////////////////////////////////////

PlayerConnectionList::PlayerConnectionList(QObject */*parent*/)
{
}
PlayerConnectionList::~PlayerConnectionList()
{
	//disconnect(conn, 0, this, 0);
}
	
int PlayerConnectionList::rowCount(const QModelIndex &/*parent*/) const
{
	return m_players.size();
}

QVariant PlayerConnectionList::data( const QModelIndex & index, int role ) const
{
	if (!index.isValid())
		return QVariant();
	
	if (index.row() >= rowCount(QModelIndex()))
		return QVariant();
	
	if (role == Qt::DisplayRole || Qt::EditRole == role)
	{
		PlayerConnection *d = m_players.at(index.row());
		QString value = d->name().isEmpty() ? QString("Player %1").arg(index.row()+1) : d->name();
		return value;
	}
// 	else if(Qt::DecorationRole == role)
// 	{
// 		GLSceneLayout *lay = m_scene->m_layouts.at(index.row());
// 		return lay->pixmap();
// 	}
	else
		return QVariant();
}
Qt::ItemFlags PlayerConnectionList::flags(const QModelIndex &index) const
{
	if (index.isValid())	
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable; //| Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
	
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled ;
}
bool PlayerConnectionList::setData(const QModelIndex &index, const QVariant & value, int /*role*/)
{
	if (!index.isValid())
		return false;
	
	if (index.row() >= rowCount(QModelIndex()))
		return false;
	
	PlayerConnection *d = m_players.at(index.row());
	qDebug() << "PlayerConnectionList::setData: "<<this<<" row:"<<index.row()<<", value:"<<value; 
	if(value.isValid() && !value.isNull())
	{
		d->setName(value.toString());
		dataChanged(index,index);
		return true;
	}
	return false;
}
	
void PlayerConnectionList::addPlayer(PlayerConnection *player)
{
	if(!player)
		return;
	
	m_players << player;
	connect(player, SIGNAL(playerNameChanged(const QString&)), this, SLOT(playerNameChanged(const QString&)));
	
	beginInsertRows(QModelIndex(),m_players.size()-1,m_players.size());
	
	QModelIndex top    = createIndex(m_players.size()-2, 0),
		    bottom = createIndex(m_players.size()-1, 0);
	dataChanged(top,bottom);
	
	endInsertRows();
	
	emit playerAdded(player);
}

void PlayerConnectionList::removePlayer(PlayerConnection *player)
{
	if(!player)
		return;
	
	m_players.removeAll(player);
	disconnect(player, 0, this, 0);
	
	beginRemoveRows(QModelIndex(),0,m_players.size()+1);
	
	int idx = m_players.indexOf(player);
	QModelIndex top    = createIndex(idx, 0),
		    bottom = createIndex(m_players.size(), 0);
	dataChanged(top,bottom);
	
	endRemoveRows();

	emit playerRemoved(player);

}

void PlayerConnectionList::playerNameChanged(const QString&)
{
	PlayerConnection *player = dynamic_cast<PlayerConnection *>(player);
	if(!player)
		return;
	
	int row = m_players.indexOf(player);
	if(row < 0)
		return;
	
	QModelIndex idx = createIndex(row, row);
	dataChanged(idx, idx);
}
	

QByteArray PlayerConnectionList::toByteArray()
{
	QByteArray array;
	QDataStream stream(&array, QIODevice::WriteOnly);

	QVariantList players;
	foreach(PlayerConnection *player, m_players)
		players << player->toByteArray();
	
	QVariantMap map;
	map["players"] = players;
	
	stream << map;
	
	return array;
}

void PlayerConnectionList::fromByteArray(QByteArray& array)
{
	QDataStream stream(&array, QIODevice::ReadOnly);
	QVariantMap map;
	stream >> map;
	
	if(map.isEmpty())
		return;
	
	m_players.clear();
	QVariantList views = map["players"].toList();
	foreach(QVariant var, views)
	{
		QByteArray data = var.toByteArray();
		m_players << new PlayerConnection(data);
	}
}
