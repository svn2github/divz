#ifndef BibleModel_H
#define BibleModel_H

#include <QObject>
#include <QList>
#include <QRegExp>
#include <QPair>
#include <QStringList>

#include "../PropertyUtils.h"

class BibleBook;
class BibleChapter;
class BibleVerse;
class BibleVerseRef;

class BibleVersion 
{
public:
	BibleVersion(const QString& name="", const QString& code="");
	
// 	BibleVersion& operator=(const BibleVersion &b)
// 	{
// 		BibleVersion a
// 		a.m_name = b.m_name;
// 		a.m_code = b.m_code;
// 		return a;
// 	}
// 	
	
	PROP_DEF_FULL(QString, name, Name);
	PROP_DEF_FULL(QString, code, Code);
};

typedef QList<BibleChapter> BibleChapterList;
class BibleBook 
{
public:
// 	BibleBook& operator=(const BibleBook &b)
// 	{
// 		BibleBook a;
// 		a.m_version = b.version;
// 		a.m_name = b.m_name;
// 		a.m_chapters = b.m_chapters;
// 		return a;
// 	}
// 	
	BibleBook(const QString& bookName="", const BibleVersion& v= BibleVersion());

	PROP_DEF_FULL(QString, name, Name);
	PROP_DEF_FULL(BibleVersion, version, Version);
	PROP_DEF_FULL(BibleChapterList, chapters, Chapters);
};

typedef QList<BibleVerse> BibleVerseList;
class BibleChapter 
{
	//Q_OBJECT
public:
	BibleChapter(const BibleBook& b= BibleBook(), int chapterNumber=1);
	
	PROP_DEF_FULL(BibleBook, book, Book);
	PROP_DEF_FULL(int, chapterNumber, ChapterNumber);
	PROP_DEF_FULL(BibleVerseList, verses, Verses);
};

class BibleVerse 
{
public:
	BibleVerse(const BibleBook &book=BibleBook(), const BibleChapter &chapter=BibleChapter(), int verseNumber=1, const QString &text="");
	BibleVerse(const BibleChapter &chapter, int verseNumber=1, const QString &text="");
	
	PROP_DEF_FULL(BibleBook, book, Book);
	PROP_DEF_FULL(BibleChapter, chapter, Chapter);
	PROP_DEF_FULL(int, verseNumber, VerseNumber);
	PROP_DEF_FULL(QString, text, Text);
	
	BibleVerseRef verseRef() const;
};

#ifndef QIntPair
	typedef QPair<int,int> QIntPair;
#endif

class BibleVerseRef
{
public:
	BibleVerseRef(const BibleBook &book=BibleBook(), const BibleChapter &chapter=BibleChapter(), int verseNumber=1, int range=-1);
	BibleVerseRef(const BibleChapter &chapter, int verseNumber=1, int range=-1);
	BibleVerseRef(bool valid) : m_valid(valid) {}
	
	PROP_DEF_FULL(BibleBook, book, Book);
	PROP_DEF_FULL(BibleChapter, chapter, Chapter);
	PROP_DEF_FULL(int, verseNumber, VerseNumber);
	PROP_DEF_FULL(int, verseRange, VerseRange);
	PROP_DEF_FULL(QString, text, Text);
	PROP_DEF_FULL(bool, valid, Valid);
	
	BibleVerseRef verseRef(int) const;
	
	QString toString(bool includeVersion=false) const;
	
	QString cacheKey() const;
	
	static BibleVerseRef normalize(const QString&, const BibleVersion &version = BibleVersion());
	
	typedef QPair<BibleVerseRef,QIntPair> TextTag;
	typedef QList<BibleVerseRef::TextTag> TextTagList;
	
	static TextTagList taggedVerseRefs(const QString&);
	
	static QStringList bookCompleterList() { initNameMap(); return bookNameList; }
		
private:
	static QRegExp normalizeRegExp;
	static QRegExp referenceExtractRegExp;
	
	static void initNameMap();
	static bool bookNameMap_initalized;
	static QHash<QString,QString> bookNameMap;
	static QStringList bookNameList;
};

QDebug operator<<(QDebug dbg, const BibleVerseList &list);
QDebug operator<<(QDebug dbg, const BibleVerseRef &ref);


#endif // BibleModel_H
