#include "AppSettings.h"
#include "RenderOpts.h"

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

#include "model/Output.h"
#include "model/SlideGroupFactory.h"
#include "songdb/SongSlideGroupFactory.h"

#include "itemlistfilters/SlideTextOnlyFilter.h"
#include "itemlistfilters/SlideNonTextOnlyFilter.h"
#include "itemlistfilters/SlideBackgroundOnlyFilter.h"
#include "itemlistfilters/SlideForegroundOnlyFilter.h"

// init RenderOpts defaults
bool RenderOpts::LastMirrorEnabled = true;
bool RenderOpts::ARGBWindow = false;
bool RenderOpts::HQRendering = false;
bool RenderOpts::FirstRun = false;
bool RenderOpts::OxygenStyleQuirks = false;
bool RenderOpts::DisableVideoProvider = false;
bool RenderOpts::DisableOpenGL = false;
QColor RenderOpts::hiColor;



QList<Output*> AppSettings::m_outputs;
bool AppSettings::m_useOpenGL = false;

QSizeF AppSettings::m_gridSize = QSizeF(10,10);
bool AppSettings::m_gridEnabled = true;
bool AppSettings::m_thirdGuideEnabled = true;
QMap<QString,QString> AppSettings::m_previousPathList;

double AppSettings::m_liveAspect = 0;
// double AppSettings::m_docAspect = 0;

int AppSettings::m_pixmapCacheSize = 256;
int AppSettings::m_crossFadeSpeed = 250; // ms
int AppSettings::m_crossFadeQuality = 15; // frames

AppSettings::LiveEditMode AppSettings::m_liveEditMode = AppSettings::LiveEdit; 

int AppSettings::m_autosaveTime = 60; // seconds

void AppSettings::initApp(const QString& appName)
{
	QString pluginPath = QString("%1/plugins").arg(QDir::currentPath());
	//qDebug() << "DViz Plugin Path:"<<pluginPath;

	qApp->addLibraryPath(pluginPath);
	//qDebug() << "Core Plugin Paths: "<< app.libraryPaths();

 	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	// start the cache at 256 MB of pixmaps
	QPixmapCache::setCacheLimit(256 * 1024);

	qRegisterMetaType<AbstractVisualItem::FillType>("FillType");

	AbstractItemFilter::registerFilterInstance(SlideTextOnlyFilter::instance());
	AbstractItemFilter::registerFilterInstance(SlideNonTextOnlyFilter::instance());
	AbstractItemFilter::registerFilterInstance(SlideBackgroundOnlyFilter::instance());
	AbstractItemFilter::registerFilterInstance(SlideForegroundOnlyFilter::instance());
	AbstractItemFilter::registerFilterInstance(SongFoldbackTextFilter::instance());

#if defined(VER)
	printf("DViz Version %s\n", VER);
	qApp->setApplicationVersion(VER); //"0.1.5");
#endif

	qApp->setApplicationName(appName);
	qApp->setOrganizationName("Josiah Bryan");
	qApp->setOrganizationDomain("mybryanlife.com");


	SlideGroupFactory::registerFactoryForType(SlideGroup::Generic, new SlideGroupFactory());
	SlideGroupFactory::registerFactoryForType(SlideGroup::Song,    new SongSlideGroupFactory());

	RenderOpts::OxygenStyleQuirks = qApp->style()->objectName() == QLatin1String("oxygen");

	QSettings s;
	//RenderOpts::FirstRun = s.value("fotowall/firstTime", true).toBool();
	RenderOpts::hiColor = qApp->palette().color(QPalette::Highlight);
	RenderOpts::DisableVideoProvider = qApp->arguments().contains("-novideo");
	//s.setValue("fotowall/firstTime", false);

	bool noOpenGL = false;

	if(qApp->arguments().contains("-nogl"))
	{
		noOpenGL = true;
	}
	else
	if(qApp->arguments().contains("-usegl"))
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

}

void AppSettings::load()
{
	m_outputs.clear();

	QSettings s;
	loadOutputs(&s);

	m_useOpenGL = s.value("app/use-opengl",true).toBool();
	m_gridSize = s.value("app/grid/size",m_gridSize).toSizeF();
	m_gridEnabled = s.value("app/grid/enabled",true).toBool();
	m_thirdGuideEnabled = s.value("app/thirdguide",true).toBool();
	QMap<QString,QVariant> map = s.value("app/previous-path-list").toMap();
	QList<QString> keys = map.keys();
	foreach(QString key, keys)
		m_previousPathList[key] = map[key].toString();
	
	m_pixmapCacheSize  = s.value("app/cache-size",256).toInt();
	m_crossFadeSpeed   = s.value("app/fade-speed",250).toInt();
	m_crossFadeQuality = s.value("app/fade-quality",15).toInt();
	
	m_liveEditMode = (LiveEditMode)s.value("app/live-edit-mode",0).toInt();
	
	m_autosaveTime = s.value("app/autosave",60).toInt();
	
	QPixmapCache::setCacheLimit(m_pixmapCacheSize * 1024);
	
	updateLiveAspectRatio();
}

