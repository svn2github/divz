#include "GLWidget.h"
#include <QApplication>
#include "AppWindow.h"
#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif

int main(int argc, char *argv[])
{
	#ifdef Q_WS_X11
		XInitThreads();
	#endif
	QApplication app(argc, argv);

	AppWindow aw;
	aw.show();

	
// 	GLWidget widget(0);
// 	widget.setWindowTitle("Thread0");
// 	widget.show();
//  	widget.startRendering();

	return app.exec();
}
