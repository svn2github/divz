#ifndef QVIDEOPROVIDER_H
#define QVIDEOPROVIDER_H
#include <QObject>
#include <QMap>
#include <QPixmap>
class QVideo;

#ifdef PHONON_ENABLED

#include <phonon/AudioOutput>
#include <phonon/SeekSlider>
#include <phonon/MediaObject>
#include <phonon/VolumeSlider>
#include <phonon/BackendCapabilities>
#include <phonon/Effect>
#include <phonon/EffectParameter>
#include <phonon/ObjectDescriptionModel>
#include <phonon/AudioOutput>
#include <phonon/MediaSource>
#include <phonon/VideoWidget>
#include <phonon/VideoPlayer>

#endif

class MjpegClient;


class QVideoProvider;

class QVideoConsumer
{
public:
	virtual ~QVideoConsumer() {};
// 	bool isMediaInUse();
// 	bool allowMediaPause();
	virtual bool allowMediaStop(QVideoProvider*) { return true; }
};

class QVideoIconGenerator : public QObject
{
	Q_OBJECT
	
public /*static*/:
	// If icon in disk cache for file, returns the icon. Otherwise, returns a null 
	// pixmap and starts a QVideoIconGenerator to generate the icon so it will be 
	// in the cache for the next time (stores QDir::tempPath() + "/qvideoprovider/" + md5sum of canonical file path)
	static QPixmap iconForFile(const QString&);
	
	// Moved these methods public because there is some value in allowing users of this class to find out
	// where the cache file is actually stored, such as in the phonon/VideoSlideGroup
	static QString cacheFile(QVideoProvider *);
	static QString cacheFile(const QString&);

private:
	static void storePixmap(const QPixmap&, QVideoProvider *);

	QVideoIconGenerator(QVideoProvider*);
	
private slots:
	void newPixmap(const QPixmap &);

private:
	QVideoProvider * m_provider;

};

#ifdef PHONON_ENABLED
class PhononTuplet 
{
public:
	PhononTuplet()
		: media(0)
		, audio(0) {}
		
	bool isValid() { return media && audio; }
	Phonon::MediaObject *media;
	Phonon::AudioOutput *audio;
	
	Phonon::VideoWidget *video() 
	{
		Phonon::VideoWidget * video = new Phonon::VideoWidget();
		if(Phonon::createPath(media, video).isValid())
		{
			return video;
		}
		else
		{
			qDebug("PhononTuplet: Unable to create video path");
			delete video;
			return 0;
		}
	}
	
	int refCount;
};
#endif

class QVideoProvider : public QObject
{
	Q_OBJECT
public:
	// return a provider for the file, creating one if doesnt exist
	// inc's refCount
	static QVideoProvider * providerForFile(const QString &);
	
	// de-inc refcount. If refcount<=0, delete provider
	static void releaseProvider(QVideoProvider*);
	
	// Returns the first cached frame of the video or Qt::lightGray pixmap if no frame cached.
	static QPixmap iconForFile(const QString &);

#ifdef PHONON_ENABLED
	// Returns true if Phonon is available and supports the mime type for the given file
	static bool canUsePhonon(const QString&);
	
	// Returns the PhononTuplet for the given file 
	static PhononTuplet * phononForFile(const QString&);
#endif

private:
	static QMap<QString,QVideoProvider*> m_fileProviderMap;

#ifdef PHONON_ENABLED
	static QMap<QString,PhononTuplet*> m_phononMap;
#endif


public:
	~QVideoProvider();
	
	bool isPlaying();
	
	void connectReceiver(QObject * receiver, const char * method);
	void disconnectReceiver(QObject * receiver);
	
	int refCount() { return m_refCount; }
	
	bool isValid() { return m_isValid; }
	
	QPixmap pixmap() { return m_lastPixmap; }
	
	QString canonicalFilePath() { return m_canonicalFilePath; }
	
	bool isStreamStarted() { return m_streamStarted; }
	
	int duration();
	double videoClock();
	
	QVideo * videoObject() { return m_video; }
	
	bool canPlayPause() { return ! m_mjpeg; }
	
	bool stopAllowed();
	
	int numConsumers(){ return m_receivers.size(); }
	
signals:
	void streamStarted();
	void streamStopped();
	
	void newMjpegPixmap(const QPixmap & pixmap);	

public slots:
	void stop();
	void play();
	void pause();
	void seekTo(int ms, int flags=0);
	
private slots:
	void newPixmap(const QPixmap & pixmap);
	void newImage(QImage);
	
private:
	QVideoProvider(const QString &);
	
	QString m_canonicalFilePath;
	
	QVideo *m_video;
	int m_refCount;
	
	bool m_isValid; 
	QPixmap m_lastPixmap;
	
	int m_playCount;
	
	bool m_streamStarted;
	
	QList<QObject*> m_receivers;
	
	MjpegClient * m_mjpeg;
	
};

#endif
