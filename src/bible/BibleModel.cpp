#include "BibleModel.h"

BibleVersion::BibleVersion(const QString& name, const QString& code) :
	  m_name(name.isEmpty() ? "New International Version" : name)
	, m_code(code.isEmpty() ? "NIV" : code)
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
	dbg.nospace() << "\n";
	foreach(BibleVerse v, list)
		dbg.nospace()
		<< qPrintable(QString("%1 %2:%3").arg(v.book().name()).arg(v.chapter().chapterNumber()).arg(v.verseNumber())) 
		<< " "
		<< v.text()
		<< "\n";
	
	return dbg.space();
}


BibleVerseRef BibleVerse::verseRef() const
{
	// return a ref for a single verse (hence the -1 for the range)
	return BibleVerseRef(m_book, m_chapter, m_verseNumber, -1);
}


// cap 1 = book, 2 = chapter, 3 = verse, 4 = range end
QRegExp BibleVerseRef::normalizeRegExp("((?:[1-9]\\s+)?\\w+)\\s+([0-9]+)(?:[:\\.]([0-9]+))?(?:\\s*-\\s*([0-9]+))?"); 

BibleVerseRef::BibleVerseRef(const BibleBook &book, const BibleChapter &chapter, int verseNumber, int range)
	: m_book(book)
	, m_chapter(chapter)
	, m_verseNumber(verseNumber)
	, m_verseRange(range)
	, m_valid(true)
{}

BibleVerseRef::BibleVerseRef(const BibleChapter &chapter, int verseNumber, int range)
	: m_book(chapter.book())
	, m_chapter(chapter)
	, m_verseNumber(verseNumber)
	, m_verseRange(range)
	, m_valid(true)
{}

QString BibleVerseRef::toString() const
{
	return QString("%1 %2%3%4")
		.arg(m_book.name())
		.arg(m_chapter.chapterNumber())
		.arg(m_verseNumber>0? QString(":%1").arg(m_verseNumber) : QString(""))
		.arg(m_verseRange>0 && m_verseRange!=m_verseNumber ? QString("-%1").arg(m_verseRange) : QString(""));
}

QString BibleVerseRef::cacheKey() const
{
	return QString("%1 (%2)")
		.arg(toString())
		.arg(m_book.version().code());
}

BibleVerseRef BibleVerseRef::verseRef(int newVerseNumber) const
{
	// return a ref for a single verse (hence the -1 for the range)
	return BibleVerseRef(m_book, m_chapter, newVerseNumber, -1);
}


#define DEBUG_NORMALIZE 0
BibleVerseRef BibleVerseRef::normalize(const QString& tmp, const BibleVersion &version)
{
	if(!bookNameMap_initalized)
		initNameMap();
		
	QString raw = tmp;
	QString ref;
	
	if(DEBUG_NORMALIZE)
		qDebug() << "normalize("<<tmp<<"): step 0: raw:"<<raw;
	
	int pos = referenceExtractRegExp.indexIn(raw);
	if(pos > -1)
		ref = referenceExtractRegExp.cap(1);
	else
	{
		if(DEBUG_NORMALIZE)
			qDebug() << "normalize("<<tmp<<"): step 0: exit - no match for referenceExtractRegExp";
		return BibleVerseRef(false);
	}
	
	if(DEBUG_NORMALIZE)
		qDebug() << "normalize("<<tmp<<"): step 1: ref:"<<ref;
	
	pos = normalizeRegExp.indexIn(ref);
	
	QString book    = normalizeRegExp.cap(1);
	QString chapter = normalizeRegExp.cap(2);
	QString verse   = normalizeRegExp.cap(3);
	QString range   = normalizeRegExp.cap(4);
	
	QString bookTmp = book.toLower().trimmed();
	if(bookNameMap.contains(bookTmp))
		book = bookNameMap.value(bookTmp);
		
	if(book.isEmpty() || chapter.isEmpty() || verse.isEmpty())
		return BibleVerseRef(false);
	
	if(DEBUG_NORMALIZE)
		qDebug() << "normalize("<<tmp<<"): step 2: norm:"<<book<<chapter<<verse<<range;
	
	BibleVerseRef final = BibleVerseRef(BibleChapter(BibleBook(book,version), chapter.toInt()), verse.toInt(), range.toInt());
	
	if(DEBUG_NORMALIZE)
		qDebug() << "normalize("<<tmp<<"): step 3: final string: "<<final .toString();
	
	return final;
	
}


QDebug operator<<(QDebug dbg, const BibleVerseRef &ref)
{
	dbg.nospace() << qPrintable(ref.cacheKey());
	
	return dbg.nospace();
}

