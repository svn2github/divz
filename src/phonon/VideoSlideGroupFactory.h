#ifndef VideoSlideGroupFactory_H
#define VideoSlideGroupFactory_H

#include "model/SlideGroupFactory.h"

#include <Phonon/SeekSlider>
#include <Phonon/MediaObject>
#include <Phonon/VolumeSlider>

#include <QAction>
#include <QLCDNumber>
#include <QLabel>

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
	void phononPlayerFinished();
	
	void loopActionToggled(bool);
	
	// to catch changes to SlideGroup::endOfGroupAction property
	void slideChanged(Slide *slide, QString slideOperation, AbstractItem *item, QString operation, QString fieldName, QVariant value);

private:
	VideoSlideGroup * m_videoGroup;

	Phonon::VolumeSlider *m_volumeSlider;
	Phonon::SeekSlider *m_seekSlider;
	Phonon::MediaObject *m_mediaObject;

	QAction *m_playAction;
	QAction *m_pauseAction;
	QAction *m_stopAction;
	QAction *m_loopAction;
// 	QAction *m_nextAction;
	QLCDNumber *m_timeLcd;
	QWidget * m_controlBase;
	QLabel * m_playingLabel;


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
