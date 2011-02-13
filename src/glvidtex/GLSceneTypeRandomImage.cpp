#include "GLSceneTypeRandomImage.h"
#include "../imgtool/exiv2-0.18.2-qtbuild/src/image.hpp"
#include "GLTextDrawable.h" // for htmlToPlainText

GLSceneTypeRandomImage::GLSceneTypeRandomImage(QObject *parent)
	: GLSceneType(parent)
	, m_currentIndex(0)
{
	m_fieldInfoList 
		<< FieldInfo("image", 
			"Image", 
			"The current image", 
			"Image", 
			true)
			
		<< FieldInfo("comments", 
			"Comments", 
			"The comments attached to the image via the EXIF data (if present.)", 
			"Text", 
			false)
			
		<< FieldInfo("filename", 
			"Filename", 
			"This class will attempt to parse the filename and extract a photograph number from it", 
			"Text", 
			false)
			
		<< FieldInfo("datetime", 
			"Date/Time", 
			"The date/time this file was taken (from the EXIF data.)", 
			"Text", 
			false);
		
		
	m_paramInfoList
		<< ParameterInfo("folder",
			"Folder",
			"Folder of images to display",
			QVariant::String,
			true,
			SLOT(setFolder(const QString&)))
			
		<< ParameterInfo("updateTime",
			"Update Time",
			"Time in minutes to wait between updates",
			QVariant::Int,
			true,
			SLOT(setUpdateTime(int)))
		
		<< ParameterInfo("changeTime",
			"Update Time",
			"Time in seconds to wait between changing images if needed",
			QVariant::Int,
			true,
			SLOT(setChangeTime(int)))
			
		<< ParameterInfo("notRandom",
			"Not Random",
			"Show images in order, not randomly",
			QVariant::Bool,
			true,
			SLOT(setNotRandom(bool)))
			
		<< ParameterInfo("autoRotate",
			"Auto Rotate",
			"Auto-rotate based on EXIF data",
			QVariant::Bool,
			true,
			SLOT(setAutoRotate(bool)))
		
		<< ParameterInfo("ignoreAR",
			"Ignore AR",
			"Stretch image to fill field provided",
			QVariant::Bool,
			true,
			SLOT(setIgnoreAR(bool)));
			
	PropertyEditorFactory::PropertyEditorOptions opts;
	
	opts.reset();
	opts.stringIsDir = true;
	m_paramInfoList[0].hints = opts;
	 
	opts.reset();
	opts.min = 1;
	opts.max = 15;
	m_paramInfoList[1].hints = opts;
	
	opts.reset();
	opts.min = 1;
	opts.max = 300;
	m_paramInfoList[2].hints = opts;
	
	opts.reset();
	opts.type = QVariant::Bool;
	opts.text = m_paramInfoList[3].title; 
	m_paramInfoList[3].hints = opts;
	
	opts.reset();
	opts.type = QVariant::Bool;
	opts.text = m_paramInfoList[4].title; 
	m_paramInfoList[4].hints = opts;
	
	opts.reset();
	opts.type = QVariant::Bool;
	opts.text = m_paramInfoList[5].title; 
	m_paramInfoList[5].hints = opts;
	
	connect(&m_reloadTimer, SIGNAL(timeout()), this, SLOT(reloadData()));
	connect(&m_changeTimer, SIGNAL(timeout()), this, SLOT(showNextImage()));
	//m_reloadTimer.setInterval(1 * 60 * 1000); // every 1 minute
	//setParam
	setParam("updateTime", 1);
	setParam("changeTime", 60);
	setParam("notRandom",  false);
	setParam("autoRotate", true);
	setParam("ignoreAR",   false);
	
	// Create seed for the random
	// That is needed only once on application startup
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());
}

void GLSceneTypeRandomImage::setLiveStatus(bool flag)
{
	GLSceneType::setLiveStatus(flag);
	
	if(flag)
	{
		m_reloadTimer.start();
		m_changeTimer.start();
		applyFieldData();
	}
	else
	{
		m_changeTimer.stop();
	}
}

