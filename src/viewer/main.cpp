#include "MainWindow.h"
#include "RenderOpts.h"
#include "AppSettings.h"

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
bool RenderOpts::DisableOpenGL = false;
QColor RenderOpts::hiColor;

#include <QApplication>
#include "model/SlideGroupFactory.h"
#include "songdb/SongSlideGroupFactory.h"
#include <QPixmapCache>


#include "itemlistfilters/SlideTextOnlyFilter.h"
#include "itemlistfilters/SlideNonTextOnlyFilter.h"
#include "itemlistfilters/SlideBackgroundOnlyFilter.h"
#include "itemlistfilters/SlideForegroundOnlyFilter.h"


int main(int argc, char **argv)
{
	#if !defined(Q_OS_MAC) // raster on OSX == b0rken
		// use the Raster GraphicsSystem as default on 4.5+
		#if QT_VERSION >= 0x040500
		QApplication::setGraphicsSystem("raster");
		#endif
 	#endif
 	
 	QApplication app(argc, argv);
 	
 	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	
	// start the cache at 256 MB of pixmaps
	QPixmapCache::setCacheLimit(256 * 1024);
	
	int qtype1 = qRegisterMetaType<AbstractVisualItem::FillType>("FillType");

	
	AbstractItemFilter::registerFilterInstance(SlideTextOnlyFilter::instance());
	AbstractItemFilter::registerFilterInstance(SlideNonTextOnlyFilter::instance());
	AbstractItemFilter::registerFilterInstance(SlideBackgroundOnlyFilter::instance());
	AbstractItemFilter::registerFilterInstance(SlideForegroundOnlyFilter::instance());
	
	app.setApplicationName("DViz Viewer");
	app.setOrganizationName("Josiah Bryan");
	app.setOrganizationDomain("mybryanlife.com");
	
	
	SlideGroupFactory::registerFactoryForType(SlideGroup::Generic, new SlideGroupFactory());
	SlideGroupFactory::registerFactoryForType(SlideGroup::Song,    new SongSlideGroupFactory());

	RenderOpts::OxygenStyleQuirks = app.style()->objectName() == QLatin1String("oxygen");
	
	QSettings s;
	//RenderOpts::FirstRun = s.value("fotowall/firstTime", true).toBool();
	RenderOpts::hiColor = app.palette().color(QPalette::Highlight);
	RenderOpts::DisableVideoProvider = app.arguments().contains("-novideo");
	//s.setValue("fotowall/firstTime", false);
	
	bool noOpenGL = false;
	
	if(app.arguments().contains("-nogl"))
	{
		noOpenGL = true;
	}
	else
	if(app.arguments().contains("-usegl"))
	{
		noOpenGL = false;
	}
	else
	{
		noOpenGL = ! s.value("opengl/disable").toBool();
	}
	s.setValue("opengl/disable",noOpenGL);
	noOpenGL = false;
	noOpenGL = true;

	RenderOpts::DisableOpenGL = noOpenGL;
	
	AppSettings::load();

	MainWindow mw;
	mw.show();
	int ret = app.exec();

	AppSettings::save();

	return ret;
}

