#include "AppSettings.h"

#if defined(STATIC_LINK)
Q_IMPORT_PLUGIN(qgif)
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qsvg)
Q_IMPORT_PLUGIN(qtiff)
#endif

#include <QDir>
#include <QSettings>
#include <QStyle>
#include <QList>
#include <QPixmapCache>
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QTime>

bool AppSettings::m_useOpenGL = false;

QMap<QString,QString> AppSettings::m_previousPathList;

int AppSettings::m_pixmapCacheSize = 256;

QDir AppSettings::m_cacheDir = QDir::temp();


void AppSettings::initApp(const QString& appName)
{
	QString pluginPath = QString("%1/plugins").arg(QDir::currentPath());
	//qDebug() << "DViz Plugin Path:"<<pluginPath;

	QTranslator * qtTranslator = new QTranslator();
	qtTranslator->load("qt_" + QLocale::system().name());
	qApp->installTranslator(qtTranslator);
	
// 	QTranslator myappTranslator;
// 	myappTranslator.load("myapp_" + QLocale::system().name());
// 	app.installTranslator(&myappTranslator);


	qApp->addLibraryPath(pluginPath);
	//qDebug() << "Core Plugin Paths: "<< app.libraryPaths();

 	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	// start the cache at 256 MB of pixmaps
	QPixmapCache::setCacheLimit(256 * 1024);

#if defined(VER)
	//printf("Version %s\n", VER);
	qApp->setApplicationVersion(VER); //"0.1.5");
#endif

	qApp->setApplicationName(appName);
	qApp->setOrganizationName("Josiah Bryan");
	qApp->setOrganizationDomain("mybryanlife.com");
}

void AppSettings::load()
{
	
	QSettings s;
	
	m_useOpenGL = s.value("app/use-opengl",true).toBool();
	QMap<QString,QVariant> map = s.value("app/previous-path-list").toMap();
	QList<QString> keys = map.keys();
	foreach(QString key, keys)
		m_previousPathList[key] = map[key].toString();
	
	m_pixmapCacheSize  = s.value("app/cache-size",256).toInt();
	m_cacheDir = QDir(s.value("app/cache-dir",QDir::temp().absolutePath()).toString());
	
	QPixmapCache::setCacheLimit(m_pixmapCacheSize * 1024);
}

void AppSettings::save()
{
	QSettings s;

	s.setValue("app/use-opengl",m_useOpenGL);

	
	QMap<QString,QVariant> map;
	QList<QString> keys = m_previousPathList.keys();
	foreach(QString key, keys)
		map[key] = QVariant(m_previousPathList[key]);
	s.setValue("app/previous-path-list",map);
	
	s.setValue("app/cache-size",m_pixmapCacheSize);
	s.setValue("app/cache-dir",m_cacheDir.absolutePath());
	
}


QString AppSettings::previousPath(const QString& key)
{
	return m_previousPathList[key];
}

void AppSettings::setPreviousPath(const QString& key, const QString & path)
{
	m_previousPathList[key] = path;
}

void AppSettings::setPixmapCacheSize(int x)
{
	m_pixmapCacheSize = x;
	QPixmapCache::setCacheLimit(x * 1024);
}

void AppSettings::setUseOpenGL(bool f)
{
	m_useOpenGL = f;
}

// The Cache Dir setting is designed to be used with the Network Viewer so that multiple viewers / controllers
// can share a central rendering cache for expensive operations, such as scaling a large image. With a shared
// central cache, an image (for example), only needs to be scaled down once, then all the clients hit the
// shared cache and load the scaled image, rather than each having to scale the original on their own.
void AppSettings::setCacheDir(const QDir &dir) { m_cacheDir = dir; }
