#ifndef VIDEOFILECONTENT_H
#define VIDEOFILECONTENT_H

#include "AbstractContent.h"
//#include "qvideo/QVideoBuffer.h"
#ifndef NO_LIBAV
#include "qvideo/QVideo.h"
#endif
#include "qvideo/QVideoProvider.h"
#include "ButtonItem.h"

class QGraphicsProxyWidget;


/// \brief The VideoFileContent class plays videos from disk using the FFMPEG via QVideoProvider.

class VideoFileContent : public AbstractContent
{
	Q_OBJECT
	Q_PROPERTY(QString filename READ filename WRITE setFilename)

    public:
	VideoFileContent(QGraphicsScene * scene, QGraphicsItem * parent = 0);
	~VideoFileContent();
	
    public Q_SLOTS:
	QString filename() const { return m_filename; }
	void setFilename(const QString & text);

    private slots:
	void setPixmap(const QPixmap & pixmap);
	void slotTogglePlay();
	
// 	void phononPlayerFinished();
#ifdef PHONON_ENABLED
	void phononStateChanged(Phonon::State newState, Phonon::State /* oldState */);
	void phononTick(qint64 time);
#endif

    public:
        // ::AbstractContent
	QString contentName() const { return tr("VideoFile"); }
	QWidget * createPropertyWidget();
	
	QPixmap renderContent(const QSize & size, Qt::AspectRatioMode ratio) const;
	int contentHeightForWidth(int width) const;
	
	void syncFromModelItem(AbstractVisualItem*);
	AbstractVisualItem * syncToModelItem(AbstractVisualItem*);
	
	void applySceneContextHint(MyGraphicsScene::ContextHint);
	
	// ::QGraphicsItem
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
	
	virtual QWidget * controlWidget();
	
    private:

	QString m_filename;
	
	QPixmap m_pixmap;
	QSize m_imageSize;
	
	QVideoProvider * m_videoProvider;
	ButtonItem * m_bSwap;
	bool m_still;
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
