#include "TabletServer.h"
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

#include "songdb/SongSlideGroup.h"
#include "songdb/SongRecordListModel.h"

#include <QTcpSocket>

#include <QDateTime>
#include <QFile>
#include <QFileInfo>

TabletServer::TabletServer(quint16 port, QObject* parent)
	: HttpServer(port,parent)
{
	mw = MainWindow::mw();
	m_songListModel = SongRecordListModel::instance();
}

// Tablet server:
// Provides mobile interface for a tablet-sized, touch pased interface, primarily for stage usage by musicians.
// To that end, this focuses primarily on the song module, disregarding general slides, etc.
// It provides two "modes", both with the same functions. One mode operates on the database, the other on the current schedule.
// Functions available:
// - Search the database/schedule by name (really, title and full text), display list of results
// - Select and display the song in the right panel
// - Touch verses/"block" to make that verse live
 
// Notes to self:
// - Also need an online / "web connector"
// - Enable searching/editing of song databfase to add chords
// - Add select songs to Document
	
void TabletServer::dispatch(QTcpSocket *socket, const QStringList &path, const QStringMap &query)
{
	QString pathStr = path.join("/");
	qDebug() << "TabletServer::dispatch(): path: "<<path;
	
	if(pathStr.startsWith("data/")   ||
	   pathStr.startsWith(":/data/") ||
	   pathStr.startsWith("www/")    ||
	   pathStr.startsWith(":/data/www/"))
	{
		serveFile(socket,pathStr);
	}
	else
	if(pathStr.startsWith("favicon.ico"))
	{
		serveFile(socket,":/data/http/favicon.ico");
	}
	else
	{
		mainScreen(socket,path,query);
	}
	
// 	else
// 	{
// 		Http_Send_404(socket) 
// 			<< "<h1>Oops!</h1>\n"
// 			<< "Hey, my bad! I can't find \"<code>"<<toPathString(path,query)<<"</code>\"! Sorry!";
// 	}
	
}

