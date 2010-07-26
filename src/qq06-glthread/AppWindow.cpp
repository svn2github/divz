#include "AppWindow.h"
#include "GLWidget.h"
#include <QMenu>
#include <QMenuBar>
#include <QApplication>

AppWindow::AppWindow()
	: QMainWindow(0)
{

	
	QMenu *menu = menuBar()->addMenu(tr("&Thread"));

	
	menu->addAction("&New thread", this, SLOT(newThread())); //, CTRL+Key_N);
	menu->addAction("&End current thread", this, SLOT(killThread())); //, CTRL+Key_K);
	menu->addSeparator();
	menu->addAction("E&xit", qApp, SLOT(quit()));//, CTRL+Key_Q);
	
	ws = new QWorkspace(this);
	setCentralWidget(ws);
	
	//reszie(100,100);
	//newThread();
}

void AppWindow::closeEvent(QCloseEvent *evt)
{
	QWidgetList windows = ws->windowList();
	for (int i = 0; i < int(windows.count()); ++i) 
	{
		GLWidget *window = (GLWidget *)windows.at(i);
		window->stopRendering();
	}
	QMainWindow::closeEvent(evt);
}

void AppWindow::newThread()
{
	QWidgetList windows = ws->windowList();
 	GLWidget *widget = new GLWidget(0);
	//QWidget *widget = new QWidget();
	widget->setWindowTitle("Thread #" + QString::number(windows.count() + 1));
	widget->show();
 	widget->startRendering();
}

void AppWindow::killThread()
{
	GLWidget *widget = (GLWidget *)ws->activeWindow();    
	if (widget) 
	{
		widget->stopRendering();
		delete widget;
	}
}
	
