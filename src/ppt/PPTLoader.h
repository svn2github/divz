#ifndef PPTLOADER_H
#define PPTLOADER_H

namespace MSScriptControl {
	class ScriptControl;
}

#ifdef WIN32_PPT_ENABLED
#include <windows.h>
#include "msscriptcontrol.h"
#endif
//PowerPoint.Application: dumpcpp {91493440-5A91-11CF-8700-00AA0060263B}
#include <QObject>
#include <QRect>

class PPTLoaderTest : public QObject
{
	Q_OBJECT

public slots:
	void test1();
	void test2();
};

class PPTLoader : public QObject
{
	Q_OBJECT

public:
	PPTLoader();
	~PPTLoader();

	void openPPT();
	void openFile(const QString&);
	void closeFile();
	int  numSlides();
	void runShow();
	void setWindowRect(int x=-1, int y=-1, int w=-1, int h=-1);
	void setWindowRect(const QRect&r) { setWindowRect(r.left(),r.top(),r.width(),r.height()); }
	void gotoSlide(int);
	int  currentSlide();
	typedef enum PpSlideShowState {
		Running=1, // OLE enums offset from 1 instead of 0 like C
		Paused=2,
		Black=3,
		White=4,
		Done=5,
	};
	PpSlideShowState state();
	void setState(PpSlideShowState state);
	void exitShow();
	void closePPT();


private slots:
	void axException(int,QString,QString,QString);

private:
	void setupScripts();

	int m_numSlides;

	MSScriptControl::ScriptControl * m_script;
};



#endif
