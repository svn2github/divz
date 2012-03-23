#include "SongRecord.h"
#include "SongRecordListModel.h"

#include "../model/SlideGroup.h"

bool SongDatabase::m_dbIsOpen = false;
QSqlDatabase SongDatabase::m_db;


QHash<int,SongRecord*> SongRecord::m_recordHash;
QHash<int,SongArrangement*> SongArrangement::m_recordHash;

QSqlDatabase SongDatabase::db()
{
	if(!m_dbIsOpen)
		initSongDatabase();
	return m_db;
}

void SongDatabase::initSongDatabase()
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

SongArrangement * SongArrangement::retrieve(int id)
{
	if(m_recordHash.contains(id))
		return m_recordHash.value(id);
		
	SongDatabase::db(); // hit to make sure db is open
	
	QSqlQuery query;
	query.prepare(QString("SELECT * FROM %1 WHERE arrid=?").arg(SONG_ARR_TABLE)); 
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
			SongArrangement *arr = SongArrangement::fromQuery(query);
			m_recordHash.insert(id, arr);
			return arr;
		}
		else
		{
			return 0;
		}
	}
}

QList<SongArrangement*> SongArrangement::searchBySongId(int id)
{
// 	if(!m_dbIsOpen)
// 		initSongDatabase();
	SongDatabase::db(); // hit to make sure db is open
	
	QSqlQuery query;
	query.prepare(QString("SELECT arrid FROM %1 WHERE songid = :x").arg(SONG_TABLE)); 
	query.bindValue(":x",id);
	query.exec();
		
	if (query.lastError().isValid())
	{
		qDebug() << "SongRecord::searchBySongId():"<<query.lastError();
		return QList<SongArrangement*>();
	}
	else
	{
		if(query.size())
		{
			QList<SongArrangement*> list;
			while(query.isValid())
			{
				int id = query.record().value("arrid").toInt();
				// Call retrieve() instead of fromQuery()
				// to make use of cached objects
				list << SongArrangement::retrieve(id);
			}
			return list;
			
		}
		else
		{
			return QList<SongArrangement*>();
		}
	}
	
}

bool SongArrangement::addRecord(SongArrangement* arr)
{
	if(!arr)
	{
		qDebug("SongArrangement::addRecord(): Cannot add a null record.");
		return false;
	}
	
	SongDatabase::db(); // hit to make sure db is open
	
	QSqlQuery query;
	//CREATE TABLE song_arrangement (arrid integer primary key autoincrement, songid integer, arrangement text, template blob, title varchar(255));
	query.prepare(QString("INSERT INTO %1 ( songid,  title,  arrangement,  template) "
			              "VALUES (:songid, :title, :arrangement, :template)").arg(SONG_ARR_TABLE));
	
	query.bindValue(":songid",	arr->songId());
	query.bindValue(":title",	arr->title());
	query.bindValue(":arrangement",	arr->arr());
	query.bindValue(":template",	arr->tmpl());
	
	query.exec();
	
	if (query.lastError().isValid())
	{
		qDebug() << "Error adding arrangement: "<<query.lastError();
		return false;
	}
	else
	{
		QVariant var = query.lastInsertId();
		if(!var.isValid())
		{
			qDebug() << "SongArrangement::addRecord(): Insert succeeded, but no lastInsertId() available.";
		}
		else
		{
			arr->setArrId(var.toInt());
			qDebug() << "[DEBUG] SongArrangement::addRecord(): Inserted arrid "<<arr->arrId();
		}
	}

	// TODO do we need a song arragement list model...?
	//SongArrangementListModel::instance()->addSong(song);

	return true;
}

void SongArrangement::deleteRecord(SongArrangement* arr, bool deletePtr)
{
	SongDatabase::db(); // hit to make sure db is open
		
	qDebug() << "SongArrangement::deleteRecord(): Deleting arrid:"<<arr->arrId();
	
	QSqlQuery query;
	query.prepare(QString("DELETE FROM %1 WHERE arrid=?").arg(SONG_ARR_TABLE)); 
	query.addBindValue(arr->arrId());
	query.exec();
	
	if (query.lastError().isValid())
	{
		qDebug() << "SongArrangement::deleteRecord():"<<query.lastError();
		return;
	}
	else
	{
		//SongRecordListModel::instance()->removeSong(song);	
		if(deletePtr)
		{
			delete arr;
			arr = 0;
		}
	}
}

SongArrangement * SongArrangement::fromQuery(QSqlQuery q)
{
	return fromSqlRecord(q.record());
}

