#ifndef NativeViewerCamera_H
#define NativeViewerCamera_H


#include "SlideGroupViewer.h"
class CameraSlideGroup;
#include "../camera_test/CameraViewerWidget.h"
/*
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
#include <phonon/VideoPlayer>*/


// #define PHONON_ENABLED 1

class NativeViewerCamera : public NativeViewer
{
	Q_OBJECT
public:
	NativeViewerCamera();
	~NativeViewerCamera();

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

// 	Phonon::MediaObject * mediaObject() { return m_media; }
// 	Phonon::AudioOutput * audioOutput() { return m_audio; }
// 	Phonon::VideoWidget * videoWidget() { return m_widget; }
	
// 	bool autoPlay() { return m_autoPlay; }
// 	void setAutoPlay(bool flag) { m_autoPlay = flag; }
	
	bool isEmbeddable() { return true; }
	QWidget * renderWidget() { return m_widget; }
	
	CameraViewerWidget * cameraViewer() { return m_widget; }
	
private:
	NativeShowState m_state;
	CameraSlideGroup * m_cameraGroup;
	CameraViewerWidget * m_widget;
/*#ifdef PHONON_ENABLED
	//Phonon::VideoPlayer * m_player;
	Phonon::MediaObject * m_media;
	Phonon::VideoWidget * m_widget;
	Phonon::AudioOutput * m_audio;
	Phonon::Path m_audioPath; // store for disconnection if needed
#endif*/
// 	bool m_autoPlay;
};

#endif
