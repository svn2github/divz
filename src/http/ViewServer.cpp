#include "ViewServer.h"
#include "SimpleTemplate.h"

#include "AppSettings.h"
#include "MainWindow.h"
#include "DocumentListModel.h"
#include "OutputInstance.h"
#include "OutputControl.h"
#include "model/Document.h"
#include "model/Output.h"
#include "model/SlideGroup.h"
#include "model/Slide.h"
#include "model/SlideGroupFactory.h"

#include "MyGraphicsScene.h"

#include <QTcpSocket>

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QImageWriter>

ViewServer::ViewServer(quint16 port, QObject* parent)
	: HttpServer(port,parent)
	, m_scene(0)
	, m_iconSize(400,0)
	, m_sceneRect(0,0,1024,768)
	
{
	//qDebug() << "ViewServer: using port: "<<port;
	mw = MainWindow::mw();
	
	// TODO: This code should be in a slot to listen for changes to aspect ration in real time.
	// For now, its static at the start of the server creation becuase I'm in a hurry.
	m_sceneRect = MainWindow::mw()->standardSceneRect();
	
	QRect r = m_sceneRect;
	qreal a = (qreal)r.height() / (qreal)r.width();
	
	m_iconSize.setHeight((int)(m_iconSize.width() * a));
}


// Basic Idea:
// Send client page with Title of Doc | Title of Group | Slide Title (if applicable), and Imge of Slide
//	 Page will use AJAX to poll for changes to slide and update slide, group title, and slide title as needed
// Group Title & Slide Title will be sent both in XHTTP response and in initial page
// Image will be sent using dedicated request
// Polling for changes will be handled by dedicated routine 

	
void ViewServer::dispatch(QTcpSocket *socket, const QStringList &path, const QStringMap &query)
{
	
	//generic404(socket,path,query);
	QString pathStr = path.join("/");
// 	qDebug() << "ViewServer::dispatch(): pathStr: "<<pathStr;
	
	if(pathStr.isEmpty())
	{
		reqSendPage(socket,path,query);
	}
	else
	if(pathStr.startsWith("image"))
	{
		reqSendImage(socket,path,query);
	}
	else
	if(pathStr.startsWith("poll"))
	{
		reqCheckForChange(socket,path,query);
	}
	else
	if(pathStr.startsWith("data/") ||
	   pathStr.startsWith(":/data/"))
	{
		serveFile(socket,pathStr);
	}
	else
	if(pathStr.startsWith("favicon.ico"))
	{
		serveFile(socket,":/data/http/favicon.ico");
	}
	
// 		// to access/set quickslide
// 		int liveId = AppSettings::taggedOutput("live")->id();
// 		SlideGroupViewControl *viewControl = mw->viewControl(liveId);

/*		// to access/set black/clear
		int liveId = AppSettings::taggedOutput("live")->id();
		OutputControl * outputControl = mw->outputControl(liveId);
		
		// to access/set quickslide
		SlideGroupViewControl *viewControl = mw->viewControl(liveId);
		
		if(control == "black")
			outputControl->fadeBlackFrame(flag);	*/
		
		
	else
	{
		generic404(socket,path,query);
	}

	
}

void ViewServer::reqSendPage(QTcpSocket *socket, const QStringList &path, const QStringMap &query)
{
 	Document * doc = mw->currentDocument();
	
	QString docName = "No Document Loaded",
		groupName = "No Group Loaded",
		slideName = "No Slide Loaded";
	int slideId = -1;
	
	if(doc)
	{
		int liveId = AppSettings::taggedOutput("live")->id();
		SlideGroup *liveGroup = mw->outputInst(liveId)->slideGroup();
		
		SlideGroupViewControl *viewControl = mw->viewControl(liveId);
		Slide * liveSlide = viewControl->selectedSlide();
		
		docName = doc->filename().isEmpty() ? tr("New File") : QFileInfo(doc->filename()).baseName();
		if(liveGroup)
			groupName = liveGroup->assumedName();
		if(liveSlide)
			slideName = liveSlide->assumedName();
		
		if(liveSlide)
			slideId = liveSlide->slideId();
	}
	
	
	SimpleTemplate tmpl(":/data/http/view_page.tmpl");
	tmpl.param("doc_name",docName);
	tmpl.param("group_name",groupName);
	tmpl.param("slide_name",slideName);
	tmpl.param("slide_id",slideId);
	
	Http_Send_Ok(socket) << tmpl.toString();
}

