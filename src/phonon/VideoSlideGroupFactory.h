#ifndef VideoSlideGroupFactory_H
#define VideoSlideGroupFactory_H

#include "model/SlideGroupFactory.h"

#include <Phonon/VolumeSlider>
#include <Phonon/SeekSlider>
#include <Phonon/MediaObject>
#include <QAction>
#include <QLCDNumber>

class VideoSlideGroup;
class VideoSlideGroupViewControl : public SlideGroupViewControl
{
	Q_OBJECT
public:
	VideoSlideGroupViewControl(OutputInstance * view=0, QWidget * parent = 0);

public slots:
	virtual void setSlideGroup(SlideGroup *g, Slide *curSlide=0, bool allowProgressDialog=true);
	virtual void setIsPreviewControl(bool);

protected slots:
	void phononStateChanged(Phonon::State newState, Phonon::State /* oldState */);
	void phononTick(qint64 time);

private:
	VideoSlideGroup * m_videoGroup;

	Phonon::VolumeSlider *m_volumeSlider;
	Phonon::SeekSlider *m_seekSlider;
	Phonon::MediaObject *m_mediaObject;

	QAction *m_playAction;
	QAction *m_pauseAction;
	QAction *m_stopAction;
// 	QAction *m_nextAction;
	QLCDNumber *m_timeLcd;
	QWidget * m_controlBase;


};


class VideoSlideGroupFactory : public SlideGroupFactory
{
public:
	VideoSlideGroupFactory();
	
	SlideGroup * newSlideGroup();
	
	//AbstractItemFilterList customFiltersFor(OutputInstance *instace = 0);
	SlideGroupViewControl * newViewControl();
	AbstractSlideGroupEditor   * newEditor();
	NativeViewer * newNativeViewer();
	
// 	QPixmap generatePreviewPixmap(SlideGroup*, QSize iconSize, QRect sceneRect);
};


#endif
