#include "WebSlideGroup.h"

#include "model/Document.h"
#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/BackgroundItem.h"
#include "MainWindow.h"
#include "AppSettings.h"

#include "3rdparty/md5/qtmd5.h"

//#include "NativeViewerWebKit.h"

#include "qvideo/QVideoProvider.h"

#include <QProgressBar>
#include <QWidget>
#include <QVBoxLayout>
#include <QDir>
#include <QMessageBox>
#include <QApplication>

#define DEBUG_WebSlideGroup 0

#include "NativeViewerWebKit.h"

#include "3rdparty/md5/qtmd5.h"
#include "AppSettings.h"

#define CACHE_DIR "dviz-webiconcache"



WebSlideGroup::WebSlideGroup() : SlideGroup(),
	m_url(""),
	m_mtime(0),
	m_native(0)
{
	if(MainWindow::mw())
		connect(MainWindow::mw(), SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
}

void WebSlideGroup::setUrl(const QUrl &url)
{
	if(DEBUG_WebSlideGroup)
	    qDebug() << "WebSlideGroup::setUrl(): "<<url;

	removeAllSlides();

	m_url = url;
	createSnapshot();
}

void WebSlideGroup::setNativeViewer(NativeViewerWebKit* v)
{
	m_native = v;
}

/* protected */
void WebSlideGroup::removeAllSlides()
{
	foreach(Slide *slide, m_slides)
	{
		disconnect(slide,0,this,0);
		//m_slides.removeAll(slide);
		emit slideChanged(slide, "remove", 0, "", "", QVariant());
	}

	m_slides.clear();
	qDeleteAll(m_slides);
}

void WebSlideGroup::aspectRatioChanged(double x)
{
// 	removeAllSlides();
// 	createSnapshot();
}


void WebSlideGroup::createSnapshot()
{
	if(DEBUG_WebSlideGroup)
	    qDebug() << "WebSlideGroup::createSnapshot(): "<<m_url;

// 	QWidget * win = new QWidget();
// 	win->setWindowTitle(QString("Importing %1 - DViz").arg(m_file));
// 	win->setWindowIcon(QIcon(":/data/icon-d.png"));
// 	
// 	QVBoxLayout *vbox = new QVBoxLayout(win);

	// TODO create code to snapshot actual webpage
	
	
	Slide *slide = new Slide();
	BackgroundItem * bg = dynamic_cast<BackgroundItem*>(slide->background());
	bg->setFillType(AbstractVisualItem::Image);
	
	QString file = cacheFile();
	if(!QFile(file).exists())
		QFile(":/data/url-icon.png").copy(file);
	
	bg->setFillImageFile(file);

	slide->setSlideNumber(0);

	addSlide(slide);
}

QString WebSlideGroup::cacheFile()
{
			
	QDir path(QString("%1/%2").arg(AppSettings::cachePath()).arg(CACHE_DIR));
	if(!path.exists())
		QDir(AppSettings::cachePath()).mkdir(CACHE_DIR);

	QString cacheFile = QString("%1/%2/%3")
		.arg(AppSettings::cachePath())
		.arg(CACHE_DIR)
		.arg(MD5::md5sum(m_url.toString()));

	return cacheFile;
}

bool WebSlideGroup::fromXml(QDomElement & pe)
{

	qDebug() << "WebSlideGroup::fromXml(): Start";
	saveGroupAttributes(pe);
	setUrl(pe.attribute("url"));
	m_mtime = pe.attribute("mtime").toInt();

	qDebug() << "WebSlideGroup::fromXml(): Done loading song group, num slides:"<<numSlides();
	return true;
}

void WebSlideGroup::toXml(QDomElement & pe) const
{
	pe.setTagName("web");

	saveGroupAttributes(pe);
	pe.setAttribute("url",m_url.toString());
	pe.setAttribute("mtime",m_mtime);
}


void WebSlideGroup::fromVariantMap(QVariantMap &map)
{
	loadProperties(map);
	setUrl(map["url"].toUrl());
	m_mtime = map["mtime"].toInt();
}
	
void WebSlideGroup::toVariantMap(QVariantMap &map) const
{
	saveProperties(map);
	map["url"] = m_url;
	map["mtime"] = m_mtime;
}

void WebSlideGroup::changeBackground(AbstractVisualItem::FillType, QVariant, Slide* )
{
	// NOT IMPLEMENTED for PowerPoint files
}
