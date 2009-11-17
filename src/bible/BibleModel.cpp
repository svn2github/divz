#include "BibleModel.h"

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
	dbg.nospace() << "\n";
	foreach(BibleVerse v, list)
		dbg.nospace()
		<< qPrintable(QString("%1 %2:%3").arg(v.book().name()).arg(v.chapter().chapterNumber()).arg(v.verseNumber())) 
		<< " "
		<< v.text()
		<< "\n";
	
	return dbg.space();
}



QRegExp BibleVerseRef::referenceExtractRegExp("((?:Genesis|Gen|Ge|Gn|Exodus|Exo|Ex|Exod|Leviticus|Lev|Le|Lv|Numbers|Num|Nu|Nm|Nb|Deuteronomy|Deut|Dt|Joshua|Josh|Jos|Jsh|Judges|Judg|Jdg|Jg|Jdgs|Ruth|Rth|Ru|1 Samuel|1 Sam|1 Sa|1Samuel|1S|I Sa|1 Sm|1Sa|I Sam|1Sam|I Samuel|1st Samuel|First Samuel|2 Samuel|2 Sam|2 Sa|2S|II Sa|2 Sm|2Sa|II Sam|2Sam|II Samuel|2Samuel|2nd Samuel|Second Samuel|1 Kings|1 Kgs|1 Ki|1K|I Kgs|1Kgs|I Ki|1Ki|I Kings|1Kings|1st Kgs|1st Kings|First Kings|First Kgs|1Kin|2 Kings|2 Kgs|2 Ki|2K|II Kgs|2Kgs|II Ki|2Ki|II Kings|2Kings|2nd Kgs|2nd Kings|Second Kings|Second Kgs|2Kin|1 Chronicles|1 Chron|1 Ch|I Ch|1Ch|1 Chr|I Chr|1Chr|I Chron|1Chron|I Chronicles|1Chronicles|1st Chronicles|First Chronicles|2 Chronicles|2 Chron|2 Ch|II Ch|2Ch|II Chr|2Chr|II Chron|2Chron|II Chronicles|2Chronicles|2nd Chronicles|Second Chronicles|Ezra|Ezr|Nehemiah|Neh|Ne|Esther|Esth|Es|Job|Job|Jb|Psalm|Pslm|Ps|Psalms|Psa|Psm|Pss|Proverbs|Prov|Pr|Prv|Ecclesiastes|Eccles|Ec|Qoh|Qoheleth|Song of Solomon|Song|So|Canticle of Canticles|Canticles|Song of Songs|SOS|Isaiah|Isa|Is|Jeremiah|Jer|Je|Jr|Lamentations|Lam|La|Ezekiel|Ezek|Eze|Ezk|Daniel|Dan|Da|Dn|Hosea|Hos|Ho|Joel|Joe|Jl|Amos|Am|Obadiah|Obad|Ob|Jonah|Jnh|Jon|Micah|Mic|Nahum|Nah|Na|Habakkuk|Hab|Hab|Zephaniah|Zeph|Zep|Zp|Haggai|Hag|Hg|Zechariah|Zech|Zec|Zc|Malachi|Mal|Mal|Ml|Matthew|Matt|Mt|Mark|Mrk|Mk|Mr|Luke|Luk|Lk|John|Jn|Jhn|Acts|Ac|Romans|Rom|Ro|Rm|1 Corinthians|1 Cor|1 Co|I Co|1Co|I Cor|1Cor|I Corinthians|1Corinthians|1st Corinthians|First Corinthians|2 Corinthians|2 Cor|2 Co|II Co|2Co|II Cor|2Cor|II Corinthians|2Corinthians|2nd Corinthians|Second Corinthians|Galatians|Gal|Ga|Ephesians|Ephes|Eph|Philippians|Phil|Php|Colossians|Col|Col|1 Thessalonians|1 Thess|1 Th|I Th|1Th|I Thes|1Thes|I Thess|1Thess|I Thessalonians|1Thessalonians|1st Thessalonians|First Thessalonians|2 Thessalonians|2 Thess|2 Th|II Th|2Th|II Thes|2Thes|II Thess|2Thess|II Thessalonians|2Thessalonians|2nd Thessalonians|Second Thessalonians|1 Timothy|1 Tim|1 Ti|I Ti|1Ti|I Tim|1Tim|I Timothy|1Timothy|1st Timothy|First Timothy|2 Timothy|2 Tim|2 Ti|II Ti|2Ti|II Tim|2Tim|II Timothy|2Timothy|2nd Timothy|Second Timothy|Titus|Tit|Philemon|Philem|Phm|Hebrews|Heb|James|Jas|Jm|1 Peter|1 Pet|1 Pe|I Pe|1Pe|I Pet|1Pet|I Pt|1 Pt|1Pt|I Peter|1Peter|1st Peter|First Peter|2 Peter|2 Pet|2 Pe|II Pe|2Pe|II Pet|2Pet|II Pt|2 Pt|2Pt|II Peter|2Peter|2nd Peter|Second Peter|1 John|1 Jn|I Jn|1Jn|I Jo|1Jo|I Joh|1Joh|I Jhn|1 Jhn|1Jhn|I John|1John|1st John|First John|2 John|2 Jn|II Jn|2Jn|II Jo|2Jo|II Joh|2Joh|II Jhn|2 Jhn|2Jhn|II John|2John|2nd John|Second John|3 John|3 Jn|III Jn|3Jn|III Jo|3Jo|III Joh|3Joh|III Jhn|3 Jhn|3Jhn|III John|3John|3rd John|Third John|Jude|Jud|Revelation|Rev|Re|The Revelation)\\s+ (?:[0-9]+)(?:[:\\.](?:[0-9]*))?(?:\\s*-\\s*(?:[0-9]*))?)"); 
// cap 1 = reference, if matched

