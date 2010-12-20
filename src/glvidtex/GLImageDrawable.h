#ifndef GLImageDrawable_h
#define GLImageDrawable_h

#include "GLVideoDrawable.h"

class GLImageDrawable : public GLVideoDrawable
{
	Q_OBJECT
	
	Q_PROPERTY(QImage image READ image WRITE setImage);
	Q_PROPERTY(QString imageFile READ imageFile WRITE setImageFile USER true);
	
public:
	GLImageDrawable(QString file="", QObject *parent=0);
	
	QString imageFile() { return m_imageFile; }
	QImage image() { return m_image; }
		
signals:
	void imageFileChanged(const QString&);

public slots:
	void setImage(const QImage&);
	bool setImageFile(const QString&);
	
private slots:
	void testXfade();
	
private:
	void setVideoSource(VideoSource*);
	
	QImage m_image;
	QString m_imageFile;
};

#endif
