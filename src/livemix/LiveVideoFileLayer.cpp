#include <QFileInfo>
#include "LiveVideoFileLayer.h"
#include "ExpandableWidget.h"
#include "../glvidtex/GLWidget.h"
#include "../glvidtex/GLVideoDrawable.h"

#ifdef HAS_QT_VIDEO_SOURCE
#include "../glvidtex/QtVideoSource.h"
#else
class QtVideoSource
{

};
#endif

LiveVideoFileLayer::LiveVideoFileLayer(QObject *parent)
	: LiveVideoLayer(parent)
	, m_video(0)
{
}

LiveVideoFileLayer::~LiveVideoFileLayer()
{
	setVideoSource(0); // doesnt change m_video
	if(m_video)
	{
		delete m_video;
		m_video = 0;
	}
}

GLDrawable *LiveVideoFileLayer::createDrawable(GLWidget *widget)
{
	// We overrride createDrawable here just for future expansiosn sake
	return LiveVideoLayer::createDrawable(widget);
}

void LiveVideoFileLayer::initDrawable(GLDrawable *drawable, bool isFirst)
{
	//qDebug() << "LiveVideoFileLayer::setupDrawable: drawable:"<<drawable<<", copyFrom:"<<copyFrom;
	LiveVideoLayer::initDrawable(drawable, isFirst);

	if(isFirst)
	{
		// nothing to do 
		// Maybe in future set a static video source with a blue image?
	}
	
	if(m_video)
	{
		qDebug() << "LiveVideoFileLayer::initDrawable: setting video:"<<m_video;
		setVideo(m_video);
	}
	else
	{
		qDebug() << "LiveVideoFileLayer::initDrawable: m_video is null";
	}
}

void LiveVideoFileLayer::setVideo(QtVideoSource *vid)
{
	qDebug() << "LiveVideoFileLayer::setVideo: "<<vid;
	
// 	if(vid == m_video)
// 	{
// 		qDebug() << "LiveVideoFileLayer::setVideo: Not setting, vid == m_video";
// 		return;
// 	}
	
	#ifdef HAS_QT_VIDEO_SOURCE
	setVideoSource(vid);
	m_video = vid;
	setInstanceName(guessTitle(QFileInfo(vid->file()).baseName()));
	#endif
}

QWidget * LiveVideoFileLayer::createLayerPropertyEditors()
{
	//qDebug() << "LiveVideoFileLayer::createLayerPropertyEditors: mark1";
	QWidget * base = new QWidget();
	QVBoxLayout *blay = new QVBoxLayout(base);
	blay->setContentsMargins(0,0,0,0);
	
	ExpandableWidget *groupContent = new ExpandableWidget("Play List",base);
	blay->addWidget(groupContent);
	
	QWidget *groupContentContainer = new QWidget;
	QGridLayout *formLayout = new QGridLayout(groupContentContainer);
	formLayout->setContentsMargins(3,3,3,3);
	
	groupContent->setWidget(groupContentContainer);
	
	PropertyEditorOptions opts;
	opts.stringIsFile = true;
	opts.type = QVariant::String;
	opts.value = "";
	opts.fileTypeFilter = tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif);;Any File (*.*)");
	
	int row = 0;
	formLayout->addWidget(new QLabel(tr("Add File:")), row, 0);
	formLayout->addWidget(generatePropertyEditor(this, "file", SLOT(addFile(const QString&)), opts), row, 1);
	
	row ++;
	m_listWidget = new QListWidget(groupContentContainer);
	formLayout->addWidget(m_listWidget, row, 0, 1, 2);
	
	row ++;
	QWidget *buttonBase = new QWidget(groupContentContainer);
	QHBoxLayout *hbox = new QHBoxLayout(buttonBase);
	
	QPushButton *moveItemUp = new QPushButton("Move Up");
	hbox->addWidget(moveItemUp);
	connect(moveItemUp, SIGNAL(clicked()), this, SLOT(btnMoveItemUp()));
	
	QPushButton *moveItemDown = new QPushButton("Move Down");
	hbox->addWidget(moveItemDown);
	connect(moveItemDown, SIGNAL(clicked()), this, SLOT(btnMoveItemDown()));
	
	QPushButton *delItem = new QPushButton("Remove File");
	hbox->addWidget(delItem);
	connect(delItem, SIGNAL(clicked()), this, SLOT(btnDelItem()));
	formLayout->addWidget(buttonBase, row, 0, 1, 2);
	
	setupListWidget();
	
 	
 	groupContent->setExpandedIfNoDefault(true);
	
	/////////////////////////////////////////
	
	QWidget *basics =  LiveVideoLayer::createLayerPropertyEditors();
	blay->addWidget(basics);
	
	return base;
}

