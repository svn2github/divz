#ifndef GLVideoInputDrawable_h
#define GLVideoInputDrawable_h

#include "GLVideoDrawable.h"

#ifdef Q_OS_WIN
	#define DEFAULT_INPUT "vfwcap://0"
#else
	#define DEFAULT_INPUT "/dev/video0"
#endif

class CameraThread;

class GLVideoInputDrawable : public GLVideoDrawable
{
	Q_OBJECT
	
	Q_PROPERTY(QString videoInput READ videoInput WRITE setVideoInput USER true);
	Q_PROPERTY(QString cardInput READ cardInput WRITE setCardInput);
	Q_PROPERTY(bool deinterlace READ deinterlace WRITE setDeinterlace);
	
public:
	GLVideoInputDrawable(QString file=DEFAULT_INPUT, QObject *parent=0);
	
	QString videoInput() { return m_videoInput; }
	QString cardInput();
	bool deinterlace();
	
	QStringList cardInputs();
	
public slots:
	bool setVideoInput(const QString&);
	bool setCardInput(const QString&);
	void setDeinterlace(bool flag=true);
	
private slots:
	void testXfade();
	
private:
	QString m_videoInput;
	CameraThread *m_source;
};

#endif
