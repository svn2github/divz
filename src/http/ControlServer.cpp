#include "ControlServer.h"
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

#include <QTcpSocket>

#include <QDateTime>
#include <QFile>
#include <QFileInfo>

ControlServer::ControlServer(quint16 port, QObject* parent)
	: HttpServer(port,parent)
{
	mw = MainWindow::mw();
}


// Basic Idea:
// Start:
// List of Groups in Document (table: col1: image, col2: name, col3: live flag) 
// Click on name/image/live to load group, go live if not live already
// After click on group, show list of slides in that group using the SlideGroupViewControl active for the live output instance
// 	get the model from the control
//	show list of slides (table: col1: image, col2: text if any, col3: live flag)
//	click slide to go live

	
void ControlServer::dispatch(QTcpSocket *socket, const QStringList &path, const QStringMap &query)
{
	
	//generic404(socket,path,query);
	QString pathStr = path.join("/");
	//qDebug() << "pathStr: "<<pathStr;
	
	if(pathStr.isEmpty())
	{
		screenListGroups(socket,path,query);
	}
	else
	if(pathStr.startsWith("group/"))
	{
		screenLoadGroup(socket,path,query);
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
	else
	if(pathStr.startsWith("go/"))
	{
		QStringList pathCopy = path;
		pathCopy.takeFirst();
		QString dir = pathCopy.isEmpty() ? "next" : pathCopy.takeFirst().toLower();

		// to access/set quickslide
		int liveId = AppSettings::taggedOutput("live")->id();
		SlideGroupViewControl *viewControl = mw->viewControl(liveId);

		if(dir == "prev")
			viewControl->prevSlide();
		else
		if(dir == "next")
			viewControl->nextSlide();
		else
			generic404(socket,path,query);

		Http_Send_Response(socket,"HTTP/1.0 204 Gone!") << "";

	}
	else
	if(pathStr.startsWith("toggle/"))
	{
		QStringList pathCopy = path;
		pathCopy.takeFirst();
		QString control = pathCopy.isEmpty() ? "black" : pathCopy.takeFirst().toLower();
		bool flag = pathCopy.isEmpty() ? 0 : pathCopy.takeFirst().toInt();
		
		// to access/set black/clear
		int liveId = AppSettings::taggedOutput("live")->id();
		OutputControl * outputControl = mw->outputControl(liveId);
		
		// to access/set quickslide
		SlideGroupViewControl *viewControl = mw->viewControl(liveId);
		
		if(control == "black")
			outputControl->fadeBlackFrame(flag);	
		else
		if(control == "clear")
			outputControl->fadeClearFrame(flag);
		else
		if(control == "qslide")
		{
			viewControl->setQuickSlideText(query["text"]);
			viewControl->showQuickSlide(flag);
		}
		else
			generic404(socket,path,query);
			
		Http_Send_Response(socket,"HTTP/1.0 204 Toggled Control") << "";
	}
	else
	{
		generic404(socket,path,query);
	}
// 	else
// 	{
// 		Http_Send_404(socket) 
// 			<< "<h1>Oops!</h1>\n"
// 			<< "Hey, my bad! I can't find \"<code>"<<toPathString(path,query)<<"</code>\"! Sorry!";
// 	}
	
}

void ControlServer::screenListGroups(QTcpSocket *socket, const QStringList &path, const QStringMap &query)
{
	Document * doc = mw->currentDocument();
	if(!doc)
	{
		generic404(socket,path,query);
		return;
	}
	
	int liveId = AppSettings::taggedOutput("live")->id();
	SlideGroup *liveGroup = mw->outputInst(liveId)->slideGroup();
	
	DocumentListModel * model = mw->documentListModel();
	
	uint secs = QDateTime::currentDateTime().toTime_t();
	QVariantList outputGroupList;
	for(int idx = 0; idx < model->rowCount(); idx++)
	{
		SlideGroup * group = model->groupAt(idx);
		
		QVariantMap row;
		
		QVariant tooltip = model->data(model->index(group->groupNumber(),0), Qt::ToolTipRole);
		QString viewText = group->assumedName();
		
		if(tooltip.isValid())
			viewText = tooltip.toString();
		
		row["group"]     = idx;
		row["text"]      = viewText;
		row["live_flag"] = group == liveGroup;
		row["date"]	 = secs;
		
		outputGroupList << row;
	}
	
	SimpleTemplate tmpl(":/data/http/doc.tmpl");
	tmpl.param("list",outputGroupList);
	
	OutputControl * outputControl = mw->outputControl(liveId);
	tmpl.param("black_toggled", outputControl->isBlackToggled());
	tmpl.param("clear_toggled", outputControl->isClearToggled());
	
	SlideGroupViewControl *viewControl = mw->viewControl(liveId);
	tmpl.param("qslide_toggled", viewControl->isQuickSlideToggled());
		
	if(doc->filename().isEmpty())
		tmpl.param("docfile",tr("New File"));
	else
		tmpl.param("docfile",QFileInfo(doc->filename()).baseName());
	
	Http_Send_Ok(socket) << tmpl.toString();
}


void ControlServer::screenLoadGroup(QTcpSocket *socket, const QStringList &path, const QStringMap &query)
{
	QStringList pathCopy = path;
	pathCopy.takeFirst();
	int groupIdx = pathCopy.takeFirst().toInt();
	QString nextPathElement = pathCopy.isEmpty() ? "" : pathCopy.takeFirst().toLower();

	Document * doc = mw->currentDocument();
	if(!doc)
	{
		generic404(socket,path,query);
		return;
	}
	
	if(groupIdx < 0 || groupIdx >= doc->numGroups())
	{
		generic404(socket,path,query);
		return;
	}
	
	DocumentListModel * docModel = mw->documentListModel();
	SlideGroup *group = docModel->groupAt(groupIdx);
	if(!group)
	{
		generic404(socket,path,query);
		return;
	}
	
	int liveId = AppSettings::taggedOutput("live")->id();
	SlideGroup *liveGroup = mw->outputInst(liveId)->slideGroup();
	
	SlideGroupViewControl *viewControl = mw->viewControl(liveId);
	Slide * liveSlide = viewControl->selectedSlide();
	
	// to access/set black/clear
	OutputControl * outputControl = mw->outputControl(liveId);
	
	Slide * slide = 0;
		
	if(query.contains("slide"))
	{
		int idx = query.value("slide").toInt();
		if(idx < 0 || idx > group->numSlides())
		{
			generic404(socket,path,query);
			return;
		}
		
		slide = group->at(idx);
		
		if(liveGroup == group &&
		   liveSlide != slide &&
		   nextPathElement != "icon")
		{
			// this is JUST a change slide request
			mw->setLiveGroup(liveGroup,slide);
			
			// 204 = HTTP No Content, ("...[the browser] SHOULD NOT change its document view...")
			Http_Send_Response(socket,"HTTP/1.0 204 Changed Slide") << "";
			
			// BlackBerry Internet Browser doesnt honor HTTP 204, so we have to use javascript to make it not change the current document
			
			return;
		}
			
	}
	
	SlideGroupListModel *model = viewControl->slideGroupListModel();
	
	if(nextPathElement == "icon")
	{
		QVariant icon;
		if(slide)
		{
			// If the slide group was just loaded to live for the first time
			// this session, the icons could come back gray if left in 
			// queued icon gen mode. Therefore, turn that mode off for now.
			bool oldMode = model->queuedIconGenerationMode();
			model->setQueuedIconGenerationMode(false);
			
			icon = model->data(model->indexForSlide(slide), Qt::DecorationRole);
			
			model->setQueuedIconGenerationMode(oldMode);
		
		}
		else
		{
			icon = docModel->data(docModel->indexForGroup(group), Qt::DecorationRole);
		}
		
		if(icon.isValid())
		{
			QHttpResponseHeader header(QString("HTTP/1.0 200 OK"));
			header.setValue("content-type", "image/png");
			respond(socket,header);
			
			QPixmap iconPixmap = icon.value<QPixmap>();
			iconPixmap.save(socket, "PNG");
		}
		else
		{
			generic404(socket,path,query);
			return;
		}
	}
	else
	{
		if(liveGroup != group ||
		   liveSlide != slide)
			mw->setLiveGroup(group,
					 ! slide ? liveSlide : slide); // prevent changing slides when loading the group page if group already live on different slide
		
		SlideGroupViewControl *viewControl = mw->viewControl(liveId);
		SlideGroupListModel *model = viewControl->slideGroupListModel();
		Slide * liveSlide = viewControl->view()->slide(); //selectedSlide();
		
		if(!model)
		{
			Http_Send_404(socket) 
				<< "Wierd - slide group model not found!";
			return;
		}
		
		QVariantList outputSlideList;
		for(int idx = 0; idx < model->rowCount(); idx++)
		{
			Slide * slide = model->slideAt(idx);
			QVariantMap row;
			
			QString viewText = model->data(model->index(slide->slideNumber(),0), Qt::DisplayRole).toString();
			QString toolText = model->data(model->index(slide->slideNumber(),0), Qt::ToolTipRole).toString();
			
			if(!toolText.trimmed().isEmpty())
				viewText = toolText;
			
			row["slide"]     = idx;
			row["text"]      = viewText;
			row["live_flag"] = slide == liveSlide;
			
			outputSlideList << row;
		}
		
		SimpleTemplate tmpl(":/data/http/group.tmpl");
		tmpl.param("list",outputSlideList);
		tmpl.param("groupidx", docModel->indexForGroup(group).row());
		tmpl.param("grouptitle", group->assumedName());
		
		tmpl.param("black_toggled", outputControl->isBlackToggled());
		tmpl.param("clear_toggled", outputControl->isClearToggled());
		tmpl.param("qslide_toggled", viewControl->isQuickSlideToggled());
		
		tmpl.param("date", QDateTime::currentDateTime().toTime_t());
		
		Http_Send_Ok(socket) << tmpl.toString();
	}
	
}

