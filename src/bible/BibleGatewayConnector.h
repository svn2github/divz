#ifndef BibleGatewayConnector_H
#define BibleGatewayConnector_H

#include <QObject>
#include <QList>

#include "../PropertyUtils.h"

class BibleBook;
class BibleChapter;
class BibleVerse;

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
};

QDebug operator<<(QDebug dbg, const BibleVerseList &list);



class BibleGatewayConnector : public QObject
{
	Q_OBJECT
public:
	BibleGatewayConnector();
	
	BibleVerseList download(const QString& reference);
	
};

#endif
