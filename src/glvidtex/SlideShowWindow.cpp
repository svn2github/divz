#include "SlideShowWindow.h"

#include <QtGui>
#include "GLWidget.h"
#include "GLDrawable.h"
#include "GLVideoDrawable.h"
#include "GLRectDrawable.h"

#include "GLPlayerServer.h"
// #include "GLPlayerClient.h"
#include "GLPlayerCommandNames.h"

#include "GLSceneGroup.h"
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


/// SlideShowWindow

SlideShowWindow::SlideShowWindow(QWidget *parent)
	: QWidget(parent)
	, m_group(0)
	, m_scene(0)
	, m_oldScene(0)
	, m_useGLWidget(true)
	, m_glWidget(0)
	, m_graphicsView(0)
	, m_xfadeSpeed(300)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	
	bool verbose = true;
	QString configFile = "player.ini";
	
	if(verbose)
		qDebug() << "SlideShowWindow: Reading settings from "<<configFile;
		
	QSettings settings(configFile,QSettings::IniFormat);
	
	QString str;
	QStringList parts;
	QPoint point;
	
	QString activeGroup = settings.value("config").toString();
	
	str = settings.value("verbose").toString();
	if(!str.isEmpty())
		verbose = str == "true";
	
	if(verbose && !activeGroup.isEmpty())
		qDebug() << "SlideShowWindow: Using config:"<<activeGroup;
	
	#define READ_STRING(key,default) \
		(!activeGroup.isEmpty() ? \
			(!(str = settings.value(QString("%1/%2").arg(activeGroup).arg(key)).toString()).isEmpty() ?  str : \
				settings.value(key,default).toString()) : \
			settings.value(key,default).toString())
			
	#define READ_POINT(key,default) \
		str = READ_STRING(key,default); \
		parts = str.split("x"); \
		point = QPoint(parts[0].toInt(),parts[1].toInt()); \
		if(verbose) qDebug() << "SlideShowWindow: " key ": " << point; 
		
	m_useGLWidget = READ_STRING("compat","false") == "false";
	if(m_useGLWidget)
	{
		m_glWidget = new GLWidget(this);
		layout->addWidget(m_glWidget);
		qDebug() << "SlideShowWindow: Using OpenGL to provide high-quality graphics.";
		
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
		
		qDebug() << "SlideShowWindow: Using vendor-provided stock graphics engine for compatibility with older hardware.";
		
		m_graphicsView->setCursor(Qt::BlankCursor);
	}
	
	
	// Window position and size
	READ_POINT("window-pos","10x10");
	QPoint windowPos = point;
	
	READ_POINT("window-size","640x480");
	QPoint windowSize = point;
	
	if(verbose)
		qDebug() << "SlideShowWindow: pos:"<<windowPos<<", size:"<<windowSize;
	
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
			qDebug() << "SlideShowWindow: flip-h: "<<fliph;
		m_glWidget->setFlipHorizontal(fliph);
		
		bool flipv = READ_STRING("flip-v","false") == "true";
		if(verbose)
			qDebug() << "SlideShowWindow: flip-v: "<<flipv;
		m_glWidget->setFlipVertical(flipv);
		
		// Rotate
		int rv = READ_STRING("rotate","0").toInt();
		if(verbose)
			qDebug() << "SlideShowWindow: rotate: "<<rv;
		
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
				qDebug() << "SlideShowWindow: Error loading alphamask "<<alphaFile;
			else
				m_glWidget->setAlphaMask(alphamask);
		}
	}
	
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
	//qDebug() << "SlideShowWindow: Crossfade speed: "<<m_xfadeSpeed;
	
