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
		songChanged(this,#memberName,value); \
	}
	
#define SONG_PROPDEF(setterName,typeName,memberName) void set##setterName(typeName value); typeName memberName() const { return m_##memberName; }

#define SONG_FILE "songs.db"
#define SONG_TABLE "songs"

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

/* Static */
public:
	static SongRecord * retrieve(int songId);
	static SongRecord * songByNumber(int number);
	static QList<SongRecord*> search(QString text, bool onlyTitle = false);

	static bool addSong(SongRecord*);
	static void deleteSong(SongRecord*, bool deletePointer = true);
	
	static SongRecord * fromQuery(QSqlQuery);
	static SongRecord * fromSqlRecord(QSqlRecord);
	
	static QSqlDatabase db();

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
	
	QStringList tagList() const;
	void setTagList(const QStringList &);
	
// 	SongSlideGroup * toSlideGroup();

signals:
	void songChanged(SongRecord*, QString field, QVariant value);

private:
	quint32 m_songId;
	QString m_title;
	QString m_tags;
	int     m_number;
	QString m_text;
	QString m_author;
	QString m_copyright;
	QString m_lastUsed;

	bool m_init;
	bool updateDb(QString field, QVariant value);
	
	QSqlRecord toSqlRecord();
	
	static void initSongDatabase();
	static bool m_dbIsOpen;
	static QSqlDatabase m_db;
};


#endif // SongRecord_H
