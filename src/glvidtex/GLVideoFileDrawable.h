#ifndef GLVideoFileDrawable_h
#define GLVideoFileDrawable_h

#include "GLVideoDrawable.h"


class GLVideoFileDrawable : public GLVideoDrawable
{
	Q_OBJECT
	
	Q_PROPERTY(QString videoFile READ videoFile WRITE setVideoFile USER true);
	
public:
	GLVideoFileDrawable(QString file="", QObject *parent=0);
	
	QString videoFile() { return m_videoFile; }
	double videoLength() { return m_videoLength; } // in seconds
	
signals:
	void videoFileChanged(const QString&);
	
public slots:
	bool setVideoFile(const QString&);
	
private slots:
	void testXfade();
	void deleteSource(VideoSource *source);
	
private:
	QString m_videoFile;
	double m_videoLength;
};

#endif