SongArrangement * SongArrangement::fromSqlRecord(QSqlRecord r)
{
	// TODO this is a memory leack unless the caller deletes this
	SongArrangement * s = new SongArrangement();
	//qDebug()<<"fromSqlRecord:"<<r<<", isEmpty? "<<r.isEmpty();
	//qDebug()<<"fromSqlRecord: title:"<<q.value(r.indexOf("title"));
	s->setInInit(true);
	s->setArrId(		r.value("arrid").toInt());
	s->setSongId(		r.value("songid").toInt());
	s->setTitle(		r.value("title").toString());
	s->setArr(		r.value("arrangement").toString());
	s->setTmpl(		r.value("template").toByteArray());
	s->setInInit(false);
	return s;
}

void SongArrangement::setInInit(bool flag)
{
	m_init = flag;
}


QSqlRecord SongArrangement::toSqlRecord()
{
	QSqlRecord r;
	r.setValue("arrid",	arrId());
	r.setValue("songid",	songId());
	r.setValue("title",	title());
	r.setValue("arrangement", arr());
	r.setValue("template",	tmpl());
	return r;
}

SongArrangement::SongArrangement(SongRecord *song, QString title, QString arr)
	: m_arrId(0)
	, m_songId(song ? song->songId() : 0)
	, m_title(title)
	, m_arr(arr)
	, m_tmplGroup(0)
	, m_init(false)
{
}

SongArrangement::SongArrangement(SongRecord *song, QString title, QStringList arr)
	: m_arrId(0)
	, m_songId(song ? song->songId() : 0)
	, m_title(title)
	, m_arr(arr.join(", "))
	, m_tmplGroup(0)
	, m_init(false)
{
}


SONG_PROPSET(SongArrangement, ArrId,	int,		arrId,		"arrid");
SONG_PROPSET(SongArrangement, SongId,	int,		songId,		"songid");
SONG_PROPSET(SongArrangement, Title,	QString,	title,		"title");
SONG_PROPSET(SongArrangement, Arr,	QString,	arr,		"arrangement");
SONG_PROPSET(SongArrangement, Tmpl,	QByteArray,	tmpl,		"template");

SongRecord *SongArrangement::song() const
{
	return SongRecord::retrieve(songId());
}

void SongArrangement::setSong(SongRecord *rec)
{
	setSongId(rec ? rec->songId() : 0);
}

QStringList SongArrangement::arrangement() const
{
	static QRegExp tagRegex("\\s*,\\s*");
	return arr().split(tagRegex);
}

void SongArrangement::setArrangement(const QStringList & list)
{
	setArr(list.join(", "));
}

SlideGroup *SongArrangement::templateGroup()
{
	if(!m_tmplGroup)
		m_tmplGroup = SlideGroup::fromByteArray(m_tmpl);
	
	return m_tmplGroup;
}

void SongArrangement::setTemplateGroup(SlideGroup *tmpl)
{
	m_tmplGroup = tmpl;
	setTmpl(tmpl ? tmpl->toByteArray() : QByteArray());
}

bool SongArrangement::updateDb(QString field, QVariant v)
{
	if(m_init)
		return true;
		
	SongDatabase::db(); // hit to make sure db is open
	
	QSqlQuery query;
	QString sql = QString("UPDATE %1 SET %2 = ? WHERE arrid = ?").arg(SONG_ARR_TABLE).arg(field);
	qDebug() << "SongArrangement::updateDb():"<<sql<<", value:"<<v<<", arrid:"<<arrId();
	query.prepare(sql); 
	query.addBindValue(v);
	query.addBindValue(arrId());
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





SongRecord * SongRecord::retrieve(int id)
{
	if(m_recordHash.contains(id))
		return m_recordHash.value(id);
		
// 	if(!m_dbIsOpen)
// 		initSongDatabase();
	SongDatabase::db(); // hit to make sure db is open
	
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
			SongRecord *rec = SongRecord::fromQuery(query);
			m_recordHash.insert(id, rec);
			return rec;
		}
		else
		{
			return 0;
		}
	}
}

