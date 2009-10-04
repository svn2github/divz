#include "QVideoProvider.h"
#include <QFileInfo>
#include <QDebug>
	
	
QMap<QString,QVideoProvider*> QVideoProvider::m_fileProviderMap;

// return a provider for the file, creating one if doesnt exist
// inc's refCount
QVideoProvider * QVideoProvider::providerForFile(const QString & file)
{
	QFileInfo inf(file);
	QString can = inf.canonicalFilePath();
	//qDebug() << "QVideoProvider::providerForFile: Checking file:"<<file;
	if(m_fileProviderMap.contains(can))
	{
		//qDebug() << "QVideoProvider::providerForFile: Found provider for file:"<<file<<", loading...";
		QVideoProvider *v = m_fileProviderMap[can];
		v->m_refCount++;
		qDebug() << "QVideoProvider::providerForFile: + Found existing provider for file:"<<file<<", refCount:"<<v->m_refCount;
		return v;
	}
	else
	{
		qDebug() << "QVideoProvider::providerForFile: - Creating new provider for file:"<<file;
		QVideoProvider *v = new QVideoProvider(can);
		m_fileProviderMap[can] = v;
		v->m_refCount=1;
		return v;
	}
}
	
// de-inc refcount. If refcount<=0, delete provider
void QVideoProvider::releaseProvider(QVideoProvider *v)
{
	if(!v)
		return;
	v->m_refCount --;
	qDebug() << "QVideoProvider::releaseProvider: Released provider for file:"<<v->m_canonicalFilePath<<", refCount:"<<v->m_refCount;
	if(v->m_refCount < 0)
	{
		qDebug() << "QVideoProvider::releaseProvider: DELETING PROVIDER FOR:"<<v->m_canonicalFilePath<<", refCount:"<<v->m_refCount;
		
		m_fileProviderMap.remove(v->m_canonicalFilePath);
		delete v;
		v=0;
	}
}
	
	
QVideoProvider::QVideoProvider(const QString &f) :
	QObject(),
	m_canonicalFilePath(f),
	m_video(new QVideo(this)),
	m_refCount(0),
	m_isValid(true)
{
	if(!m_video->load(f))
	{
		qDebug() << "QVideoProvider: ERROR: Unable to load video"<<f;
		m_isValid = false;
	}
	if(m_isValid)
	{
		m_video->setAdvanceMode(QVideo::Manual);
		m_video->setLooped(true);
		m_video->play();
	}
}

QVideoProvider::~QVideoProvider()
{
	m_video->stop();
	delete m_video;
	m_video = 0;
}

void QVideoProvider::connectReceiver(QObject * receiver, const char * method)
{
	if (!connect(m_video, SIGNAL(newPixmap(QPixmap)), receiver, method)) {
		qWarning("QVideoProvider::connectReceiver: error connecting provider %s to %s", m_canonicalFilePath.toAscii().data(), method);
		return;
	}
}

void QVideoProvider::disconnectReceiver(QObject * receiver)
{
	disconnect(m_video, 0, receiver, 0);
}
//	void newPixmap(const QPixmap & pixmap);
	
void QVideoProvider::stop()
{
	m_video->stop();
}

void QVideoProvider::play()
{
	m_video->play();
}
void QVideoProvider::pause()
{
	m_video->pause();
}
void QVideoProvider::seekTo(int ms)
{
	m_video->seek(ms);
}
