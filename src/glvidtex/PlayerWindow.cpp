#include "PlayerWindow.h"

#include <QtGui>
#include "GLWidget.h"
#include "GLDrawable.h"
#include "GLVideoDrawable.h"
#include "GLRectDrawable.h"

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



/// PlayerCompatOutputStream

PlayerCompatOutputStream::PlayerCompatOutputStream(PlayerWindow *parent)
	: VideoSource(parent)
	, m_win(parent)
	, m_fps(5)
{
 	setIsBuffered(false);
	setImage(QImage("dot.gif"));
	
	connect(&m_frameReadyTimer, SIGNAL(timeout()), this, SLOT(renderScene()));
	setFps(m_fps);
	
	m_frameReadyTimer.start();
}

void PlayerCompatOutputStream::setImage(QImage img)
{
	m_image = img;
	
	//qDebug() << "PlayerCompatOutputStream::setImage(): Received frame buffer, size:"<<m_image.size()<<", img format:"<<m_image.format();
	enqueue(new VideoFrame(m_image,1000/m_fps, QTime::currentTime()));
	
	emit frameReady();
}

void PlayerCompatOutputStream::renderScene()
{
	QImage image(320,240,QImage::Format_ARGB32);
	memset(image.scanLine(0),0,image.byteCount());
	QPainter p(&image);
	
	if(m_win->graphicsScene())
		m_win->graphicsScene()->render(&p);
	else
		qDebug() << "PlayerCompatOutputStream::renderScene: No graphics scene available, unable to render.";
	
	p.end();
	
// 	qDebug() << "PlayerCompatOutputStream::renderScene: Image size:"<<image.size();
// 	image.save("comapt.jpg");
	
	setImage(image);
}

void PlayerCompatOutputStream::setFps(int fps)
{
	m_fps = fps;
	m_frameReadyTimer.setInterval(1000/m_fps);
}


/// PlayerWindow

