#include "JpegServer.h"

#include "MainWindow.h"
#include <QNetworkInterface>

#include "MyGraphicsScene.h"
#include "glvidtex/VideoSender.h"

#include <QPainter>

#define FRAME_WIDTH  1024
#define FRAME_HEIGHT 768
#define FRAME_FORMAT QImage::Format_ARGB32
//_Premultiplied

JpegServer::JpegServer(QObject *parent)
	: QTcpServer(parent)
	, m_scene(0)
	, m_fps(10)
	, m_adaptiveWriteEnabled(true)
	, m_timeAccum(0)
	, m_frameCount(0)
	, m_onlyRenderOnSlideChange(false)
	, m_slideChanged(true)
	, m_sender(0)
{
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(generateNextFrame()));
	setFps(m_fps);
}

JpegServer::~JpegServer()
{
	if(m_sender)
	{
		delete m_sender;
		m_sender = 0;
	}
}

void JpegServer::setFps(int fps)
{
	m_fps = fps; 
	if(fps < 1)
		fps = 1;
	m_timer.setInterval(1000/fps);
}

void JpegServer::onlyRenderOnSlideChange(bool flag)
{
	m_onlyRenderOnSlideChange = flag;
}

void JpegServer::slideChanged()
{
	m_slideChanged = true; // will be checked by generateNextFrame(), below
	//generateNextFrame();
}
	
void JpegServer::setScene(MyGraphicsScene *scene)
{
	if(m_scene)
		disconnect(m_scene, 0, this, 0);
		
	m_scene = scene;
	connect(m_scene, SIGNAL(transitionFinished(Slide*)), this, SLOT(generateNextFrame()));
}

bool JpegServer::start(int port, bool isVideoSender)
{
	if(!isVideoSender)
	{
		if(!listen(QHostAddress::Any,port))
		{
			qDebug() << "JpegServer::start(): Unable to open MJPEG server on port"<<port<<": Is it already in use?";
			return false; 
		}
		
		qDebug() << "JpegServer::start(): Started MJPEG server on port "<<port;
	}
	else
	{
		m_sender = new VideoSender();
		m_sender->setTransmitFps(-1); // auto fps
		m_sender->setTransmitSize(QSize()); // transmit original size, no scaling
		if(!m_sender->listen(QHostAddress::Any,port))
		{
			qDebug() << "JpegServer::start(): Unable to open VideoSender server on port"<<port<<": Is it already in use?";
			return false; 
		}
		
		qDebug() << "JpegServer::start(): Started LivePro-compatible VideoSender server on port "<<port;
	}
	
	return true;
}


void JpegServer::incomingConnection(int socketDescriptor)
{
	JpegServerThread *thread = new JpegServerThread(socketDescriptor, m_adaptiveWriteEnabled);
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(this, SIGNAL(frameReady(QImage)), thread, SLOT(imageReady(QImage)), Qt::QueuedConnection);
	thread->start();
	qDebug() << "JpegServer: Client Connected, Socket Descriptor:"<<socketDescriptor;
	
	
	thread->moveToThread(thread);
	
	if(!m_timer.isActive())
		m_timer.start();
}

void JpegServer::updateRects()
{
	QRect targetFrame(0,0, FRAME_WIDTH, FRAME_HEIGHT);
	QSize nativeSize = m_sourceRect.size();
	nativeSize.scale(targetFrame.size(), Qt::KeepAspectRatio);
	
	m_targetRect = QRect(0, 0, nativeSize.width(), nativeSize.height());
	m_targetRect.moveCenter(targetFrame.center());
}

