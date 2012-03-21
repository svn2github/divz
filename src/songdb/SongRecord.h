#ifndef SongRecord_H
#define SongRecord_H

#include <QtSql>
#include <QSqlTableModel>
#include <QStringList>
#include <QList>

#define SONG_PROPSET(className,setterName,typeName,memberName,dbField) \
	void className::set##setterName(typeName value) { \
		m_##memberName = value;  \
		updateDb(dbField,value); \
		recordChanged(this,#memberName,value); \
	}
	
#define SONG_PROPDEF(setterName,typeName,memberName) void set##setterName(typeName value); typeName memberName() const { return m_##memberName; }

#define SONG_FILE "songs.db"
#define SONG_TABLE "songs"
#define SONG_ARR_TABLE "song_arrangement"

/* Current Schema:
CREATE TABLE song_arrangement (arrid integer primary key autoincrement, songid integer, arrangement text, template blob, title varchar(255));
CREATE TABLE song_usagehistory (lineid integer primary key autoincrement, songid integer, datetime datetime);
CREATE TABLE songs (
                songid integer primary key AUTOINCREMENT,
                number int,
                title varchar(255),
                text text,
                author varchar(255),
                copyright varchar(255),
                last_used datetime
        , tags varchar(255), default_arrid integer);
*/

class SongDatabase 
{
public:
	static QSqlDatabase db();

private:
	static void initSongDatabase();
	static bool m_dbIsOpen;
	static QSqlDatabase m_db;
};


class SongRecord;
class SlideGroup;
class SongArrangement : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int arrId		READ arrId		WRITE setArrId);
	Q_PROPERTY(int songId		READ songId		WRITE setSongId);
	Q_PROPERTY(QString title	READ title		WRITE setTitle);
	Q_PROPERTY(QString arr		READ arr		WRITE setArr);
	Q_PROPERTY(QByteArray tmpl	READ tmpl		WRITE setTmpl);

/* Static */
public:
	static SongArrangement * retrieve(int songId);
	
	static bool addRecord(SongArrangement*);
	static void deleteRecord(SongArrangement*, bool deletePointer = true);
	
	static SongArrangement * fromQuery(QSqlQuery);
	static SongArrangement * fromSqlRecord(QSqlRecord);
	
public:
	SongArrangement(SongRecord *song=0, QString title = "", QString arr= "");
	SongArrangement(SongRecord *song, QString title, QStringList arr);

	SONG_PROPDEF(ArrId,		int,		arrId);
	SONG_PROPDEF(SongId,		int,		songId);
	SONG_PROPDEF(Title,		QString,	title);
	SONG_PROPDEF(Arr,		QString,	arr);
	SONG_PROPDEF(Tmpl,		QByteArray,	tmpl);
	
	static QList<SongArrangement*> searchBySongId(int songId);
	
	SongRecord *song() const;
	void setSong(SongRecord *);
	
	QStringList arrangement() const;
	void setArrangement(const QStringList &);
	
	SlideGroup *templateGroup();
	void setTemplateGroup(SlideGroup*);
	
signals:
	void recordChanged(SongArrangement*, QString field, QVariant value);

private:
	bool updateDb(QString field, QVariant v);
	void setInInit(bool flag);
	
	quint32 m_arrId;
	quint32 m_songId;
	QString m_title;
	QString m_arr;
	QByteArray m_tmpl;
	
	SlideGroup *m_tmplGroup;
	
	bool m_init;
	
	QSqlRecord toSqlRecord();
	
	static QHash<int,SongArrangement*> m_recordHash;
};

class SongRecord : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int songId		READ songId		WRITE setSongId);
	Q_PROPERTY(QString title	READ title		WRITE setTitle);
	Q_PROPERTY(QString tags		READ tags		WRITE setTags);
	Q_PROPERTY(int     number	READ number		WRITE setNumber);
	Q_PROPERTY(QString text		READ text		WRITE setText);
	Q_PROPERTY(QString author	READ author		WRITE setAuthor);
	Q_PROPERTY(QString copyright	READ copyright		WRITE setCopyright);
	Q_PROPERTY(QString lastUsed	READ lastUsed		WRITE setLastUsed);
	Q_PROPERTY(int defaultArrId	READ defaultArrId	WRITE setDefaultArrId);

/* Static */
public:
	static SongRecord * retrieve(int songId);
	static SongRecord * songByNumber(int number);
	static QList<SongRecord*> search(QString text, bool onlyTitle = false);

	static bool addSong(SongRecord*);
	static void deleteSong(SongRecord*, bool deletePointer = true);
	
	static SongRecord * fromQuery(QSqlQuery);
	static SongRecord * fromSqlRecord(QSqlRecord);
	
public:
	SongRecord(QString title = "", QString text = "", int number = 0);

	SONG_PROPDEF(SongId,		int,		songId);
	SONG_PROPDEF(Title,		QString,	title);
	SONG_PROPDEF(Tags,		QString,	tags);
	SONG_PROPDEF(Number,		int,		number);
	SONG_PROPDEF(Text,		QString,	text);
	SONG_PROPDEF(Author,		QString,	author);
	SONG_PROPDEF(Copyright,		QString,	copyright);
	SONG_PROPDEF(LastUsed,		QString,	lastUsed);
	SONG_PROPDEF(DefaultArrId,	int,		defaultArrId);
	
	QStringList tagList() const;
	void setTagList(const QStringList &);
	
	SongArrangement *defaultArrangement();
	void setDefaultArrangement(SongArrangement *);
	QList<SongArrangement*> arrangements();
	void addArrangement(SongArrangement *);
	void removeArrangement(SongArrangement *);

signals:
	void recordChanged(SongRecord*, QString field, QVariant value);

private:
	bool updateDb(QString field, QVariant v);
	void setInInit(bool flag);
	
	quint32 m_songId;
	QString m_title;
	QString m_tags;
	int     m_number;
	QString m_text;
	QString m_author;
	QString m_copyright;
	QString m_lastUsed;
	int     m_defaultArrId;

	bool m_init;
	
	QList<SongArrangement*> m_arrs;	

	QSqlRecord toSqlRecord();
	
	static QHash<int,SongRecord*> m_recordHash;
};


#endif // SongRecord_H
