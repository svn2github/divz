#include "MainWindow.h"
#include "RenderOpts.h"

#include <QStyle>
#include <QSettings>

#if defined(STATIC_LINK)
Q_IMPORT_PLUGIN(qgif)
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qsvg)
Q_IMPORT_PLUGIN(qtiff)
#endif

// init RenderOpts defaults
bool RenderOpts::LastMirrorEnabled = true;
bool RenderOpts::ARGBWindow = false;
bool RenderOpts::HQRendering = false;
bool RenderOpts::FirstRun = false;
bool RenderOpts::OxygenStyleQuirks = false;
bool RenderOpts::DisableVideoProvider = false;
QColor RenderOpts::hiColor;


#include <QtGui/QApplication>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
		
	#if !defined(Q_OS_MAC) // raster on OSX == b0rken
	// use the Raster GraphicsSystem as default on 4.5+
	#if QT_VERSION >= 0x040500
	QApplication::setGraphicsSystem("raster");
	#endif
	#endif

	app.setApplicationName("DViz");
	app.setApplicationVersion("0.1.0");
	app.setOrganizationName("Josiaih Bryan");
	
	RenderOpts::OxygenStyleQuirks = app.style()->objectName() == QLatin1String("oxygen");
	
	QSettings s;
	//RenderOpts::FirstRun = s.value("fotowall/firstTime", true).toBool();
	RenderOpts::hiColor = app.palette().color(QPalette::Highlight);
	RenderOpts::DisableVideoProvider = app.arguments().contains("-novideo");
	//s.setValue("fotowall/firstTime", false);


	
	MainWindow mw;
	//mw.showMaximized();
	mw.show();
	
	return app.exec();
}

