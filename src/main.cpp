#include "MainWindow.h"
#include "AppSettings.h"

#include <QtGui/QApplication>
#include <QPixmapCache>
#include <QMessageBox>

#ifdef Q_WS_X11
	#include <X11/Xlib.h>
#endif


#if defined(Q_OS_WIN)
	#include <qtdotnetstyle.h>
#endif

int main(int argc, char **argv)
{
// 	#ifdef Q_WS_X11
// 		XInitThreads();
// 	#endif
// 	
	#if !defined(Q_OS_MAC) // raster on OSX == b0rken
		// use the Raster GraphicsSystem as default on 4.5+
		#if QT_VERSION >= 0x040500
		QApplication::setGraphicsSystem("raster");
		#endif
 	#endif

 	#if defined(Q_OS_WIN)
		QApplication::setStyle(new QtDotNetStyle()); //QtDotNetStyle::Office));
 	#endif

 	QApplication app(argc, argv);
	AppSettings::initApp("DVizControl");

	AppSettings::load();

	QSettings s;
	//if(s.value("first-run-date").isValid())
	//{
		QDate today = QDate::currentDate();
		QDate expires(2010,3,31);
		//QDate f = s.value("first-run-date").toDate();
		//int days = f.daysTo(d);
		QString ver = "DViz Beta";
#ifdef VER
		ver += QString(", Build %1").arg(VER);
#endif

// 		if(today > expires)
// 		{
// 			QMessageBox::critical(0,QString("%1 Expired").arg(ver),"Sorry, but this beta copy of DViz has expired. You can download the latest beta or buy the released version (if available) at:\n\n    http://code.google.com/p/dviz\n\nOr, you can set your computer's clock back a day or two and keep using this beta till you make up your mind. Thanks for trying out DViz!");
// 			return 0;
// 		}
// 		else
// 		{
// 			QMessageBox::information(0,ver,QString("Thanks for trying %1! This beta is valid until 2010-03-31, at which time you'll be asked to download a newer version. If you find ANY issues or have any ideas for improvement, PLEASE speak up and log a new issue at:\n\n    http://code.google.com/p/dviz/issues/list\n").arg(ver));
// 		}

	//}

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

