#ifndef SharedMemoryImageWriter_H
#define SharedMemoryImageWriter_H

#include <QObject>
#include <QSharedMemory>
#include <QTimer>
#include <QGraphicsScene>

#define FRAME_WIDTH 1024
#define FRAME_HEIGHT 768
#define FRAME_FORMAT QImage::Format_ARGB32_Premultiplied

class SharedMemoryImageWriter : public QObject
{
	Q_OBJECT
public:
	SharedMemoryImageWriter(QObject *parent=0);
	~SharedMemoryImageWriter();
	
	void enable(const QString& key, QGraphicsScene *scene);
	void disable();

private slots:
	void generateFrame();
	
private:
	void updateRects();
	
private:
	QTimer m_frameTimer;
	QSharedMemory m_sharedMemory;
	int m_memorySize;
	QGraphicsScene *m_scene;
	QImage m_image;
	
	QRect m_targetRect;
	QRect m_sourceRect;
	
	int m_timeAccum;
	int m_frameCount;
		
};


#endif
