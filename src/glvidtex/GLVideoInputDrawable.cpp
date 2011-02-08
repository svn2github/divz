#include "GLVideoInputDrawable.h"

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include "../livemix/CameraThread.h"

#include "VideoReceiver.h"
#include <QNetworkInterface>

GLVideoInputDrawable::GLVideoInputDrawable(QString file, QObject *parent)
	: GLVideoDrawable(parent)
	, m_source(0)
	, m_rx(0)
	, m_useNetworkSource(false)
{
	if(!file.isEmpty())
		setVideoInput(file);
	
        //QTimer::singleShot(1500, this, SLOT(testXfade()));
}

GLVideoInputDrawable::~GLVideoInputDrawable()
{
	if(m_rx)
	{
		m_rx->exit();
		delete m_rx;
	}
}

void GLVideoInputDrawable::setVideoConnection(const QString& con)
{
	m_videoConnection = con;
	
	// Example: dev=/dev/video0,input=S-Video0,net=10.0.1.70:8877
	if(con.isEmpty())
		return;
	
	QStringList opts = con.split(",");
	foreach(QString pair, opts)
	{
		QStringList values = pair.split("=");
		if(values.size() < 2)
		{
			qDebug() << "GLVideoInputDrawable::setVideoConnection: Parse error for option:"<<pair;
			continue;
		}
		
		QString name = values[0].toLower();
		QString value = values[1];
		
		if(name == "dev")
		{
			if(!m_localHasError[value])
				setVideoInput(value);
		}
		else
		if(name == "input")
		{
			setCardInput(value);
		}
		else
		if(name == "net" || name == "remote")
		{
			setNetworkSource(value);
		}
		else
		{
			qDebug() << "GLVideoInputDrawable::setVideoConnection: Unknown option:"<<name<<", value:"<<value<<", ignored.";
		}
	}
}

void GLVideoInputDrawable::setNetworkSource(const QString& src)
{
	m_networkSource = src;
		
	if(src.isEmpty())
		return;
		
	QStringList url = src.split(":");
	QString host = url[0];
// 	int port = url.size() > 1 ? url[1].toInt() : 7755;
				
// 	QUrl url(src);
// 	if(!url.isValid())
// 	{
// 		qDebug() << "GLVideoInputDrawable: URL:"<<src<<", Error: Sorry, the URL you entered is not a properly-formatted URL. Please try again.";
// 		return ;
// 	}
	
	//if(!m_thread->connectTo(url.host(), url.port(), url.path(), url.userName(), url.password()))
	
	bool isLocalHost = false;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// use the first non-localhost IPv4 address
	for (int i = 0; i < ipAddressesList.size(); ++i)
	{
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
		    ipAddressesList.at(i).toIPv4Address())
			if(!isLocalHost)
				isLocalHost = host == ipAddressesList.at(i).toString();
	}

	// if we did not find one, use IPv4 localhost
	if (!isLocalHost)
		isLocalHost = host == QHostAddress(QHostAddress::LocalHost).toString();
	
	// If we already tried opening a local device, force to use the network
	if(m_source && m_source->hasError())
	{
		qDebug() << "GLVideoInputDrawable::setNetworkSource: Forcing to use the network for src:"<<src<<" due to local device error.";
		isLocalHost = false;
		m_localHasError[m_videoInput] = true;
	}
	
	if(isLocalHost)
		setUseNetworkSource(false);
	else
		setUseNetworkSource(true);
		
	qDebug() << "GLVideoInputDrawable::setNetworkSource: src:"<<src<<", isLocalHost:"<<isLocalHost;
}

void GLVideoInputDrawable::setUseNetworkSource(bool flag)
{
	m_useNetworkSource = flag;
	
	if(m_networkSource.isEmpty())
		return;
		
	if(flag)
	{
		VideoReceiver *oldRx = m_rx;
// 		if(m_rx)
// 			m_rx->release(this);
			
		//QUrl url(m_networkSource);
		QStringList url = m_networkSource.split(":");
		QString host = url[0];
		int port = url.size() > 1 ? url[1].toInt() : 7755;
		
		m_rx = VideoReceiver::getReceiver(host,port);
		if(!m_rx)
		{
			qDebug() << "GLVideoInputDrawable::setUseNetworkSource: Error connecting to: "<<host<<":"<<port;
		}
		else
		if(m_rx == oldRx)
		{
			// do nothing
		}
		else
		{
			m_rx->registerConsumer(this);
			if(oldRx)
				oldRx->release(this);
			
			setVideoSource(m_rx);
		}
	}
	else
	{
		setVideoInput(videoInput());
		setCardInput(cardInput());
	}
	
}

void GLVideoInputDrawable::testXfade()
{
	qDebug() << "GLVideoInputDrawable::testXfade(): loading input /dev/video1";
	setVideoInput("/dev/video1");
	setCardInput("S-Video");
}
	
bool GLVideoInputDrawable::setVideoInput(const QString& camera)
{
	m_videoInput = camera;
	
	CameraThread *source = CameraThread::threadForCamera(camera.isEmpty() ?  DEFAULT_INPUT : camera);
	
	if(!source)
	{
		qDebug() << "GLVideoInputDrawable::setVideoInput: "<<camera<<" does not exist!";
		return false;
	}
	
	if(m_source == source)
		return true;
	
	qDebug() << "GLVideoInputDrawable::setVideoInput(): camera:"<<camera;
	if(m_source && m_source != source)
	{
		m_source->release(this);
	}
	
	m_source = source;
	m_source->setFps(30);
	m_source->registerConsumer(this);
// 	if(camera == "/dev/video1")
// 		source->setInput("S-Video");
	//usleep(750 * 1000); // This causes a race condition to manifist itself reliably, which causes a crash every time instead of intermitently.
	// With the crash reproducable, I can now work to fix it.
	#ifndef Q_OS_WINDOWS
	m_source->enableRawFrames(true);
	#endif
	
	setVideoSource(source);
	
	return true;
}

QString GLVideoInputDrawable::cardInput()
{
	if(!m_source)
		return QString();
	QStringList list = cardInputs();
	return list[m_source->input()];
}

QStringList GLVideoInputDrawable::cardInputs()
{
	if(!m_source)
		return QStringList();
	return m_source->inputs();	
}

bool GLVideoInputDrawable::deinterlace()
{
	return m_source ? m_source->deinterlace() : false;
}

void GLVideoInputDrawable::setDeinterlace(bool flag)
{
	if(m_source)
		m_source->setDeinterlace(flag);
}

bool GLVideoInputDrawable::setCardInput(const QString& input)
{
	if(!m_source)
		return false;
	return m_source->setInput(input);
}