void TabletServer::mainScreen(QTcpSocket *socket, const QStringList &path, const QStringMap &query)
{
	Document * doc = mw->currentDocument();
	if(!doc)
	{
		generic404(socket,path,query);
		return;
	}
	
	QStringList pathCopy = path;
	//pathCopy.takeFirst();
	QString control = pathCopy.isEmpty() ? "" : pathCopy.takeFirst().toLower();
	//bool flag = pathCopy.isEmpty() ? 0 : pathCopy.takeFirst().toInt();
	
	qDebug() << "TabletServer::mainScreen(): control: "<<control;
	
	if(control.isEmpty())
	{
	
		SimpleTemplate tmpl("data/http/tablet.tmpl");
		
// 		tmpl.param("list",outputGroupList);
// 		
// 		OutputControl * outputControl = mw->outputControl(liveId);
// 		tmpl.param("black_toggled", outputControl->isBlackToggled());
// 		tmpl.param("clear_toggled", outputControl->isClearToggled());
// 		
// 		SlideGroupViewControl *viewControl = mw->viewControl(liveId);
// 		tmpl.param("qslide_toggled", viewControl->isQuickSlideToggled());
// 			
// 		if(doc->filename().isEmpty())
// 			tmpl.param("docfile",tr("New File"));
// 		else
// 			tmpl.param("docfile",QFileInfo(doc->filename()).baseName());
		
		// 	Http_Send_Ok(socket) << 
		// 		"Content-Type: text/html\n\n" <<
		// 		tmpl.toString();
		
		QHttpResponseHeader header(QString("HTTP/1.0 200 OK"));
		header.setValue("Content-Type", "text/html");
	
		respond(socket,header);
		
		QTextStream output(socket);
		output.setAutoDetectUnicode(true);
		output << tmpl.toString();
	}
	else
	if(control == "list")
	{
		QString mode = query["mode"]; // Either "db" or "file"
		QString filter = query["filter"]; // Can be empty
		
		// Compile list of results matching mode (from db or from current sched) and the filter, and return as json string
		
		// TODO poll for changes to schedule / database
		
		// TODO add button to search online
		
		// TODO: will this code be useful?
		// 	int liveId = AppSettings::taggedOutput("live")->id();
		// 	SlideGroup *liveGroup = mw->outputInst(liveId)->slideGroup();
		// 	
		// 	DocumentListModel * model = mw->documentListModel();
		// 	
		// 	uint secs = QDateTime::currentDateTime().toTime_t();
		// 	QVariantList outputGroupList;
		// 	for(int idx = 0; idx < model->rowCount(); idx++)
		// 	{
		// 		SlideGroup * group = model->groupAt(idx);
		// 		
		// 		QVariantMap row;
		// 		
		// 		QVariant tooltip = model->data(model->index(group->groupNumber(),0), Qt::ToolTipRole);
		// 		QString viewText = group->assumedName();
		// 		
		// 		if(tooltip.isValid())
		// 			viewText = tooltip.toString();
		// 		
		// 		row["group"]     = idx;
		// 		row["text"]      = viewText;
		// 		row["live_flag"] = group == liveGroup;
		// 		row["date"]	 = secs;
		// 		
		// 		outputGroupList << row;
		// 	}
		// 	
		
		qDebug() << "TabletServer::mainScreen(): list: mode: "<<mode<<", filter: "<<filter;
		qDebug() << "TabletServer::mainScreen(): list: assuming 'db' mode for development right now...";
		
		m_songListModel->filter(filter);
		
		
		QVariantMap result;
		QVariantList resultList;
		
		bool moreResults = m_songListModel->rowCount() > 10;
		int resultCount = qMin(10, m_songListModel->rowCount());
		
		for(int i=0; i<resultCount; i++)
		{
			SongRecord *song = m_songListModel->songAt(i);
			if(!song)
			{
				qDebug() << "TabletServer::mainScreen(): list: db: No song at index: "<<i;
				continue; 
			}
			
			QVariantMap line;
			line["id"] = song->songId();
			line["title"] = song->title();
			line["text"] = song->text();
			
			resultList << line;
		}
		
		result["list"] = resultList;
		result["more"] = moreResults;
		
		// ...
		
		QString jsonString = m_toJson.serialize(result);
		
		Http_Send_Ok(socket) << 
			"Content-Type: text/plain\n\n" <<
			jsonString;
	}
	else
	if(control == "view_item")
	{
		QString mode = query["mode"]; // Either "db" or "file"
		QString itemid = query["itemid"]; // If mode==db, expect songId. If mode==file, expect groupId
		
		// Find item from DB or File, extract text blocks, return blocks
		// TODO resolve - blocks linked to slides
		// If mode==db, user should have option of adding to schedule
		// TODO provide editor for songs on tablet?
		
		QVariantMap result;
		
		// ...
		
		QString jsonString = m_toJson.serialize(result);
		
		Http_Send_Ok(socket) << 
			"Content-Type: text/plain\n\n" <<
			jsonString;
	}
	else
	if(control == "show_slide")
	{
		QString mode = query["mode"]; // Either "db" or "file"
		QString itemid = query["itemid"]; // If mode==db, expect songId. If mode==file, expect groupId
		QString blockid = query["blockid"]; // If mode==db, expect block name, if mode==file, expect slideId
		
		// find slide in schedule and send to output
		// return confirmation
		// TODO sync with any other tablets
		
		QVariantMap result;
		
		// ...
		
		QString jsonString = m_toJson.serialize(result);
		
		Http_Send_Ok(socket) << 
			"Content-Type: text/plain\n\n" <<
			jsonString;
	}
	else
	{
		generic404(socket,path,query);
	}
}

/*
void TabletServer::screenLoadGroup(QTcpSocket *socket, const QStringList &path, const QStringMap &query)
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
		
		SimpleTemplate tmpl("data/http/group.tmpl");
		tmpl.param("list",outputSlideList);
		tmpl.param("grid", dynamic_cast<SongSlideGroup*>(group) == NULL);
		tmpl.param("groupidx", docModel->indexForGroup(group).row());
		tmpl.param("grouptitle", group->assumedName());
		
		tmpl.param("black_toggled", outputControl->isBlackToggled());
		tmpl.param("clear_toggled", outputControl->isClearToggled());
		tmpl.param("qslide_toggled", viewControl->isQuickSlideToggled());
		
		tmpl.param("date", QDateTime::currentDateTime().toTime_t());
		
		Http_Send_Ok(socket) << tmpl.toString();
	}
	
}*/

