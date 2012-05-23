#include "MainWindow.h"


#include "GLWidget.h"
#include "GLSceneGroup.h"
#include "GLDrawables.h"

//#include "VariantMapServer.h"
#include "VariantMapClient.h"


#include "ServerCommandNames.h"

/**

	TODO:
	
	- Setup simple app to render video receiver frames using the GLImageHttpDrawable from DViz
	- Setup a VarNet server to handle the cross-fade times
	- Adapt DViz to use a VarNet client (optionally compiled)
		Make sure to leave room for future uses in the command strucutre, such as camera input crossfading and text overlays
		Dont forget security...
	- Add alternate transition types (flips, fades, blocks, disolves, coverflow...)  

*/





MainWindow::MainWindow(QString host, int cmdPort, int vidPort)
	: QWidget()
	, m_host(host)
	, m_cmdPort(cmdPort)
	, m_vidPort(vidPort)
	, m_isConnected(false)
	, m_client(0)
	, m_isBlack(false)
	, m_xfadeSpeed(750)
	, m_scene(0)
	, m_glWidget(0)
	, m_drw(0)
{
	// Setup the layout of the window
	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setContentsMargins(0,0,0,0);
		
	// Create the GLWidget to do the actual rendering
	m_glWidget = new GLWidget(this);
	vbox->addWidget(m_glWidget);
	
	m_scene = new GLScene();

	m_drw = new GLImageHttpDrawable();
	
	// Add the drawable to the scene
	m_scene->addDrawable(m_drw);
	
	// setGLWidget() adds all the drawables in the scene to the GLWidget
	m_scene->setGLWidget(m_glWidget);


	// Adjust window for a 4:3 aspect ratio
	resize(640, 480);
/*
	// Create timers to maintain connection of client
	connect(&m_connectTimer, SIGNAL(timeout()), this, SLOT(connectTimeout()));
	m_connectTimer.setInterval(5 * 1000);
	m_connectTimer.setSingleShot(true);
	
	connect(&m_pingTimer, SIGNAL(timeout()), this, SLOT(pingServer()));
	m_pingTimer.setInterval(1000);
	
	connect(&m_pingDeadTimer, SIGNAL(timeout()), this, SLOT(lostConnection()));
	m_pingDeadTimer.setInterval(2000);
	m_pingDeadTimer.setSingleShot(true);
*/
	connectClient();
}

