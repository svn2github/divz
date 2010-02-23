#ifndef NativeViewerPhonon_H
#define NativeViewerPhonon_H


#include "SlideGroupViewer.h"
class VideoSlideGroup;
// #include <phonon/AudioOutput>
// #include <phonon/SeekSlider>
// #include <phonon/MediaObject>
// #include <phonon/VolumeSlider>
// #include <phonon/BackendCapabilities>
// #include <phonon/Effect>
// #include <phonon/EffectParameter>
// #include <phonon/ObjectDescriptionModel>
// #include <phonon/AudioOutput>
// #include <phonon/MediaSource>
// #include <phonon/VideoWidget>
// #include <phonon/VideoPlayer>

#include <phonon/Phonon/AudioOutput>
#include <phonon/Phonon/SeekSlider>
#include <phonon/Phonon/MediaObject>
#include <phonon/Phonon/VolumeSlider>
#include <phonon/Phonon/BackendCapabilities>
#include <phonon/Phonon/Effect>
#include <phonon/Phonon/EffectParameter>
#include <phonon/Phonon/ObjectDescriptionModel>
#include <phonon/Phonon/AudioOutput>
#include <phonon/Phonon/MediaSource>
#include <phonon/Phonon/VideoWidget>
#include <phonon/Phonon/VideoPlayer>


#define PHONON_ENABLED 1

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

	Phonon::MediaObject * mediaObject() { return m_media; }
	Phonon::AudioOutput * audioOutput() { return m_audio; }
	Phonon::VideoWidget * videoWidget() { return m_widget; }
	
private:
	NativeShowState m_state;
	VideoSlideGroup * m_videoGroup;
#ifdef PHONON_ENABLED
	//Phonon::VideoPlayer * m_player;
	Phonon::MediaObject * m_media;
	Phonon::VideoWidget * m_widget;
	Phonon::AudioOutput * m_audio;
#endif
};

#endif
