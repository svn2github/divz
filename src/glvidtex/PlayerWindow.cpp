#include "PlayerWindow.h"

#include <QtGui>
#include "GLWidget.h"

#include "GLPlayerServer.h"
// #include "GLPlayerClient.h"

#include "GLPlayerCommandNames.h"

#include "GLSceneGroup.h"

PlayerWindow::PlayerWindow(QWidget *parent)
	: GLWidget(parent)
	, m_group(0)
	, m_scene(0)
{
	bool verbose = true;
	QString configFile = "player.ini";
	
	if(verbose)
		qDebug() << "PlayerWindow: Reading settings from "<<configFile;
		
	QSettings settings(configFile,QSettings::IniFormat);
	
	QString str;
	QStringList parts;
	QPoint point;
	
	QString activeGroup = settings.value("config").toString();
	
	str = settings.value("verbose").toString();
	if(!str.isEmpty())
		verbose = str == "true";
	
	if(verbose && !activeGroup.isEmpty())
		qDebug() << "PlayerWindow: Using config:"<<activeGroup;
	
	#define READ_STRING(key,default) \
		(!activeGroup.isEmpty() ? \
			(!(str = settings.value(QString("%1/%2").arg(activeGroup).arg(key)).toString()).isEmpty() ?  str : \
				settings.value(key,default).toString()) : \
			settings.value(key,default).toString())
			
	#define READ_POINT(key,default) \
		str = READ_STRING(key,default); \
		parts = str.split("x"); \
		point = QPoint(parts[0].toInt(),parts[1].toInt()); \
		if(verbose) qDebug() << "PlayerWindow: " key ": " << point; 
	
	// Window position and size
	READ_POINT("window-pos","10x10");
	QPoint windowPos = point;
	
	READ_POINT("window-size","640x480");
	QPoint windowSize = point;
	
	if(verbose)
		qDebug() << "PlayerWindow: pos:"<<windowPos<<", size:"<<windowSize;
	
	resize(windowSize.x(),windowSize.y());
	move(windowPos.x(),windowPos.y());
	
	bool frameless = READ_STRING("frameless","true") == "true";
	if(frameless)
		setWindowFlags(Qt::FramelessWindowHint);// | Qt::ToolTip);
	
	// Keystoning / Corner Translations
	READ_POINT("key-tl","0x0");
	setTopLeftTranslation(point);
	
	READ_POINT("key-tr","0x0");
	setTopRightTranslation(point);
	
	READ_POINT("key-bl","0x0");
	setBottomLeftTranslation(point);
	
	READ_POINT("key-br","0x0");
	setBottomRightTranslation(point);
	
	// Brightness/Contrast, Hue/Sat
	setBrightness(READ_STRING("brightness","0").toInt());
	setContrast(READ_STRING("contrast","0").toInt());
	setHue(READ_STRING("hue","0").toInt());
	setSaturation(READ_STRING("saturation","0").toInt());
	
	// Flip H/V
	bool fliph = READ_STRING("flip-h","false") == "true";
	if(verbose)
		qDebug() << "PlayerWindow: flip-h: "<<fliph;
	setFlipHorizontal(fliph);
	
	bool flipv = READ_STRING("flip-v","false") == "true";
	if(verbose)
		qDebug() << "PlayerWindow: flip-v: "<<flipv;
	setFlipVertical(flipv);
	
	// Rotate
	int rv = READ_STRING("rotate","0").toInt();
	if(verbose)
		qDebug() << "PlayerWindow: rotate: "<<rv;
	
	if(rv != 0)
		setCornerRotation(rv == -1 ? GLWidget::RotateLeft  : 
				  rv ==  1 ? GLWidget::RotateRight : 
				             GLWidget::RotateNone);
	
	// Aspet Ratio Mode
	setAspectRatioMode(READ_STRING("ignore-ar","false") == "true" ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio);
	
	// Canvas Size
	READ_POINT("canvas-size","1000x750");
	setCanvasSize(QSizeF((qreal)point.x(),(qreal)point.y()));
	
	// Alpha Mask
	QString alphaFile = READ_STRING("alphamask","");
	if(!alphaFile.isEmpty())
	{
		QImage alphamask(alphaFile);
		if(alphamask.isNull())
			qDebug() << "PlayerWindow: Error loading alphamask "<<alphaFile;
		else
			setAlphaMask(alphamask);
	}
	
	m_validUser = READ_STRING("user","player");
	m_validPass = READ_STRING("pass","player");
	
	
	m_server = new GLPlayerServer();
	
	if (!m_server->listen(QHostAddress::Any,9977)) 
	{
		qDebug() << "GLPlayerServer could not start: "<<m_server->errorString();
	}
	else
	{
		qDebug() << "GLPlayerServer listening on "<<m_server->myAddress();
	}
	
	connect(m_server, SIGNAL(receivedMap(QVariantMap)), this, SLOT(receivedMap(QVariantMap)));
	
// 	// Send test map back to self
// 	QTimer::singleShot(50, this, SLOT(sendTestMap()));
}

