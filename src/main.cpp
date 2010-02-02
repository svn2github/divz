#include "MainWindow.h"
#include "AppSettings.h"

#include <QtGui/QApplication>
#include <QPixmapCache>
#include <QMessageBox>

// I'll use this code at the church to debug some crashes.
// However, disbaling it for now because I'm not sure
// how well it performs in production - I don't want somebody
// checking out the code from svn and getting bit by this code
// breaking something.
#define CUSTOM_MSG_HANDLER

#if defined(CUSTOM_MSG_HANDLER)

	#if defined(Q_OS_WIN)
	extern Q_CORE_EXPORT void qWinMsgHandler(QtMsgType t, const char* str);
	#endif

	static QtMsgHandler qt_origMsgHandler = 0;

	void myMessageOutput(QtMsgType type, const char *msg)
	{
		#if defined(Q_OS_WIN)
		if (!qt_origMsgHandler)
			qt_origMsgHandler = qWinMsgHandler;
		#endif

		switch (type)
		{
			case QtDebugMsg:
				if(qt_origMsgHandler)
					qt_origMsgHandler(type,msg);
				else
					fprintf(stderr, "Debug: %s\n", msg);
				break;
			case QtWarningMsg:
				if(qt_origMsgHandler)
					qt_origMsgHandler(QtDebugMsg,msg);
				else
					fprintf(stderr, "Warning: %s\n", msg);
				break;
			case QtCriticalMsg:
	// 			if(qt_origMsgHandler)
	// 				qt_origMsgHandler(type,msg);
	// 			else
	// 				fprintf(stderr, "Critical: %s\n", msg);
	// 			break;
			case QtFatalMsg:
				if(qt_origMsgHandler)
				{
					qt_origMsgHandler(QtDebugMsg,msg);
					//qt_origMsgHandler(type,msg);
				}
				else
				{

					fprintf(stderr, "Fatal: %s\n", msg);
				}
				//QMessageBox::critical(0,"Fatal Error",msg);
				//qt_origMsgHandler(QtDebugMsg,msg);
				/*
				if(strstr(msg,"out of memory, returning null image") != NULL)
				{
					QPixmapCache::clear();
					qt_origMsgHandler(QtDebugMsg, "Attempted to clear QPixmapCache, continuing");
					return;
				}
				*/
				abort();
		}
	}
#endif // CUSTOM_MSG_HANDLER

int main(int argc, char **argv)
{
	#if defined(CUSTOM_MSG_HANDLER)
		qt_origMsgHandler = qInstallMsgHandler(myMessageOutput);
	#endif

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

