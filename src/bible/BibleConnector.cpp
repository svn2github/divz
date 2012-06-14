#include "BibleConnector.h"
#include "BibleModel.h"
#include "LocalBibleManager.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>
#include <QRegExp>


BibleConnector::BibleConnector(QObject *parent) 
	: QObject(parent)
	, m_net(0)
{
	m_net = new QNetworkAccessManager(this);
	connect(m_net, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
}

BibleConnector::~BibleConnector() {}

void BibleConnector::downloadReference(const BibleVerseRef& reference)
{
	BibleVerseList list;
	 
	QString urlString = urlForReference(reference);
	qDebug() << "BibleConnector::downloadReference("<<reference.cacheKey()<<"): "<<urlString;
	QUrl url(urlString);
	QNetworkReply * reply = m_net->get(QNetworkRequest(url));
	m_inProgress[reply] = reference;
}


void BibleConnector::downloadFinished(QNetworkReply *reply)
{
	BibleVerseRef reference = m_inProgress[reply];
	m_inProgress.remove(reply);
	
	QUrl url = reply->url();
	if (reply->error()) 
	{
		fprintf(stderr, "Download of %s failed: %s\n",
			url.toEncoded().constData(),
			qPrintable(reply->errorString()));
	} 
	else 
	{
		QByteArray ba = reply->readAll();
	
		//qDebug() << "Reply: "<<QString(ba);
		BibleVerseList list = parseHtmlReply(ba);
		
		cacheList(list);
		
		qDebug() << "BibleConnector::downloadFinished(): Downloaded: "<<list;
		
		if(!list.isEmpty())
		{
			BibleVerse last = list.last();
			if(reference.verseNumber() < 1)
				reference.setVerseNumber(1);
			reference.setVerseRange(last.verseNumber());
		}
	
		emit referenceAvailable(reference,list);
	}
	
	reply->deleteLater();
}

QString BibleConnector::html2text(const QString &tmp)
{
	QString text = tmp;
	QRegExp rx = QRegExp("<(script|style)[^>]*>(.|\n)*</(script|style)>",Qt::CaseInsensitive);
	rx.setMinimal(true);
	text.replace(rx,"");
	
	rx = QRegExp("<sup[^>]*>(.|\n)*</sup>",Qt::CaseInsensitive);
	rx.setMinimal(true);
	text.replace(rx,"");
	
	text.replace(QRegExp("<!--(.|\n)*?-->",Qt::CaseInsensitive),"");
	text.replace(QRegExp("(<br>|</(p|div|blockquote)>)",Qt::CaseInsensitive),"\n");
	text.replace(QRegExp("</li><li>",Qt::CaseInsensitive)," ");
	text.replace(QRegExp("<[^>]+>",Qt::CaseInsensitive),"");
	text.replace("&amp;","&");
	text.replace("&nbsp;"," ");
	text.replace("&quot;","\"");
	text.replace("&mdash;","--");
	text.replace("&rsquo;","'");
	text.replace("—"," - ");
	text.replace("”","\"");
	text.replace("“","\"");
	text.replace("’","'");
	
	
	return text;
}

	
bool BibleConnector::findReference(const BibleVerseRef &ref)
{
	if(isCached(ref))
		return true;

	BibleData *localBible = LocalBibleManager::inst()->bibleData(ref.book().version().code());
	// Return true because the entire Bible text for this version is already
	// present on disk, so we don't need to check further
	if(localBible)
		return true;
	
	
// 	BibleVerseList list = loadCached(ref);
// 	if(list.isEmpty())
// 		return false;
// 	
// 	m_cache[ref] = list;
// 	
// 	emit referenceAvailable(ref, list);
// 	
// 	return true;
	return false;
}

BibleVerseList BibleConnector::loadReference(const BibleVerseRef& ref)
{
	if(!findReference(ref))
		return BibleVerseList();
	
	BibleData *bibleData = LocalBibleManager::inst()->bibleData(ref.book().version().code());
	// Return true because the entire Bible text for this version is already
	// present on disk, so we don't need to check further
	if(bibleData && !isCached(ref))
	{
		//qDebug() << "BibleConnector::loadReference: Local Bible Version "<<ref.book().version().code()<<", reference not cached: "<<ref<<", range: "<<ref.verseRange();
		
		QString book = ref.book().name();
		int chap     = ref.chapter().chapterNumber();
			
		if(!bibleData->bookInfo.contains(book))
		{
			qDebug() << "BibleConnector::loadReference: [Book Info] book not found:"<<book;
			return BibleVerseList();
		}
		
		// Load the (hopefully) localised display name for book
		QString displayName = bibleData->bookInfo[book].displayName;
		BibleChapter bookChapter(BibleBook(displayName, ref.book().version()), chap);
		
		// If the string entered was the local (non english) name, reset 'book' to the english name for looking up the text
		book = bibleData->bookInfo[book].englishName;
		
		//qDebug() << "BibleConnector::loadReference: [Book Info] Loaded displayName "<<displayName<<" for book "<<book;
		
		// The list of verses to return
		BibleVerseList outputList;
		
		if(ref.verseNumber() > -1 && ref.verseRange() < 1)
		{
			int verse = ref.verseNumber();
			
			if(!bibleData->bibleText.contains(book))
			{
				qDebug() << "BibleConnector::loadReference: [Single Verse] book not found:"<<book;
				return BibleVerseList();
			}
			
			QHash<int, QHash<int, QString > > bookHash = bibleData->bibleText[book];
			if(!bookHash.contains(chap))
			{
				qDebug() << "BibleConnector::loadReference: [Single Verse] book "<<book<<" ok, chapter not found:"<<chap;
				return BibleVerseList();
			}
				
			QHash<int, QString > chapHash = bookHash[chap];
			if(!chapHash.contains(verse))
			{
				qDebug() << "BibleConnector::loadReference: [Single Verse] book "<<book<<" ok, chapter "<<chap<<" ok, verse not found:"<<verse;
				return BibleVerseList();
			}
			
			QString text = chapHash[verse];
			
			outputList << BibleVerse(bookChapter, verse, text);
			//qDebug() << "BibleConnector::loadReference: [Single Verse] Loaded local bible text for reference "<<ref<<": "<<text;
		}
		else
		{
			//qDebug() << "BibleConnector::loadReference: [Multi Verse:"<<ref<<"]: Looking for refs from "<<ref.verseNumber()<<" to "<<ref.verseRange()+1;
			if(!bibleData->bibleText.contains(book))
			{
				qDebug() << "BibleConnector::loadReference: [Multi Verse] book not found:"<<book;
				return BibleVerseList();
			}
			
			QHash<int, QHash<int, QString > > bookHash = bibleData->bibleText[book];
			if(!bookHash.contains(chap))
			{
				qDebug() << "BibleConnector::loadReference: [Multi Verse] book "<<book<<" ok, chapter not found:"<<chap;
				return BibleVerseList();
			}
			
			BibleVerseRef workingRef = ref;
			
			QHash<int, QString > chapHash = bookHash[chap];
			if(workingRef.verseNumber() < 0)
			{
				workingRef.setVerseNumber(1);
				workingRef.setVerseRange(chapHash.values().size());
				qDebug() << "BibleConnector::loadReference: [Multi Verse] book "<<book<<" ok, chapter "<<chap<<" ok, whole chapter: 1 -"<<ref.verseRange();
			}
			
			for(int i=workingRef.verseNumber(); i<workingRef.verseRange()+1; i++)
			{
				BibleVerseRef thisRef = workingRef.verseRef(i);
				
				int verse = thisRef.verseNumber();
					
				if(!chapHash.contains(verse))
				{
					qDebug() << "BibleConnector::loadReference: [Multi Verse] book "<<book<<" ok, chapter "<<chap<<" ok, verse not found:"<<verse;
					return BibleVerseList();
				}
				
				QString text = chapHash[verse];
				
				outputList << BibleVerse(bookChapter, thisRef.verseNumber(), text);
				//qDebug() << "BibleConnector::loadReference: [Multi Verse:"<<ref<<"] Loaded local bible text for reference "<<thisRef<<": "<<text;
			}
		}
		
		//qDebug() << "BibleConnector::loadReference: Local Bible Version "<<ref.book().version().code()<<", should be in cache now...";
		
		// NOTE we did not load these into the cache since it would just duplicate RAM used - the cache is designed primarily
		// to prevent extra/unecessary network access for Bibles located on the internet
		return outputList;
	}
	
	return loadCached(ref);
}

BibleVerseList BibleConnector::loadCached(const BibleVerseRef& ref)
{
// 	//TODO implement disk caching
// 	BibleVerseList list;
// 	BibleBook bJohn("John");
// 	BibleChapter bJohn3(bJohn, 3);
// 	list << BibleVerse(bJohn3, 16, "For God so loved the world, that He gave His only Son, that whosoever believes in Him should have everlasting life.");
//	return list;
		
	if(ref.verseRange() < 1)
	{
		BibleVerseList list;
		list << m_cache[ref.cacheKey()];
		return list;
	}
	else
	{
		BibleVerseList list;
		for(int i=ref.verseNumber(); i<ref.verseRange()+1; i++)
			list << m_cache.value(ref.verseRef(i).cacheKey());
			
		return list;
	}
}

void BibleConnector::cacheList(const BibleVerseList &list)
{
	//TODO implement disk caching
	
	if(list.isEmpty())
		return;
		
	foreach(BibleVerse verse, list)
		m_cache[verse.verseRef().cacheKey()] = verse;
}

bool BibleConnector::isCached(const BibleVerseRef& ref)
{
	if(ref.verseRange() < 1)
	{
		return m_cache.contains(ref.cacheKey());
	}
	else
	{
		for(int i=ref.verseNumber(); i<ref.verseRange()+1; i++)
			if(!m_cache.contains(ref.verseRef(i).cacheKey()))
				return false;
		return true;
	}
}
