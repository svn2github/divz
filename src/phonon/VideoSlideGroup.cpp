#include "VideoSlideGroup.h"

#include "model/Document.h"
#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/BackgroundItem.h"
#include "MainWindow.h"
#include "AppSettings.h"

#include "3rdparty/md5/qtmd5.h"

#include "NativeViewerPhonon.h"

#include "qvideo/QVideoProvider.h"

#include <QProgressBar>
#include <QWidget>
#include <QVBoxLayout>
#include <QDir>
#include <QMessageBox>
#include <QApplication>

//#define PPT_SNAPSHOT_CACHE_DIR "dviz-ppt-snapshot-cache"
#define DEBUG_VIDEOSLIDEGROUP 0

VideoSlideGroup::VideoSlideGroup() : SlideGroup(),
	m_file(""),
	m_mtime(0),
	m_native(0)
{
	if(MainWindow::mw())
		connect(MainWindow::mw(), SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
}

void VideoSlideGroup::setFile(const QString &file)
{
	if(DEBUG_VIDEOSLIDEGROUP)
	    qDebug() << "VideoSlideGroup::setFile(): "<<file;

	removeAllSlides();

	m_file = file;
	loadFile();
}

/* protected */
void VideoSlideGroup::removeAllSlides()
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

void VideoSlideGroup::aspectRatioChanged(double x)
{
	removeAllSlides();
	loadFile();
}


void VideoSlideGroup::loadFile()
{
	if(DEBUG_VIDEOSLIDEGROUP)
	    qDebug() << "VideoSlideGroup::loadFile(): "<<m_file;

	// ignore return value, we will "steal" the cache file as the background for our fake slide
	QVideoProvider::iconForFile(m_file);

	Slide *slide = new Slide();
	BackgroundItem * bg = dynamic_cast<BackgroundItem*>(slide->background());
	bg->setFillType(AbstractVisualItem::Image);
	bg->setFillImageFile(QVideoIconGenerator::cacheFile(QFileInfo(m_file).canonicalFilePath()));

	slide->setSlideNumber(0);

	addSlide(slide);


}

bool VideoSlideGroup::fromXml(QDomElement & pe)
{

	qDebug() << "VideoSlideGroup::fromXml(): Start";
	saveGroupAttributes(pe);
	setFile(pe.attribute("file"));
	m_mtime = pe.attribute("mtime").toInt();

	qDebug() << "VideoSlideGroup::fromXml(): Done loading song group, num slides:"<<numSlides();
	return true;
}

void VideoSlideGroup::toXml(QDomElement & pe) const
{
	pe.setTagName("video");

	saveGroupAttributes(pe);
	pe.setAttribute("file",m_file);
	pe.setAttribute("mtime",m_mtime);
}


void VideoSlideGroup::fromVariantMap(QVariantMap &map)
{
	loadProperties(map);
	setFile(map["file"].toString());
	m_mtime = map["mtime"].toInt();
}
	
void VideoSlideGroup::toVariantMap(QVariantMap &map) const
{
	saveProperties(map);
	map["file"] = m_file;
	map["mtime"] = m_mtime;
}

void VideoSlideGroup::changeBackground(AbstractVisualItem::FillType, QVariant, Slide* )
{
	// NOT IMPLEMENTED for PowerPoint files
}
