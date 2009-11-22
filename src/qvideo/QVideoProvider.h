#ifndef QVIDEOPROVIDER_H
#define QVIDEOPROVIDER_H
#include <QObject>
#include <QMap>
#include <QPixmap>
#include "QVideo.h"

#if QT_VERSION >= 0x040600
#include <Phonon/AudioOutput>
#include <Phonon/SeekSlider>
#include <Phonon/MediaObject>
#include <Phonon/VolumeSlider>
#include <Phonon/BackendCapabilities>
#include <Phonon/Effect>
#include <Phonon/EffectParameter>
#include <Phonon/ObjectDescriptionModel>
#include <Phonon/AudioOutput>
#include <Phonon/MediaSource>
#include <Phonon/VideoWidget>
#include <Phonon/VideoPlayer>
#endif



class QVideoProvider;

class QVideoIconGenerator : public QObject
{
	Q_OBJECT
	
public /*static*/:
	// If icon in disk cache for file, returns the icon. Otherwise, returns a null 
	// pixmap and starts a QVideoIconGenerator to generate the icon so it will be 
	// in the cache for the next time (stores QDir::tempPath() + "/qvideoprovider/" + md5sum of canonical file path)
	static QPixmap iconForFile(const QString&);
	
private:
	static QString cacheFile(QVideoProvider *);
	static QString cacheFile(const QString&);
	static void storePixmap(const QPixmap&, QVideoProvider *);

	QVideoIconGenerator(QVideoProvider*);
	
private slots:
	void newPixmap(const QPixmap &);

private:
	QVideoProvider * m_provider;

};

#if QT_VERSION >= 0x040600
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

#if QT_VERSION >= 0x040600
	// Returns true if Phonon is available and supports the mime type for the given file
	static bool canUsePhonon(const QString&);
	
	// Returns the PhononTuplet for the given file 
	static PhononTuplet * phononForFile(const QString&);
#endif

private:
	static QMap<QString,QVideoProvider*> m_fileProviderMap;

#if QT_VERSION >= 0x040600
	static QMap<QString,PhononTuplet*> m_phononMap;
#endif


public:
	~QVideoProvider();
	
	bool isPlaying() { return m_video->status() == QVideo::Running; }
	
	void connectReceiver(QObject * receiver, const char * method);
	void disconnectReceiver(QObject * receiver);
	
	int refCount() { return m_refCount; }
	
	bool isValid() { return m_isValid; }
	
	QPixmap pixmap() { return m_lastPixmap; }
	
	QString canonicalFilePath() { return m_canonicalFilePath; }
	
signals:
	void streamStarted();
	void streamStopped();	

public slots:
	void stop();
	void play();
	void pause();
	void seekTo(int ms);
	
private slots:
	void newPixmap(const QPixmap & pixmap);
	
private:
	QVideoProvider(const QString &);
	
	QString m_canonicalFilePath;
	
	QVideo *m_video;
	int m_refCount;
	
	bool m_isValid; 
	QPixmap m_lastPixmap;
	
	int m_playCount;
	
	bool m_streamStarted;
	
};

#endif
