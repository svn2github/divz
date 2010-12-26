#include "PlayerWindow.h"

#include <QtGui>
#include "GLWidget.h"
#include "GLDrawable.h"
#include "GLVideoDrawable.h"

#include "GLPlayerServer.h"
// #include "GLPlayerClient.h"
#include "GLPlayerCommandNames.h"

#include "GLSceneGroup.h"
#include "VideoInputSenderManager.h"
#include "VideoEncoder.h"
#include "VideoSender.h"

#include <QTimer>
#include <QApplication>

class ScaledGraphicsView : public QGraphicsView
{
public:
	ScaledGraphicsView(QWidget *parent=0) : QGraphicsView(parent) 
	{
		setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform );
		setCacheMode(QGraphicsView::CacheBackground);
		setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
		setOptimizationFlags(QGraphicsView::DontSavePainterState);
		setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
		setTransformationAnchor(AnchorUnderMouse);
		setResizeAnchor(AnchorViewCenter);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	} 

protected:
	void resizeEvent(QResizeEvent *)
	{
		adjustViewScaling();
	}

	void adjustViewScaling()
	{
		if(!scene())
			return;
		
		float sx = ((float)width()) / scene()->width();
		float sy = ((float)height()) / scene()->height();

		float scale = qMin(sx,sy);
		setTransform(QTransform().scale(scale,scale));
		//qDebug("Scaling: %.02f x %.02f = %.02f",sx,sy,scale);
		update();
		//m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatioByExpanding);
		//m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
	}
	
};