void AppSettings::save()
{
	QSettings s;

	s.setValue("app/use-opengl",m_useOpenGL);

	s.setValue("app/grid/size",m_gridSize);
	s.setValue("app/grid/enabled",m_gridEnabled);
	s.setValue("app/thirdguide",m_thirdGuideEnabled);
	
	QMap<QString,QVariant> map;
	QList<QString> keys = m_previousPathList.keys();
	foreach(QString key, keys)
		map[key] = QVariant(m_previousPathList[key]);
	s.setValue("app/previous-path-list",map);
	
	s.setValue("app/cache-size",m_pixmapCacheSize);
	s.setValue("app/fade-speed",m_crossFadeSpeed);
	s.setValue("app/fade-quality",m_crossFadeQuality);
	
	s.setValue("app/live-edit-mode",(int)m_liveEditMode);
	s.setValue("app/autosave",m_autosaveTime);

	saveOutputs(&s);

	updateLiveAspectRatio();
}

void AppSettings::setLiveEditMode(LiveEditMode mode)
{
	m_liveEditMode = mode;
}

void AppSettings::setAutosaveTime(int time)
{
	m_autosaveTime = time;
}

QString AppSettings::previousPath(const QString& key)
{
	return m_previousPathList[key];
}

void AppSettings::setPreviousPath(const QString& key, const QString & path)
{
	m_previousPathList[key] = path;
}

void AppSettings::setCrossFadeSpeed(int x)
{
	m_crossFadeSpeed = x;
}

void AppSettings::setCrossFadeQuality(int x)
{
	m_crossFadeQuality = x;
}

void AppSettings::setPixmapCacheSize(int x)
{
	m_pixmapCacheSize = x;
	QPixmapCache::setCacheLimit(x * 1024);
}


void AppSettings::saveOutputs(QSettings *s)
{
	s->setValue("outputs/num",m_outputs.size());
	int num = 0;
	foreach(Output *out, m_outputs)
	{
		s->setValue(QString("outputs/output%1").arg(num++), out->toByteArray());
	}

}

void AppSettings::loadOutputs(QSettings *s)
{
	int num = s->value("outputs/num").toInt();
	for(int x=0; x<num;x++)
	{
		Output * out = new Output();
		out->fromByteArray(s->value(QString("outputs/output%1").arg(x)).toByteArray());
		m_outputs.append(out);
	}

	if(num == 0)
	{
		setupSystemPresetOutputs();
	}


}

void AppSettings::setupSystemPresetOutputs()
{
	Output * out = new Output();
		out->setIsSystem(true);
		out->setIsEnabled(true);
		out->setName("Live");
		out->setOutputType(Output::Screen);
		out->setScreenNum(0);
		out->setTags("live");
	m_outputs << out;

	out = new Output();
		out->setIsSystem(true);
		out->setIsEnabled(false);
		out->setName("Foldback");
		out->setOutputType(Output::Screen);
		out->setScreenNum(1);
		out->setTags("foldback");
	m_outputs << out;

	out = new Output();
		out->setIsSystem(true);
		out->setIsEnabled(false);
		out->setName("Secondary");
		out->setOutputType(Output::Screen);
		out->setScreenNum(2);
		out->setTags("secondary");
	m_outputs << out;
}

void AppSettings::addOutput(Output *out) { m_outputs.append(out); }

bool AppSettings::removeOutput(Output *out)
{
	if(out->isSystem())
		return false;

	m_outputs.removeAll(out);
	return true;
}

Output * AppSettings::taggedOutput(QString tag)
{
	foreach(Output *out, m_outputs)
		if(out->tags().indexOf(tag) > -1 || out->name().toLower().indexOf(tag) > -1)
			return out;
	return 0;
}

Output * AppSettings::outputById(int id)
{
	foreach(Output *out, m_outputs)
		if(out->id() == id)
			return out;
	return 0;
}

void AppSettings::setUseOpenGL(bool f)
{
	m_useOpenGL = f;
}

void AppSettings::setGridSize(QSizeF sz)
{
	m_gridSize = sz;
}

void AppSettings::setGridEnabled(bool flag)
{
	m_gridEnabled = flag;
}

void AppSettings::setThirdGuideEnabled(bool flag)
{
	m_thirdGuideEnabled = flag;
}

QPointF AppSettings::snapToGrid(QPointF newPos, bool halfGrid)
{
	if(gridEnabled())
	{
		QSizeF sz = AppSettings::gridSize();
		qreal x = sz.width()  / (halfGrid ? 2:1);
		qreal y = sz.height() / (halfGrid ? 2:1);
		newPos.setX(((int)(newPos.x() / x)) * x);
		newPos.setY(((int)(newPos.y() / y)) * y);
	}
	return newPos;
}

void AppSettings::updateLiveAspectRatio()
{
	Output *out = taggedOutput("live");
	if(out)
	{
		m_liveAspect = out->aspectRatio();
	}
	else
	{
		m_liveAspect = -1;
	}
}

