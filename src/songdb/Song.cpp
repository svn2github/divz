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
// 	qDebug()<<"Ok?"<<ok;
	m_dbIsOpen = true;
	
// 	QSqlQuery query;
// 	//query.exec("SELECT title, number FROM songs where songid=16");
// 	query.prepare("SELECT * FROM songs where number=?");  
// 	query.addBindValue(203);
// 	query.exec();
// 	if (query.lastError().isValid())
// 	{
// 		qDebug() << "initSongDatabase(): query.lastError();
// 	}
// 	else
// 	{
// 		while (query.next()) 
// 		{
// 			QString title = query.value(0).toString();
// 			int number = query.value(1).toInt();
// 			if(number == 0)
// 				continue;
// 			//check value(1).isNull() before using
// 			qDebug() << title << number;
// 		}
// 	}
}

Song * Song::retrieve(int id)
{
	if(!m_dbIsOpen)
		initSongDatabase();
	QSqlQuery query;
	query.prepare(QString("SELECT * FROM %1 WHERE songid=?").arg(SONG_TABLE)); 
	query.addBindValue(id);
	query.exec();
	if (query.lastError().isValid())
	{
		qDebug() << "retrieve(): "<<query.lastError();
		return 0;
	}
	else
	{
		if(query.size())
		{
			query.next();
			return Song::fromQuery(query);
		}
		else
		{
			return 0;
		}
	}
}

Song * Song::songByNumber(int id)
{
	if(!m_dbIsOpen)
		initSongDatabase();
	QSqlQuery query("",m_db);
	QString sql = QString("SELECT * FROM %1 WHERE number=?").arg(SONG_TABLE);
	query.prepare(sql); 
	query.addBindValue(id);
	query.exec();
	
	if (query.lastError().isValid())
	{
		qDebug() << "songByNumber():"<<query.lastError();
		return 0;
	}
	else
	{
		if(query.size())
		{
			query.next();
			return Song::fromQuery(query);
		}
		else
		{
			return 0;
		}
	}
}

QList<Song*> Song::search(QString text, bool onlyTitle)
{
	if(!m_dbIsOpen)
		initSongDatabase();
	QSqlQuery query;
	query.prepare(QString("SELECT * FROM %1 WHERE title LIKE :x").arg(SONG_TABLE) + (onlyTitle ? "" : " AND text LIKE :y")); 
	query.bindValue(":x",QString("%%1%").arg(text));
	query.exec();
	if(!onlyTitle)
		query.bindValue(":y",QString("%%1%").arg(text));
	if (query.lastError().isValid())
	{
		qDebug() << "search():"<<query.lastError();
		return QList<Song*>();
	}
	else
	{
		if(query.size())
		{
			QList<Song*> list;
			while(query.isValid())
				list << Song::fromQuery(query);
			return list;
			
		}
		else
		{
			return QList<Song*>();
		}
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
	
	if(!tbl.insertRecord(-1,song->toSqlRecord()))
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
		
	QSqlQuery query;
	query.prepare(QString("DELETE FROM %1 WHERE songid=:x").arg(SONG_TABLE)); 
	query.bindValue(":x",song->m_songId);
	query.exec();
	
	if (query.lastError().isValid())
	{
		qDebug() << "deleteSong():"<<query.lastError();
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

Song * Song::fromQuery(QSqlQuery q)
{
	return fromSqlRecord(q.record());
}

Song * Song::fromSqlRecord(QSqlRecord r)
{
	
	Song * s = new Song();
	//qDebug()<<"fromQuery:"<<r<<", isEmpty? "<<r.isEmpty();
	//qDebug()<<"fromQuery: title:"<<q.value(r.indexOf("title"));
	s->m_init = true;
	s->setSongId(		r.value("songid").toInt());
	s->setTitle(		r.value("title").toString());
	s->setTags(		r.value("tags").toString());
	s->setNumber(		r.value("number").toInt());
	s->setText(		r.value("text").toString());
	s->setAuthor(		r.value("author").toString());
	s->setCopyright(	r.value("copyright").toString());
	s->setLastUsed(		r.value("last_used").toString());
	s->m_init = false;
	return s;
}

QSqlRecord Song::toSqlRecord()
{
	QSqlRecord r;
	r.setValue("songid",	songId());
	r.setValue("title",	title());
	r.setValue("tags",	tags());
	r.setValue("number",	number());
	r.setValue("text",	text());
	r.setValue("author",	author());
	r.setValue("copyright",	copyright());
	r.setValue("last_used",	lastUsed());
	return r;
}

Song::Song(QString title, QString text, int number) :
	m_songId(0)
	, m_title(title)
	, m_tags("")
	, m_number(number)
	, m_text(text)
	, m_init(false)

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

bool Song::updateDb(QString field, QVariant v)
{
	if(m_init)
		return true;
		
	if(!m_dbIsOpen)
		initSongDatabase();
	
	QSqlQuery query;
	QString sql = QString("UPDATE %1 SET %2 = ? WHERE songid= ?").arg(SONG_TABLE).arg(field);
	//qDebug() << "updateDb():"<<sql<<", value:"<<v<<", songid:"<<m_songId;
	query.prepare(sql); 
	query.addBindValue(v);
	query.addBindValue(m_songId);
	query.exec();
	
	if (query.lastError().isValid())
	{
		qDebug() << "updateDb():"<<query.lastError();
		return false;
	}
	else
	{
		return true;
	}
}