PlayerWindow::PlayerWindow(QWidget *parent)
	: QWidget(parent)
	, m_group(0)
	, m_scene(0)
	, m_useGLWidget(true)
	, m_glWidget(0)
	, m_graphicsView(0)
	, m_playerVersionString("GLPlayer 0.5")
	, m_playerVersion(15)
	, m_outputEncoder(0)
	, m_xfadeSpeed(300)
{
	m_vidSendMgr = new VideoInputSenderManager();
	m_vidSendMgr->setSendingEnabled(true);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	
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
	
		
	m_useGLWidget = READ_STRING("compat","false") == "false";
	if(m_useGLWidget)
	{
		m_glWidget = new GLWidget(this);
		layout->addWidget(m_glWidget);
		qDebug() << "PlayerWindow: Using OpenGL to provide high-quality graphics.";
		
		m_glWidget->setCursor(Qt::BlankCursor);
	}
	else
	{
		m_graphicsView = new ScaledGraphicsView();
		m_graphicsScene = new QGraphicsScene();
		m_graphicsView->setScene(m_graphicsScene);
		m_graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
		m_graphicsScene->setSceneRect(QRectF(0,0,1000.,750.));
		m_graphicsView->setBackgroundBrush(Qt::black);
		layout->addWidget(m_graphicsView);
		
		qDebug() << "PlayerWindow: Using vendor-provided stock graphics engine for compatibility with older hardware.";
		
		m_graphicsView->setCursor(Qt::BlankCursor);
	}
	
	
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
	
	if(m_useGLWidget)
	{
		
		// Keystoning / Corner Translations
		READ_POINT("key-tl","0x0");
		m_glWidget->setTopLeftTranslation(point);
		
		READ_POINT("key-tr","0x0");
		m_glWidget->setTopRightTranslation(point);
		
		READ_POINT("key-bl","0x0");
		m_glWidget->setBottomLeftTranslation(point);
		
		READ_POINT("key-br","0x0");
		m_glWidget->setBottomRightTranslation(point);
		
		// Brightness/Contrast, Hue/Sat
		m_glWidget->setBrightness(READ_STRING("brightness","0").toInt());
		m_glWidget->setContrast(READ_STRING("contrast","0").toInt());
		m_glWidget->setHue(READ_STRING("hue","0").toInt());
		m_glWidget->setSaturation(READ_STRING("saturation","0").toInt());
		
		// Flip H/V
		bool fliph = READ_STRING("flip-h","false") == "true";
		if(verbose)
			qDebug() << "PlayerWindow: flip-h: "<<fliph;
		m_glWidget->setFlipHorizontal(fliph);
		
		bool flipv = READ_STRING("flip-v","false") == "true";
		if(verbose)
			qDebug() << "PlayerWindow: flip-v: "<<flipv;
		m_glWidget->setFlipVertical(flipv);
		
		// Rotate
		int rv = READ_STRING("rotate","0").toInt();
		if(verbose)
			qDebug() << "PlayerWindow: rotate: "<<rv;
		
		if(rv != 0)
			m_glWidget->setCornerRotation(rv == -1 ? GLRotateLeft  : 
						      rv ==  1 ? GLRotateRight : 
							         GLRotateNone);
		
		// Aspet Ratio Mode
		m_glWidget->setAspectRatioMode(READ_STRING("ignore-ar","false") == "true" ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio);
		
		// Alpha Mask
		QString alphaFile = READ_STRING("alphamask","");
		if(!alphaFile.isEmpty())
		{
			QImage alphamask(alphaFile);
			if(alphamask.isNull())
				qDebug() << "PlayerWindow: Error loading alphamask "<<alphaFile;
			else
				m_glWidget->setAlphaMask(alphamask);
		}
	}
	
	m_validUser = READ_STRING("user","player");
	m_validPass = READ_STRING("pass","player");
	
	// Canvas Size
	READ_POINT("canvas-size","1000x750");
	if(m_useGLWidget)
		m_glWidget->setCanvasSize(QSizeF((qreal)point.x(),(qreal)point.y()));
	else
		m_graphicsScene->setSceneRect(QRectF(0,0,(qreal)point.x(),(qreal)point.y()));
	
	
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
	
	
	
// 	QString outputFile = READ_STRING("output","output.avi");
// 	if(!outputFile.isEmpty())
// 	{
// 		if(!m_glWidget)
// 		{
// 			qDebug() << "PlayerWindow: Unable to output video to"<<outputFile<<" because OpenGL is not enabled. (Set comapt=false in player.ini to use OpenGL)";
// 		}
// 		
// 		QFileInfo info(outputFile);
// 		if(info.exists())
// 		{
// 			int counter = 1;
// 			QString newFile;
// 			while(QFileInfo(newFile = QString("%1-%2.%3")
// 				.arg(info.baseName())
// 				.arg(counter)
// 				.arg(info.completeSuffix()))
// 				.exists())
// 				
// 				counter++;
// 			
// 			qDebug() << "PlayerWindow: Video output file"<<outputFile<<"exists, writing to"<<newFile<<"instead.";
// 			outputFile = newFile;
// 		}
// 		
// 		m_outputEncoder = new VideoEncoder(outputFile, this);
// 		m_outputEncoder->setVideoSource(m_glWidget->outputStream());
// 		//m_outputEncoder->startEncoder();
// 	}
	
	
	
	QString loadGroup = READ_STRING("load-group","");
	if(!loadGroup.isEmpty())
	{
		QFile file(loadGroup);
		if (!file.open(QIODevice::ReadOnly)) 
		{
			qDebug() << "PlayerWindow: Unable to read group file: "<<loadGroup;
		}
		else
		{
			QByteArray array = file.readAll();
			
			GLSceneGroup *group = new GLSceneGroup();
			group->fromByteArray(array);
			setGroup(group);
			
			GLScene *scene = group->at(0);
			if(scene)
			{
				//scene->setGLWidget(this);
				setScene(scene);
				qDebug() << "PlayerWindow: [DEBUG]: Loaded File: "<<loadGroup<<", GroupID: "<<group->groupId()<<", SceneID: "<< scene->sceneId();
				
				if(m_outputEncoder && 
				  !m_outputEncoder->encodingStarted())
					m_outputEncoder->startEncoder();

			}
			else
			{
				qDebug() << "PlayerWindow: [DEBUG]: Loaded File: "<<loadGroup<<", GroupID: "<<group->groupId()<<" - no scenes found at index 0";
			}
		}	
	}
	
	if(m_glWidget)
	{
		VideoSender *sender = new VideoSender(this);
		sender->setVideoSource(m_glWidget->outputStream());
		if(sender->listen(QHostAddress::Any,9978))
		{
			qDebug() << "PlayerWindow: Live monitor available on port 9978";
		}
		else
		{
			qDebug() << "PlayerWindow: [ERROR] Unable start Live Monitor server on port 9978!";
		}
	}
	

// 	// Send test map back to self
// 	QTimer::singleShot(50, this, SLOT(sendTestMap()));

	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	
	/// THIS IS ONLY TO TEST FILE SIZES AND ENCODER PERFORMANCE
	
	//QTimer::singleShot(1000 * 60, qApp, SLOT(quit()));
	
	/// THIS IS ONLY TO TEST FILE SIZES AND ENCODER PERFORMANCE
	
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
	/// NB REMOVE FOR PRODUCTION
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
	
	
	//qDebug() << "PlayerWindow::sendReply: [DEBUG] map:"<<map;
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
					      
				
			if(m_outputEncoder && 
			  !m_outputEncoder->encodingStarted())
				m_outputEncoder->startEncoder();

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
		if(m_glWidget)
			m_glWidget->fadeBlack(map["flag"].toBool());
			
		sendReply(QVariantList() 
				<< "cmd" << GLPlayer_SetBlackout
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_SetCrossfadeSpeed)
	{
		int ms = map["ms"].toInt();
		if(m_glWidget)
			m_glWidget->setCrossfadeSpeed(ms);
		m_xfadeSpeed = ms;
		
		sendReply(QVariantList() 
				<< "cmd" << cmd
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
	else
	if(cmd == GLPlayer_SetLayout)
	{
		/// TODO implement layout setting
		sendReply(QVariantList() 
				<< "cmd" << GLPlayer_SetLayout
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_SetUserProperty  ||
	   cmd == GLPlayer_SetVisibility    ||
	   cmd == GLPlayer_QueryProperty)
	{
		if(!m_scene)
		{
			sendReply(QVariantList() 
					<< "cmd" << cmd
					<< "status" << "error"
					<< "message" << "No scene selected. First transmit GLPlayer_SetSlide before calling GLPlayer_SetUserProperty.");
		}
		else
		{
			int id = map["drawableid"].toInt();
			GLDrawable *gld = m_scene->lookupDrawable(id);
			if(!gld)
			{
				sendReply(QVariantList() 
					<< "cmd" << cmd
					<< "status" << "error"
					<< "message" << "Invalid DrawableID");
			}
			else
			{
				QString name = 
					cmd == GLPlayer_SetVisibility ? "isVisible" : 
					map["name"].toString();
				
				if(name.isEmpty())
				{
					name = QString(gld->metaObject()->userProperty().name());
				}
				
				if(name.isEmpty())
				{
					sendReply(QVariantList() 
						<< "cmd" << cmd
						<< "status" << "error"
						<< "message" << "No property name given in 'name', and could not find a USER-flagged Q_PROPERTY on the GLDrawable requested by 'drawableid'.");
				}
				else
				{
					if(cmd == GLPlayer_QueryProperty)
					{
						sendReply(QVariantList() 
							<< "cmd" << cmd
							<< "drawableid" << id
							<< "name" << name
							<< "value" << gld->property(qPrintable(name)));
					}
					else
					{
						QVariant value = map["value"];
						
						if(GLVideoDrawable *vid = dynamic_cast<GLVideoDrawable*>(gld))
							vid->setXFadeLength(m_xfadeSpeed);
							
						gld->setProperty(qPrintable(name), value);
						
						sendReply(QVariantList() 
							<< "cmd" << cmd
							<< "status" << true);
					}
				}
			}
		}
	}
	else
	if(cmd == GLPlayer_DownloadFile)
	{
		/// TODO implement download file
		sendReply(QVariantList() 
				<< "cmd" << GLPlayer_DownloadFile
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_QueryLayout)
	{
		/// TODO implement query layout
		sendReply(QVariantList() 
				<< "cmd" << GLPlayer_QueryLayout
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_SetViewport)
	{
		QRectF rect = map["viewport"].toRectF();
		if(m_glWidget)
			m_glWidget->setViewport(rect);
		//else
			//m_graphicsScene->setSceneRect(QRectF(0,0,(qreal)point.x(),(qreal)point.y()));
		
		sendReply(QVariantList() 
				<< "cmd" << cmd
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_SetCanvasSize)
	{
		QSizeF size = map["canvas"].toSizeF();
		if(m_glWidget)
			m_glWidget->setCanvasSize(size);
		else
			m_graphicsScene->setSceneRect(QRectF(QPointF(0,0),size));
		
		sendReply(QVariantList() 
				<< "cmd" << cmd
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_SetIgnoreAR)
	{
		bool flag = map["flag"].toBool();
		if(m_glWidget)
			m_glWidget->setAspectRatioMode(flag ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio);
		
		sendReply(QVariantList() 
				<< "cmd" << cmd
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_SetScreen)
	{
		QRect size = map["rect"].toRect();
		if(!size.isEmpty())
		{
			move(size.x(),size.y());
			resize(size.width(),size.height());
		}
			
		sendReply(QVariantList() 
				<< "cmd" << cmd
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_AddSubview)
	{
		if(m_glWidget)
		{
			GLWidgetSubview *view = new GLWidgetSubview();
			
			QByteArray ba = map["data"].toByteArray();
			view->fromByteArray(ba);
		
			if(GLWidgetSubview *oldSubview = m_glWidget->subview(view->subviewId()))
			{
				qDebug() << "PlayerWindow: GLPlayer_AddSubview: Deleting oldSubview: "<<oldSubview; 
				m_glWidget->removeSubview(oldSubview);
				delete oldSubview;
			}
			else
			{
				qDebug() << "PlayerWindow: GLPlayer_AddSubview: NO OLD SUBVIEW DELETED, id:"<<view->subviewId();
			}
		
			qDebug() << "PlayerWindow: GLPlayer_AddSubview: Added subview: "<<view<<", id: "<<view->subviewId();
			m_glWidget->addSubview(view);
		}
		
		sendReply(QVariantList() 
				<< "cmd" << cmd
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_RemoveSubview)
	{
		if(m_glWidget)
		{
			int subviewId = map["subviewid"].toInt();
			if(subviewId>0)
			{
				if(GLWidgetSubview *oldSubview = m_glWidget->subview(subviewId))
				{
					m_glWidget->removeSubview(oldSubview);
					delete oldSubview;
				}	
			}
			else
			{
				GLWidgetSubview *view = new GLWidgetSubview();
				
				QByteArray ba = map["data"].toByteArray();
				view->fromByteArray(ba);
				
				if(GLWidgetSubview *oldSubview = m_glWidget->subview(view->subviewId()))
				{
					m_glWidget->removeSubview(oldSubview);
					delete oldSubview;
				}
			
				delete view;
			}
		}
		
		sendReply(QVariantList() 
				<< "cmd" << cmd
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_ClearSubviews)
	{
		if(m_glWidget)
		{
			QList<GLWidgetSubview*> views = m_glWidget->subviews();
			foreach(GLWidgetSubview *view, views)
			{
				m_glWidget->removeSubview(view);
				delete view;
			}
		}
		
		sendReply(QVariantList() 
				<< "cmd" << cmd
				<< "status" << true);
	}
	else
	if(cmd == GLPlayer_ListVideoInputs)
	{
		QStringList inputs = m_vidSendMgr->videoConnections();
		QVariantList varList;
		foreach(QString str, inputs)
			varList << str;
		QVariant list = varList; // convert to a varient so sendReply() doesnt try to add each element to the map
		sendReply(QVariantList() 
				<< "cmd"  << cmd
				<< "list" << list);
		
	}
	else
	{
		sendReply(QVariantList() 
				<< "cmd" << cmd
				<< "status" << "error"
				<< "message" << "Unknown command.");
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
	{
		delete m_group;
		m_group = 0;
	}
	
	m_group = group;
}

void PlayerWindow::setScene(GLScene *scene)
{
	m_scene = scene;
	
	GLDrawableList newSceneList = m_scene->drawableList();
	
	if(m_glWidget)
	{
		QList<GLDrawable*> items = m_glWidget->drawables();
		foreach(GLDrawable *drawable, items)
		{
			m_glWidget->removeDrawable(drawable);
// 			qDebug() << "PlayerWindow::setScene: Removing old drawable:" <<(QObject*)drawable;
		}
		
		foreach(GLDrawable *drawable, newSceneList)
		{
			m_glWidget->addDrawable(drawable);
// 			qDebug() << "PlayerWindow::setScene: Adding new drawable:" <<(QObject*)drawable;
		}
	}
	else
	{
		QList<QGraphicsItem*> items = m_graphicsScene->items();
		foreach(QGraphicsItem *item, items)
			if(GLDrawable *gld = dynamic_cast<GLDrawable*>(item))
				m_graphicsScene->removeItem(gld);
		
		m_graphicsScene->clear();
		foreach(GLDrawable *drawable, newSceneList)
			m_graphicsScene->addItem(drawable);
	}
}
