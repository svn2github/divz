#include "PPTSlideGroup.h"

#include "model/Document.h"
#include "model/Slide.h"
#include "model/SlideGroup.h"
#include "model/BackgroundItem.h"
#include "MainWindow.h"
#include "AppSettings.h"

#include "3rdparty/md5/qtmd5.h"

#include "NativeViewerWin32PPT.h"

#include <QProgressBar>
#include <QWidget>
#include <QVBoxLayout>
#include <QDir>
#include <QMessageBox>
#include <QApplication>

#define PPT_SNAPSHOT_CACHE_DIR "dviz-ppt-snapshot-cache"
#define DEBUG_PPTSLIDEGROUP 1

PPTSlideGroup::PPTSlideGroup() : SlideGroup(),
	m_file(""),
	m_mtime(0)
{
	if(MainWindow::mw())
		connect(MainWindow::mw(), SIGNAL(aspectRatioChanged(double)), this, SLOT(aspectRatioChanged(double)));
}

void PPTSlideGroup::setFile(const QString &file)
{
	if(DEBUG_PPTSLIDEGROUP)
	    qDebug() << "PPTSlideGroup::setFile(): "<<file;

	removeAllSlides();

	m_file = file;
	loadFile();
}

/* protected */
void PPTSlideGroup::removeAllSlides()
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

void PPTSlideGroup::aspectRatioChanged(double x)
{
	removeAllSlides();
	loadFile();
}


void PPTSlideGroup::loadFile()
{
	if(DEBUG_PPTSLIDEGROUP)
	    qDebug() << "PPTSlideGroup::loadFile(): "<<m_file;

	QWidget * win = new QWidget();
	win->setWindowTitle(QString("Importing %1 - DViz").arg(m_file));
	win->setWindowIcon(QIcon(":/data/icon-d.png"));

	double ar = 4/3 ; //MainWindow::mw() ? MainWindow::mw()->currentDocument()->aspectRatio()
			//	    : AppSettings::liveAspectRatio();
	int height = 240;
	QSize pptBoxSize(320,240); //(int)(height * ar),height);

	QVBoxLayout *vbox = new QVBoxLayout(win);

	QWidget * pptBox = new QWidget(win);
	pptBox->setMinimumSize(pptBoxSize);
	pptBox->setMaximumSize(pptBoxSize);
	vbox->addWidget(pptBox);

	QProgressBar *progress = new QProgressBar(win);
	vbox->addWidget(progress);

	NativeViewerWin32PPT *viewer = new NativeViewerWin32PPT();
	viewer->setContainerWidget(pptBox);
	viewer->setSlideGroup(this);

	int numSlides = viewer->numSlides();

	progress->setMaximum(numSlides);
	progress->setValue(0);

	win->adjustSize();
	win->show();

	qDebug() << "PPTSlideGroup::loadFile(): pptBoxSize:"<<pptBoxSize<<", pptBox->geom:"<<pptBox->geometry();

	viewer->show();

	QDir path(QString("%1/%2").arg(AppSettings::cachePath()).arg(PPT_SNAPSHOT_CACHE_DIR));
	if(!path.exists())
		QDir(AppSettings::cachePath()).mkdir(PPT_SNAPSHOT_CACHE_DIR);

	//QMessageBox::information(0, "Debug", QString("Cache Dir: %1").arg(path.absolutePath()));



	int lastModified = QFileInfo(m_file).lastModified().toTime_t();
	bool forceRecache = m_mtime != lastModified;
	m_mtime = lastModified;

	//numSlides = 1;
	for(int i=0; i<numSlides; i++)
	{
		QString cacheKey = QString("%1/%2/%3-slide%6-%4x%5.png")
					.arg(AppSettings::cachePath())
					.arg(PPT_SNAPSHOT_CACHE_DIR)
					.arg(MD5::md5sum(m_file))
					.arg(pptBoxSize.width())
					.arg(pptBoxSize.height())
					.arg(i+1);

		// remove existing cache file
		QFile cacheFile(cacheKey);
		if(forceRecache)
		    if(cacheFile.exists())
			cacheFile.remove();

		if(!cacheFile.exists())
		{
		    viewer->setSlide(i);
		    QPixmap snapshot = viewer->snapshot();
		    snapshot.save(cacheKey,"PNG");
		}

		Slide *slide = new Slide();
		BackgroundItem * bg = dynamic_cast<BackgroundItem*>(slide->background());
		bg->setFillType(AbstractVisualItem::Image);
		bg->setFillImageFile(cacheKey);

		slide->setSlideNumber(i);


		
		addSlide(slide);

		progress->setValue(i);
		qApp->processEvents();

		// QMessageBox::information(0, "Debug", QString("This is slide #%1").arg(i+1));
	}

	viewer->close();
	win->close();
	delete viewer;
	delete win;


}

bool PPTSlideGroup::fromXml(QDomElement & pe)
{

	qDebug() << "PPTSlideGroup::fromXml(): Start";
	saveGroupAttributes(pe);
	setFile(pe.attribute("file"));
	m_mtime = pe.attribute("mtime").toInt();

	qDebug() << "PPTSlideGroup::fromXml(): Done loading song group, num slides:"<<numSlides();
	return true;
}

void PPTSlideGroup::toXml(QDomElement & pe) const
{
	pe.setTagName("powerpoint");

	saveGroupAttributes(pe);
	pe.setAttribute("file",m_file);
	pe.setAttribute("mtime",m_mtime);
}


void PPTSlideGroup::fromVariantMap(QVariantMap &map)
{
	loadProperties(map);
	setFile(map["file"].toString());
	m_mtime = map["mtime"].toInt();
}
	
void PPTSlideGroup::toVariantMap(QVariantMap &map) const
{
	saveProperties(map);
	map["file"] = m_file;
	map["mtime"] = m_mtime;
}

void PPTSlideGroup::changeBackground(AbstractVisualItem::FillType, QVariant, Slide* )
{
	// NOT IMPLEMENTED for PowerPoint files
}
