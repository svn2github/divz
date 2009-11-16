#include "BibleGatewayConnector.h"
#include "BibleModel.h"

#include <QRegExp>
#include <QStringList>


#define URL_BASE "http://www.biblegateway.com/passage/?version="

BibleGatewayConnector::BibleGatewayConnector(QObject *parent) 
	: BibleConnector(parent)
{
}

QString BibleGatewayConnector::urlForReference(const QString& reference, const BibleVersion&)
{
	// include gen 2:7 just to generate the multipassage table
	return QString("%1NIV&search=Genesis 2:7; %2").arg(URL_BASE).arg(reference);
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
