#include "CameraSlideGroup.h"

#include "model/Document.h"
#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/BackgroundItem.h"
#include "MainWindow.h"
#include "AppSettings.h"

#include "3rdparty/md5/qtmd5.h"

#include "NativeViewerCamera.h"

//#include "qvideo/QVideoProvider.h"
#include "../camera_test/CameraThread.h"

#include <QProgressBar>
#include <QWidget>
#include <QVBoxLayout>
#include <QDir>
#include <QMessageBox>
#include <QApplication>

//#define PPT_SNAPSHOT_CACHE_DIR "dviz-ppt-snapshot-cache"
#define DEBUG_CAMERASLIDEGROUP 0

CameraSlideGroup::CameraSlideGroup() : SlideGroup(),
	m_device(""),
	m_mtime(0)
	//m_native(0)
{
	if(MainWindow::mw())
		connect(MainWindow::mw(), SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
}

void CameraSlideGroup::setDevice(const QString &file)
{
	if(DEBUG_CAMERASLIDEGROUP)
	    qDebug() << "CameraSlideGroup::setDevice(): "<<file;

	removeAllSlides();

	m_device = file;
	loadFile();
}

/* protected */
void CameraSlideGroup::removeAllSlides()
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

void CameraSlideGroup::aspectRatioChanged(double x)
{
	removeAllSlides();
	loadFile();
}


void CameraSlideGroup::loadFile()
{
	if(DEBUG_CAMERASLIDEGROUP)
	    qDebug() << "CameraSlideGroup::loadFile(): "<<m_device;

	// ignore return value, we will "steal" the cache file as the background for our fake slide
	//QVideoProvider::iconForFile(m_device);

	Slide *slide = new Slide();
	BackgroundItem * bg = dynamic_cast<BackgroundItem*>(slide->background());
	bg->setFillType(AbstractVisualItem::Image);
	bg->setFillImageFile(":/data/stock-panel-screenshot.png"); //QVideoIconGenerator::cacheFile(QFileInfo(m_device).canonicalFilePath()));

	slide->setSlideNumber(0);

	addSlide(slide);
}

bool CameraSlideGroup::fromXml(QDomElement & pe)
{

	qDebug() << "CameraSlideGroup::fromXml(): Start";
	saveGroupAttributes(pe);
	setDevice(pe.attribute("device"));
	m_mtime = pe.attribute("mtime").toInt();

	qDebug() << "CameraSlideGroup::fromXml(): Done loading song group, num slides:"<<numSlides();
	return true;
}

void CameraSlideGroup::toXml(QDomElement & pe) const
{
	pe.setTagName("camera");

	saveGroupAttributes(pe);
	pe.setAttribute("device",m_device);
	pe.setAttribute("mtime",m_mtime);
}


void CameraSlideGroup::fromVariantMap(QVariantMap &map)
{
	loadProperties(map);
	setDevice(map["device"].toString());
	m_mtime = map["mtime"].toInt();
}
	
void CameraSlideGroup::toVariantMap(QVariantMap &map) const
{
	saveProperties(map);
	map["device"] = m_device;
	map["mtime"] = m_mtime;
}

void CameraSlideGroup::changeBackground(AbstractVisualItem::FillType, QVariant, Slide* )
{
	// NOT IMPLEMENTED for Camera groups
}
