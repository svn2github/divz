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
	//app.setMainWidget(&aw);
	aw.show();
	return app.exec();
}
