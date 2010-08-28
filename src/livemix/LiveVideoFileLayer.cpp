#include <QFileInfo>
#include "LiveVideoFileLayer.h"
#include "ExpandableWidget.h"
#include "VideoThread.h"
#include "../glvidtex/GLWidget.h"
#include "../glvidtex/GLVideoDrawable.h"

LiveVideoFileLayer::LiveVideoFileLayer(QObject *parent)
	: LiveVideoLayer(parent)
	, m_video(0)
{
}

LiveVideoFileLayer::~LiveVideoFileLayer()
{
	// TODO close camera
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
	else
	{
		if(m_video)
			setVideo(m_video);
	}
}

void LiveVideoFileLayer::setVideo(VideoThread *vid)
{
	qDebug() << "LiveVideoFileLayer::setVideo: "<<vid;
	setVideoSource(vid);
	m_video = vid;
	setInstanceName(QFileInfo(vid->videoFile()).baseName());
}

QWidget * LiveVideoFileLayer::createLayerPropertyEditors()
{
	QWidget * base = new QWidget();
	QVBoxLayout *blay = new QVBoxLayout(base);
	blay->setContentsMargins(0,0,0,0);
	
	ExpandableWidget *groupContent = new ExpandableWidget("Video File",base);
	blay->addWidget(groupContent);
	
	QWidget *groupContentContainer = new QWidget;
	QFormLayout *formLayout = new QFormLayout(groupContentContainer);
	formLayout->setContentsMargins(3,3,3,3);
	
	groupContent->setWidget(groupContentContainer);
	
	PropertyEditorOptions opts;
	opts.stringIsFile = true;
	opts.fileTypeFilter = tr("Video Files (*.wmv *.mpeg *.mpg *.avi *.wmv *.flv *.mov *.mp4 *.m4a *.3gp *.3g2 *.mj2 *.mjpeg *.ipod *.m4v *.gsm *.swf *.dv *.dvd *.asf *.mtv *.roq *.aac *.ac3 *.aiff *.alaw *.iif);;Any File (*.*)");
	
	formLayout->addRow(tr("&File:"), generatePropertyEditor(this, "file", SLOT(setFile(const QString&)), opts));
	
 	
 	groupContent->setExpandedIfNoDefault(true);
	
	/////////////////////////////////////////
	
	QWidget *basics =  LiveVideoLayer::createLayerPropertyEditors();
	blay->addWidget(basics);
	
	return base;
}

void LiveVideoFileLayer::setFile(const QString& file)
{
	QFileInfo info(file);
	
	if(!info.exists())
	{
		qDebug() << "LiveVideoFileLayer::setFile: File does not exist: "<<file;
		return;
	}
	
	if(m_video)
	{
		m_video->stop();
		m_video->quit();
		m_video->deleteLater();
	}
	
	m_video = new VideoThread();
	m_video->setVideo(file);
	m_video->start();
	m_video->play();
	setVideo(m_video);
	
	m_props["file"].value = file;
	
	
}

void LiveVideoFileLayer::setLayerProperty(const QString& key, const QVariant& value)
{
	if(key == "file")
	{
		setFile(value.toString());
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
