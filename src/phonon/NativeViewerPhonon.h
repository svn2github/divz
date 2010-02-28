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

#include <Phonon/AudioOutput>
#include <Phonon/SeekSlider>
#include <Phonon/MediaObject>
#include <Phonon/VolumeSlider>
#include <Phonon/BackendCapabilities>
#include <Phonon/Effect>
#include <Phonon/EffectParameter>
#include <Phonon/ObjectDescriptionModel>
#include <Phonon/AudioOutput>
#include <Phonon/MediaSource>
#include <Phonon/VideoWidget>
#include <Phonon/VideoPlayer>


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
	
	bool autoPlay() { return m_autoPlay; }
	void setAutoPlay(bool flag) { m_autoPlay = flag; }
	
	bool isEmbeddable() { return true; }
	QWidget * renderWidget() { return m_widget; }
	
private:
	NativeShowState m_state;
	VideoSlideGroup * m_videoGroup;
#ifdef PHONON_ENABLED
	//Phonon::VideoPlayer * m_player;
	Phonon::MediaObject * m_media;
	Phonon::VideoWidget * m_widget;
	Phonon::AudioOutput * m_audio;
	Phonon::Path m_audioPath; // store for disconnection if needed
#endif
	bool m_autoPlay;
};

#endif
