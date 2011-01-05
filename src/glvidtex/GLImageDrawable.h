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
	virtual ~GLImageDrawable();
	
	QString imageFile() { return m_imageFile; }
	QImage image() { return m_image; }
		
signals:
	void imageFileChanged(const QString&);

public slots:
	void setImage(const QImage&);
	virtual bool setImageFile(const QString&);
	
protected:
	void internalSetFilename(QString);
	
	virtual void reloadImage();
	virtual void releaseImage();
	virtual bool canReleaseImage();
	virtual void setGLWidget(GLWidget*);
	
	QImage m_image;
	QString m_imageFile;
	
	bool m_releasedImage;
	
	static int m_allocatedMemory;
	static int m_activeMemory;
	
private:
	void setVideoSource(VideoSource*);
};

#endif
