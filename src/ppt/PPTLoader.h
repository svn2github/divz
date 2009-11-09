#ifndef PPTLOADER_H
#define PPTLOADER_H
#include "msscriptcontrol.h"
//PowerPoint.Application: dumpcpp {91493440-5A91-11CF-8700-00AA0060263B}

class QAxScript;
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
	void showRun(int x=-1, int y=-1, int w=-1, int h=-1);
	void gotoSlide(int);
	void showExit();
	void closePPT();

private slots:
	void axException(int,QString,QString,QString);

private:
	void setupScripts();

	MSScriptControl::ScriptControl * m_script;
};



#endif
