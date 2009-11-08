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


BibleGatewayConnector::BibleGatewayConnector() : QObject()
{}

BibleVerseList BibleGatewayConnector::download(const QString& /*reference*/)
{
	BibleVerseList list;
	 
	BibleBook bJohn("John");
	BibleChapter bJohn3(bJohn, 3);
	list << BibleVerse(bJohn3, 16, "For God so loved the world, that He gave His only Son, that whosoever believes in Him should have everlasting life.");
		
	return list;
};
