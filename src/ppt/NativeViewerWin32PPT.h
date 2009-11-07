#ifndef NativeViewerWin32PPT_H
#define NativeViewerWin32PPT_H


#ifdef WIN32_PPT_ENABLED
#include "powerpointviewer.h"
#endif

#include "SlideGroupViewer.h"

class NativeViewerWin32PPT : public NativeViewerWin32
{
	Q_OBJECT
public:
	NativeViewerWin32PPT();
	~NativeViewerWin32PPT();

	void setSlideGroup(SlideGroup*);
	void setSlide(int);

	int numSlides() { return m_numSlides; }
	int currentSlide();

protected slots:
	void embedHwnd();

private slots:
	void axException(int,QString,QString,QString);


private:
	void destroyPpt();

	int m_numSlides;

	#ifdef Q_OS_WIN32
		PowerPointViewer::Application   *m_ppt;
		PowerPointViewer::SlideShowView *m_show;
	#endif


};

#endif