// 	QString loadGroup = READ_STRING("load-group","");
// 	if(!loadGroup.isEmpty())
// 	{
// 		QFile file(loadGroup);
// 		if (!file.open(QIODevice::ReadOnly)) 
// 		{
// 			qDebug() << "SlideShowWindow: Unable to read group file: "<<loadGroup;
// 		}
// 		else
// 		{
// 			QByteArray array = file.readAll();
// 			
// 			GLSceneGroup *group = new GLSceneGroup();
// 			group->fromByteArray(array);
// 			setGroup(group);
// 			
// 			GLScene *scene = group->at(0);
// 			if(scene)
// 			{
// 				//scene->setGLWidget(this);
// 				setScene(scene);
// 				qDebug() << "SlideShowWindow: [DEBUG]: Loaded File: "<<loadGroup<<", GroupID: "<<group->groupId()<<", SceneID: "<< scene->sceneId();
// 				
// 				if(m_outputEncoder && 
// 				  !m_outputEncoder->encodingStarted())
// 					m_outputEncoder->startEncoder();
// 
// 			}
// 			else
// 			{
// 				qDebug() << "SlideShowWindow: [DEBUG]: Loaded File: "<<loadGroup<<", GroupID: "<<group->groupId()<<" - no scenes found at index 0";
// 			}
// 		}	
// 	}
// 	else
// 	{
// 		QString loadGroup = READ_STRING("collection","");
// 		if(!loadGroup.isEmpty())
// 		{
// 			QFile file(loadGroup);
// 			if (!file.open(QIODevice::ReadOnly)) 
// 			{
// 				qDebug() << "SlideShowWindow: Unable to read group file: "<<loadGroup;
// 			}
// 			else
// 			{
// 				QByteArray array = file.readAll();
// 				
// 				GLSceneGroupCollection *collection = new GLSceneGroupCollection();
// 				collection->fromByteArray(array);
// 				setGroup(collection->at(0));
// 				
// 				if(m_group)
// 				{
// 					GLScene *scene = m_group->at(0);
// 					if(scene)
// 					{
// 						//scene->setGLWidget(this);
// 						setScene(scene);
// 						qDebug() << "SlideShowWindow: [DEBUG]: Loaded File: "<<loadGroup<<", GroupID: "<<m_group->groupId()<<", SceneID: "<< scene->sceneId();
// 						
// 						GLDrawableList list = scene->drawableList();
// 						foreach(GLDrawable *gld, list)
// 							if(gld->playlist()->size() > 0)
// 								gld->playlist()->play();	
// 						
// 						if(m_outputEncoder && 
// 						!m_outputEncoder->encodingStarted())
// 							m_outputEncoder->startEncoder();
// 		
// 					}
// 					else
// 					{
// 						qDebug() << "SlideShowWindow: [DEBUG]: Loaded File: "<<loadGroup<<", GroupID: "<<m_group->groupId()<<" - no scenes found at index 0";
// 					}
// 				}
// 			}	
// 		}
// 	}


	QStringList argList = qApp->arguments();
	if(argList.size() > 1)
	{
		QString dirName = argList.at(1);
		QDir dir(dirName);
		dir.setNameFilters(QStringList() << "*.jpg" << "*.JPG" << "*.jpeg" << "*.png" << "*.PNG");
		QFileInfoList list = dir.entryInfoList(QDir::Files, QDir::Name);
		
		if(m_glWidget)
			m_glWidget->setFboEnabled(false);
		
		//QStringList list = dir.entryList();
		//foreach(QString file, list)
		foreach(QFileInfo info, list)
		{
			//QFileInfo info(QString("%1/%2").arg(dirName,file));
			//if(!info.isFile())
			//	continue;
			//QString ext = info.suffix().toLower();
			//if(ext != "jpg" || ext != "png" || ext != "jpeg")
			//	continue;
			
			QString fullFile = info.absoluteFilePath();
			qDebug() << "SlideShowWindow: Loading "<<fullFile;//<<" (ext:"<<ext<<")";
			GLScene *scene = new GLScene();
			GLImageDrawable *image = new GLImageDrawable(fullFile);
			//image->setCrossFadeMode(GLVideoDrawable::JustFront);
			scene->addDrawable(image);
			m_scenes << scene;
		}
		
		m_currentIdx = -1;
		
		qDebug() << "SlideShowWindow: Loaded"<<m_scenes.size()<<"images";
		if(list.isEmpty())
			qDebug() << "SlideShowWindow: Unable to find any images in folder:"<<dirName;
		else
			setSceneNum(0);
			
		connect(&m_sceneTimer, SIGNAL(timeout()), this, SLOT(timerTick()));
		m_sceneTimer.setInterval(4000);
		m_sceneTimer.start();
	}
	else
	{
		qDebug() << "SlideShowWindow: No director given on command line.";
	}
	
}

void SlideShowWindow::timerTick()
{
	m_currentIdx ++;
	if(m_currentIdx > m_scenes.size()-1)
		m_currentIdx = 0;
	setSceneNum(m_currentIdx);
}

void SlideShowWindow::setSceneNum(int num)
{
	if(num < 0 || num >= m_scenes.size())
	{
		qDebug() << "SlideShowWindow::setSceneNum:"<<num<<": Number out of range.";
		return;
	}
	m_currentIdx = num;
	GLScene *scene = m_scenes.at(num);
	qDebug() << "SlideShowWindow::setSceneNum: Showing image #"<<num;
	setScene(scene);
}

void SlideShowWindow::setGroup(GLSceneGroup *group)
{
	if(m_group)
	{
		delete m_group;
		m_group = 0;
	}
	
	m_group = group;
}

void SlideShowWindow::setScene(GLScene *scene)
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
// 			connect(drawable->playlist(), SIGNAL(currentItemChanged(GLPlaylistItem*)), this, SLOT(currentPlaylistItemChanged(GLPlaylistItem*)));
// 			connect(drawable->playlist(), SIGNAL(playerTimeChanged(double)), this, SLOT(playlistTimeChanged(double)));
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
// 			connect(drawable->playlist(), SIGNAL(currentItemChanged(GLPlaylistItem*)), this, SLOT(currentPlaylistItemChanged(GLPlaylistItem*)));
// 			connect(drawable->playlist(), SIGNAL(playerTimeChanged(double)), this, SLOT(playlistTimeChanged(double)));
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

void SlideShowWindow::opacityAnimationFinished()
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
		
		qDebug() << "SlideShowWindow::opacityAnimationFinished: removing drawable:"<<(QObject*)drawable;	
	}
	
	disconnect(m_oldScene, 0, this, 0);
	m_oldScene = 0;
}
