#ifndef SONG_H
#define SONG_H

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

class Song : public QObject
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
	static Song * retrieve(int songId);
	static Song * songByNumber(int number);
	static QList<Song*> search(QString text, bool onlyTitle = false);

	static bool addSong(Song*);
	static void deleteSong(Song*, bool deletePointer = true);
	
	static Song * fromQuery(QSqlQuery);
	static Song * fromSqlRecord(QSqlRecord);
	
	static QSqlDatabase db();

public:
	Song(QString title = "", QString text = "", int number = 0);

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
	void songChanged(Song*, QString field, QVariant value);

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


#endif // SONG_H
