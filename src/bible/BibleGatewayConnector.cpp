#include "BibleGatewayConnector.h"


BibleVersion::BibleVersion(const QString& name, const QString& code) :
	  m_name(name)
	, m_code(code)
{}
	
BibleBook::BibleBook(const QString& bookName, const BibleVersion& v) :
	  m_name(bookName)
	, m_version(v)
{}

BibleChapter::BibleChapter(const BibleBook& b, int chapterNumber) :
	  m_book(b)
	, m_chapterNumber(chapterNumber)
{}

BibleVerse::BibleVerse(const BibleBook &book, const BibleChapter &chapter, int verseNumber, const QString &text) :
	  m_book(book)
	, m_chapter(chapter)
	, m_verseNumber(verseNumber)
	, m_text(text)
{}

BibleVerse::BibleVerse(const BibleChapter &chapter, int verseNumber, const QString &text) :
	  m_book(chapter.book())
	, m_chapter(chapter)
	, m_verseNumber(verseNumber)
	, m_text(text)
{}

QDebug operator<<(QDebug dbg, const BibleVerseList &list)
{
	foreach(BibleVerse v, list)
		dbg.nospace() << QString("%1 %2:%3 \"%4\"").arg(v.book().name()).arg(v.chapter().chapterNumber()).arg(v.verseNumber()).arg(v.text());
	
	return dbg.space();
}


#define URL_BASE "http://www.biblegateway.com/passage/?version="
#include <QRegExp>

BibleGatewayConnector::BibleGatewayConnector() 
	: QObject()
	, m_net(0)
{
	m_net = new QNetworkAccessManager(this);
 	connect(m_net, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));

}

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>

void BibleGatewayConnector::downloadReference(const QString& reference)
{
	BibleVerseList list;
	 
	// include gen 2:7 just to generate the multipassage table
	QString urlString = QString("%1NIV&search=Genesis 2:7; %2").arg(URL_BASE).arg(reference);
	
	QUrl url(urlString);
	QNetworkReply * reply = m_net->get(QNetworkRequest(url));
	m_inProgress[reply] = reference;
}


void BibleGatewayConnector::downloadFinished(QNetworkReply *reply)
{
	QString reference = m_inProgress[reply];
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
		BibleVerseList list = parseBibleGatewayReply(ba);
		m_cache[reference] = list;
		
		qDebug() << "Downloaded: "<<list;
		
		emit referenceAvailable(reference,list);
	}
	
	reply->deleteLater();
}

BibleVerseList BibleGatewayConnector::parseBibleGatewayReply(QByteArray &ba)
{
	// Much of this parsing code and regexps are derived from book_downloader.pl in this folder.

	QString str(ba);
	
	QRegExp rxPassages("<td class=\"multipassage-box\" width=\"100%\">(.*)</td>", Qt::CaseInsensitive);
	QRegExp rxStrong("<strong>(.*)</strong>(.*)</p>");
	rxStrong.setMinimal(true);
	rxPassages.setMinimal(true);
	
 	// loop style from: http://doc.trolltech.com/4.5/qregexp.html#cap-in-a-loop
 	QStringList passagesText;

	// only will hold the last normalizedReference
	QString normalizedReference; 
	
	int pos = 0;
	while ((pos = rxPassages.indexIn(str, pos)) != -1) 
	{
		QString passage = rxPassages.cap(1);
		int pos2 = rxStrong.indexIn(passage);
		if(pos2 > -1)
			normalizedReference = rxStrong.cap(1);
		passage.replace(rxStrong, "");
		passagesText << passage;
		
		pos += rxPassages.matchedLength();
	}

	if(passagesText.isEmpty())
	{
		qDebug() << "parseBibleGatewayReply(): No passages found, returning empty BibleVerseList";
		return BibleVerseList();
	}
	
	QString passage = passagesText.last();
	int idxDiv = passage.toLower().indexOf("</div>");
	passage = passage.left(idxDiv);
	
	BibleVerseList list;
	
	//qDebug() << "normalizedReference:"<<normalizedReference;
	
	QRegExp rxNorm("(\\w+)\\s+(\\d+):");
	pos = rxNorm.indexIn(normalizedReference);
	QString book    = rxNorm.cap(1);
	QString chapter = rxNorm.cap(2);
	
	BibleChapter bookChapter(BibleBook(book), chapter.toInt());
	
	QRegExp rxVerseNum("^[^>]+>(\\d+)</sup>");
	QRegExp rxVerseSplit("<sup.*class=\"versenum\"");
	rxVerseSplit.setMinimal(true);
	
	QStringList rawVerses = passage.split(rxVerseSplit);
	foreach(QString raw, rawVerses)
	{
		QString verseNumber;
		int pos = rxVerseNum.indexIn(raw);
		if(pos > -1)
			verseNumber = rxVerseNum.cap(1);
		raw = raw.replace(rxVerseNum, "");
		
		raw = BibleGatewayConnector::html2text(raw);
		
		raw = raw.trimmed();
		
		if(raw.isEmpty() && verseNumber.isEmpty())
			continue;
		
		//qDebug() << "Parsed: "<<verseNumber<<": "<<raw;
		
		list << BibleVerse(bookChapter, verseNumber.toInt(), raw);
	}
	
	return list;

}

QString BibleGatewayConnector::html2text(const QString &tmp)
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
	
	return text;
}

	
bool BibleGatewayConnector::findReference(const QString &ref)
{
	if(m_cache.contains(ref))
		return true;
	
	BibleVerseList list = loadCached(ref);
	if(list.isEmpty())
		return false;
	
	m_cache[ref] = list;
	
	emit referenceAvailable(ref, list);
	
	return true;
}

BibleVerseList BibleGatewayConnector::loadReference(const QString& ref)
{
	if(!findReference(ref))
		return BibleVerseList();
		
	return m_cache[ref];
}
	 

BibleVerseList BibleGatewayConnector::loadCached(const QString& /*ref*/)
{
	BibleVerseList list;
	BibleBook bJohn("John");
	BibleChapter bJohn3(bJohn, 3);
	list << BibleVerse(bJohn3, 16, "For God so loved the world, that He gave His only Son, that whosoever believes in Him should have everlasting life.");
		
	return list;
}

void BibleGatewayConnector::cacheList(const QString& /*reference*/, const BibleVerseList &/*list*/)
{

}