QRegExp BibleVerseRef::normalizeRegExp("(\\w+)\\s+([0-9]+)(?:[:\\.]([0-9]+))?(?:\\s*-\\s*([0-9]+))?"); // cap 1 = book, 2 = chapter, 3 = verse, 4 = range end

// Use lookup table to normalize book name



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

QString BibleVerseRef::toString()
{
	return QString("%1 %2%3%4")
		.arg(m_book.name())
		.arg(m_chapter.chapterNumber())
		.arg(m_verseNumber>0? QString(":%1").arg(m_verseNumber) : QString(""))
		.arg(m_verseRange>0 && m_verseRange!=m_verseNumber ? QString("-%1").arg(m_verseRange) : QString(""));
}

BibleVerseRef BibleVerseRef::normalize(const QString& tmp)
{
// 	QString raw = tmp;
// 	QString ref;
// 	
// 	qDebug() << "normalize("<<tmp<<"): step 0: raw:"<<raw;
// 	
// 	int pos = referenceExtractRegExp.indexIn(raw);
// 	if(pos > -1)
// 		ref = referenceExtractRegExp.cap(1);
// 	else
// 	{
// 		qDebug() << "normalize("<<tmp<<"): step 0: exit - no match for referenceExtractRegExp";
// 		return BibleVerseRef(false);
// 	}
	int pos;
	QString ref = tmp;
	
	qDebug() << "normalize("<<tmp<<"): step 1: ref:"<<ref;
	
	pos = normalizeRegExp.indexIn(ref);
	
	QString book    = normalizeRegExp.cap(1);
	QString chapter = normalizeRegExp.cap(2);
	QString verse   = normalizeRegExp.cap(3);
	QString range   = normalizeRegExp.cap(4);
	
	qDebug() << "normalize("<<tmp<<"): step 2: norm:"<<book<<chapter<<verse<<range;
	
	BibleVerseRef final = BibleVerseRef(BibleChapter(BibleBook(book), chapter.toInt()), verse.toInt(), range.toInt());
	qDebug() << "normalize("<<tmp<<"): step 3: final string: "<<final .toString();
	
	return final;
	
}


