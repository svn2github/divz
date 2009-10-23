#include "QVideoProvider.h"
#include <QFileInfo>
#include <QDebug>
#include <QPixmapCache>

#include "3rdparty/md5/md5.h"

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
			qDebug() << "[REF +] QVideoProvider::providerForFile(): + Found existing provider for file:"<<file<<", refCount:"<<v->m_refCount;
		//v->play();
		return v;
	}
	else
	{
		if(DEBUG_QVIDEOPROVIDER)
			qDebug() << "[REF +] QVideoProvider::providerForFile(): - Creating new provider for file:"<<file;
		QVideoProvider *v = new QVideoProvider(can);
		m_fileProviderMap[can] = v;
		v->m_refCount=1;
		//v->play();

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
	
	
// If icon in disk cache for file, returns the icon. Otherwise, returns a null 
// pixmap and starts a QVideoIconGenerator to generate the icon so it will be 
// in the cache for the next time (stores QDir::tempPath() + "/qvideoprovider/" + md5sum of canonical file path)
QPixmap QVideoProvider::iconForFile(const QString & file)
{
	QFileInfo info(file);
	QString abs = info.absoluteFilePath();
	
	QPixmap cache;
	if(!QPixmapCache::find(abs,cache))
	{
		cache = QVideoIconGenerator::iconForFile(file);
		if(!cache.isNull())
		{
			QPixmapCache::insert(abs,cache);
			return cache;
		}
	}
	
	static QPixmap grayPixmap(64,64);
	if(grayPixmap.isNull())
	{
		grayPixmap.fill(Qt::lightGray);
		QPixmap overlay(":/data/videoframeoverlay.png");
		QPainter paint(&grayPixmap);
		paint.drawPixmap(grayPixmap.rect(), overlay);
		paint.end();
	}
		
		
	return grayPixmap;
}



QPixmap QVideoIconGenerator::iconForFile(const QString & file)
{
	QFileInfo info(file);
	
	QString cacheFiename = cacheFile(info.canonicalFilePath());
	if(QFile(cacheFiename).exists())
	{
		return QPixmap(cacheFiename);
	}
	else
	{
		QVideoProvider * p = QVideoProvider::providerForFile(file);
		
		if(p->isPlaying())
		{
			QPixmap pix = p->pixmap();
			if(!pix.isNull())
			{
				storePixmap(pix,p);
				QVideoProvider::releaseProvider(p);
				return pix;
			}
		}
		
		new QVideoIconGenerator(p);
		return QPixmap();
	}
}

QString QVideoIconGenerator::cacheFile(QVideoProvider *p)
{
	return cacheFile(p->canonicalFilePath());
}

QString QVideoIconGenerator::cacheFile(const QString& canonicalFilePath)
{
	return QString("%1/qvideoframecache_%2").arg(QDir::tempPath()).arg(MD5::md5sum(canonicalFilePath));
}

QVideoIconGenerator::QVideoIconGenerator(QVideoProvider* p) : QObject(), m_provider(p)
{
	p->connectReceiver(this,SLOT(newPixmap(const QPixmap &)));
	p->play();
}

void QVideoIconGenerator::storePixmap(const QPixmap & pixmap, QVideoProvider *p)
{
	QPixmap copy = pixmap;
	QPixmap overlay(":/data/videoframeoverlay.png");
	QPainter paint(&copy);
	paint.drawPixmap(copy.rect(), overlay);
	paint.end();
	
	//qDebug() << "QVideoIconGenerator::storePixmap: Saving to:"<<cacheFile(p);
	copy.save(cacheFile(p), "PNG");
}

void QVideoIconGenerator::newPixmap(const QPixmap & pixmap)
{
	storePixmap(pixmap,m_provider);
	
	m_provider->pause();
	m_provider->disconnectReceiver(this);
	
	QVideoProvider::releaseProvider(m_provider);
	deleteLater();
}

	
QVideoProvider::QVideoProvider(const QString &f) :
	QObject(),
	m_canonicalFilePath(f),
	m_video(new QVideo(this)),
	m_refCount(0),
	m_isValid(true),
	m_playCount(0),
	m_streamStarted(false)
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
	if(!m_streamStarted)
	{
		m_streamStarted = true;
		emit streamStarted();
	}
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
	if(m_playCount>0)
		m_playCount --;
	if(m_playCount <= 0)
	{
		m_streamStarted = false;
		emit streamStopped();
		m_video->stop();
	}
	
	if(DEBUG_QVIDEOPROVIDER)
		qDebug() << "[PLAY -] QVideoProvider::stop(): "<<m_canonicalFilePath<<" m_playCount:"<<m_playCount;
}

void QVideoProvider::play()
{
	m_video->play();
	m_playCount ++;
	if(DEBUG_QVIDEOPROVIDER)
		qDebug() << "[PLAY +] QVideoProvider::play(): "<<m_canonicalFilePath<<" m_playCount:"<<m_playCount;
}
void QVideoProvider::pause()
{
	// dont pause unless all players are paused
	if(m_playCount>0)
		m_playCount --;
	if(DEBUG_QVIDEOPROVIDER)
		qDebug() << "[PLAY -] QVideoProvider::pause(): "<<m_canonicalFilePath<<" m_playCount:"<<m_playCount;
	if(m_playCount <= 0)
	{
		if(DEBUG_QVIDEOPROVIDER)
			qDebug() << "QVideoProvider::pause(): "<<m_canonicalFilePath<<" m_video->pause() hit";
		
		m_video->pause();
		
		m_streamStarted = false;
		emit streamStopped();
	}
	
}
void QVideoProvider::seekTo(int ms)
{
	m_video->seek(ms);
}
