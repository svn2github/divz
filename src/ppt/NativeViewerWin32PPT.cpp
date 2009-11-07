#include "NativeViewerWin32PPT.h"

#include "PPTSlideGroup.h"

NativeViewerWin32PPT::NativeViewerWin32PPT()
	: NativeViewerWin32()
#ifdef Q_OS_WIN32
	, m_ppt(0)
	, m_show(0)
#endif
{}

NativeViewerWin32PPT::~NativeViewerWin32PPT()
{
	destroyPpt();
}

void NativeViewerWin32PPT::setSlideGroup(SlideGroup *group)
{
	destroyPpt();

#ifdef Q_OS_WIN32
	PPTSlideGroup * pptGroup = dynamic_cast<PPTSlideGroup*>(group);


	m_ppt = new PowerPointViewer::Application();
	connect(m_ppt, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(axException(int,QString,QString,QString)));

	//HWND testHwnd = FindWindow(L"screenClass",  NULL);
	//qDebug() << "NativeViewerWin32PPT::setSlideGroup(): testHwnd:"<<testHwnd;

	m_show = m_ppt->NewShow(pptGroup->file(),
		PowerPointViewer::ppViewerSlideShowUseSlideTimings,
		PowerPointViewer::ppVFalse
	);
	m_numSlides = m_show->SlidesCount();
	connect(m_show, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(axException(int,QString,QString,QString)));

	//qDebug() << "NativeViewerWin32PPT::setSlideGroup(): Num slides in "<<pptGroup->file()<<": "<<m_numSlides;

	setHwnd(FindWindow(L"screenClass",  NULL));
	//qDebug() << "NativeViewerWin32PPT::setSlideGroup(): hwnd:"<<hwnd();

	hide();

#endif

}

void NativeViewerWin32PPT::setSlide(int x)
{
#ifdef Q_OS_WIN32
	qDebug() << "NativeViewerWin32PPT::setSlide(): Slide#"<<x;
	if(m_show)
		m_show->GotoSlide(x+1, PowerPointViewer::ppVCTrue);
	else
		qDebug() << "NativeViewerWin32PPT::setSlide(): 'm_show' is NULL!";
#endif
}

int NativeViewerWin32PPT::currentSlide()
{

#ifdef Q_OS_WIN32
	if(m_show)
		return m_show->SlideNumber();
#endif

	return -1;
}

void NativeViewerWin32PPT::axException(int code,QString source,QString description,QString help)
{
	qDebug() << "[AxException] Code"<<code<<": "<<source<<":"<<description<<"("<<help<<")";
}

void NativeViewerWin32PPT::destroyPpt()
{
#ifdef Q_OS_WIN32
	if(m_show)
	{
		m_show->Exit();
		disconnect(m_show,0,this,0);
		delete m_show;
	}

	if(m_ppt)
	{
		m_ppt->Quit();
		disconnect(m_ppt,0,this,0);
		delete m_ppt;
    }
#endif
}


void NativeViewerWin32PPT::embedHwnd()
{
	// adjust parent window
	NativeViewerWin32::embedHwnd();

#ifdef Q_OS_WIN32
	// now find the actual widget that holds the show and stretch it out
	HWND hwnd2 = FindWindowEx(hwnd(), NULL, L"paneClassDC", NULL);
	if(hwnd2)
	{
		QRect rect = containerWidget()->geometry();
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
#endif
}


