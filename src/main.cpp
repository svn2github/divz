#include "MainWindow.h"
#include "AppSettings.h"

#include <QtGui/QApplication>
#include <QPixmapCache>
#include <QMessageBox>
#include <QInputDialog>
#include <QSplashScreen>

#include "CheckUpdatesDialog.h"

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
        //	QApplication::setStyle(new QtDotNetStyle()); //QtDotNetStyle::Office));
	#endif

 	QApplication app(argc, argv);

        QPixmap pixmap(":/data/DViz-splash.png");
        QSplashScreen splash(pixmap);
        splash.show();
        app.processEvents();
        app.processEvents();

	AppSettings::initApp("DVizControl");

	AppSettings::load();

	QSettings s;
	//if(s.value("first-run-date").isValid())
	//{
		QDate today = QDate::currentDate();
		QDate expires(2010,12,31);
		//QDate f = s.value("first-run-date").toDate();
		//int days = f.daysTo(d);
		QString ver = "DViz Beta";
		#define VER "r701"
#ifdef VER
		ver += QString(", Build %1").arg(VER);
#endif
/*
 		if(today > expires)
 		{
 			QMessageBox::critical(0,QString("%1 Expired").arg(ver),"Sorry, but this evaluation copy of DViz has expired. You can buy the non-eval version from Josiah Bryan - email josiahbryan@gmail.com for more information.\n\nOr, you can set your computer's clock back a day or two and keep using this evaluation version till you make up your mind. Thanks for trying out DViz!");
 			return 0;
 		}
 		else
 		{
 			QMessageBox::information(0,ver,QString("Thanks for trying %1! This evaluation copy is valid until 2010-12-31, at which time you'll be asked to purchase the non-eval verson from Josiah Bryan. If you find ANY issues or have any ideas for improvement, PLEASE speak up and log a new issue at:\n\n    http://code.google.com/p/dviz/issues/list\n").arg(ver));
 		}
*/
	//}

	if(AppSettings::registrationName().isEmpty() ||
	   AppSettings::registrationOrgName().isEmpty())
	{
		QString nameGuess = AppSettings::registrationName();
		QString orgGuess = AppSettings::registrationOrgName();
		
		qDebug() << "main debug:"<<nameGuess<<orgGuess;
		
		#ifdef Q_OS_WIN
		/*
		QSettings settings;
		settings.insertSearchPath( QSettings::Windows,"/Microsoft/Windows/CurrentVersion");
		nameGuess = settings.readEntry( "/RegisteredOwner" );
		orgGuess = settings.readEntry( "/RegisteredOrganization" );

		if(nameGuess.isEmpty())
		{
			LPTSTR lpszSystemInfo; // pointer to system information
			DWORD cchBuff = 256; // size of user name
			TCHAR tchBuffer[UNLEN + 1]; // buffer for expanded string

			lpszSystemInfo = tchBuffer;

			// Get and display the user name.
			GetUserName(lpszSystemInfo, &cchBuff);

			//Unicode string needs to be converted
			nameGuess = QString::fromUcs2(lpszSystemInfo);
		}
		*/
		#endif
		
		#ifdef Q_OS_LINUX
		nameGuess = QString( getenv("USER") );
		#endif
		
		bool ok;
		QString text = QInputDialog::getText(0, QObject::tr("Confirm Your Name"),
							QObject::tr("Your name:"), QLineEdit::Normal,
							nameGuess, &ok);
		if (ok && !text.isEmpty())
			AppSettings::setRegistrationName(text);
		
		text = QInputDialog::getText(0,		QObject::tr("Confirm Your Organization"),
							QObject::tr("Your organization:"), QLineEdit::Normal,
							orgGuess, &ok);
		if (ok && !text.isEmpty())
			AppSettings::setRegistrationOrgName(text);
	
		AppSettings::save();
	}


	MainWindow *mw = new MainWindow();
	mw->show();
	
	CheckUpdatesDialogLauncher *update = new CheckUpdatesDialogLauncher();
	QObject::connect(update, SIGNAL(updateCheckFinished()), update, SLOT(deleteLater()));
	update->start();
	

	splash.hide();
	int ret = app.exec();

	AppSettings::save();

	// dont delete mw because something in the Qt library is causing
	// a SEGFLT on windows and I cant get gdb to work right on windows
	// inorder to trace it. Therefore, just bypass the bug by not deleting
	// mw - the memory will be released to the OS anyway since we are
	// exiting the program here.

	return ret;
}

