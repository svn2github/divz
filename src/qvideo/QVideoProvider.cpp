#include "QVideoProvider.h"
#include <QFileInfo>
#include <QDebug>
#include <QPixmapCache>
#include "AppSettings.h"

#include "3rdparty/md5/md5.h"

#define DEBUG_QVIDEOPROVIDER 0
#define DEBUG_QVIDEOPROVIDER_PLAY 0

#include <QMutex>
#include <QMutexLocker>
QMutex icon_mutex;

#include <QDir>

#define CACHE_DIR "dviz-qvideoframecache"
	
#include "MjpegClient.h"

#include "MimeTypes.h"

#ifdef PHONON_ENABLED
QMap<QString,PhononTuplet*> QVideoProvider::m_phononMap;

// return a PhononTuplet for the file, creating one if doesnt exist
// inc's refCount
bool QVideoProvider::canUsePhonon(const QString & file)
{	
	QString mime = MimeTypes::mimeType(file);
	qDebug() << "QVideoProvider::canUsePhonon("<<file<<"): mime:"<<mime;
	return Phonon::BackendCapabilities::isMimeTypeAvailable(mime);
}

PhononTuplet * QVideoProvider::phononForFile(const QString& file)
{
	QFileInfo inf(file);
	QString can = file.startsWith("http://") ? file : inf.canonicalFilePath();
	//qDebug() << "QVideoProvider::providerForFile: Checking file:"<<file;
	if(m_phononMap.contains(can))
	{
		//qDebug() << "QVideoProvider::providerForFile: Found provider for file:"<<file<<", loading...";
		PhononTuplet *v = m_phononMap[can];
		v->refCount++;
		if(DEBUG_QVIDEOPROVIDER)
			qDebug() << "[REF +] QVideoProvider::phononForFile(): + Found existing provider for file:"<<file<<", refCount:"<<v->refCount;
		//v->play();
		return v;
	}
	else
	{
		if(DEBUG_QVIDEOPROVIDER)
			qDebug() << "[REF +] QVideoProvider::phononForFile(): - Creating new provider for file:"<<file;
		PhononTuplet *v = new PhononTuplet();
		
		v->media = new Phonon::MediaObject();
		v->audio = new Phonon::AudioOutput(Phonon::VideoCategory);
		bool goodPath = 
			Phonon::createPath(v->media, v->audio).isValid();
		
		if(goodPath)
		{
			v->media->setCurrentSource(can);
			
			m_phononMap[can] = v;
			v->refCount=1;
			//v->play();
	
			return v;
		}
		else
		{
			return 0;
		}
	}
}
	
#endif

QMap<QString,QVideoProvider*> QVideoProvider::m_fileProviderMap;

// return a provider for the file, creating one if doesnt exist
// inc's refCount
QVideoProvider * QVideoProvider::providerForFile(const QString & file)
{
	QFileInfo inf(file);
	QString can = file.startsWith("http://") ? file : inf.canonicalFilePath();
	
	if(can.isEmpty())
		return 0;
	
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
// in the cache for the next time (stores AppSettings::cachePath() + "/qvideoprovider/" + md5sum of canonical file path)
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
	
	if(!cache.isNull())
		return cache;
	
	// Dont insert the gray pixmap into cache in case we actually get 
	// a pixmap on the next call to the generator for the same file
	QPixmap grayPixmap(48,48);
	grayPixmap.fill(Qt::lightGray);
	QPixmap overlay(":/data/videoframeoverlay.png");
	QPainter paint(&grayPixmap);
	paint.setRenderHint(QPainter::SmoothPixmapTransform,true);
	paint.drawPixmap(grayPixmap.rect(), overlay);
	paint.end();
		
	return grayPixmap;
}



QPixmap QVideoIconGenerator::iconForFile(const QString & file)
{
	QFileInfo info(file);
	
	QString cacheFiename = cacheFile(file.startsWith("http://") ? file : info.canonicalFilePath());
	if(QFile(cacheFiename).exists())
	{
		return QPixmap(cacheFiename);
	}
	else
	{
		QVideoProvider * p = QVideoProvider::providerForFile(file);
		if(p)
		{
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
		}
		
		return QPixmap();
	}
}

QString QVideoIconGenerator::cacheFile(QVideoProvider *p)
{
	return cacheFile(p->canonicalFilePath());
}