void LiveVideoFileLayer::setupListWidget()
{
	if(!m_listWidget)
		return;
	
	m_listWidget->clear();
	
	QStringList list = fileList();
	
	foreach(QString file, list)
	{
		QFileInfo info(file);
		QListWidgetItem *item = new QListWidgetItem(info.fileName());
		m_listWidget->addItem(item);
	}
}



void LiveVideoFileLayer::btnDelItem()
{
	if(!m_listWidget)
		return;
		
	QModelIndex idx = m_listWidget->currentIndex();
	
	if(!idx.isValid())
		return;
	
	QStringList list = fileList();
	if(list.isEmpty())
		return;
		
	int row = idx.row();
	if(row < 0 || row >= list.size())
		return;
		
	list.removeAt(row);
	
	setFileList(list);
}


void LiveVideoFileLayer::btnMoveItemUp()
{
	if(!m_listWidget)
		return;
		
	QModelIndex idx = m_listWidget->currentIndex();
	
	if(!idx.isValid())
		return;
	
	QStringList list = fileList();
	if(list.isEmpty())
		return;
		
	int row = idx.row();
	if(row < 1 || row >= list.size())
		return;
		
	QString file = list.takeAt(row);
	list.insert(row-1, file);
	
	setFileList(list);
}

void LiveVideoFileLayer::btnMoveItemDown()
{
	if(!m_listWidget)
		return;
		
	QModelIndex idx = m_listWidget->currentIndex();
	
	if(!idx.isValid())
		return;
	
	QStringList list = fileList();
	if(list.isEmpty())
		return;
		
	int row = idx.row();
	if(row < 0 || row >= list.size()-1)
		return;
		
	QString file = list.takeAt(row);
	list.insert(row+1, file);
	
	setFileList(list);
}

void LiveVideoFileLayer::addFile(const QString& file)
{
	QStringList list = fileList();
	list << file;
	setFileList(list);
}

void LiveVideoFileLayer::setFileList(const QStringList& list)
{
	qDebug() << "LiveVideoFileLayer::setFileList: list: "<<list;
	if(fileList() == list)
		return;
		
	if(!m_video)
	{
	
		#ifdef HAS_QT_VIDEO_SOURCE
		//qDebug() << "LiveVideoFileLayer::setFile: Creating video source";
		QtVideoSource *video = new QtVideoSource();
		setVideo(video);
		#else
		qDebug() << "LiveVideoFileLayer::setFile: Unable to play any videos - not compiled with QtVideoSource or QtMobility.";
		#endif
	}
	
	if(m_video)
	{
		QMediaPlaylist *medialist = playlist();
		medialist->clear();
	
		foreach(QString file, list)
		{
			QFileInfo info(file);
			
			if(!info.exists())
			{
				qDebug() << "LiveVideoFileLayer::setFileList: Warning: File does not exist: "<<file;
				
			}
			else
			{
				QUrl url = QUrl::fromLocalFile(info.absoluteFilePath());
				qDebug() << "LiveVideoFileLayer::setFileList: Adding media: "<<url;
				if (info.suffix().toLower() == QLatin1String("m3u"))
					medialist->load(url);
				else
					medialist->addMedia(url);
			}
		}
		
		m_video->player()->play();
	}
	
	m_props["fileList"] = list;
	
	setupListWidget();

	setInstanceName(guessTitle(QString("%1 Files").arg(list.size())));
}

QMediaPlaylist * LiveVideoFileLayer::playlist()
{
	#ifdef HAS_QT_VIDEO_SOURCE
	if(m_video)
		return m_video->playlist();
	#endif
	
	return 0;
	
}

QMediaPlayer * LiveVideoFileLayer::player()
{
	#ifdef HAS_QT_VIDEO_SOURCE
	if(m_video)
		return m_video->player();
	#endif
	
	return 0;
	
}

void LiveVideoFileLayer::setLayerProperty(const QString& key, const QVariant& value)
{
	if(key == "fileList")
	{
		setFileList(value.toStringList());
	}
	else
	if(m_video)
	{
		const char *keyStr = qPrintable(key);
		if(m_video->metaObject()->indexOfProperty(keyStr)>=0)
		{
			m_video->setProperty(keyStr, value);
		}
		
	}
	
	LiveLayer::setLayerProperty(key,value);
}

