#include "PPTLoader.h"
#include <windows.h>
#include <QApplication>
#include <QDebug>
#include <QTimer>

PPTLoader::PPTLoader()
	: QWidget()
{

}

PPTLoader::~PPTLoader()
{
    if(m_show)
    {
	m_show->Exit();
	delete m_show;
    }

    if(m_ppt)
    {
	m_ppt->Quit();
	delete m_ppt;
    }
}


void PPTLoader::open(const QString& showFile)
{
	    m_ppt = new PowerPointViewer::Application();
	    connect(m_ppt, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(axException(int,QString,QString,QString)));

	    m_show = m_ppt->NewShow(showFile, PowerPointViewer::ppViewerSlideShowManualAdvance, PowerPointViewer::ppVFalse);
	    int numSlides = m_show->SlidesCount();
	    qDebug() << "Num slides in "<<showFile<<": "<<numSlides;
	    connect(m_show, SIGNAL(exception(int,QString,QString,QString)), this, SLOT(axException(int,QString,QString,QString)));
	    //m_timer.start(100);

	    m_pptWindow = FindWindow(L"screenClass",  0);
	    qDebug() << "HWnd:"<<m_pptWindow;

	    SetWindowPos(m_pptWindow, 0, 0,0,0,0, SWP_HIDEWINDOW);
}

void PPTLoader::axException(int code,QString source,QString description,QString help)
{
	qDebug() << "[AxException] Code"<<code<<": "<<source<<":"<<description<<"("<<help<<")";
}
void PPTLoader::embedHwnd()
{
	QRect frame = geometry();
	QRect rect = geometry();
	//MoveWindow(m_pptWindow, rect.x(), rect.x(), rect.width(), rect.height(), 1);
	SetWindowPos(m_pptWindow, 0, rect.x(), rect.y() + (rect.y() - frame.y()), rect.width(), rect.height(), SWP_SHOWWINDOW);
	BringWindowToTop(m_pptWindow);
}

void PPTLoader::showEvent(QShowEvent*)
{
	QTimer::singleShot(1,this,SLOT(embedHwnd()));
}

void PPTLoader::closeEvent(QCloseEvent*)
{
	CloseWindow(m_pptWindow);
}
