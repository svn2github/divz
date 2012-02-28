#include "JpegServer.h"

#include "MainWindow.h"
#include <QNetworkInterface>

#include <QPainter>

#define FRAME_WIDTH  640
#define FRAME_HEIGHT 480
#define FRAME_FORMAT QImage::Format_ARGB32_Premultiplied

JpegServer::JpegServer(QObject *parent)
	: QTcpServer(parent)
	, m_scene(0)
	, m_fps(10)
	, m_adaptiveWriteEnabled(true)
	, m_timeAccum(0)
	, m_frameCount(0)
	, m_onlyRenderOnSlideChange(false)
	, m_slideChanged(true)
{
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(generateNextFrame()));
	setFps(m_fps);
}

void JpegServer::setFps(int fps)
{
	m_timer.setInterval(1000/fps);
}

void JpegServer::onlyRenderOnSlideChange(bool flag)
{
	m_onlyRenderOnSlideChange = flag;
}

void JpegServer::slideChanged()
{
	m_slideChanged = true; // will be checked by generateNextFrame(), below
}
	
void JpegServer::setScene(QGraphicsScene *scene)
{
	m_scene = scene;
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
		return;
	}
	
	if(m_onlyRenderOnSlideChange)
	{
		m_slideChanged = false;
		//qDebug() << "JpegServer::generateNextFrame(): Cache fallthru ...";
	}
	
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
	
	if(m_onlyRenderOnSlideChange)
		m_cachedImage = image;
	
// 	QImageWriter writer("frame.png", "png");
// 	writer.write(image);

	m_frameCount ++;
	m_timeAccum  += m_time.elapsed();
	
	if(m_frameCount % m_fps == 0)
	{
		QString msPerFrame;
		msPerFrame.setNum(((double)m_timeAccum) / ((double)m_frameCount), 'f', 2);
	
		qDebug() << "JpegServer::generateNextFrame(): Avg MS per Frame:"<<msPerFrame<<", threadId:"<<QThread::currentThreadId();
	}
			
	if(m_frameCount % (m_fps * 10) == 0)
	{
		m_timeAccum  = 0;
		m_frameCount = 0;
	}
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

