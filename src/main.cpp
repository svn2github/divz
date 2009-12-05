#include "MainWindow.h"
#include "AppSettings.h"

#include <QtGui/QApplication>
int main(int argc, char **argv)
{
	#if !defined(Q_OS_MAC) // raster on OSX == b0rken
		// use the Raster GraphicsSystem as default on 4.5+
		#if QT_VERSION >= 0x040500
		QApplication::setGraphicsSystem("raster");
		#endif
 	#endif

 	QApplication app(argc, argv);
	AppSettings::initApp("DVizControl");

	AppSettings::load();

	MainWindow *mw = new MainWindow();
	mw->show();

	int ret = app.exec();

	AppSettings::save();

	// dont delete mw because something in the Qt library is causing
	// a SEGFLT on windows and I cant get gdb to work right on windows
	// inorder to trace it. Therefore, just bypass the bug by not deleting
	// mw - the memory will be released to the OS anyway since we are
	// exiting the program here.

	return ret;
}