void MainWindow::connectClient()
{
	m_drw->setUrl(tr("raw://%1:%2").arg(m_host).arg(m_vidPort));
	
	if(m_client)
		disconnectClient();

	// Create the client 
	m_client = new VariantMapClient();
	m_client->connectTo(m_host, m_cmdPort);
	
	m_connectTimer.start();

	// Connect client slots
	connect(m_client, SIGNAL(socketConnected()), this, SLOT(clientConnected()));
	connect(m_client, SIGNAL(receivedMap(QVariantMap)), this, SLOT(receivedMap(QVariantMap)));
	connect(m_client, SIGNAL(socketDisconnected()), this, SLOT(clientDisconnected()));
	connect(m_client, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}	


void MainWindow::pingServer()
{
	if(!m_isConnected)
		return;
		
	sendCommand(QVariantList() << "cmd" << Server_DeadPing);
	
	#ifdef DEBUG_SOCKET
	//qDebug() << "PlayerConnection::pingServer(): ping sent";
	#endif
	
	if(!m_pingDeadTimer.isActive())
		m_pingDeadTimer.start();
	
}

void MainWindow::clientConnected()
{
	m_connectTimer.stop();
		
	#ifdef DEBUG_SOCKET
	qDebug() << "MainWindow::clientConnected()"; //: m_justTesting:"<<m_justTesting;
	#endif
	
	m_pingTimer.start();
	
	m_isConnected = true;
		
// 	if(m_justTesting)
// 	{
// 		sendCommand(QVariantList()
// 			<< "cmd" 	<< Server_Login
// 			<< "user"	<< m_user
// 			<< "pass"	<< m_pass);
// 	}
// 	else
// 	{
// 		if(!m_preconnectionCommandQueue.isEmpty())
// 		{
// 			foreach(QVariant var, m_preconnectionCommandQueue)
// 				m_client->sendMap(var.toMap());
// 			m_preconnectionCommandQueue.clear();
// 		}
// 	
// 		emit connected();
// 	}
}

void MainWindow::disconnectClient()
{
	disconnect(m_client, 0, this, 0);

	m_isConnected = false;
	
	m_pingTimer.stop();

        //m_client->exit();
	m_client->deleteLater();
	m_client = 0;

// 	if(m_justTesting)
// 	{
// 		m_justTesting = false;
// 		emit testResults(m_lastError.isEmpty());
// 			
// 		emit testEnded();
// 	}
// 	else
// 		emit disconnected();
}

void MainWindow::socketError(QAbstractSocket::SocketError socketError)
{
	switch (socketError)
	{
		case QAbstractSocket::RemoteHostClosedError:
			//setError("Remote player closed the connection unexpectedly.","Network Error");
			lostConnection();
			break;
		case QAbstractSocket::HostNotFoundError:
			//QMessageBox::critical(0,"Host Not Found",tr("The host was not found. Please check the host name and port settings."));
			//setError("The host specified was not found.","Host Not Found");
			break;
		case QAbstractSocket::ConnectionRefusedError:
			//setError("The host specified actively refused the connection.","Connection Refused");
			lostConnection();
			break;
		default:
			//QMessageBox::critical(0,"Connection Problem",);
			//setError(m_client->errorString(),"Connection Problem");
			break;
	}
	
// 	if(m_justTesting)
// 		disconnectClient();
}

void MainWindow::lostConnection()
{
	m_connectTimer.stop();
	m_pingTimer.stop();
	
	m_isConnected = false;
	
	//qDebug() << "PlayerConnection::lostConnection: m_autoReconnect:"<<m_autoReconnect; 
// 	if(m_autoReconnect)
// 	{
		QTimer::singleShot(1000,this,SLOT(reconnect()));
// 	}
// 	else
// 	{
// 		disconnectClient();
// 	}
}

void MainWindow::connectTimeout()
{
// 	if(m_autoReconnect)
// 	{
		qDebug() << "MainWindow::connectTimeout: Connect call timed out, attempting reconnect.";
		
		reconnect();
// 	}
// 	else
// 	{
// 		qDebug() << "MainWindow::connectTimeout: Connect call timed out, disconnecting.";
// 		disconnectClient();
// 	}
}

void MainWindow::reconnect()
{
        qDebug() << "PlayerConnection::reconnect: NOT RECONNECTING";
        //qDebug() << "PlayerConnection::reconnect: Attempting to reconnect.";
        //connectClient();
}

void MainWindow::clientDisconnected()
{
	disconnectClient();
}

// void MainWindow::setHost(const QString& value)
// {
// 	m_host = value;
// }

void MainWindow::sendCommand(QVariantList reply)
{
	QVariantMap map;
	if(reply.size() % 2 != 0)
	{
		qDebug() << "MainWindow::sendReply: [WARNING]: Odd number of elelements in reply: "<<reply;
	}
	
	// Add the API version on all outgoing replies
// 	reply << "api" << PLAYER_API_VER;

	for(int i=0; i<reply.size(); i+=2)
	{
		if(i+1 >= reply.size())
			continue;

		QString key = reply[i].toString();
		QVariant value = reply[i+1];

		map[key] = value;
	}


	//qDebug() << "MainWindow::sendReply: [DEBUG] map:"<<map;
	m_client->sendMap(map);
}

//! Process the QVariantMap received from a client connected to the VariantMapServer.
// Start by processing the 'cmd' parameter as a string, matching it to a command name defined in ServerCommandNames.h
void MainWindow::receivedMap(QVariantMap map)
{
	//qDebug() << "MainWindow::receivedMap: "<<map;

	QString cmd = map["cmd"].toString();
	
	if(cmd != Server_DeadPing)
		qDebug() << "MainWindow::receivedMap: [COMMAND]: "<<cmd;

	if(cmd == Server_Login)
	{
// 		if(map["user"].toString() != m_validUser ||
// 		   map["pass"].toString() != m_validPass)
// 		{
// 			qDebug() << "MainWindow::receivedMap: ["<<cmd<<"] Invalid user/pass combo";
// 
// 			sendCommand(QVariantList()
// 				<< "cmd" << Server_Login
// 				<< "status" << "error"
// 				<< "message" << "Invalid username/password.");
// 		}
// 		else
// 		{
// 			m_loggedIn = true;
// 			sendCommand(QVariantList()
// 				<< "cmd" << Server_Login
// 				<< "status" << "success"
// 				<< "version" << m_playerVersionString
// 				<< "ver" << m_playerVersion);
// 
// 
// 			if(m_outputEncoder &&
// 			  !m_outputEncoder->encodingStarted())
// 				m_outputEncoder->startEncoder();
// 
// 		}
	}
	else
	if(cmd == Server_Ping)
	{
// 		if(m_vidSendMgr)
// 		{
// 			QStringList inputs = m_vidSendMgr->videoConnections();
// 
// 			sendCommand(QVariantList()
// 					<< "cmd" << Server_Ping
// 					<< "version" << m_playerVersionString
// 					<< "ver" << m_playerVersion
// 					<< "inputs" << inputs.size());
// 		}
// 		else
// 		{
// 			sendCommand(QVariantList()
// 					<< "cmd" << Server_Ping
// 					<< "version" << m_playerVersionString
// 					<< "ver" << m_playerVersion
// 					<< "inputs" << -1);
// 		}
	}
// 	else
// 	if(!m_loggedIn)
// 	{
// 		sendCommand(QVariantList()
// 				<< "cmd" << Server_Login
// 				<< "status" << "error"
// 				<< "message" << "Not logged in, command will not succeed.");
// 	}
	else
	if(cmd == Server_SetStreamURL)
	{
		QString url = map["url"].toString();
		setStreamURL(url);

		sendCommand(QVariantList()
				<< "cmd" << cmd
				<< "status" << true);
	}
	else
	if(cmd == Server_SetBlackout)
	{
// 		if(m_glWidget)
// 			m_glWidget->fadeBlack(map["flag"].toBool());
 		bool flag = map["flag"].toBool();
		setBlack(flag);

		sendCommand(QVariantList()
				<< "cmd" << Server_SetBlackout
				<< "status" << true);
	}
	else
	if(cmd == Server_SetCrossfadeSpeed)
	{
		int ms = map["ms"].toInt();
		setCrossfadeSpeed(ms);

		sendCommand(QVariantList()
				<< "cmd" << cmd
				<< "status" << true);
	}
	else
	if(cmd == Server_SetIgnoreAR)
	{
		bool flag = map["flag"].toBool();
		setIgnoreAR(flag);

		sendCommand(QVariantList()
				<< "cmd" << cmd
				<< "status" << true);
	}
	else
	if(cmd == Server_SetScreen)
	{
		QRect size = map["rect"].toRect();
		setScreen(size);

		sendCommand(QVariantList()
				<< "cmd" << cmd
				<< "status" << true);
	}
	else
	if(cmd == Server_DeadPing)
	{
		#ifdef DEBUG_SOCKET
		//qDebug() << "PlayerConnection::receivedMap: Ping received";
		#endif
		
		m_pingDeadTimer.stop();
	}
	else
	{
		sendCommand(QVariantList()
				<< "cmd" << cmd
				<< "status" << "error"
				<< "message" << "Unknown command.");
	}
}



void MainWindow::setScreen(QRect size)
{
	if(!size.isEmpty())
	{
		move(size.x(),size.y());
		resize(size.width(),size.height());
		raise();
	}
}

void MainWindow::setIgnoreAR(bool flag)
{
	if(m_glWidget)
		m_glWidget->setAspectRatioMode(flag ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio);
}

void MainWindow::setCrossfadeSpeed(int ms)
{
	//qDebug() << "MainWindow::setCrossfadeSpeed: "<<ms;
	if(m_glWidget)
		m_glWidget->setCrossfadeSpeed(ms);

	if(m_scene)
	{
		GLDrawableList list = m_scene->drawableList();
		foreach(GLDrawable *drawable, list)
			if(GLVideoDrawable *vid = dynamic_cast<GLVideoDrawable*>(drawable))
				vid->setXFadeLength(ms);
	}
	
// 	foreach(GLScene *scene, m_overlays->sceneList())
// 	{
// 		GLDrawableList list = scene->drawableList();
// 		foreach(GLDrawable *drawable, list)
// 			if(GLVideoDrawable *vid = dynamic_cast<GLVideoDrawable*>(drawable))
// 				vid->setXFadeLength(ms);
// 	}

	m_xfadeSpeed = ms;
}

void MainWindow::setBlack(bool flag)
{
        //qDebug() << "MainWindow::setBlack: blackout flag:"<<flag<<", m_isBlack:"<<m_isBlack;
	if(flag == m_isBlack)
		return;

	m_isBlack = flag;
	
	m_glWidget->fadeBlack(flag);
	
}

void MainWindow::setStreamURL(QString url)
{
	m_drw->setUrl(url);
}
