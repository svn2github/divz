#include "BibleGatewayConnector.h"
#include "BibleModel.h"

#include <QRegExp>
#include <QStringList>


#define URL_BASE "http://www.biblegateway.com/passage/"

BibleGatewayConnector::BibleGatewayConnector(QObject *parent) 
	: BibleConnector(parent)
{
}

QString BibleGatewayConnector::urlForReference(const BibleVerseRef& reference)
{
	// Include gen 2:7 just to generate the multipassage table
	// Note: In book_downloader.pl, gen 2:7 was used to find the 'bad server' in biblegateway.com's round robin. 
	// However, that logic is not implemented here - but gen 2:7 (or any other additional passage) still serves 
	// the purpose of triggering the 'multipassage-box' output from their servers.
	BibleVersion ver = reference.book().version();
	return QString("%1?version=%2&search=Genesis 2:7; %3")
		.arg(URL_BASE)
		.arg(ver.code().isEmpty() ? "NIV" : ver.code())
		.arg(reference.toString());
}



BibleVerseList BibleGatewayConnector::parseHtmlReply(QByteArray &ba)
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
	
	QRegExp rxNorm("(\\w+)(?:\\s+(\\d+))?");
	pos = rxNorm.indexIn(normalizedReference);
	QString book    = rxNorm.cap(1);
	QString chapter = rxNorm.cap(2);
	
	if(chapter.isEmpty())
		chapter = "1";
	
	BibleChapter bookChapter(BibleBook(book), chapter.toInt());
	
	QRegExp rxVerseNum("^[^>]+>(\\d+)</sup>");
	rxVerseNum.setMinimal(true);
	QRegExp rxVerseSplit("class=\"versenum\"");
	rxVerseSplit.setMinimal(true);
	
	QRegExp rxSupEnd("<sup\\s*$");
	
	QStringList rawVerses = passage.split(rxVerseSplit);
	foreach(QString raw, rawVerses)
	{
		raw.replace(rxSupEnd,"");
		
		QString verseNumber;
		
		int pos = rxVerseNum.indexIn(raw);
		if(pos > -1)
			verseNumber = rxVerseNum.cap(1);
		
		raw = raw.replace(rxVerseNum, "");
		raw = html2text(raw);
		raw = raw.replace("‘","\'");
		raw = raw.replace("…","...");
		raw = raw.trimmed();
		
		if(raw.isEmpty() && 
		   verseNumber.isEmpty())
			continue;
		
		//qDebug() << "Parsed: "<<verseNumber<<": "<<raw;
		
		int intNum = verseNumber.toInt();
		if(intNum < 1)
			continue;
			
		list << BibleVerse(bookChapter, intNum, raw);
	}
	
	return list;

}
