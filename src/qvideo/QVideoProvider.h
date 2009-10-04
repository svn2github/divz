#ifndef QVIDEOPROVIDER_H
#define QVIDEOPROVIDER_H
#include <QObject>
#include <QMap>
#include <QPixmap>
#include "QVideo.h"

class QVideoProvider : public QObject
{
	Q_OBJECT
public:
	// return a provider for the file, creating one if doesnt exist
	// inc's refCount
	static QVideoProvider * providerForFile(const QString &);
	
	// de-inc refcount. If refcount<=0, delete provider
	static void releaseProvider(QVideoProvider*);
	
private:
	static QMap<QString,QVideoProvider*> m_fileProviderMap;
	
public:
	~QVideoProvider();
	
	bool isPlaying() { return m_video->status() == QVideo::Running; }
	
	void connectReceiver(QObject * receiver, const char * method);
	void disconnectReceiver(QObject * receiver);
	
	int refCount() { return m_refCount; }
	
	bool isValid() { return m_isValid; }
	
signals:
	void newPixmap(const QPixmap & pixmap);
	
public slots:
	void stop();
	void play();
	void pause();
	void seekTo(int ms);
	

private:
	QVideoProvider(const QString &);
	
	QString m_canonicalFilePath;
	
	QVideo *m_video;
	int m_refCount;
	
	bool m_isValid; 
	
};

#endif
