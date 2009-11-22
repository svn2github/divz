#include "MainWindow.h"
// #include "AppSettings.h"

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
// 	AppSettings::initApp("DVizViewer");

// 	AppSettings::load();

	MainWindow mw;
	mw.show();

	int ret = app.exec();

// 	AppSettings::save();

	return ret;
}