void PlayerWindow::sendReply(QVariantList reply)
{
	QVariantMap map;
	if(reply.size() % 2 != 0)
	{
		qDebug() << "PlayerWindow::sendReply: [WARNING]: Odd number of elelements in reply: "<<reply;
	}
	
	for(int i=0; i<reply.size(); i+=2)
	{
		if(i+1 >= reply.size())
			continue;
		
		QString key = reply[i].toString();
		QVariant value = reply[i+1];
		
		map[key] = value;
	}
	
	
	qDebug() << "PlayerWindow::sendReply: [DEBUG] map:"<<map;
	m_server->sendMap(map);
}


void PlayerWindow::receivedMap(QVariantMap map)
{
	qDebug() << "PlayerWindow::receivedMap: "<<map;
	
	QString cmd = map["cmd"].toString();
	if(cmd == GLPlayer_Login)
	{
		if(map["user"].toString() != m_validUser ||
			map["pass"].toString() != m_validPass)
		{
			qDebug() << "PlayerWindow::receivedMap: ["<<cmd<<"] Invalid user/pass combo";
			
			sendReply(QVariantList() 
				<< "cmd" << GLPlayer_Login 
				<< "status" << "error"
				<< "message" << "Invalid username/password.");
		}
		else
		{
			m_loggedIn = true;
			sendReply(QVariantList() 
				<< "cmd" << GLPlayer_Login
				<< "status" << "success"
				<< "version" << m_playerVersionString
				<< "ver" << m_playerVersion);
					      
		}
	}
	else
	if(cmd == GLPlayer_Ping)
	{
		sendReply(QVariantList() 
				<< "cmd" << GLPlayer_Ping
				<< "version" << m_playerVersionString
				<< "ver" << m_playerVersion);
	}
	else
	if(!m_loggedIn)
	{
		sendReply(QVariantList() 
				<< "cmd" << GLPlayer_Login 
				<< "status" << "error"
				<< "message" << "Not logged in, command will not succeed.");
	}
	else
	if(cmd == GLPlayer_SetBlackout)
	{
		/// TODO implement blackout
		sendReply(QVariantList() 
				<< "cmd" << GLPlayer_SetBlackout
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_LoadSlideGroup)
	{
		QByteArray ba = map["data"].toByteArray();
		GLSceneGroup *group = new GLSceneGroup(ba);
		setGroup(group);
		
		if(group->size() > 0)
			setScene(group->at(0));
		
		sendReply(QVariantList() 
				<< "cmd" << GLPlayer_LoadSlideGroup
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_SetSlide)
	{
		if(!m_group)
		{
			sendReply(QVariantList() 
					<< "cmd" << GLPlayer_SetSlide
					<< "status" << "error"
					<< "message" << "No group loaded. First transmit GLPlayer_LoadSlideGroup before calling GLPlayer_SetSlide.");
		}
		else
		{
			int id = map["sceneid"].toInt();
			GLScene *scene = m_group->lookupScene(id);
			if(!scene)
			{
				sendReply(QVariantList() 
					<< "cmd" << GLPlayer_SetSlide
					<< "status" << "error"
					<< "message" << "Invalid SceneID");
			}
			else
			{
				setScene(scene);
				
				sendReply(QVariantList() 
					<< "cmd" << GLPlayer_SetSlide
					<< "status" << true);
			}
		}
	}
	
	
	
// 	if(map["cmd"].toString() != "ping")
// 	{
// 		QVariantMap map2;
// 		map2["cmd"] = "ping";
// 		map2["text"] = "Got a map!";
// 		map2["bday"] = 101010;
// 		
// 		qDebug() << "PlayerWindow::receivedMap: Sending response: "<<map2;
// 		m_server->sendMap(map2);
// 	}
}

// void PlayerWindow::sendTestMap()
// {
// 	qDebug() << "PlayerWindow::sendTestMap: Connecting...";
// 	m_client = new GLPlayerClient();
// 	m_client->connectTo("localhost",9977);
// 	
// 	connect(m_client, SIGNAL(socketConnected()), this, SLOT(slotConnected()));
// 	connect(m_client, SIGNAL(receivedMap(QVariantMap)), this, SLOT(receivedMap(QVariantMap)));
// }
// 
// void PlayerWindow::slotConnected()
// {
// 	QVariantMap map;
// 	map["cmd"] = "testConn";
// 	map["text"] = "Hello, World!";
// 	map["bday"] = 103185;
// 	
// 	qDebug() << "PlayerWindow::slotConnected: Connected, sending map: "<<map;
// 	m_client->sendMap(map);
// }


void PlayerWindow::setGroup(GLSceneGroup *group)
{
	if(m_group)
		delete m_group;
	
	m_group = group;
}

void PlayerWindow::setScene(GLScene *scene)
{
	m_scene = scene;
	
	QList<GLDrawable*> items = drawables();
	
	foreach(GLDrawable *drawable, items)
		removeDrawable(drawable);
	
	GLDrawableList list = m_scene->drawableList();
	foreach(GLDrawable *drawable, list)
		addDrawable(drawable);
}