PlayerWindow::PlayerWindow(QWidget *parent)
	: QWidget(parent)
	, m_group(0)
	, m_scene(0)
	, m_oldScene(0)
	, m_useGLWidget(true)
	, m_glWidget(0)
	, m_graphicsView(0)
	, m_playerVersionString("GLPlayer 0.5")
	, m_playerVersion(15)
	, m_outputEncoder(0)
	, m_xfadeSpeed(300)
	, m_compatStream(0)
	, m_isBlack(false)
	, m_blackScene(new GLScene)
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
	{
		m_glWidget->setCanvasSize(QSizeF((qreal)point.x(),(qreal)point.y()));
	}
	else
	{
		QRectF r = QRectF(0,0,(qreal)point.x(),(qreal)point.y());
		m_graphicsScene->setSceneRect(r);
	}
	
	m_xfadeSpeed = READ_STRING("xfade-speed",300).toInt();
	//qDebug() << "PlayerWindow: Crossfade speed: "<<m_xfadeSpeed;
	
	
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
	else
	{
		QString loadGroup = READ_STRING("collection","");
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
				
				GLSceneGroupCollection *collection = new GLSceneGroupCollection();
				collection->fromByteArray(array);
				setGroup(collection->at(0));
				
				if(m_group)
				{
					GLScene *scene = m_group->at(0);
					if(scene)
					{
						//scene->setGLWidget(this);
						setScene(scene);
						qDebug() << "PlayerWindow: [DEBUG]: Loaded File: "<<loadGroup<<", GroupID: "<<m_group->groupId()<<", SceneID: "<< scene->sceneId();
						
						GLDrawableList list = scene->drawableList();
						foreach(GLDrawable *gld, list)
							if(gld->playlist()->size() > 0)
								gld->playlist()->play();	
						
						if(m_outputEncoder && 
						!m_outputEncoder->encodingStarted())
							m_outputEncoder->startEncoder();
		
					}
					else
					{
						qDebug() << "PlayerWindow: [DEBUG]: Loaded File: "<<loadGroup<<", GroupID: "<<m_group->groupId()<<" - no scenes found at index 0";
					}
				}
			}	
		}
	}
	
	if(!m_glWidget)
		m_compatStream = new PlayerCompatOutputStream(this);
		
	int outputPort = READ_STRING("output-port",9978).toInt();
	
	if(outputPort > 0 )
	{
		VideoSender *sender = new VideoSender(this);
		sender->setTransmitSize(320,240);
		//sender->setTransmitFps(5);
		sender->setVideoSource(m_compatStream ? (VideoSource*)m_compatStream : (VideoSource*)m_glWidget->outputStream());
		if(sender->listen(QHostAddress::Any,outputPort))
		{
			qDebug() << "PlayerWindow: Live monitor available on port"<<outputPort;
		}
		else
		{
			qDebug() << "PlayerWindow: [ERROR] Unable start Live Monitor server on port"<<outputPort;
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

	//QTimer::singleShot(1000, this, SLOT(setBlack()));
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
	//qDebug() << "PlayerWindow::receivedMap: "<<map;
	
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
		QStringList inputs = m_vidSendMgr->videoConnections();
		
		sendReply(QVariantList() 
				<< "cmd" << GLPlayer_Ping
				<< "version" << m_playerVersionString
				<< "ver" << m_playerVersion
				<< "inputs" << inputs.size());
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
// 		if(m_glWidget)
// 			m_glWidget->fadeBlack(map["flag"].toBool());
		bool flag = map["flag"].toBool();
		setBlack(flag);

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
		{
			if(m_isBlack)
				m_scenePreBlack = group->at(0);
			else
				setScene(group->at(0));
		}
		
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
				if(m_isBlack)
					m_scenePreBlack = scene;
				else	
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
	if(cmd == GLPlayer_SetUserProperty    ||
	   cmd == GLPlayer_SetVisibility      ||
	   cmd == GLPlayer_QueryProperty      || 
	   cmd == GLPlayer_SetPlaylistPlaying ||
	   cmd == GLPlayer_UpdatePlaylist)
	{
		if(!m_scene)
		{
			sendReply(QVariantList() 
					<< "cmd" << cmd
					<< "status" << "error"
					<< "message" << QString("No scene selected. First transmit GLPlayer_SetSlide before sending %1").arg(cmd));
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
				if(cmd == GLPlayer_SetPlaylistPlaying)
				{
				 	bool flag = map["flag"].toBool();
				 	gld->playlist()->setIsPlaying(flag);
				 	
				}
				else
				if(cmd == GLPlayer_UpdatePlaylist)
				{
					QByteArray ba = map["data"].toByteArray();
					gld->playlist()->fromByteArray(ba);
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

void PlayerWindow::setBlack(bool flag)
{
        //qDebug() << "PlayerWindow::setBlack: blackout flag:"<<flag<<", m_isBlack:"<<m_isBlack; 
	if(flag == m_isBlack)
		return;
		
	m_isBlack = flag;
	
	if(flag)
	{
		m_scenePreBlack = m_scene;
		setScene(m_blackScene);
	}
	else
	{
		setScene(m_scenePreBlack);
	}
	
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
	m_oldScene = m_scene;
	m_scene = scene;
	
	GLDrawableList newSceneList = m_scene->drawableList();
	
	if(m_glWidget)
	{
		if(m_oldScene)
		{
			m_oldScene->setOpacity(0,true,m_xfadeSpeed); // animate fade out
			// remove drawables from oldScene in finished slot
			connect(m_oldScene, SIGNAL(opacityAnimationFinished()), this, SLOT(opacityAnimationFinished()));
		}
		else
		{
			QList<GLDrawable*> items = m_glWidget->drawables();
			foreach(GLDrawable *drawable, items)
			{
				disconnect(drawable->playlist(), 0, this, 0);
				m_glWidget->removeDrawable(drawable);
			}
		}
		
		m_scene->setOpacity(0); // no anim yet...
		
		double maxZIndex = -100000;
		foreach(GLDrawable *drawable, newSceneList)
		{
			connect(drawable->playlist(), SIGNAL(currentItemChanged(GLPlaylistItem*)), this, SLOT(currentPlaylistItemChanged(GLPlaylistItem*)));
			connect(drawable->playlist(), SIGNAL(playerTimeChanged(double)), this, SLOT(playlistTimeChanged(double)));
			m_glWidget->addDrawable(drawable);
			
			if(GLVideoDrawable *vid = dynamic_cast<GLVideoDrawable*>(drawable))
			{
				//qDebug() << "GLWidget mode, item:"<<(QObject*)drawable<<", xfade length:"<<m_xfadeSpeed;
				vid->setXFadeLength(m_xfadeSpeed);
			}
			
			if(drawable->zIndex() > maxZIndex)
				maxZIndex = drawable->zIndex();
		}
		
		m_scene->setOpacity(1,true,m_xfadeSpeed); // animate fade in
	}
	else
	{
		if(m_oldScene)
		{
			m_oldScene->setOpacity(0,true,m_xfadeSpeed); // animate fade out
			// remove drawables from oldScene in finished slot
			connect(m_oldScene, SIGNAL(opacityAnimationFinished()), this, SLOT(opacityAnimationFinished()));
		}
		else
		{
			QList<QGraphicsItem*> items = m_graphicsScene->items();
			foreach(QGraphicsItem *item, items)
			{
				if(GLDrawable *drawable = dynamic_cast<GLDrawable*>(item))
				{
					disconnect(drawable->playlist(), 0, this, 0);
					m_graphicsScene->removeItem(drawable);
				}
			}
		}
		
		//m_graphicsScene->clear();
		m_scene->setOpacity(0); // no anim yet...
		
		foreach(GLDrawable *drawable, newSceneList)
		{
			connect(drawable->playlist(), SIGNAL(currentItemChanged(GLPlaylistItem*)), this, SLOT(currentPlaylistItemChanged(GLPlaylistItem*)));
			connect(drawable->playlist(), SIGNAL(playerTimeChanged(double)), this, SLOT(playlistTimeChanged(double)));
			m_graphicsScene->addItem(drawable);
			
			if(GLVideoDrawable *vid = dynamic_cast<GLVideoDrawable*>(drawable))
			{
				//qDebug() << "QGraphicsView mode, item:"<<(QObject*)drawable<<", xfade length:"<<m_xfadeSpeed;
				vid->setXFadeLength(m_xfadeSpeed);
			}
		}
		
		m_scene->setOpacity(1,true,m_xfadeSpeed); // animate fade in
	}
}

void PlayerWindow::opacityAnimationFinished()
{
	//GLScene *scene = dynamic_cast<GLScene*>(sender());
		
	//disconnect(drawable, 0, this, 0);
	
	if(!m_oldScene)
		return;
		
	GLDrawableList list = m_oldScene->drawableList();
	foreach(GLDrawable *drawable, list)
	{
		if(m_glWidget)
			m_glWidget->removeDrawable(drawable);
		else
			m_graphicsScene->removeItem(drawable);
		
		qDebug() << "PlayerWindow::opacityAnimationFinished: removing drawable:"<<(QObject*)drawable;	
	}
	
	disconnect(m_oldScene, 0, this, 0);
	m_oldScene = 0;
}

void PlayerWindow::currentPlaylistItemChanged(GLPlaylistItem* item)
{
	GLDrawablePlaylist *playlist = dynamic_cast<GLDrawablePlaylist*>(sender());
	if(!playlist)
		return;
		
	GLDrawable *drawable = playlist->drawable();
	if(!drawable)
		return;
	 
	sendReply(QVariantList() 
			<< "cmd"	<< GLPlayer_CurrentPlaylistItemChanged
			<< "drawableid"	<< drawable->id()
			<< "itemid"	<< item->id());
}

void PlayerWindow::playlistTimeChanged(double time)
{
	GLDrawablePlaylist *playlist = dynamic_cast<GLDrawablePlaylist*>(sender());
	if(!playlist)
		return;
		
	GLDrawable *drawable = playlist->drawable();
	if(!drawable)
		return;
	 
	sendReply(QVariantList() 
			<< "cmd"	<< GLPlayer_PlaylistTimeChanged
			<< "drawableid"	<< drawable->id()
			<< "time"	<< time);
}

