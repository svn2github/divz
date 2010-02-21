#ifndef NativeViewerPhonon_H
#define NativeViewerPhonon_H


#include "SlideGroupViewer.h"
class VideoSlideGroup;
#include <phonon/AudioOutput>
#include <phonon/SeekSlider>
#include <phonon/MediaObject>
#include <phonon/VolumeSlider>
#include <phonon/BackendCapabilities>
#include <phonon/Effect>
#include <phonon/EffectParameter>
#include <phonon/ObjectDescriptionModel>
#include <phonon/AudioOutput>
#include <phonon/MediaSource>
#include <phonon/VideoWidget>
#include <phonon/VideoPlayer>


class NativeViewerPhonon : public NativeViewer
{
	Q_OBJECT
public:
	NativeViewerPhonon();
	~NativeViewerPhonon();

	void setSlideGroup(SlideGroup*);
	void setSlide(int);

	int numSlides() { return 1; }
	int currentSlide() { return 0; }
	
	void show();
	void close();
	void hide();
	
	QPixmap snapshot();

	void setState(NativeShowState);
	NativeShowState state() {  return m_state; }
	
private:
	NativeShowState m_state;
	VideoSlideGroup * m_videoGroup;
#ifdef PHONON_ENABLED
	Phonon::VideoPlayer * m_player;
#endif
};

#endif