void JpegServer::generateNextFrame()
{
	if(!m_scene || !MainWindow::mw())
		return;
		
	if(m_onlyRenderOnSlideChange &&
	   !m_slideChanged &&
	   !m_cachedImage.isNull())
	{
		//qDebug() << "JpegServer::generateNextFrame(): Hit Cache";
		emit frameReady(m_cachedImage);
		if(m_sender)
			m_sender->transmitImage(m_cachedImage);
		return;
	}
	
	if(m_onlyRenderOnSlideChange)
	{
		m_slideChanged = false;
		//qDebug() << "JpegServer::generateNextFrame(): Cache fallthru ...";
	}
	
	//qDebug() << "JpegServer::generateNextFrame(): Rendering scene "<<m_scene<<", slide:"<<m_scene->slide();
	
	m_time.start();
	
	QImage image(FRAME_WIDTH,
	             FRAME_HEIGHT,
		     FRAME_FORMAT);
	memset(image.scanLine(0), 0, image.byteCount());
	
	QPainter painter(&image);
	painter.fillRect(image.rect(),Qt::transparent);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setRenderHint(QPainter::TextAntialiasing, false);
	
	if(!m_sourceRect.isValid())
		m_sourceRect = MainWindow::mw()->standardSceneRect();
		
	if(m_sourceRect != m_targetRect)
		updateRects();
	
	m_scene->render(&painter,
		m_targetRect,
		m_sourceRect);
	
	painter.end();
	
 	emit frameReady(image);
	
	if(m_sender)
	{
		//qDebug() << "JpegServer::generateNextFrame(): Sending image via VideoSender";
		m_sender->transmitImage(image);
	}
	else
	{
		//qDebug() << "JpegServer::generateNextFrame(): No VideoSender created";
	}
	
	if(m_onlyRenderOnSlideChange)
		m_cachedImage = image;
	
// 	QImageWriter writer("frame.png", "png");
// 	writer.write(image);

	m_frameCount ++;
	m_timeAccum  += m_time.elapsed();
	
// 	if(m_frameCount % (m_fps?m_fps:10) == 0)
// 	{
// 		QString msPerFrame;
// 		msPerFrame.setNum(((double)m_timeAccum) / ((double)m_frameCount), 'f', 2);
// 	
// 		qDebug() << "JpegServer::generateNextFrame(): Avg MS per Frame:"<<msPerFrame<<", threadId:"<<QThread::currentThreadId();
// 	}
// 			
// 	if(m_frameCount % ((m_fps?m_fps:10) * 10) == 0)
// 	{
// 		m_timeAccum  = 0;
// 		m_frameCount = 0;
// 	}
	
	//qDebug() << "JpegServer::generateNextFrame(): Done rendering "<<m_scene;
}

/** Thread **/
#define BOUNDARY "JpegServerThread-uuid-0eda9b03a8314df4840c97113e3fe160"
#include <QImageWriter>
#include <QImage>

JpegServerThread::JpegServerThread(int socketDescriptor, bool adaptiveWriteEnabled, QObject *parent)
    : QThread(parent)
    , m_socketDescriptor(socketDescriptor)
    , m_adaptiveWriteEnabled(adaptiveWriteEnabled)
{
	
}

JpegServerThread::~JpegServerThread()
{
	m_socket->abort();
	delete m_socket;
}

void JpegServerThread::run()
{
	m_socket = new QTcpSocket();
	
	if (!m_socket->setSocketDescriptor(m_socketDescriptor)) 
	{
		emit error(m_socket->error());
		return;
	}
	
	writeHeaders();
	
	m_writer.setDevice(m_socket);
	m_writer.setFormat("png");
	m_writer.setQuality(50);

	m_adaptiveIgnore = 0;
	
	// enter event loop
	exec();
	
	// when imageReady() signal arrives, write data with header to socket
}

void JpegServerThread::writeHeaders()
{
	m_socket->write("HTTP/1.0 200 OK\r\n");
	m_socket->write("Server: DViz/JpegServer - Josiah Bryan <josiahbryan@gmail.com>\r\n");
	m_socket->write("Content-Type: multipart/x-mixed-replace;boundary=" BOUNDARY "\r\n");
	m_socket->write("\r\n");
	m_socket->write("--" BOUNDARY "\r\n");
}

void JpegServerThread::imageReady(QImage image)
{
	static int frameCounter = 0;
 	frameCounter++;
//  	qDebug() << "JpegServerThread: [START] Writing Frame#:"<<frameCounter;
	
	if(m_adaptiveWriteEnabled && m_socket->bytesToWrite() > 0 && m_adaptiveIgnore < 30)
	{
		qDebug() << "JpegServerThread::imageReady():"<<m_socket->bytesToWrite()<<"bytes pending write on socket, not sending image"<<frameCounter;
		m_adaptiveIgnore ++;
	}
	else
	{
		qDebug() << "JpegServerThread::imageReady(): Sending image"<<frameCounter<<", threadId:"<<QThread::currentThreadId();
		m_adaptiveIgnore = 0;
		//if(image.format() != QImage::Format_RGB32)
		//	image = image.convertToFormat(QImage::Format_RGB32);
		
		if(m_socket->state() == QAbstractSocket::ConnectedState)
		{
			//m_socket->write("Content-type: image/jpeg\r\n\r\n");
			m_socket->write("Content-type: image/png\r\n\r\n");
		}
		
		//QImageWriter writer(m_socket, "png");
		//writer.write(image);
	
		if(!m_writer.write(image))
		{
			qDebug() << "ImageWriter reported error:"<<m_writer.errorString();
			quit();
		}
		
		m_socket->write("--" BOUNDARY "\r\n");
	}

}

