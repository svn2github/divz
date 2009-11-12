#include "ControlServer.h"
#include "SimpleTemplate.h"

#include "AppSettings.h"
#include "MainWindow.h"
#include "DocumentListModel.h"
#include "OutputInstance.h"
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
	if(pathStr.startsWith("data/"))
	{
		serveFile(socket,pathStr);
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
		
		outputGroupList << row;
	}
	
	SimpleTemplate tmpl("doc.tmpl");
	tmpl.param("list",outputGroupList);
	
	Http_Send_Ok(socket) << tmpl.toString();
}


void ControlServer::screenLoadGroup(QTcpSocket *socket, const QStringList &path, const QStringMap &query)
{
	QStringList pathCopy = path;
	pathCopy.takeFirst();
	int groupIdx = pathCopy.takeFirst().toInt();
	
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
	}
	
	SlideGroupListModel *model = viewControl->slideGroupListModel();
	
	QString nextPathElement = pathCopy.isEmpty() ? "" : pathCopy.takeFirst().toLower();
	if(nextPathElement == "icon")
	{
		QVariant icon;
		if(slide)
		{
			icon = model->data(model->indexForSlide(slide), Qt::DecorationRole);
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
		qDebug() << "ControlServer: groupIdx:"<<groupIdx<<", groupPtr:"<<group<<", liveGroup:"<<liveGroup<<", slidePtr:"<<slide<<", liveSlide:"<<liveSlide;
		if(liveGroup != group ||
		   liveSlide != slide)
			mw->setLiveGroup(liveGroup,slide);

		SlideGroupViewControl *viewControl = mw->viewControl(liveId);
		Slide * liveSlide = viewControl->selectedSlide();
		SlideGroupListModel *model = viewControl->slideGroupListModel();

		qDebug() << "ControlServer: after change, slidePtr:"<<slide<<", liveSlide:"<<liveSlide<<", model: "<<model;
			
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
			
			QVariant tooltip = model->data(model->index(slide->slideNumber(),0), Qt::ToolTipRole);
			QString viewText = QString(tr("Slide # %1")).arg(slide->slideNumber()+1);
			
			if(tooltip.isValid())
				viewText = tooltip.toString();
			
			row["slide"]     = idx;
			row["text"]      = viewText;
			row["live_flag"] = slide == liveSlide;
			
			outputSlideList << row;
		}
		
		SimpleTemplate tmpl("group.tmpl");
		tmpl.param("list",outputSlideList);
		tmpl.param("groupidx", docModel->indexForGroup(group).row());
		tmpl.param("grouptitle", group->assumedName());
		
		Http_Send_Ok(socket) << tmpl.toString();
	}
	
}

