#include "QVideoProvider.h"
#include <QFileInfo>
#include <QDebug>

#define DEBUG_QVIDEOPROVIDER 0
	
	
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
		if(DEBUG_QVIDEOPROVIDER)
			qDebug() << "[REFF +] QVideoProvider::providerForFile(): + Found existing provider for file:"<<file<<", refCount:"<<v->m_refCount;
		v->play();
		return v;
	}
	else
	{
		if(DEBUG_QVIDEOPROVIDER)
			qDebug() << "[REFF +] QVideoProvider::providerForFile(): - Creating new provider for file:"<<file;
		QVideoProvider *v = new QVideoProvider(can);
		m_fileProviderMap[can] = v;
		v->m_refCount=1;
		v->play();

		return v;
	}
}
	
// de-inc refcount. If refcount<=0, delete provider
void QVideoProvider::releaseProvider(QVideoProvider *v)
{
	if(!v)
		return;
	v->m_refCount --;
	if(DEBUG_QVIDEOPROVIDER)
		qDebug() << "[REF -] QVideoProvider::releaseProvider(): - Released provider for file:"<<v->m_canonicalFilePath<<", refCount:"<<v->m_refCount;
	if(v->m_refCount <= 0)
	{
		if(DEBUG_QVIDEOPROVIDER)
			qDebug() << "[REF -] QVideoProvider::releaseProvider: DELETING PROVIDER FOR:"<<v->m_canonicalFilePath<<", refCount:"<<v->m_refCount;
		
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
	m_isValid(true),
	m_playCount(0)
{
	if(!m_video->load(f))
	{
		if(DEBUG_QVIDEOPROVIDER)
			qDebug() << "QVideoProvider: ERROR: Unable to load video"<<f;
		m_isValid = false;
	}
	if(m_isValid)
	{
		connect(m_video, SIGNAL(newPixmap(QPixmap)), this, SLOT(newPixmap(QPixmap)));
		m_video->setAdvanceMode(QVideo::Manual);
		m_video->setLooped(true);
		//m_video->play();
	}
}

QVideoProvider::~QVideoProvider()
{
	m_video->stop();
	//disconnect(m_video,0,0,0);
	delete m_video;
	m_video = 0;
}

void QVideoProvider::newPixmap(const QPixmap & pix)
{
	m_lastPixmap = pix;
}

void QVideoProvider::connectReceiver(QObject * receiver, const char * method)
{
	if (!connect(m_video, SIGNAL(newPixmap(QPixmap)), receiver, method))
	{
		qWarning("QVideoProvider::connectReceiver(): error connecting provider %s to %s", m_canonicalFilePath.toAscii().data(), method);
		return;
	}
}

void QVideoProvider::disconnectReceiver(QObject * receiver)
{
	if(receiver && m_video)
	{
		if(DEBUG_QVIDEOPROVIDER)
			qDebug() << "QVideoProvider::disconnectReceiver()";
		disconnect(m_video, 0, receiver, 0);
	}
}
//	void newPixmap(const QPixmap & pixmap);
	
void QVideoProvider::stop()
{
	m_video->stop();
	m_playCount --;
	if(DEBUG_QVIDEOPROVIDER)
		qDebug() << "[PLAY -] QVideoProvider::stop(): m_playCount:"<<m_playCount;
}

void QVideoProvider::play()
{
	m_video->play();
	m_playCount ++;
	if(DEBUG_QVIDEOPROVIDER)
		qDebug() << "[PLAY +] QVideoProvider::play(): m_playCount:"<<m_playCount;
}
void QVideoProvider::pause()
{
	// dont pause unless all players are paused
	//if(m_playCount > 0)
		m_playCount --;
	if(DEBUG_QVIDEOPROVIDER)
		qDebug() << "[PLAY -] QVideoProvider::pause(): m_playCount:"<<m_playCount;
	if(m_playCount <= 0)
	{
		if(DEBUG_QVIDEOPROVIDER)
			qDebug() << "QVideoProvider::pause(): m_video->pause() hit";
		m_video->pause();
	}
	
}
void QVideoProvider::seekTo(int ms)
{
	m_video->seek(ms);
}
