#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>
#include <QList>
#include <QMap>
#include <QSizeF>
#include <QPair>
#include <QDir>


class AppSettings
{
public:
	static void initApp(const QString& appName="DViz");
	static void load();
	static void save();
	
	
	static bool useOpenGL() { return m_useOpenGL; }
	static void setUseOpenGL(bool);

	static QString previousPath(const QString& key);
	static void setPreviousPath(const QString& key, const QString & path);
	
	static int pixmapCacheSize() { return m_pixmapCacheSize; }
	static void setPixmapCacheSize(int);
	
	static QString cachePath() { return cacheDir().absolutePath(); }
	static QDir cacheDir() { return m_cacheDir; }
	static void setCacheDir(const QDir &);
	

private:
	static bool m_useOpenGL;

	static QMap<QString,QString> m_previousPathList;

	static int m_pixmapCacheSize;
	
	static QDir m_cacheDir;

};

#endif
