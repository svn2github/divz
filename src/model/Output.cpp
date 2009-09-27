#include "Output.h"

#include <QDataStream>
#include <QIODevice>

Output::Output() :
	m_isSystem(false)
	, m_isEnabled(true)
	, m_name("Untitled Output")
	, m_outputType(Custom)
	, m_screenNum(0)
	, m_customRect(QRect(0,0,1024,768))
	, m_networkRole(Server)
	, m_host("")
	, m_port(7777)
	, m_allowMultiple(true)
{}


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
}
