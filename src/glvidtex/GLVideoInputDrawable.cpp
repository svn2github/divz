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

void GLVideoInputDrawable::setVideoConnection(const QString& con)
{
	m_videoConnection = con;
	
	// Example: dev=/dev/video0,input=S-Video0,deint=true,net=10.0.1.70:8877
	
	QStringList opts = con.split(",");
	foreach(QString pair, opts)
	{
		QStringList values = pair.split("=");
		QString name = values[0].toLowerCase();
		QString value = values[1];
		
		if(name == "dev")
		{
			setVideoInput(value);
		}
		else
		if(name == "input")
		{
			setCardInput(value);
		}
		else
		if(name == "deint" || name == "di" || name == "deinterlace")
		{
			setDeinterlace(value == "true");
		}
		else
		if(name == "net" || name == "remote")
		{
			setNetworkSource(src);
		}
	}
}

void GLVideoInputDrawable::setNetworkSource(const QString& src)
{
	m_networkSource = src;
	
	QUrl url(src);
	if(!url.isValid())
	{
		qDebug() << "GLVideoInputDrawable: URL:"<<src<<", Error: Sorry, the URL you entered is not a properly-formatted URL. Please try again.";
		return ;
	}
	
	//if(!m_thread->connectTo(url.host(), url.port(), url.path(), url.userName(), url.password()))
	
	bool isLocalHost = false;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// use the first non-localhost IPv4 address
	for (int i = 0; i < ipAddressesList.size(); ++i)
	{
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
		    ipAddressesList.at(i).toIPv4Address())
			if(!isLocalHost)
				isLocalHost = url.host() == ipAddressesList.at(i).toString();
	}

	// if we did not find one, use IPv4 localhost
	if (ipAddress.isEmpty() && !isLocalHost)
		isLocalHost = url.host() == QHostAddress(QHostAddress::LocalHost).toString();
		
	if(isLocalHost)
		setUseNetworkSource(false);
	else
		setUseNetworkSource(true);
}

void GLVideoInputDrawable::setUseNetworkSource(bool flag)
{
	m_useNetworkSource = flag;
	
	if(m_networkSource.isEmpty())
		return;
		
	if(flag)
	{
		if(!m_rx)
			m_rx = new VideoReceiver();
		
		QUrl url(m_networkSource);
		m_rx->connectTo(url.host(), url.port()>0 ? url.port() : 7755);
		
		setVideoSource(m_rx);
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
	qDebug() << "GLVideoInputDrawable::setVideoInput(): camera:"<<camera;
	
	CameraThread *source = CameraThread::threadForCamera(camera.isEmpty() ?  DEFAULT_INPUT : camera);
	
	if(!source)
	{
		qDebug() << "GLVideoInputDrawable::setVideoInput: "<<camera<<" does not exist!";
		return false;
	}
	
	m_source = source;
	m_source->setFps(30);
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