QString QVideoIconGenerator::cacheFile(const QString& canonicalFilePath)
{
	QPixmap cache;
	QDir path(QString("%1/%2").arg(AppSettings::cachePath()).arg(CACHE_DIR));
	if(!path.exists())
		QDir(AppSettings::cachePath()).mkdir(CACHE_DIR);
	
	return QString("%1/%2/%3").arg(AppSettings::cachePath()).arg(CACHE_DIR).arg(MD5::md5sum(canonicalFilePath));
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
	m_streamStarted(false),
	m_mjpeg(0)
{
	if(f.startsWith("http://"))
	{
		m_mjpeg = new MjpegClient();
		QUrl url(f);
		QString path = url.path();
		if(url.hasQuery())
			path += QString("?%1").arg(QString(url.encodedQuery()));
		m_mjpeg->connectTo(url.host(),url.port(),path);
		m_mjpeg->start();
		connect(m_mjpeg, SIGNAL(newImage(QImage)), this, SLOT(newImage(QImage)));
	}
	else
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
}

QVideoProvider::~QVideoProvider()
{
	disconnect(m_video,0,this,0);
	m_video->stop();
	m_video->deleteLater();
	m_video = 0;
	
	if(m_mjpeg)
	{
		m_mjpeg->quit();
		m_mjpeg->wait();
		m_mjpeg->deleteLater();
		m_mjpeg = 0;
	}
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

void QVideoProvider::newImage(QImage image)
{
// 	qDebug() << "QVideoProvider::newImage(): Received new mjpeg image, size:"<<image.size();
	QPixmap pix = QPixmap::fromImage(image);
	newPixmap(pix);
	emit newMjpegPixmap(pix);
}

void QVideoProvider::connectReceiver(QObject * receiver, const char * method)
{
	if(m_mjpeg)
	{
		if (!connect(this, SIGNAL(newMjpegPixmap(QPixmap)), receiver, method))
		{
			qWarning("QVideoProvider::connectReceiver(): error connecting provider %s to %s", m_canonicalFilePath.toAscii().data(), method);
			return;
		}
	}
	else
	{
		if (!connect(m_video, SIGNAL(newPixmap(QPixmap)), receiver, method))
		{
			qWarning("QVideoProvider::connectReceiver(): error connecting provider %s to %s", m_canonicalFilePath.toAscii().data(), method);
			return;
		}
	}
	
	m_receivers.append(receiver);
}

void QVideoProvider::disconnectReceiver(QObject * receiver)
{
	if(receiver)
	{
		if(DEBUG_QVIDEOPROVIDER)
			qDebug() << "QVideoProvider::disconnectReceiver()";
		if(m_mjpeg)
			disconnect(m_mjpeg, 0, receiver, 0);
		else
		if(m_video)
			disconnect(m_video, 0, receiver, 0);
	}
	
	if(receiver)
		m_receivers.removeAll(receiver);
}
//	void newPixmap(const QPixmap & pixmap);
	
void QVideoProvider::stop()
{
	if(m_playCount>0)
		m_playCount --;
	if(DEBUG_QVIDEOPROVIDER || DEBUG_QVIDEOPROVIDER_PLAY)
		qDebug() << "[PLAY -] "<<this<<" QVideoProvider::stop(): "<<m_canonicalFilePath<<" m_playCount:"<<m_playCount;
	if(m_playCount <= 0)
// 	if(stopAllowed())
	{
		if(DEBUG_QVIDEOPROVIDER || DEBUG_QVIDEOPROVIDER_PLAY)
			qDebug() << "[PLAY -] "<<this<<" QVideoProvider::stop(): "<<m_canonicalFilePath<<" m_video->stop() hit";
			
		m_video->stop();
		
		m_streamStarted = false;
		emit streamStopped();
	}
	
}

void QVideoProvider::play()
{
	m_video->play();
	m_playCount ++;
	if(DEBUG_QVIDEOPROVIDER || DEBUG_QVIDEOPROVIDER_PLAY)
		qDebug() << "[PLAY +] "<<this<<" QVideoProvider::play(): "<<m_canonicalFilePath<<" m_playCount:"<<m_playCount;
	m_playCount = m_playCount;
}
void QVideoProvider::pause()
{
//	dont pause unless all players are paused
	if(m_playCount>0)
		m_playCount --;
	if(DEBUG_QVIDEOPROVIDER || DEBUG_QVIDEOPROVIDER_PLAY)
		qDebug() << "[PLAY -] "<<this<<" QVideoProvider::pause(): "<<m_canonicalFilePath<<" m_playCount:"<<m_playCount;
	if(m_playCount <= 0)
//	if(stopAllowed())
	{
		if(DEBUG_QVIDEOPROVIDER || DEBUG_QVIDEOPROVIDER_PLAY)
			qDebug() << "[PLAY -] "<<this<<" QVideoProvider::pause(): "<<m_canonicalFilePath<<" m_video->pause() hit";
		
		m_video->pause();
		
		m_streamStarted = false;
		emit streamStopped();
	}
	
}
void QVideoProvider::seekTo(int ms, int flags)
{
	if(!m_video)
		return;
	m_video->seek(ms,flags);
}

double QVideoProvider::videoClock()
{
	return m_video ? m_video->videoClock() : 0;
}

int QVideoProvider::duration()
{	
	return m_video ? m_video->duration() : 0;
}

bool QVideoProvider::stopAllowed()
{
	foreach(QObject * obj, m_receivers)
	{
		if(obj)
		{
			QVideoConsumer * consumer = dynamic_cast<QVideoConsumer*>(obj);
			if(consumer)
			{
				if(!consumer->allowMediaStop(this))
				{
					qDebug() << "QVideoProvider::stopAllowed(): "<<m_canonicalFilePath<<" Stop rejected by "<<obj;
					return false;
				}
			
			}
		}
	}
	
	qDebug() << "QVideoProvider::stopAllowed(): "<<m_canonicalFilePath<<" Allowing media stop";
	return true;
}