void ViewServer::reqCheckForChange(QTcpSocket *socket, const QStringList &path, const QStringMap &query)
{
 	Document * doc = mw->currentDocument();
	
	QString docName = "No Document Loaded",
		groupName = "No Group Loaded",
		slideName = "No Slide Loaded";
	int slideId = -1;
	
	if(doc)
	{
		int liveId = AppSettings::taggedOutput("live")->id();
		SlideGroup *liveGroup = mw->outputInst(liveId)->slideGroup();
		
		SlideGroupViewControl *viewControl = mw->viewControl(liveId);
		Slide * liveSlide = viewControl->selectedSlide();
		
		docName = doc->filename().isEmpty() ? tr("New File") : QFileInfo(doc->filename()).baseName();
		if(liveGroup)
			groupName = liveGroup->assumedName();
		if(liveSlide)
			slideName = liveSlide->assumedName();
		
		if(liveSlide)
			slideId = liveSlide->slideId();
	}
	
	int id = query["slide_id"].toInt();
	QString name = QUrl::fromPercentEncoding(query["slide_name"].toAscii());
	if(id   != slideId ||
	   name != slideName)
	{
// 		qDebug() << "ViewServer::reqCheckForchange: Changed!";
		Http_Send_Response(socket,"HTTP/1.0 200 Slide Changed") << 
			"{slide_id:"	 <<slideId<<","<<
			 "slide_name:\"" <<slideName<<"\","<<
			 "group_name:\"" <<groupName<<"\","<<
			 "doc_name:\""	 <<docName<<"\"};";
	}
	else
	{
// 		qDebug() << "ViewServer::reqCheckForchange: No Change!";
		Http_Send_Response(socket,"HTTP/1.0 200 No Change") << "{no_change:true};";
	}
	
}

void ViewServer::reqSendImage(QTcpSocket *socket, const QStringList &path, const QStringMap &query)
{
	// Generate Image
	QImage image;
	
	Document * doc = mw->currentDocument();
	
	QSize size = m_iconSize;
	
	if(!query["size"].trimmed().isEmpty())
	{
		QStringList sizeStr = query["size"].toLower().trimmed().split("x");
		if(sizeStr.size() >= 2)
		{
			int w = sizeStr[0].toInt();
			int h = sizeStr[1].toInt();
			size.scale(w,h,Qt::KeepAspectRatio);
		}
	}
	
	
	QString format = query["format"].toLower().trimmed();
	
	if(format.isEmpty())
		format = "jpeg";
	if(format == "jpg")
		format = "jpeg";
	
	if(format != "jpeg" && 
	   format != "png")
		format = "jpeg";
	
	if(doc)
	{
		int liveId = AppSettings::taggedOutput("live")->id();
		SlideGroup *liveGroup = mw->outputInst(liveId)->slideGroup();
		if(liveGroup)
		{
			SlideGroupViewControl *viewControl = mw->viewControl(liveId);
			Slide * liveSlide = viewControl->selectedSlide();
			if(liveSlide)
			{
				
				if(!m_scene)
					m_scene = new MyGraphicsScene(MyGraphicsScene::StaticPreview);
				if(m_scene->sceneRect() != m_sceneRect)
					m_scene->setSceneRect(m_sceneRect);
					
				m_scene->setMasterSlide(liveGroup->masterSlide());
				m_scene->setSlide(liveSlide);
					
				int icon_w = size.width();
				int icon_h = size.height();
					
				QPixmap icon(icon_w,icon_h);
				QPainter painter(&icon);
				painter.fillRect(0,0,icon_w,icon_h, format == "jpeg" ? Qt::white : Qt::transparent);
				painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
				painter.setRenderHint(QPainter::Antialiasing, true);
				painter.setRenderHint(QPainter::TextAntialiasing, true);
				
				m_scene->render(&painter,QRectF(0,0,icon_w,icon_h),m_sceneRect);
				painter.setPen(Qt::black);
				painter.setBrush(Qt::NoBrush);
				painter.drawRect(0,0,icon_w-1,icon_h-1);
				
				m_scene->clear();
				
				image = icon.toImage();
			}
		}
	}
	
	// Send Header
	QHttpResponseHeader header(QString("HTTP/1.0 200 OK"));
	header.setValue("Content-Type", QString("image/%1").arg(format));
	respond(socket,header);
	
	if(format == "jpeg")
	{
		if(image.format() != QImage::Format_RGB32)
			image = image.convertToFormat(QImage::Format_RGB32);
	}
	else
	{
		if(image.format() != QImage::Format_ARGB32)
			image = image.convertToFormat(QImage::Format_ARGB32);
	}
			
	// Write Jpeg Image
	QImageWriter writer;	
	writer.setDevice(socket);
	writer.setFormat(format == "png" ? "png" : "jpg");
	
	if(!writer.write(image))
		qDebug() << "ViewServer::reqSendImage(): QImageWriter reported error:"<<writer.errorString();

}