SongRecord * SongRecord::songByNumber(int id)
{
// 	if(!m_dbIsOpen)
// 		initSongDatabase();
	
	QSqlQuery query("",SongDatabase::db());
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
// 	if(!m_dbIsOpen)
// 		initSongDatabase();
	SongDatabase::db(); // hit to make sure db is open
	
	QSqlQuery query;
	query.prepare(QString("SELECT songid FROM %1 WHERE title LIKE :x").arg(SONG_TABLE) + (onlyTitle ? "" : " AND text LIKE :y")); 
	query.bindValue(":x",QString("%%1%").arg(text));
	if(!onlyTitle)
		query.bindValue(":y",QString("%%1%").arg(text));
	query.exec();
		
	if (query.lastError().isValid())
	{
		qDebug() << "SongRecord::search():"<<query.lastError();
		return QList<SongRecord*>();
	}
	else
	{
		if(query.size())
		{
			QList<SongRecord*> list;
			while(query.isValid())
			{
				int id = query.record().value("songid").toInt();
				// Call retrieve() instead of fromQuery()
				// to make use of cached objects
				list << SongRecord::retrieve(id);
			}
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
	
// 	if(!m_dbIsOpen)
// 		initSongDatabase();
	SongDatabase::db(); // hit to make sure db is open
	
	QSqlQuery query;
	query.prepare(QString("INSERT INTO %1 ( title,  tags,  number,  text,  author,  copyright,  last_used,  default_arrid) "
			              "VALUES (:title, :tags, :number, :text, :author, :copyright, :last_used, :default_arrid)").arg(SONG_TABLE));
	
	//query.bindValue(":songid",	song->songId());
	query.bindValue(":title",	song->title());
	query.bindValue(":tags",	song->tags());
	query.bindValue(":number",	song->number());
	query.bindValue(":text",	song->text());
	query.bindValue(":author",	song->author());
	query.bindValue(":copyright",	song->copyright());
	query.bindValue(":last_used",	song->lastUsed());
	query.bindValue(":default_arrid", song->defaultArrId());
	
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

void SongRecord::deleteSong(SongRecord* song, bool deletePtr)
{
// 	if(!m_dbIsOpen)
// 		initSongDatabase();
	SongDatabase::db(); // hit to make sure db is open
		
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
	s->setInInit(true);
	s->setSongId(		r.value("songid").toInt());
	s->setTitle(		r.value("title").toString());
	s->setTags(		r.value("tags").toString());
	s->setNumber(		r.value("number").toInt());
	s->setText(		r.value("text").toString());
	s->setAuthor(		r.value("author").toString());
	s->setCopyright(	r.value("copyright").toString());
	s->setLastUsed(		r.value("last_used").toString());
	s->setDefaultArrId(	r.value("default_arrid").toInt());
	s->setInInit(false);
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
	r.setValue("default_arrid", defaultArrId());
	return r;
}

void SongRecord::setInInit(bool flag)
{
	m_init = flag;
}


SongRecord::SongRecord(QString title, QString text, int number)
	: m_songId(0)
	, m_title(title)
	, m_tags("")
	, m_number(number)
	, m_text(text)
	, m_defaultArrId(-1)
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
SONG_PROPSET(SongRecord, DefaultArrId,	int,		defaultArrId,	"default_arrid");

QStringList SongRecord::tagList() const
{
	static QRegExp tagRegex("\\s*,\\s*");
	return tags().split(tagRegex);
}

void SongRecord::setTagList(const QStringList & list)
{
	setTags(list.join(", "));
}

	
SongArrangement *SongRecord::defaultArrangement()
{
	if(defaultArrId() <= 0)
	{
		SongArrangement *arr = new SongArrangement();
		arr->setTitle("(Auto-Created Default Arrangement)");
		addArrangement(arr);
		setDefaultArrangement(arr);
		return arr;
	}
	// ::retrieve performs caching by id, so it's not a waste to not cache the ptr in our object
	return SongArrangement::retrieve(defaultArrId());
}

void SongRecord::setDefaultArrangement(SongArrangement *arr)
{
	setDefaultArrId(arr ? arr->arrId() : -1);

}

QList<SongArrangement*> SongRecord::arrangements()
{
	if(m_arrs.isEmpty())
		m_arrs = SongArrangement::searchBySongId(songId());
	return m_arrs;
}

void SongRecord::addArrangement(SongArrangement *arr)
{
	m_arrs.append(arr);
	arr->setSong(this);
	SongArrangement::addRecord(arr);
}

void SongRecord::removeArrangement(SongArrangement *arr)
{
	m_arrs.removeAll(arr);
	SongArrangement::deleteRecord(arr, false);
}


bool SongRecord::updateDb(QString field, QVariant v)
{
	if(m_init)
		return true;
		
	SongDatabase::db(); // hit to make sure db is open
	
	QSqlQuery query;
	QString sql = QString("UPDATE %1 SET %2 = ? WHERE songid = ?").arg(SONG_TABLE).arg(field);
	//qDebug() << "updateDb():"<<sql<<", value:"<<v<<", songid:"<<m_songId;
	query.prepare(sql); 
	query.addBindValue(v);
	query.addBindValue(songId());
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

