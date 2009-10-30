#include "Output.h"
#include <QSettings>
#include <QDataStream>
#include <QIODevice>
#include <QApplication>
#include <QDesktopWidget>

Output * Output::m_staticPreviewInstance = new Output(Output::Preview);

Output::Output(Output::OutputType type)
{
	setupDefaults();
	generateId();
	setOutputType(type);
}

Output::Output()
{
	setupDefaults();
	generateId();
}

void Output::setupDefaults()
{
	m_isSystem		= false;
	m_isEnabled		= true;
	m_name			= "Untitled Output";
	m_outputType		= Custom;
	m_screenNum		= 0;
	m_customRect		= QRect(0,0,1024,768);
	m_networkRole		= Server;
	m_host			= "";
	m_port			= 7777;
	m_allowMultiple		= true;
	m_tags			= "";
	m_id			= -1;
	m_stayOnTop		= true;
	m_mjpegServerEnabled	= false;
	m_mjpegServerPort	= 8080;
	m_mjpegServerFPS	= 7;
}

void Output::generateId()
{
	QSettings x;
	int id = x.value("output/max-id",100).toInt();
	id++;
	x.setValue("output/max-id",id);
	m_id = id;
}

int Output::id()
{
	if(m_id<=0)
		generateId();
	return m_id;
}

void Output::setIsSystem(bool x) { m_isSystem = x; }
void Output::setIsEnabled(bool x) { m_isEnabled = x; }

void Output::setName(QString name) { m_name = name; }

void Output::setOutputType(OutputType x) { m_outputType = x; }

void Output::setScreenNum(int x) { m_screenNum = x; }

void Output::setCustomRect(QRect x) { m_customRect = x; }

void Output::setNetworkRole(NetworkRole x) { m_networkRole = x; }

void Output::setHost(const QString&x) { m_host = x; }

void Output::setPort(int x) { m_port = x; }

void Output::setAllowMultiple(bool x) { m_allowMultiple = x; }

void Output::setTags(QString x) { m_tags = x; }

void Output::setStayOnTop(bool x) { m_stayOnTop = x; }
void Output::setMjpegServerEnabled(bool x) { m_mjpegServerEnabled = x; }
void Output::setMjpegServerPort(int x) { m_mjpegServerPort = x; }
void Output::setMjpegServerFPS(int x) { m_mjpegServerFPS = x; }


QByteArray Output::toByteArray()
{
	QByteArray array;
	QDataStream b(&array, QIODevice::WriteOnly);

	b << QVariant(isSystem());
	b << QVariant(isEnabled());
	b << QVariant(name());
	b << QVariant((int)outputType());
	b << QVariant(screenNum());
	b << QVariant(customRect());
	b << QVariant((int)networkRole());
	b << QVariant(host());
	b << QVariant(port());
	b << QVariant(allowMultiple());
	b << QVariant(tags());
	b << QVariant(id());
	b << QVariant(stayOnTop());
	b << QVariant(mjpegServerEnabled());
	b << QVariant(mjpegServerPort());
	b << QVariant(mjpegServerFPS());
	
	return array;
}

void Output::fromByteArray(QByteArray array)
{
	QDataStream b(&array, QIODevice::ReadOnly);
	QVariant x;

	b >> x; setIsSystem(x.toBool());
	b >> x; setIsEnabled(x.toBool());
	b >> x; setName(x.toString());
	b >> x; setOutputType((OutputType)x.toInt());
	b >> x; setScreenNum(x.toInt());
	b >> x; setCustomRect(x.toRect());
	b >> x; setNetworkRole((NetworkRole)x.toInt());
	b >> x; setHost(x.toString());
	b >> x; setPort(x.toInt());
	b >> x; setAllowMultiple(x.toBool());
	b >> x; setTags(x.toString());
	b >> x; 
		if(!x.isNull())
			m_id = x.toInt();
	
	b >> x; setStayOnTop(x.isNull() ? true : x.toBool());
	
	b >> x; setMjpegServerEnabled(x.isNull() ? false : x.toBool());
	b >> x; setMjpegServerPort(x.isNull() ? 8080     : x.toInt());
	b >> x; setMjpegServerFPS(x.isNull() ? 7         : x.toInt());
}

void Output::setInstance(OutputInstance *instance)
{
	m_instance = instance;
}

double Output::aspectRatio()
{

	QRect r;
	bool isValid = false;
	if(outputType() == Output::Custom)
	{
		r = customRect();
		isValid = true;
	}
	else
	{
		r = QApplication::desktop()->screenGeometry(screenNum());
		isValid = true;
	}

	if(isValid)
		return (double)r.width() / (double)r.height();
	else
		return -1;
}
