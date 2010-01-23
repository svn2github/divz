#ifndef BACKGROUNDCONTENT_H
#define BACKGROUNDCONTENT_H

#include "AbstractContent.h"
class QVideoProvider;
class QSvgRenderer;
class QGraphicsProxyWidget;
class PhononTuplet;
namespace Phonon {
	class VideoPlayer;
}

#include "qvideo/QVideoProvider.h"

#include <QTimer>

#include <QAction>
#include <QLCDNumber>
	
class BackgroundItem;
class BackgroundImageWarmingThread : public QThread
{
	Q_OBJECT
public:
	BackgroundImageWarmingThread(BackgroundItem*, QString cacheKey, QRect rect);
	virtual ~BackgroundImageWarmingThread() {};
	void run();
signals:
	void renderDone(QImage *);
private:
	QPointer<BackgroundItem> m_model;
	QString m_cacheKey;
	QRect m_rect;
};

class BackgroundImageWarmingThreadManager : public QObject
{
	Q_OBJECT
public:
	BackgroundImageWarmingThreadManager(BackgroundItem*, QString cacheKey, QRect rect);
private slots:
	void renderDone(QImage*);
private:
	QPointer<BackgroundItem> m_model;
	QString m_cacheKey;
	QRect m_rect;
	BackgroundImageWarmingThread * m_thread;
};

/// \brief TODO
class BackgroundContent : public AbstractContent
{
    Q_OBJECT

    public:
	BackgroundContent(QGraphicsScene * scene, QGraphicsItem * parent = 0);
	~BackgroundContent();

    public Q_SLOTS:

    public:
	// ::AbstractDisposeable
	void dispose(bool anim=true);
		
	// ::QGraphicsItem
	virtual void show();
	
	// ::AbstractContent
	QString contentName() const { return tr("Background"); }
	QWidget * createPropertyWidget();
	bool fromXml(QDomElement & parentElement);
	void toXml(QDomElement & parentElement) const;
	QPixmap renderContent(const QSize & size, Qt::AspectRatioMode ratio) const;
	int contentHeightForWidth(int width) const;
	QRectF boundingRect() const;
	
	void syncFromModelItem(AbstractVisualItem*);
	AbstractVisualItem * syncToModelItem(AbstractVisualItem*);
	
	// ::QGraphicsItem
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
	
	QWidget * controlWidget();
	
    protected:
    	friend class BackgroundImageWarmingThread;
    	static QImage * internalLoadFile(QString file,QString cacheKey, QRect rect);
    
    private slots:
	void setPixmap(const QPixmap & pixmap);
	void sceneRectChanged(const QRectF &);
	void renderSvg();
	
	void animateZoom();
	
#ifdef PHONON_ENABLED
	void phononPlayerFinished();
	
	void phononStateChanged(Phonon::State newState, Phonon::State /* oldState */);
	void phononTick(qint64 time);
#endif

    private:
	void setVideoFile(const QString &name);
	void setImageFile(const QString&);
	void loadSvg(const QString&);
	void disposeSvgRenderer();

	QPixmap m_pixmap;
	QSize m_imageSize;
	QVideoProvider * m_videoProvider;
	bool m_still;
	
	bool m_sceneSignalConnected;
	
	QSvgRenderer * m_svgRenderer;
	
	bool m_fileLoaded;
	QString m_fileName;
	QString m_fileLastModified;
	
	QString m_lastForegroundKey;
	QString m_lastImageKey;
	
	QTimer * m_zoomAnimationTimer;
	
	QPointF m_zoomStep;
	QPointF m_zoomEndSize;
	QPointF m_zoomCurSize;
	QPointF m_zoomStartSize;
	QPointF m_zoomDestPoint;
	int m_zoomDir;
	bool m_zoomInit;
	bool m_zoomEnabled;
	QPixmap m_zoomedPixmap;
	QPointF m_zoomedPixmapSize;
	
#ifdef PHONON_ENABLED
	QGraphicsProxyWidget *m_proxy;
	Phonon::VideoPlayer *m_player;
	PhononTuplet *m_tuplet;
	
	
	QAction *playAction;
	QAction *pauseAction;
	QAction *stopAction;
	QAction *nextAction;
	QLCDNumber *timeLcd;
	Phonon::VolumeSlider *volumeSlider;
	Phonon::SeekSlider *seekSlider;
	Phonon::MediaObject *mediaObject;
#endif
	
};

#endif
