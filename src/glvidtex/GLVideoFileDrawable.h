#ifndef GLVideoFileDrawable_h
#define GLVideoFileDrawable_h

#include "GLVideoDrawable.h"

class QtVideoSource;
class GLVideoFileDrawable : public GLVideoDrawable
{
	Q_OBJECT
	
	Q_PROPERTY(QString videoFile READ videoFile WRITE setVideoFile USER true);
	
	Q_PROPERTY(int volume READ volume WRITE setVolume);
	Q_PROPERTY(bool muted READ isMuted WRITE setMuted);
	Q_PROPERTY(int status READ status WRITE setStatus);
	Q_PROPERTY(quint64 position READ position WRITE setPosition);
	
public:
	GLVideoFileDrawable(QString file="", QObject *parent=0);
	
	QString videoFile() { return m_videoFile; }
	
	double videoLength();
	int volume();
	bool isMuted();
	int status();
	quint64 position();
	
signals:
	void videoFileChanged(const QString&);
	void positionChanged(qint64 position);
	
public slots:
	bool setVideoFile(const QString&);
	
	void setVolume(int);
	void setMuted(bool);
	void setStatus(int);
	void setPosition(qint64);
	
private slots:
	void testXfade();
	void deleteSource(VideoSource *source);
	
protected:
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	
private:
	QString m_videoFile;
	double m_videoLength;
	
	QtVideoSource * m_qtSource;
};

#endif
