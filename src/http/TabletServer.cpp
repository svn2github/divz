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
#include "songdb/SongBrowser.h"

#include "3rdparty/md5/qtmd5.h"

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
	
void TabletServer::dispatch(QTcpSocket *socket, const QStringList &path, const QStringMap &query, const QHttpRequestHeader &request)
{
	QString pathStr = path.join("/");
	//qDebug() << "TabletServer::dispatch(): path: "<<path;
	
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
	pathCopy.takeFirst(); // should be "tablet", but could be anything - /tablet/ is canonized for use in the tablet cache manifest for sfari
	QString control = pathCopy.isEmpty() ? "" : pathCopy.takeFirst().toLower();
	//bool flag = pathCopy.isEmpty() ? 0 : pathCopy.takeFirst().toInt();
	
	//qDebug() << "TabletServer::mainScreen(): control: "<<control;
	
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
	
		respond(socket, header);
		
		QTextStream output(socket);
		output.setAutoDetectUnicode(true);
		output << tmpl.toString();
	}
	else
	if(control == "list")
	{
		QString mode   = query["mode"]; // Either "db" or "file"
		QString filter = query["filter"]; // Can be empty
		
		bool pollingFlag  = query["poll"] == "true";
		QString clientMD5 = query["md5"]; // the md5 of the list contents to use to check for changes if pollingFlag == true
		
		// Compile list of results matching mode (from db or from current sched) and the filter, and return as json string
		
		// TODO add button to search online
		
		//qDebug() << "TabletServer::mainScreen(): list: mode: "<<mode<<", filter: "<<filter;
		
		QVariantMap result;
		QVariantList resultList;
			
		bool moreResults = false;
		int listCutoffLimit = 10; // included in results even though only relevant to 'db' mode
		
		// Buffer for compiling MD5 hash
		QStringList md5sigList;
			
		if(mode == "db")
		{
			bool isChanged = true;
			if(pollingFlag)
			{
				int currentChangeSequence = m_songListModel->currentChangeSequence();
				QString seqString = QString::number(currentChangeSequence);
				
				// md5sigList is used to generate the md5 to send back to the client
				md5sigList << seqString;
				
				QString seqMd5 = MD5::md5sum(md5sigList.join(""));
				
				isChanged = false;
				
				if(seqMd5 != clientMD5)
					isChanged = true;
			}
			
			// Only regen list if 'isChanged' - and isChanged is only set to 'false' if:
			//  - Client is in polling mode
			//  - Client MD5 matches the MD5 of current sequence number
			// We do this because generating the list makes a call to filter()
			// which can cause trouble if the filter string is empty and the user
			// in the DViz main UI is trying to search for something (which would NOT change
			// the sequence number above, but would change the filter) 
			if(isChanged)
			{
				// List cutoff limit only relevant when searching/quering database.
				// Cutoff not honored or implemented for file schedule
				QString maxResults = query["max"];
				if(!maxResults.isEmpty())
					listCutoffLimit = maxResults.toInt();
				if(listCutoffLimit < 1)
					listCutoffLimit = 1;
				
				m_songListModel->filter(filter);
				
				moreResults = m_songListModel->rowCount() > listCutoffLimit;
				int resultCount = qMin(listCutoffLimit, m_songListModel->rowCount());
				
				for(int i=0; i<resultCount; i++)
				{
					SongRecord *song = m_songListModel->songAt(i);
					if(!song)
					{
						qDebug() << "TabletServer::mainScreen(): list: db: No song at index: "<<i;
						continue;
					}
					
					QVariantMap line;
					line["id"]    = song->songId();
					line["title"] = song->title();
					line["text"]  = song->text();
					line["arr"]   = song->defaultArrangement()->arrangement().join(", ");
					
					resultList << line;
					
					// We use the sequence md5 (above) for the md5 sig instead of the list contents now
// 					md5sigList.append(QString::number(song->songId()));
// 					md5sigList.append(song->title());
// 					md5sigList.append(song->text());
				}
			}
		}
		else
		{
// 			int liveId = AppSettings::taggedOutput("live")->id();
// 			SlideGroup *liveGroup = mw->outputInst(liveId)->slideGroup();
			
			DocumentListModel * model = mw->documentListModel();
			
			//uint secs = QDateTime::currentDateTime().toTime_t();
			
			QString lowerFilter = filter.toLower();
			
			for(int idx = 0; idx < model->rowCount(); idx++)
			{
				SlideGroup * group = model->groupAt(idx);
				
				SongSlideGroup *songGroup = dynamic_cast<SongSlideGroup*>(group);
				
				if(songGroup && ( 
					songGroup->groupTitle().toLower().contains(lowerFilter) ||
					songGroup->text().toLower().contains(lowerFilter)))
				{
					QVariantMap row;
					
					QVariant tooltip = model->data(model->index(group->groupNumber(),0), Qt::ToolTipRole);
					QString viewText = group->assumedName();
					
					if(tooltip.isValid())
						viewText = tooltip.toString();
					
					row["id"]        = idx; //songGroup->groupId();
					row["title"]     = viewText;
					row["text"]	 = songGroup->text();
					row["arr"]	 = songGroup->arrangement().join(", ");
					row["mapping"]	 = genArrMapping(songGroup->text(), songGroup->arrangement());
					
					resultList << row;
					
					md5sigList.append(QString::number(songGroup->groupId()));
					md5sigList.append(viewText);
					md5sigList.append(songGroup->text());
				}
				
				// We don't limit the number of results in the document results
				moreResults = false;
			}
		}
		
		QString currentMD5 = MD5::md5sum(md5sigList.join(""));
			
		bool isChanged = true;
		if(pollingFlag && 
		   currentMD5 == clientMD5)
			isChanged = false;
		
		if(isChanged)
		{
			result["num"]    = resultList.size();
			result["list"]   = QVariant(resultList);
			result["more"]   = moreResults;
			result["cutoff"] = listCutoffLimit;
		}
		else
			result["nochange"] = true;
			
		result["md5"] = currentMD5;
		
		
		QString jsonString = m_toJson.serialize(result);
		
		//qDebug() << "TabletServer::mainScreen(): list: result: "<<result;
		//qDebug() << "TabletServer::mainScreen(): list: json:   "<<jsonString;
		
// 		Http_Send_Ok(socket) << 
// 			"Content-Type: application/json\n\n" <<
// 			jsonString;

		QHttpResponseHeader header(QString("HTTP/1.0 200 OK"));
		header.setValue("Content-Type", "application/json");
		respond(socket, header);
		
		QTextStream output(socket);
		output.setAutoDetectUnicode(true);
		output << jsonString;
	}
	else
	if(control == "save_song")
	{
		QString mode   = query["mode"]; // Either "db" or "file"
		int itemid     = query["itemid"].toInt(); // If mode is db, itemid is a songid, otherwise, it's a group index
		
		QString title  = query["title"];
		QString text   = query["text"];
		QString arrTxt = query["arr"];
		
		SongRecord *song;
		
		if(mode == "file")
		{
			DocumentListModel * model = mw->documentListModel();
			SlideGroup * group = model->groupAt(itemid);
			SongSlideGroup *songGroup = dynamic_cast<SongSlideGroup*>(group);
			
			// Update the document model 
			songGroup->setGroupTitle(title);
			songGroup->setText(text); // automatically regenerates slides in document
			
			song = songGroup->song();
		}
		else
		{
			song = SongRecord::retrieve(itemid);
		}
		
		if(!song)
		{
			Http_Send_404(socket) 
				<< "Invalid songid";
		}
		
		song->setTitle(title);
		song->setText(text);
		
		SongArrangement *arr = song->defaultArrangement();
		arr->setArrangement(arrTxt.split(QRegExp("\\s*,\\s*")));
		
		// Changes automatically committed to database
		
		QVariantMap result;
		result["mapping"] = genArrMapping(song->text(), arr->arrangement());
		
		QString jsonString = m_toJson.serialize(result);
		
		QHttpResponseHeader header(QString("HTTP/1.0 200 OK"));
		header.setValue("Content-Type", "application/json");
		respond(socket, header);
		
		QTextStream output(socket);
		output.setAutoDetectUnicode(true);
		output << jsonString;
	}
	else
	if(control == "add_song")
	{
		int songid = query["songid"].toInt();
		
		QVariantMap result;
		
		SongRecord *song = SongRecord::retrieve(songid);
		if(!song)
		{
			Http_Send_404(socket) 
				<< "Invalid songid";
		}
		else
		{
			SlideGroup *group = mw->songBrowser()->createSlideGroup(song);
			
			group->setGroupNumber(mw->currentDocument()->numGroups());
			mw->currentDocument()->addGroup(group);
			
			result["groupnum"] = group->groupNumber();
		}
		
		QString jsonString = m_toJson.serialize(result);
		
		QHttpResponseHeader header(QString("HTTP/1.0 200 OK"));
		header.setValue("Content-Type", "application/json");
		respond(socket, header);
		
		QTextStream output(socket);
		output.setAutoDetectUnicode(true);
		output << jsonString;
	}
	else
	if(control == "del_song")
	{
		QString mode   = query["mode"]; // Either "db" or "file"
		
		int itemid = query["itemid"].toInt();
		
		if(mode == "file")
		{
			DocumentListModel * model = mw->documentListModel();
			SlideGroup * group = model->groupAt(itemid);
			SongSlideGroup *songGroup = dynamic_cast<SongSlideGroup*>(group);
			
			if(!songGroup)
			{
				Http_Send_404(socket) 
					<< "Invalid songid";
			}
			
			mw->currentDocument()->removeGroup(songGroup);
		}
		else
		{
			SongRecord *song = SongRecord::retrieve(itemid);
			if(!song)
			{
				Http_Send_404(socket) 
					<< "Invalid songid";
			}
			
			SongRecord::deleteSong(song);
		}
		
		QVariantMap result;
		
		// Nothing to see here, move along.
		
		QString jsonString = m_toJson.serialize(result);
		
		QHttpResponseHeader header(QString("HTTP/1.0 200 OK"));
		header.setValue("Content-Type", "application/json");
		respond(socket, header);
		
		QTextStream output(socket);
		output.setAutoDetectUnicode(true);
		output << jsonString;
	}
	else
	if(control == "show_slide")
	{
		int groupIdx = query["group"].toInt();
		
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
		Slide * liveSlide = mw->outputInst(liveId)->slide();
		
		//SlideGroupViewControl *viewControl = mw->viewControl(liveId);
		//Slide * liveSlide = viewControl->selectedSlide();
		
		int idx = query.value("slide").toInt();
		if(idx < 0 || idx > group->numSlides())
		{
			generic404(socket,path,query);
			return;
		}
		
		Slide *slide = group->at(idx);
		
		
		if(liveGroup != group ||
		   liveSlide != slide)
			mw->setLiveGroup(group,
				! slide ? liveSlide : slide); // prevent changing slides when loading the group page if group already live on different slide
		
		if(liveGroup == group &&
		   liveSlide != slide/* &&
		   nextPathElement != "icon"*/)
		{
			// this is JUST a change slide request
			mw->setLiveGroup(liveGroup,slide);
			
			// 204 = HTTP No Content, ("...[the browser] SHOULD NOT change its document view...")
			Http_Send_Response(socket,"HTTP/1.0 204 Changed Slide") << "";
			
			// BlackBerry Internet Browser doesnt honor HTTP 204, so we have to use javascript to make it not change the current document
			
			return;
		}
		else
			Http_Send_Response(socket,"HTTP/1.0 204 No Change") << "";
	}
	else
	if(control == "poll_live_slide")
	{
		DocumentListModel * docModel = mw->documentListModel();
		
		QVariantMap result;
		bool ok = true;
		
		int liveId = AppSettings::taggedOutput("live")->id();
		SlideGroup *liveGroup = mw->outputInst(liveId)->slideGroup();
		Slide *liveSlide = mw->outputInst(liveId)->slide();
		if(!liveGroup)
		{
			ok = false;
			result["error"] = "NLG";
		}
		
// 		SlideGroupViewControl *viewControl = mw->viewControl(liveId);
// 		Slide * liveSlide = viewControl->selectedSlide();
		
		if(!liveSlide && ok)
		{
			ok = false;
			result["error"] = "NLS";
		}
		
		if(ok)
		{
			int groupIdx = docModel->indexForGroup(liveGroup).row();
			int slideIdx = liveGroup->indexOf(liveSlide);

			result["groupIdx"] = groupIdx;
			result["slideIdx"] = slideIdx;
		}
		
		QString jsonString = m_toJson.serialize(result);
		
		QHttpResponseHeader header(QString("HTTP/1.0 200 OK"));
		header.setValue("Content-Type", "application/json");
		respond(socket, header);
		
		QTextStream output(socket);
		output.setAutoDetectUnicode(true);
		output << jsonString;
		
	}
	else
	{
		generic404(socket,path,query);
	}
}


