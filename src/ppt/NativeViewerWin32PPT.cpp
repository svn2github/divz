#include "NativeViewerWin32PPT.h"
#include <QApplication>
#include <QDesktopWidget>

#include "PPTSlideGroup.h"

#include "PPTLoader.h"

NativeViewerWin32PPT::NativeViewerWin32PPT()
	: NativeViewerWin32()
{
	m_ppt = new PPTLoader();
}

NativeViewerWin32PPT::~NativeViewerWin32PPT()
{
	delete m_ppt;
}

void NativeViewerWin32PPT::setSlideGroup(SlideGroup *group)
{
	m_ppt->showExit();

#ifdef Q_OS_WIN32
	PPTSlideGroup * pptGroup = dynamic_cast<PPTSlideGroup*>(group);

	m_ppt->openFile(pptGroup->file());

	m_numSlides = m_ppt->numSlides();

	qDebug() << "NativeViewerWin32PPT::setSlideGroup(): Num slides in "<<pptGroup->file()<<": "<<m_numSlides;






#endif

}


void NativeViewerWin32PPT::close()
{
#ifdef Q_OS_WIN32
    m_ppt->showExit();
    m_ppt->closeFile();
    CloseWindow(hwnd());
#endif
}

void NativeViewerWin32PPT::hide()
{
#ifdef Q_OS_WIN32
    SetWindowPos(hwnd(), 0, 0,0,0,0, SWP_HIDEWINDOW);
    //m_ppt->closePPT();
#endif
}

QPixmap NativeViewerWin32PPT::snapshot()
{
	QRect rect = containerWidget()->geometry();
	QPoint abs = WidgetUtil::absoluteWidgetPosition(containerWidget());
	return QPixmap::grabWindow(qApp->desktop()->winId(), abs.x(), abs.y(), rect.width(), rect.height());
}

void NativeViewerWin32PPT::setSlide(int x)
{
#ifdef Q_OS_WIN32
	qDebug() << "NativeViewerWin32PPT::setSlide(): Slide#"<<x;
	if(m_ppt)
		m_ppt->gotoSlide(x+1);
	else
		qDebug() << "NativeViewerWin32PPT::setSlide(): 'm_show' is NULL!";
#endif
}

int NativeViewerWin32PPT::currentSlide()
{

#ifdef Q_OS_WIN32
	//if(m_show)
	//	return m_show->SlideNumber();
#endif

	return -1;
}



void NativeViewerWin32PPT::embedHwnd()
{
	// adjust parent window
	//NativeViewerWin32::embedHwnd();
#ifdef Q_OS_WIN32
	QRect rect = containerWidget()->geometry();
	QPoint abs = WidgetUtil::absoluteWidgetPosition(containerWidget());
	m_ppt->showRun(abs.x(), abs.y(), rect.width(), rect.height());

	HWND ptr = NULL;
	int count = 0;
	int max = 1000;
	while(!ptr && count++ < max)
	{
		//qApp->processEvents();
		ptr = FindWindow(L"screenClass",  NULL);
	}

	//HWND hwnd = FindWindowEx(hwnd(), NULL, L"paneClassDC", NULL);
	setHwnd(ptr);
	//qDebug() << "NativeViewerWin32PPT::embedHwnd: hwnd:"<<hwnd()<<", count:"<<count;
	//m_numSlides = m_ppt->numSlides();

	//qDebug() << "NativeViewerWin32PPT::embedHwnd: Num slides:"<<m_numSlides;
	NativeViewerWin32::embedHwnd();
	BringWindowToTop(hwnd());
//
	/*

	// now find the actual widget that holds the show and stretch it out
	HWND hwnd2 = FindWindowEx(hwnd(), NULL, L"paneClassDC", NULL);
	setHwnd(FindWindow(L"screenClass",  NULL));

	if(hwnd2)
	{

		//QPoint abs = absoluteWidgetPosition(containerWidget());

		// This is a "magic forumla" for the proper stretch amount -
		// just found through trial and error. Its NOT perfect, and
		// gets more and more inaccurate the further away from 1024x768 or 320x240
		// the size is. But, it's "good enough" for now.
		int xa = ((int)(8.0/1024.0 * ((double)rect.width())));
		int ya = ((int)(67.0/768.0 * ((double)rect.height())));

		// override for known rectangle size
		if(rect.width() == 320 && rect.height() == 240)
		{
			xa = 8;
			ya = 25;
		}

		//MoveWindow(hwnd(), rect.x(), rect.x(), rect.width(), rect.height(), 1);
		qDebug() << "NativeViewerWin32PPT::embedHwnd(): hwnd2:"<<hwnd2<<", orig size:"<<rect.size()<<", xa:"<<xa<<",ya:"<<ya;


		SetWindowPos(hwnd2, 0,
				-xa, -ya,
				rect.width()  + xa*2,
				rect.height() + ya*2,
				SWP_SHOWWINDOW);
		BringWindowToTop(hwnd());
	}
*/
#endif
}


