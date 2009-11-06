#include "NativeViewerWin32PPT.h"

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

	m_ppt = new PowerPointViewer::Application();
	connect(m_ppt, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(axException(int,QString,QString,QString)));

	m_show = m_ppt->NewShow(showFile, PowerPointViewer::ppViewerSlideShowManualAdvance, PowerPointViewer::ppVFalse);
	m_numSlides = m_show->SlidesCount();
	connect(m_show, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(axException(int,QString,QString,QString)));

	//qDebug() << "Num slides in "<<showFile<<": "<<numSlides;

	setHwnd(FindWindow(L"screenClass",  NULL));
	//qDebug() << "HWnd:"<<hwnd();

	hide();

#endif

}

void NativeViewerWin32PPT::setSlide(int x)
{
#ifdef Q_OS_WIN32
	if(m_show)
		m_show->SetSlide(x, PowerPointViewer::ppVCTrue);
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