QVariantMap TabletServer::genArrMapping(QString text, QStringList arragement)
{
	QVariantMap arrMap;
	
	QString cleanedText = text.replace("\r\n","\n");
	if(arragement.isEmpty())
		return arrMap;
	
	bool isEmpty = true;
	foreach(QString block, arragement)
		if(!block.isEmpty())
			isEmpty = false;
	
	if(isEmpty)
		return arrMap;
	
	//qDebug() << "SongSlideGroup::rearrange: Original text: "<<text;
	//QStringList defaultArr = findDefaultArragement(text);
	//qDebug() << "SongSlideGroup::rearrange: Original arrangement: "<<defaultArr;
	
	QRegExp blockTitleRegexp(tr("^\\s*((?:%1)(?:\\s+\\d+)?(?:\\s*\\(.*\\))?)\\s*.*$").arg(SongSlideGroup::songTagRegexpList()), Qt::CaseInsensitive);

	QStringList blockList = cleanedText.split("\n\n");
	
	QHash<QString,QString> blockHash;
	QHash<QString,int> blockIdxHash;
	
	int blockIdxCounter = 0;
	
	QString curBlockTitle;
	QStringList curBlockText;
	bool firstBlock = true;
	foreach(QString passage, blockList)
	{
		int pos = blockTitleRegexp.indexIn(passage);
		if(pos != -1)
		{
			// first, add block to hash if we have a block already
			if(!curBlockTitle.isEmpty())
			{
				blockHash[curBlockTitle] = curBlockText.join("\n\n");
				blockIdxHash[curBlockTitle] = blockIdxCounter;
				
				blockIdxCounter += curBlockText.size();
				
			}
			else
			// or this is the first block and no title given - assume "Title" block
			if(firstBlock && curBlockTitle.isEmpty() && !curBlockText.isEmpty())
			{
				curBlockTitle = "Title";
				blockHash[curBlockTitle] = curBlockText.join("\n\n");
				blockIdxHash[curBlockTitle] = blockIdxCounter;
				
				blockIdxCounter += curBlockText.size();
			}
			
			firstBlock = false;
			
			// start new block
			curBlockTitle = blockTitleRegexp.cap(1);
			curBlockText.clear();
		}
		
		QStringList filtered;
		QStringList lines = passage.split("\n");
		foreach(QString line, lines)
			if(!line.contains(blockTitleRegexp))
				filtered << line;
				
		curBlockText.append(filtered.join("\n"));
	}
	
	if(!curBlockTitle.isEmpty())
	{
		blockHash[curBlockTitle] = curBlockText.join("\n\n");
		blockIdxHash[curBlockTitle] = blockIdxCounter;
		blockIdxCounter += curBlockText.size();
	}
		
	//qDebug() << "SongSlideGroup::rearrange: Original blocks: "<<blockHash;
	//qDebug() << "SongSlideGroup::rearrange: Processing arragnement: "<<arragement;
	
	QStringList output;
	blockIdxCounter = 0;
	foreach(QString blockTitle, arragement)
	{
		if(blockHash.contains(blockTitle))
		{
			//qDebug() << "SongSlideGroup::rearrange: [process] Block: "<<blockTitle;
			QString blockText = blockHash.value(blockTitle);
			output << QString("%1\n%2")
				.arg(blockTitle)
				.arg(blockText);
				
			QStringList innerBlocks = blockText.split("\n\n");
				
			int idx = blockIdxHash[blockTitle];
			for(int i=idx; i<idx+innerBlocks.size(); i++)
			{
				QString idxKey = QString::number(i);
				if(!arrMap.contains(idxKey))
					arrMap[idxKey] = QVariantList();
					
				QVariantList usageList = arrMap[idxKey].toList();
				usageList << blockIdxCounter ++; 
				arrMap[idxKey] = usageList;
			}
		}
		else
		{
			qDebug() << "TabletServer::genarrMapping: [process] [Error] Arrangement block not found in original text: "<<blockTitle;
		}
	}
	
	QString outputText = output.join("\n\n");
	//qDebug() << "SongSlideGroup::rearrange: Output: "<<outputText;
	//return outputText;
	
	//qDebug() << "TabletServer::genarrMap: Converted arr "<<arragement<<" to: "<<arrMap;
	
	return arrMap;
}
