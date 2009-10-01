#include "Song.h"

#define SONG_FILE "songs.db"
#define SONG_TABLE "songs"

bool Song::m_dbIsOpen = false;
QSqlDatabase Song::m_db;

void Song::initSongDatabase()
{
	m_db = QSqlDatabase::addDatabase("QSQLITE");
	m_db.setDatabaseName(SONG_FILE);
	/*bool ok = */m_db.open();
	m_dbIsOpen = true;
}

Song * Song::retrieve(int id)
{
	if(!m_dbIsOpen)
		initSongDatabase();
	QSqlQuery query(QString("SELECT * FROM %1 WHERE songid=?").arg(SONG_TABLE),m_db); 
	query.addBindValue(id);
	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
		return 0;
	}
	else
	{
		QSqlQueryModel model;
		model.setQuery(query);
		return Song::fromSqlRecord(model.record(0));
	}
}

Song * Song::songByNumber(int id)
{
	if(!m_dbIsOpen)
		initSongDatabase();
	QSqlQuery query(QString("SELECT * FROM %1 WHERE number=?").arg(SONG_TABLE),m_db); 
	query.addBindValue(id);
	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
		return 0;
	}
	else
	{
		QSqlQueryModel model;
		model.setQuery(query);
		return Song::fromSqlRecord(model.record(0));
	}
}

QList<Song*> Song::search(QString text, bool onlyTitle)
{
	if(!m_dbIsOpen)
		initSongDatabase();
	QSqlQuery query(QString("SELECT * FROM %1 WHERE title LIKE ?").arg(SONG_TABLE) + (onlyTitle ? "" : " AND text LIKE ?"),m_db); 
	query.addBindValue(QString("%%1%").arg(text));
	if(!onlyTitle)
		query.addBindValue(QString("%%1%").arg(text));
	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
		return QList<Song*>();
	}
	else
	{
		QSqlQueryModel model;
		model.setQuery(query);
		QList<Song*> list;
		for(int i=0;i<model.rowCount();i++)
			list << Song::fromSqlRecord(model.record(i));
		return list;
	}
	
}

bool Song::addSong(Song* song)
{
	if(!song)
	{
		qDebug("addSong(): Cannot add a null song.");
		return false;
	}
	
	if(!m_dbIsOpen)
		initSongDatabase();
		
	QSqlTableModel tbl(0,m_db);
	tbl.setTable(SONG_TABLE);
	
	if(!tbl.insertRecord(-1,song->m_record))
	{
		qDebug() << "Error adding Song: "<<tbl.lastError();
		return false;
	}
}

QSqlDatabase Song::db()
{
	if(!m_dbIsOpen)
		initSongDatabase();
	return m_db;
}

void Song::deleteSong(Song* song, bool deletePtr)
{
	if(!m_dbIsOpen)
		initSongDatabase();
		
	QSqlQuery query(QString("DELETE FROM %1 WHERE songid=?").arg(SONG_TABLE),m_db); 
	query.addBindValue(song->m_songId);
	
	if (query.lastError().isValid())
	{
		qDebug() << query.lastError();
		return;
	}
	else
	{
		query.exec();
		if(deletePtr)
		{
			delete song;
			song = 0;
		}
	}
}

Song * Song::fromSqlRecord(QSqlRecord r)
{
	Song * s = new Song();
	s->setSongId(		r.value("songid").toInt());
	s->setTitle(		r.value("title").toString());
	s->setTags(		r.value("tags").toString());
	s->setNumber(		r.value("number").toInt());
	s->setText(		r.value("text").toString());
	s->setAuthor(		r.value("author").toString());
	s->setCopyright(	r.value("copyright").toString());
	s->setLastUsed(		r.value("last_used").toString());
	s->m_record = r;
	return s;
}

Song::Song(QString title, QString text, int number) :
	m_songId(0)
	, m_title(title)
	, m_tags("")
	, m_number(number)
	, m_text(text)

{
}

SONG_PROPSET(Song, SongId,		int,		songId,		"songid");
SONG_PROPSET(Song, Title,		QString,	title,		"title");
SONG_PROPSET(Song, Tags,		QString,	tags,		"tags");
SONG_PROPSET(Song, Number,		int,		number,		"number");
SONG_PROPSET(Song, Text,		QString,	text,		"text");
SONG_PROPSET(Song, Author,		QString,	author,		"author");
SONG_PROPSET(Song, Copyright,		QString,	copyright,	"copyright");
SONG_PROPSET(Song, LastUsed,		QString,	lastUsed,	"last_used");

QStringList Song::tagList() const
{
	static QRegExp tagRegex("\\s*,\\s*");
	return tags().split(tagRegex);
}

void Song::setTagList(const QStringList & list)
{
	setTags(list.join(", "));
}