void GLSceneTypeRandomImage::setParam(QString param, QVariant value)
{
	GLSceneType::setParam(param, value);
	
	if(param == "folder")
		reloadData();
	else
	if(param == "updateTime")
		m_reloadTimer.setInterval(value.toInt() * 60 * 1000);
	else
	if(param == "updateTime")
		m_changeTimer.setInterval(value.toInt() * 1000);
}

void GLSceneTypeRandomImage::reloadData()
{
	readFolder(folder());
}

void GLSceneTypeRandomImage::readFolder(const QString &folder) 
{
	QDir dir(folder);
	dir.setNameFilters(QStringList() << "*.jpg" << "*.JPG" << "*.jpeg" << "*.png" << "*.PNG");
	QFileInfoList list = dir.entryInfoList(QDir::Files, QDir::Name);
	
	m_images.clear();
	
	foreach(QFileInfo info, list)
	{
		ImageItem item;
		QString fullFile = info.absoluteFilePath();
		qDebug() << "GLSceneTypeRandomImage: Loading "<<fullFile;//<<" (ext:"<<ext<<")";
		
		QString comment = "";
		QString datetime = "";
		try
		{
			Exiv2::Image::AutoPtr exiv = Exiv2::ImageFactory::open(fullFile.toStdString()); 
			if(exiv.get() != 0)
			{
				exiv->readMetadata();
				Exiv2::ExifData& exifData = exiv->exifData();
				if (exifData.empty()) 
				{
					qDebug() << fullFile << ": No Exif data found in the file";
				}

				comment = exifData["Exif.Image.ImageDescription"].toString().c_str();
				comment = GLTextDrawable::htmlToPlainText(comment);
				
				datetime = exifData["Exif.Photo.DateTimeOriginal"].toString().c_str();
	
				if(comment.trimmed().isEmpty())
				{
					Exiv2::IptcData& iptcData = exiv->iptcData();
					comment = iptcData["Iptc.Application2.Caption"].toString().c_str();
					comment = GLTextDrawable::htmlToPlainText(comment);
					
					if (exifData.empty()) 
					{
						qDebug() << fullFile << ": No IPTC data found in the file";
					}
					else
					{
						qDebug() << "GLSceneTypeRandomImage: IPTC Caption:"<<comment;
					}
				}
				else
				{
					qDebug() << "GLSceneTypeRandomImage: EXIF Caption:"<<comment;
				}
				
				
			}
		}
		catch (Exiv2::AnyError& e) 
		{
			std::cout << "Caught Exiv2 exception '" << e << "'\n";
			//return -1;
		}
		
		item.imageFile = fullFile;
		
		if(!comment.isEmpty())
			item.comments = comment;
		
		QFileInfo fileInfo(fullFile);
		QString fileName = fileInfo.baseName().toLower();
		fileName = fileName.replace(QRegExp("\\d{2,6}-\\{\\d\\}"),"");
		fileName = fileName.replace(QRegExp("(dsc_|sdc)"), "");
		
		if(!fileName.isEmpty())
		{
			item.parsedFilename = "Photograph # "+ fileName;
		}
		
		if(!datetime.isEmpty())
		{
			QDateTime parsedDate = QDateTime::fromString(datetime, "yyyy:MM:dd hh:mm:ss");
			item.datetime = "Photographed " + parsedDate.toString("dddd, MMMM d, yyyy");
		}
		
		
		m_images << item;
	}
	
	if(scene())
		showNextImage();
}

void GLSceneTypeRandomImage::showNextImage()
{
	if(!notRandom())
	{
		int high = m_images.size() - 1;
		int low = 0;
		m_currentIndex = qrand() % ((high + 1) - low) + low;
	}
	
	if(m_currentIndex < 0 || m_currentIndex >= m_images.size())
		m_currentIndex = 0;
	
	ImageItem item = m_images[m_currentIndex];
	
	setField("image", 	item.imageFile);
	setField("comments", 	item.comments);
	setField("filename",	item.parsedFilename);
	setField("datetime",	item.datetime);
	
	m_currentIndex++;
}
