#include "GLVideoReceiverDrawable.h"

#include "VideoReceiver.h"

GLVideoReceiverDrawable::GLVideoReceiverDrawable(QString host, int port, QObject *parent)
	: GLVideoDrawable(parent)
	, m_host("localhost")
	, m_port(7755)
	, m_rx(0)
{
	setHost(host);
	setPort(port);
	
	m_rx = new VideoReceiver();
	setVideoSource(m_rx);
}
	
void GLVideoReceiverDrawable::setHost(const QString& host)
{
	m_rx->connectTo(host,m_port);
	m_host = host;
}

void GLVideoReceiverDrawable::setPort(int port)
{
	m_rx->connectTo(m_host, port);
	m_port = port;
}

