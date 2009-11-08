#include "SongRecord.h"
#include "SongRecordListModel.h"

bool SongRecord::m_dbIsOpen = false;
QSqlDatabase SongRecord::m_db;

void SongRecord::initSongDatabase()
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

SongRecord * SongRecord::retrieve(int id)
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
			return SongRecord::fromQuery(query);
		}
		else
		{
			return 0;
		}
	}
}

SongRecord * SongRecord::songByNumber(int id)
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
			return SongRecord::fromQuery(query);
		}
		else
		{
			return 0;
		}
	}
}

QList<SongRecord*> SongRecord::search(QString text, bool onlyTitle)
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
		return QList<SongRecord*>();
	}
	else
	{
		if(query.size())
		{
			QList<SongRecord*> list;
			while(query.isValid())
				list << SongRecord::fromQuery(query);
			return list;
			
		}
		else
		{
			return QList<SongRecord*>();
		}
	}
	
}

bool SongRecord::addSong(SongRecord* song)
{
	if(!song)
	{
		qDebug("addSong(): Cannot add a null song.");
		return false;
	}
	
	if(!m_dbIsOpen)
		initSongDatabase();
	
	QSqlQuery query;
	query.prepare(QString("INSERT INTO %1 ( title,  tags,  number,  text,  author,  copyright,  last_used) "
			              "VALUES (:title, :tags, :number, :text, :author, :copyright, :last_used)").arg(SONG_TABLE));
	
	//query.bindValue(":songid",	song->songId());
	query.bindValue(":title",	song->title());
	query.bindValue(":tags",	song->tags());
	query.bindValue(":number",	song->number());
	query.bindValue(":text",	song->text());
	query.bindValue(":author",	song->author());
	query.bindValue(":copyright",	song->copyright());
	query.bindValue(":last_used",	song->lastUsed());
	
	query.exec();
	
	if (query.lastError().isValid())
	{
		qDebug() << "Error adding Song: "<<query.lastError();
		return false;
	}
	else
	{
		QVariant var = query.lastInsertId();
		if(!var.isValid())
		{
			qDebug() << "SongRecord::addSong(): Song insert succeeded, but no lastInsertId() available.";
		}
		else
		{
			song->setSongId(var.toInt());
			qDebug() << "[DEBUG] SongRecord::addSong(): Inserted songid "<<song->songId();
		}
	}

	SongRecordListModel::instance()->addSong(song);

	return true;
}


QSqlDatabase SongRecord::db()
{
	if(!m_dbIsOpen)
		initSongDatabase();
	return m_db;
}

void SongRecord::deleteSong(SongRecord* song, bool deletePtr)
{
	if(!m_dbIsOpen)
		initSongDatabase();
		
	qDebug() << "SongRecord::deleteSong(): Deleting songid:"<<song->songId();
	
	QSqlQuery query;
	query.prepare(QString("DELETE FROM %1 WHERE songid=?").arg(SONG_TABLE)); 
	query.addBindValue(song->m_songId);
	query.exec();
	
	if (query.lastError().isValid())
	{
		qDebug() << "deleteSong():"<<query.lastError();
		return;
	}
	else
	{
		SongRecordListModel::instance()->removeSong(song);	
		if(deletePtr)
		{
			delete song;
			song = 0;
		}
	}
}

SongRecord * SongRecord::fromQuery(QSqlQuery q)
{
	return fromSqlRecord(q.record());
}

SongRecord * SongRecord::fromSqlRecord(QSqlRecord r)
{
	// TODO this is a memory leack unless the caller deletes this
	SongRecord * s = new SongRecord();
	//qDebug()<<"fromSqlRecord:"<<r<<", isEmpty? "<<r.isEmpty();
	//qDebug()<<"fromSqlRecord: title:"<<q.value(r.indexOf("title"));
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

QSqlRecord SongRecord::toSqlRecord()
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

SongRecord::SongRecord(QString title, QString text, int number) :
	m_songId(0)
	, m_title(title)
	, m_tags("")
	, m_number(number)
	, m_text(text)
	, m_init(false)

{
}

SONG_PROPSET(SongRecord, SongId,	int,		songId,		"songid");
SONG_PROPSET(SongRecord, Title,		QString,	title,		"title");
SONG_PROPSET(SongRecord, Tags,		QString,	tags,		"tags");
SONG_PROPSET(SongRecord, Number,	int,		number,		"number");
SONG_PROPSET(SongRecord, Text,		QString,	text,		"text");
SONG_PROPSET(SongRecord, Author,	QString,	author,		"author");
SONG_PROPSET(SongRecord, Copyright,	QString,	copyright,	"copyright");
SONG_PROPSET(SongRecord, LastUsed,	QString,	lastUsed,	"last_used");

QStringList SongRecord::tagList() const
{
	static QRegExp tagRegex("\\s*,\\s*");
	return tags().split(tagRegex);
}

void SongRecord::setTagList(const QStringList & list)
{
	setTags(list.join(", "));
}

bool SongRecord::updateDb(QString field, QVariant v)
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
