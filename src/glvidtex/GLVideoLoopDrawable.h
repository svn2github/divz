#ifndef GLVideoLoopDrawable_h
#define GLVideoLoopDrawable_h

#include "GLVideoDrawable.h"

class GLVideoLoopDrawable : public GLVideoDrawable
{
	Q_OBJECT
	
	Q_PROPERTY(QString videoFile READ videoFile WRITE setVideoFile);
	
public:
	GLVideoLoopDrawable(QString file="", QObject *parent=0);
	
	QString videoFile() { return m_videoFile; }
	
public slots:
	bool setVideoFile(const QString&);
	
private slots:
	void testXfade();
	
private:
	QString m_videoFile;
};

#endif
